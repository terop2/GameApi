#!/bin/bash
var="$1"
start=${var%=*}
end=${var#*=}
path=/home/terop/cvs/emscripten/emsdk/
if [ "$start" = "EMSCRIPTEN_PATH" ]
then
    path=$end
else
    echo "Defaulting emscripten path to /home/terop/cvs/emscripten/emsdk/, use EMSCRIPTEN_PATH=hhh to change."
fi
source ${path}/emsdk_env.sh
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
