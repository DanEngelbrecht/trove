#!/usr/bin/env bash

set +e
mkdir -p build

#OPT=-O3
OPT="-g -O1"
#DISASSEMBLY='-S -masm=intel'
ASAN="-fsanitize=address -fno-omit-frame-pointer"
CXXFLAGS="$CXXFLAGS -Wall -Weverything -pedantic -Wno-zero-as-null-pointer-constant -Wno-old-style-cast -Wno-global-constructors -Wno-padded"
ARCH=-m64

SRC="src/trove.cpp"
TEST_SRC="test/main.cpp test/test.cpp"

clang++ -o ./build/test_debug $OPT $DISASSEMBLY $ARCH -std=c++14 $CXXFLAGS $ASAN -Isrc $SRC $TEST_SRC
