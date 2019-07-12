#!/usr/bin/env bash

set +e

./d_cl.bat
./output/test_debug.exe
./r_cl.bat
./output/test.exe
