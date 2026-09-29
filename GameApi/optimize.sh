#!/bin/bash
if [ -n "$1" ] && [ -n "$2" ] && [ -n "$3" ] && [ -n "$4" ]; then
   gltf-transform resize $1 $2 --width 512 --height 512
   gltf-transform simplify $2 $2 --ratio $3 --error $4
else
   echo "Usage: ./optimize.sh source.glb output.glb 0.05 0.7"
   echo "   Installation: sudo npm install --global @gltf-transform/cli"
fi
