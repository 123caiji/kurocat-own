@echo off
taskkill /F /PID 39552 > "D:\kaifa\work\kurocat-own\dist\kill-p0.log" 2>&1
taskkill /IM kurocat-own-p0.exe /F >> "D:\kaifa\work\kurocat-own\dist\kill-p0.log" 2>&1
echo DONE >> "D:\kaifa\work\kurocat-own\dist\kill-p0.log"
exit /b 0
