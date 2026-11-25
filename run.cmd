@echo off
cd /d "%~dp0"
taskkill /IM tama.exe /F >nul 2>nul
timeout /t 1 >nul
if not exist dist\tama.exe (
  echo [错误] 先双击 build.cmd 编译
  pause & exit /b 1
)
start "" "%~dp0dist\tama.exe"
