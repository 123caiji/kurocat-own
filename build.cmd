@echo off
cd /d "%~dp0"
echo 编译 Tama ...
where g++ >nul 2>nul || (echo 需要 MinGW g++ & pause & exit /b 1)
windres windows\app.rc -O coff -o windows\app.res || (echo 资源编译失败 & pause & exit /b 1)
g++ -std=c++17 -O2 -mwindows -municode -DUNICODE -D_UNICODE -D_WIN32_WINNT=0x0601 -DWINVER=0x0601 -static ^
  windows\src\main.cpp windows\app.res -o dist\tama.exe ^
  -luser32 -lgdi32 -lgdiplus -lshell32 -ladvapi32 -lwinhttp -lwtsapi32
if errorlevel 1 (echo 编译失败 & pause & exit /b 1)
if not exist dist\skins mkdir dist\skins
xcopy /y /q skins\*.kurocat dist\skins\ >nul
echo [ok] dist\tama.exe
pause
