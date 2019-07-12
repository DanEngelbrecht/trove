@echo off

set OPT=
set CXXFLAGS=
set OUTPUT=test_debug

call build\build_cl.bat
exit /B %ERRORLEVEL%
