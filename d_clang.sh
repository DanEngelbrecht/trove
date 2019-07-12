#!/usr/bin/env bash

export OPT="-g"
ASAN="-fsanitize=address -fno-omit-frame-pointer"
#CXXFLAGS="$CXXFLAGS -Wall -Weverything -pedantic -Wno-zero-as-null-pointer-constant -Wno-old-style-cast -Wno-global-constructors -Wno-padded"
export CXXFLAGS="$CXXFLAGS -Wno-deprecated-register -Wno-deprecated"
export ARCH="-m64 -maes"
export OUTPUT=test_debug

sh build/build_clang.sh
