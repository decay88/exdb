/*
                     HAT-SQUAD WINS VULNERABILITY/OS SCANNER
                       ------------------------------------
                       ------------------------------------

Note:
----------------

	By default, nothing printed on screen, 200 threads, all results in the file HS_WINS.txt
	-v..: lite verbose, will print the 'NOT_PATCHED' results on the screen
	-vv.: hard verbose, will print ALL results on the screen
	Increase or decrease the number of threads as you need.
	NT4 os are detected but not the vulnerability (not assested)

	Win32....: msvc++6
	FreeBSD..: gcc HS_WINS.cpp -o HS_WINS [-pthread|-lpthread]


sh00t:
----------------

	To all FD kiddies, boring writers, life seekers, as vulcanius, DayJay, and compagnie..
	talking about their politics, minds, ass, on a security mailinglist, shut the fuck up,
	time to gr0w up, blowjob lovers..

	Another stupid one, badpack3t, caught that one spamming on my homepage for his website (gayprotocols.com :>)
	hmm yeah so.. you can maybe claim or ppl might think that wasn't you.
	the spammer had nick/ip badpack3t/63.204.179.51, which was your nick/ip in w00w00 chann, Whaha, kiddie spotted, sh00ted :)

                              -=[&#65533;class101.org]=-
*/

#include <stdio.h>
#include <string.h>
#ifdef WIN32
#include <afxext.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32")
CWinThread* pthread;
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#define ioctlsocket    ioctl
#define UINT		   void*
#define LPVOID		   void*
#define Sleep		   sleep
pthread_t pthread;
#define SOCKET		   int
#define closesocket(s) close(s)
#endif



char data[]=
"\x00\x00\x00\x29\x88\x06\x78\x05\x00\x00\x00\x00\x00\x00\x00\x00"
"\x58\x58\x58\x58\x00\x02\x00\x05\x00\x00\x00\x00\x84\x5b\x4c\x00"
"\x08\x00\x00\xe0\x8a\x18\x02\x01\x40\x59\x02\x01\x6b",pcent[]="%",recvbuf[50],*vvv,*vvv2,*vvv3;

int ok=0,nub=0,mthread=0,mfreeze,scanend=0,done=0,done2=0,thread,sp,spb,rc,scan,ipstart,ipstop,tip;
int ping=0,bose=0,bose2=0,tot=0,se=0,ok2=0,ok3=0,k3=0,k0=0,t4=0,chk(),engine(int argc,char *argv[]);

FILE *fplog;
void ver(),usage(),sl(int time),scr1(struct sockaddr_in server),scr2(struct sockaddr_in server);
UINT engine2(LPVOID tip);
/*
HS_WINS 192.168.0.0
HS_WINS 192.168.0.0 -v
HS_WINS 192.168.0.0 -vv
HS_WINS 192.168.0.0 192.168.0.255
HS_WINS 192.168.0.0 192.168.0.255 -v
HS_WINS 192.168.0.0 192.168.0.255 -vv
HS_WINS 192.168.0.0 192.168.0.255 1000
HS_WINS 192.168.0.0 192.168.0.255 1000 -v
HS_WINS 192.168.0.0 192.168.0.255 1000 -vv
*/
int main(int argc,char *argv[])
{
	vvv=argv[3],vvv2=argv[4],vvv3=argv[2];
	if (argc<2){ver();usage();return -1;}
	for (;;)
	{
		if (argc==2&&strlen(argv[1])>7&&strlen(argv[1])<16||
				argc==3&&strlen(argv[1])>7&&strlen(argv[1])<16&&(strcmp(vvv3,"-v")==0||strcmp(vvv3,"-vv")==0)||
				argc==3&&strlen(argv[1])>7&&strlen(argv[1])<16&&strlen(argv[2])>7&&strlen(argv[2])<16&&argv[1]!=argv[2]||
				argc==4&&strlen(argv[1])>7&&strlen(argv[1])<16&&strlen(argv[2])>7&&strlen(argv[2])<16&&argv[1]!=argv[2]&&(strcmp(vvv,"-v")==0||strcmp(vvv,"-vv")==0)||
				argc==4&&strlen(argv[1])>7&&strlen(argv[1])<16&&strlen(argv[2])>7&&strlen(argv[2])<16&&argv[1]!=argv[2]&&atoi(argv[3])>0&&atoi(argv[3])<5000||
				argc==5&&strlen(argv[1])>7&&strlen(argv[1])<16&&strlen(argv[2])>7&&strlen(argv[2])<16&&argv[1]!=argv[2]&&atoi(argv[3])>0&&atoi(argv[3])<5000&&(strcmp(vvv2,"-v")==0||strcmp(vvv2,"-vv")==0))
				{
					if (argc==3&&strcmp(vvv3,"-v")==0||argc==4&&strcmp(vvv,"-v")==0||argc==5&&strcmp(vvv2,"-v")==0){bose++;}
					else if (argc==3&&strcmp(vvv3,"-vv")==0||argc==4&&strcmp(vvv,"-vv")==0||argc==5&&strcmp(vvv2,"-vv")==0){bose2++;}
					if (argc==2||argc==3&&(strcmp(vvv3,"-v")==0||strcmp(vvv3,"-vv")==0)){ping++;}
					engine(argc,argv);break;
				}
				ver();printf("[+] wrong command line, type HS_WINS without arguments for the usage.\n");return -1;
	}
#ifdef WIN32
	WSACleanup();
#endif
	return 0;
}

