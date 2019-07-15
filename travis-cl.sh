#!/usr/bin/env bash

set -e

./build.bat
./output/test_debug.exe
./build.bat release
./output/test.exe
s