#!/bin/bash
if [ -n "$1" ] && [ -n "$2" ] && [ -n "$3" ] && [ -n "$4" ]; then
    mkdir tmp00
    cd tmp00
    unzip ../$1
    gltf-transform copy scene.gltf scene.glb
    gltf-transform resize scene.glb scene2.glb --width 512 --height 512
    gltf-transform simplify scene2.glb scene3.glb --ratio $3 --error $4
    cp scene3.glb ../$2
    cd ..
    rm -rf tmp00
else
   echo "Usage: ./optimize_sketchfab.sh source.zip output.glb 0.05 0.7"
   echo "   Installation: sudo npm install --global @gltf-transform/cli"
fi
