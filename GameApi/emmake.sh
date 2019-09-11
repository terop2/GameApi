#!/bin/bash
/home/terop/emscripten/emsdk/emsdk_env.sh
source set_paths_emlinux.sh
make -j 4 -f Makefile.LinuxEm
make -j 4 -f Makefile.LinuxEm nothreads
(cd web_page;make -f Makefile.LinuxEm pthreads_both)
(cd web_page;make -f Makefile.LinuxEm both)
