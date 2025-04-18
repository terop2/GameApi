#!/bin/bash
if [[ $1 == "" ]]; then
    echo "Usage: ./ammake.sh password"
    exit
fi

./build.sh
./build_package.sh $1
adb install gameapi_cmdline.apk
