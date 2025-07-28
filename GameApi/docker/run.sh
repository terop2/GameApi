#!/bin/sh
sudo xhost +local:root
sudo docker network create gameapi_network
sudo docker run \
     --network gameapi_network \
     --rm \
     -it \
     --gpus all \
     --mount source=myvol2,target=/app \
     -v /tmp/.X11-unix:/tmp/.X11-unix \
     -e DISPLAY=$DISPLAY \
     -e WAYLAND_DISPLAY=$WAYLAND_DISPLAY \
     -v ${XDG_RUNTIME_DIR}/${WAYLAND_DISPLAY}:/tmp/${WAYLAND_DISPLAY} \
     -e QT_X11_NO_MITSHM=1 \
         --device /dev/input \
    --device /dev/dri \
    --device /dev/nvidia0 \
    --device /dev/nvidiactl \
    --device /dev/nvidia-modeset \
    --device /dev/nvidia-uvm \
    --device /dev/nvidia-uvm-tools \
    --device /dev/video0 \
    --security-opt=no-new-privileges \
    --group-add video \
     builder-test \
     gameapi-builder $1 $2 $3

# possible commandline parameters:
# --file /app/mod2.txt
