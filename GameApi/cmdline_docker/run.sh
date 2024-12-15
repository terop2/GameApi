#!/bin/bash
if [[ $1 == "" ]] then
   echo "./run.sh script_file.txt homepageurl"
   exit
fi
if [[ $2 == "" ]] then
   echo "./run.sh script_file.txt homepageurl"
   exit
fi
sudo xhost +local:root
cp $1 script.txt
sudo docker build -t builder-cmdline2:latest .
sudo docker network create gameapi_network
sudo docker run \
     --network gameapi_network \
     --rm \
     -it \
     --gpus all \
     --mount source=myvol2,target=/app \
     -v /tmp/.X11-unix:/tmp/.X11-unix \
     -e DISPLAY=$DISPLAY \
     -e QT_X11_NO_MITSHM=1 \
     -e LIBGL_DEBUG=verbose \
     -e MESA_DEBUG=1 \
     -e QT_X11_NO_MITSHM=1 \
     -e vblank_mode=0 \
     builder-cmdline2 \
     ./gameapi_cmdline --file script.txt --homepage $2
# possible commandline parameters:
# --file /app/mod2.txt
