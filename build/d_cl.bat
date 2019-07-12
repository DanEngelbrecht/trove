@echo off

set OPT=
set CXXFLAGS=
set OUTPUT=test_debug

call build_src.bat

call build\mvsc_build.bat
exit /B %ERRORLEVEL%
