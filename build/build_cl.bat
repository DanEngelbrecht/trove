set SRC=src/trove.cpp
set TEST_SRC=test\test.cpp test\main.cpp
set THIRDPARTY_SRC=

call .\mvsc_build.bat
exit /B %ERRORLEVEL%
