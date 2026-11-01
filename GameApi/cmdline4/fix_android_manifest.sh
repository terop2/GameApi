#!/bin/bash
hash=$(echo "$1" | sha1sum | awk '{print $1}')
sed "s/com.meshpage.gameapi_cmdline/com.meshpage.gameapi_cmdline$hash/g" AndroidManifest.xml |sed "s/GameAPI/$1/g" >android_cmdline/AndroidManifest.xml
mkdir android_cmdline/src/com/meshpage/gameapi_cmdline$hash
sed "s/com.meshpage.gameapi_cmdline/com.meshpage.gameapi_cmdline$hash/g" MainActivity.java >android_cmdline/src/com/meshpage/gameapi_cmdline$hash/MainActivity.java
cp $2 android_cmdline/res/mipmap/ic_launcher.png
echo "$hash"