int engine(int argc,char *argv[])
{
	ver();
	if (chk()==-1){ver();printf("[+] WARNING! can't create/write HS_WINS.txt, aborting..\n");return -1;}
	ipstart=htonl(inet_addr(argv[1]));
	if (ping==1){ipstop=htonl(inet_addr(argv[1]));}
	else ipstop=htonl(inet_addr(argv[2]));
	if (ipstart>ipstop){printf("[+] wrong command line, type HS_WINS without arguments for the usage.\n");return -1;}
	fprintf(fplog,"----------------------------------------------------------------------------\nCOMMAND: ");
	for (int argccmp=0;argccmp<argc;argccmp++){fprintf(fplog,"%s ", argv[argccmp]);}
	fprintf(fplog,"\n----------------------------------------------------------------------------\n\n");
	fflush(fplog);
	if (argc==4&&bose==0&&bose2==0||argc==5){thread=atoi(argv[3]);}
	else thread=200;
	scan=(ipstop-ipstart)+1;
	for (tip=ipstart;ipstart<=ipstop;ipstart++,tip++,nub++,mthread++,scanend++)
	{
		if (tip%256==0||tip%256==-1){scanend--;scan--;nub--;mthread--;continue;}
		for (;;){if (mthread>=thread){sl(4);}
		else break;}
//		sl(1);
#ifdef WIN32
		CWinThread* pthread=AfxBeginThread(engine2,LPVOID(tip));
#else
		pthread_create(&pthread,NULL,engine2,(void*)tip);
#endif
		if (se>20){printf("[+] too many socket errors, check your system configuration, aborting..\n");break;}
	}
#ifdef WIN32
	for(;;){
		if (done2>25){printf("[+] status..: %d%s thread(s):%d (freezing, supposed done..)       \n",(scanend)*100/(scan),pcent,mthread);break;}
		if (mthread!=0){sl(1);printf("[+] status..: %d%s thread(s):%d       \r",(scanend)*100/(scan),pcent,mthread);
		if (mthread==mfreeze&&(mthread!=0||mfreeze!=0)){done2++;}else{mfreeze=mthread;}continue;}
		else {printf("[+] status..: %d%s thread(s):%d       \n",(scanend)*100/(scan),pcent,mthread);break;}
	}
#endif
	printf("[+] results.: %d / %d IP(s) (open:%d wins:%d win2003:%d win2000:%d nt4:%d)\n",ok,nub,ok2,ok3,k3,k0,t4);
	fprintf(fplog,"----------------------------------------------------------------------------\n");
	fprintf(fplog,"Scan complete: %d / %d IP(s) (open:%d wins:%d win2003:%d win2000:%d nt4:%d)\n",ok,nub,ok2,ok3,k3,k0,t4);
	fprintf(fplog,"------------------------------------------------[class101.org 2004-2005]----\n\n\n");
	fflush(fplog);
	return 0;
}

