#!/usr/bin/env bash

export OPT=-O3
#DISASSEMBLY='-S -masm=intel'
export ASAN=""
export CXXFLAGS="$CXXFLAGS -Wno-deprecated-register -Wno-deprecated"
export ARCH="-m64 -maes"
export OUTPUT=test

sh build/build_clang.sh
