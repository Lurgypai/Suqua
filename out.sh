#!/bin/bash

rm -rf out
mkdir out && cd out
cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug
cp compile_commands.json ..