UINT engine2(LPVOID tip)
{
	int ip=int(tip);
#ifdef WIN32
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2,0),&wsadata)!=0){printf("[+] wsastartup error\n");mthread--;return -1;}
#endif
	SOCKET s;fd_set mask;struct timeval timeout, timeout2; struct sockaddr_in server;
	s=socket(AF_INET,SOCK_STREAM,0);
	if (s==-1){se++;mthread--;
#ifdef WIN32
	return -1;
#else
	return engine;
#endif
	}
	server.sin_family=AF_INET;
	server.sin_addr.s_addr=htonl(ip);
	server.sin_port=htons(42);
	if (scanend<=scan+1){printf("[+] status..: %d%s thread(s):%d       \r",(scanend)*100/(scan),pcent,mthread);}
	unsigned long flag=1;
	if (ioctlsocket(s,FIONBIO,&flag)!=0)
	{
		se++;mthread--;closesocket(s);
#ifdef WIN32
		return -1;
#else
		return engine;
#endif
	}
	connect(s,( struct sockaddr *)&server,sizeof(server));
	timeout.tv_sec=3;timeout.tv_usec=0;timeout2.tv_sec=5;timeout2.tv_usec=0;FD_ZERO(&mask);FD_SET(s,&mask);
	switch(select(s+1,NULL,&mask,NULL,&timeout))
	{
		case -1: {mthread--;closesocket(s);
#ifdef WIN32
		return -1;
#else
		return engine;
#endif
}
		case 0: {mthread--;closesocket(s);
#ifdef WIN32
		return -1;
#else
		return engine;
#endif
}
		default:
		if(FD_ISSET(s,&mask))
		{
			ok2++;
			if (send(s,data,sizeof(data)-1,0)==-1){fprintf(fplog,"IP.............: %s:%d\nSTATUS.........: error sending, not wins\n\n",inet_ntoa(server.sin_addr),ntohs(server.sin_port));fflush(fplog);
			if (bose2==1){printf("IP.............: %s:%d            \nSTATUS.........: error sending, not wins            \n\n",inet_ntoa(server.sin_addr),ntohs(server.sin_port));}
			mthread--;tot++;closesocket(s);
#ifdef WIN32
			return -1;
#else
			return engine;
#endif
}
			sl(3);
			switch(select(s+1,&mask,NULL,NULL,&timeout2))
			{
				case -1: {mthread--;closesocket(s);
#ifdef WIN32
				return -1;
#else
				return engine;
#endif
}
				case 0: {fprintf(fplog,"IP.............: %s:%d\nSTATUS.........: nothing received, not wins or vulnerable service freezing\n\n",inet_ntoa(server.sin_addr),ntohs(server.sin_port));fflush(fplog);
				if (bose2==1){printf("IP.............: %s:%d            \nSTATUS.........: nothing received, not wins or vulnerable service freezing\n\n",inet_ntoa(server.sin_addr),ntohs(server.sin_port));}
				mthread--;tot++;closesocket(s);
#ifdef WIN32
				return -1;
#else
				return engine;
#endif
}
				default:
				rc = recv(s,recvbuf,sizeof(recvbuf),0);
			}
			if (rc<40||recvbuf[3]!=41&&recvbuf[8]!=88){fprintf(fplog,"IP.............: %s:%d\nSTATUS.........: not wins, wrong datas\n\n",inet_ntoa(server.sin_addr),ntohs(server.sin_port));fflush(fplog);
			if (bose2==1){printf("IP.............: %s:%d            \nSTATUS.........: not wins, wrong datas            \n\n",inet_ntoa(server.sin_addr),ntohs(server.sin_port));}
			mthread--;tot++;closesocket(s);
#ifdef WIN32
			return -1;
#else
			return engine;
#endif
}
			ok3++;
			if (recvbuf[24]==-144&&recvbuf[25]==-107){spb=0;}
			else if (recvbuf[24]==40&&recvbuf[25]==-5){spb=1;}
			if (recvbuf[36]==37&&recvbuf[39]==1){fprintf(fplog,"IP.............: %s:%d\nSTATUS.........: wins enabled\nVULNERABILITY..: NOT_PATCHED\nOS.............: Windows 2003 SP%d\n\n",inet_ntoa(server.sin_addr),ntohs(server.sin_port),spb);fflush(fplog);
			if (bose2==1){printf("IP.............: %s:%d            \nSTATUS.........: wins enabled            \nVULNERABILITY..: NOT_PATCHED            \nOS.............: Windows 2003 SP%d            \n\n",inet_ntoa(server.sin_addr),ntohs(server.sin_port),spb);}
			ok++;k3++;tot++;if (bose==1){scr1(server);}mthread--;closesocket(s);
#ifdef WIN32
			return -1;
#else
			return engine;
#endif
}
			else if (recvbuf[36]==53&&recvbuf[39]==1){fprintf(fplog,"IP.............: %s:%d\nSTATUS.........: wins enabled\nVULNERABILITY..: patched\nOS.............: Windows 2003 SP%d\n\n",inet_ntoa(server.sin_addr),ntohs(server.sin_port),spb);fflush(fplog);
			if (recvbuf[24]==-144&&recvbuf[25]==-107){spb=0;}
			else if (recvbuf[24]==40&&recvbuf[25]==-5){spb=1;}
			if (bose2==1){printf("IP.............: %s:%d            \nSTATUS.........: wins enabled            \nVULNERABILITY..: patched            \nOS.............: Windows 2003 SP%d            \n\n",inet_ntoa(server.sin_addr),ntohs(server.sin_port),spb);}
			k3++;mthread--;tot++;closesocket(s);
#ifdef WIN32
			return -1;
#else
			return engine;
#endif
}
			else if (recvbuf[36]==71&&recvbuf[39]==1){fprintf(fplog,"IP.............: %s:%d\nSTATUS.........: wins enabled\nVULNERABILITY..: patched\nOS.............: Windows 2003 SP1\n\n",inet_ntoa(server.sin_addr),ntohs(server.sin_port));fflush(fplog);
			if (bose2==1){printf("IP.............: %s:%d            \nSTATUS.........: wins enabled            \nVULNERABILITY..: patched            \nOS.............: Windows 2003 SP1            \n\n",inet_ntoa(server.sin_addr),ntohs(server.sin_port));}
			k3++;mthread--;tot++;closesocket(s);
#ifdef WIN32
			return -1;
#else
			return engine;
#endif
}
			else if (recvbuf[36]==85&&recvbuf[37]==31&&recvbuf[40]==24&&recvbuf[41]==37||
							 recvbuf[36]==-111&&recvbuf[37]==-127&&recvbuf[40]==64&&recvbuf[41]==-106||
							 recvbuf[36]==-107&&recvbuf[37]==43&&recvbuf[40]==8&&recvbuf[41]==54||
							 recvbuf[36]==-89&&recvbuf[37]==-99&&recvbuf[40]==-128&&recvbuf[41]==38||
							 recvbuf[36]==69&&recvbuf[37]==-112&&recvbuf[40]==-144&&recvbuf[41]==31||
							 recvbuf[36]==-37&&recvbuf[37]==-128&&recvbuf[40]==-136&&recvbuf[41]==-82){
			if (recvbuf[36]==85&&recvbuf[37]==31&&recvbuf[40]==24&&recvbuf[41]==37||recvbuf[36]==-111&&recvbuf[37]==-127&&recvbuf[40]==64&&recvbuf[41]==-106){sp=4;}
			else if (recvbuf[36]==-107&&recvbuf[37]==43&&recvbuf[40]==8&&recvbuf[41]==54){sp=3;}
			else if (recvbuf[36]==-89&&recvbuf[37]==-99&&recvbuf[40]==-128&&recvbuf[41]==38){sp=2;}
			else if (recvbuf[36]==69&&recvbuf[37]==-112&&recvbuf[40]==-144&&recvbuf[41]==31){sp=1;}
			else if (recvbuf[36]==-37&&recvbuf[37]==-128&&recvbuf[40]==-136&&recvbuf[41]==-82){sp=0;}
			if (recvbuf[16]==0&&recvbuf[17]==0&&recvbuf[18]==0){fprintf(fplog,"IP.............: %s:%d\nSTATUS.........: wins enabled\nVULNERABILITY..: patched\nOS.............: Windows 2000 SP%d\n\n",inet_ntoa(server.sin_addr),ntohs(server.sin_port),sp);fflush(fplog);
			if (bose2==1){printf("IP.............: %s:%d            \nSTATUS.........: wins enabled            \nVULNERABILITY..: patched            \nOS.............: Windows 2000 SP%d            \n\n",inet_ntoa(server.sin_addr),ntohs(server.sin_port),sp);}
			k0++;mthread--;tot++;closesocket(s);
#ifdef WIN32
			return -1;
#else
			return engine;
#endif
}
			else {fprintf(fplog,"IP.............: %s:%d\nSTATUS.........: wins enabled\nVULNERABILITY..: NOT_PATCHED\nOS.............: Windows 2000 SP%d\n\n",inet_ntoa(server.sin_addr),ntohs(server.sin_port),sp);fflush(fplog);
			if (bose2==1){printf("IP.............: %s:%d            \nSTATUS.........: wins enabled            \nVULNERABILITY..: NOT_PATCHED            \nOS.............: Windows 2000 SP%d            \n\n",inet_ntoa(server.sin_addr),ntohs(server.sin_port),sp);}
			ok++;k0++;tot++;if (bose==1){scr2(server);}mthread--;closesocket(s);
#ifdef WIN32
			return -1;
#else
			return engine;
#endif
}
			}
			else {
				fprintf(fplog,"IP.............: %s:%d\nSTATUS.........: wins enabled\nVULNERABILITY..: unknown\nOS.............: NT4 (OS not implemented)\n\n",inet_ntoa(server.sin_addr),ntohs(server.sin_port));fflush(fplog);
				if (bose2==1){printf("IP.............: %s:%d            \nSTATUS.........: wins enabled            \nVULNERABILITY..: unknown            \nOS.............: NT4 (OS not implemented)            \n\n",inet_ntoa(server.sin_addr),ntohs(server.sin_port));}
				t4++;mthread--;tot++;closesocket(s);
#ifdef WIN32
				return -1;
#else
				return engine;
#endif
}
		}
	}
	mthread--;
	closesocket(s);
