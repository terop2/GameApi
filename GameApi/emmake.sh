#!/bin/bash
source /home/terop/cvs/emscripten/emsdk/emsdk_env.sh
source set_paths_emlinux.sh
make -j 8 -f Makefile.LinuxEm
make -j 8 -f Makefile.LinuxEm nothreads
(cd web_page;make -f Makefile.LinuxEm pthreads_both_nodebug)
(cd web_page;make -f Makefile.LinuxEm both_nodebug)
