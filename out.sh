#!/bin/bash
#
rm -rf out
mkdir out && cd out
cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_STANDARD=20
cp compile_commands.json ..
