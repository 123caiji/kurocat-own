@echo off
setlocal
cd /d "%~dp0"
taskkill /IM tama.exe /F >nul 2>nul
timeout /t 1 >nul
if not exist dist\tama.exe (echo [ERR] run build.cmd first & goto end)
start "" "%~dp0dist\tama.exe"
:end
endlocal
