@echo off
REM =============================================================================
REM ModLab Python frontend launcher
REM Requires: Python 3.10+ and dependencies from requirements.txt
REM =============================================================================

set ROOT=%~dp0..\..
cd /d "%ROOT%"

echo =^> ModLab Python frontend
echo    Root:    %ROOT%
echo    Backend: %MODLAB_SCRIPT%
echo.
echo =^> Run with:
echo     python -m modlab
echo.

python -m modlab
