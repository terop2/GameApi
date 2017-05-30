#!/bin/bash
/root/emscripten/emsdk_portable/emsdk_env.sh
source set_paths_emlinux.sh
make -j 4 -f Makefile.LinuxEm
