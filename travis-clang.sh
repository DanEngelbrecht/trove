#!/usr/bin/env bash

set +e

sh ./d_clang.sh
./output/test_debug
sh ./r_clang.sh
./output/test
