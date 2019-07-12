@echo off

set OPT=/O2
set CXXFLAGS=
set OUTPUT=test

call build\build_cl.bat
exit /B %ERRORLEVEL%
