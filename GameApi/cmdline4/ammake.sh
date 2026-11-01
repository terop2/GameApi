#!/bin/bash
if [[ $1 == "" || $2 == "" ]]; then
    echo "Usage: ./ammake.sh password package_name icons/face.png"
    exit
fi

./build.sh
./build_package.sh $1 $2 $3
adb install gameapi_cmdline.apk
