#!/bin/bash

rm -rf out
mkdir out && cd out
CC=gcc-8 CXX=g++-8 cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug --debug-output
cp compile_commands.json ..
