#!/bin/bash
/root/emscripten/emsdk_portable/emsdk_env.sh
source /root/cvs/GameApi/GameApi/set_paths_emlinux.sh
cd web_page
make -f Makefile.LinuxEm
