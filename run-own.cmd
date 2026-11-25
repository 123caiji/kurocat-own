@echo off
cd /d "%~dp0"
taskkill /IM kurocat-own.exe /F >nul 2>nul
timeout /t 1 >nul
if not exist dist\kurocat-own.exe (
  echo [错误] 先双击 build-own.cmd 编译
  pause & exit /b 1
)
start "" "%~dp0dist\kurocat-own.exe"
