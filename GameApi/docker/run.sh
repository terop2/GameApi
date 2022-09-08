#!/bin/sh
sudo xhost +local:root
sudo docker run \
     --rm \
     -it \
     --gpus all \
     --mount source=myvol2,target=/app \
     -v /tmp/.X11-unix:/tmp/.X11-unix \
     -e DISPLAY=$DISPLAY \
     -e QT_X11_NO_MITSHM=1 \
     builder-test \
     gameapi-builder $1 $2 $3

# possible commandline parameters:
# --file /app/mod2.txt
