#!/usr/bin/env bash

set +e
mkdir -p output

clang++ -o ./output/$OUTPUT $OPT $DISASSEMBLY $ARCH -std=c++14 $CXXFLAGS $ASAN -Isrc $THIRDPARTY_SRC $SRC $TEST_SRC -pthread
