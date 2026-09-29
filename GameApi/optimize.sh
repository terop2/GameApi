#!/bin/bash
if [ -n "$1" ] && [ -n "$2" ]; then
   gltf-transform resize $1 $2 --width 512 --height 512
   gltf-transform simplify $2 $2 --ratio 0.05 --error 0.7
else
   echo "Usage: ./optimize.sh source.glb output.glb"
   echo "   Installation: sudo npm install --global @gltf-transform/cli"
fi
