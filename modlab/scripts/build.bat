@echo off
REM =============================================================================
REM ModLab build script — Windows (MSVC or MinGW)
REM Requires: Qt6, CMake >= 3.20, Visual Studio 2019+ or MinGW-w64
REM =============================================================================

set ROOT=%~dp0..
set BUILD_DIR=%ROOT%\build
set BACKEND_PATH=%ROOT%\..\main.py

REM -- Set this to your Qt6 cmake dir, e.g.:
REM set Qt6_DIR=C:\Qt\6.7.0\msvc2019_64\lib\cmake\Qt6
REM set Qt6_DIR=C:\Qt\6.7.0\mingw_64\lib\cmake\Qt6

echo =^> ModLab build (Windows)
echo    Root:    %ROOT%
echo    Build:   %BUILD_DIR%
echo    Backend: %BACKEND_PATH%
echo.

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%"

cmake "%ROOT%" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DMODLAB_BACKEND_PATH="%BACKEND_PATH%"

cmake --build . --config Release --parallel

echo.
echo =^> Build complete: %BUILD_DIR%\Release\ModLab.exe
echo.
echo =^> Run with:
echo     set MODLAB_PYTHON=C:\path\to\python.exe
echo     set MODLAB_SCRIPT=C:\path\to\main.py
echo     ModLab.exe
