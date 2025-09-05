#!/bin/bash
if [ "$EMSCRIPTEN_PATH" = "" ]
then
   EMSCRIPTEN_PATH=/home/terop/cvs/emscripten/emsdk/
   var="$1"
   start=${var%=*}
   end=${var#*=}
   if [ "$start" = "EMSCRIPTEN_PATH" ]
   then
       EMSCRIPTEN_PATH=$end
   else
      echo "Defaulting emscripten path to /home/terop/cvs/emscripten/emsdk/, use EMSCRIPTEN_PATH=hhh to change."
   fi
fi

source ${EMSCRIPTEN_PATH}/emsdk_env.sh
source set_paths_emlinux.sh
if [ "$1" = "clean" ]
then
    emcc --clear-cache
    nice make -j 2 -f Makefile.emmake EMSCRIPTEN_PATH=${EMSCRIPTEN_PATH}
else
    if [ "$2" = "clean" ]
    then
	emcc --clear-cache
	nice make -j 2 -f Makefile.emmake EMSCRIPTEN_PATH=${EMSCRIPTEN_PATH}
    else
	nice make -j 2 -f Makefile.emmake $1 $2 $3 EMSCRIPTEN_PATH=${EMSCRIPTEN_PATH}
    fi
fi
#make -j 8 -f Makefile.LinuxEm
#make -j 8 -f Makefile.LinuxEm nothreads
#(cd web_page;make -f Makefile.LinuxEm pthreads_both)
#(cd web_page;make -f Makefile.LinuxEm both)
