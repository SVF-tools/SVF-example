#!/usr/bin/env bash

rm -rf build install example-build
cmake -G Ninja -B build --install-prefix $(realpath ./install) --fresh

# Initially, only build and install the SvfPass target
cmake --build build --target SvfPass
cmake --install build

# Add the pass to PATH/LD_[RUN|LIBRARY]_PATH so it can be found during the compiler pipeline
[ -d ./install/bin ] && export PATH=$(realpath ./install/bin):$PATH
[ -d ./install/lib ] && export LD_RUN_PATH=$(realpath ./install/lib):$LD_RUN_PATH
[ -d ./install/lib ] && export LD_LIBRARY_PATH=$(realpath ./install/lib):$LD_LIBRARY_PATH
cmake --build build --target example
cmake --install build
