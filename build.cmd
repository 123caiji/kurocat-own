@echo off
setlocal
cd /d "%~dp0"
echo [Tama] building...
where g++ >nul 2>nul
if errorlevel 1 goto nogcc
where windres >nul 2>nul
if errorlevel 1 goto nowindres
windres windows\app.rc -O coff -o windows\app.res
if errorlevel 1 goto errmain
g++ -std=c++17 -O2 -ffunction-sections -fdata-sections -Wl,--gc-sections -s -mwindows -municode -DUNICODE -D_UNICODE -D_WIN32_WINNT=0x0601 -DWINVER=0x0601 -static windows\src\main.cpp windows\app.res -o dist\tama.exe -luser32 -lgdi32 -lgdiplus -lshell32 -ladvapi32 -lwinhttp -lwtsapi32
if errorlevel 1 goto errcc
if not exist dist\skins mkdir dist\skins
xcopy /y /q skins\*.kurocat dist\skins\ >nul
echo [OK] dist\tama.exe
goto done
:nogcc
echo [ERR] g++ not found in PATH
goto done
:nowindres
echo [ERR] windres not found in PATH
goto done
:errmain
echo [ERR] manifest resource failed
goto done
:errver
echo [ERR] version resource failed
goto done
:errcc
echo [ERR] compile failed
goto done
:done
if not "%~1"=="--no-pause" pause
endlocal
