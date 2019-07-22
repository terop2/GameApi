#!/bin/bash
/home/terop/emscripten/emsdk/emsdk_env.sh
source set_paths_emlinux.sh
make -j 4 -f Makefile.LinuxEm
(cd web_page;make -f Makefile.LinuxEm pthreads_both)

