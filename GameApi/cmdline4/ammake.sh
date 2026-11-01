#!/bin/bash
if [[ $1 == "" || $2 == "" || $3 == "" || $4 = "" ]]; then
    echo 'Usage: ./ammake.sh password package_name icons/face.png "My Game"'
    exit
fi

./build.sh "$4" "$2"
./build_package.sh $1 $2 $3
adb install gameapi_cmdline.apk
