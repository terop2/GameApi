#!/bin/bash
source /home/terop/cvs/emscripten/emsdk/emsdk_env.sh
source set_paths_emlinux.sh
if [ "$1" = "clean" ]
then
    emcc --clear-cache
    nice make -j 2 -f Makefile.emmake
else
    nice make -j 2 -f Makefile.emmake $1 $2 $3
fi
#make -j 8 -f Makefile.LinuxEm
#make -j 8 -f Makefile.LinuxEm nothreads
#(cd web_page;make -f Makefile.LinuxEm pthreads_both)
#(cd web_page;make -f Makefile.LinuxEm both)