#ifdef WIN32
	return 0;
#else
	return engine;
#endif
}

int chk(){
	if ((fplog =fopen("HS_WINS.txt","a+"))==NULL)
		return -1;
	else return 1;
}

void sl(int time){
#ifdef WIN32
	Sleep(time*1000);
#else
	Sleep(time);
#endif
}

void usage(){
	printf("           [+]  . HS_WINS 192.168.0.1  [-v|-vv]\n");
	printf("           [+]  . HS_WINS 192.168.0.0 192.168.0.255  [-v|-vv]\n");
	printf("           [+]  . HS_WINS 192.168.0.0 192.168.0.255 1000  [-v|-vv]\n");
}

void ver(){
	printf("\n");
	printf("        ===================================================[v1.0]====\n");
	printf("        ============WINS Vulnerability and OS/SP scanner=============\n");
	printf("        ============multi-threaded for Linux and Windows=============\n");
	printf("        ======coded by class101=============[Hat-Squad.com 2005]=====\n");
	printf("        =============================================================\n");
	printf("\n");
}

void scr1(struct sockaddr_in server)
{
	printf("IP.............: %s:%d\nSTATUS.........: wins enabled\nVULNERABILITY..: NOT_PATCHED\nOS.............: Windows 2003 SP0\n\n",inet_ntoa(server.sin_addr),ntohs(server.sin_port));
}

void scr2(struct sockaddr_in server)
{
	printf("IP.............: %s:%d\nSTATUS.........: wins enabled\nVULNERABILITY..: NOT_PATCHED\nOS.............: Windows 2000 SP%d\n\n",inet_ntoa(server.sin_addr),ntohs(server.sin_port),sp);
}

// milw0rm.com [2005-05-02]
