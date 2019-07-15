#!/usr/bin/env bash

set -e

sh ./build.sh
./output/test_debug
sh ./build.sh release
./output/test
