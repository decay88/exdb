source: http://www.securityfocus.com/bid/48317/info

myBloggie is prone to a SQL-injection vulnerabilities and an HTML-injection vulnerability because it fails to sufficiently sanitize user-supplied input.

An attacker may leverage these issues to compromise the application, access or modify data, exploit latent vulnerabilities in the underlying database, or execute arbitrary script code in the browser of an unsuspecting user in the context of the affected site. This may allow the attacker to steal cookie-based authentication credentials, control how the site is viewed, and launch other attacks.

myBloggie 2.1.6 is vulnerable; other versions may also be affected. 

<?php
//trackback.php - Line 33 - 35
$url=urldecode($_REQUEST['url']);
if (validate_url($url)==false) { $tback->trackback_reply(1, "<p>Sorry, Trackback failed.. Reason : URL not valid</p>"); }

?>


<?php
//trackback.php - Line 750
function validate_url($url) {
    if  ( ! preg_match('#^http\\:\\/\\/[a-z0-9\-]+\.([a-z0-9\-]+\.)?[a-z]+#i', $url, $matches) ) {
       return false;
    } else {
       return true;  
    }
} 
?>