#!/bin/bash
hash=$(sha1sum script.txt | awk '{print $1}')
sed "s/com.meshpage.gameapi_cmdline/com.meshpage.gameapi_cmdline$hash/g" AndroidManifest.xml |sed "s/GameAPI/$1/g" >android_cmdline/AndroidManifest.xml
mkdir android_cmdline/src/com/meshpage/gameapi_cmdline$hash
sed "s/com.meshpage.gameapi_cmdline/com.meshpage.gameapi_cmdline$hash/g" MainActivity.java >android_cmdline/src/com/meshpage/gameapi_cmdline$hash/MainActivity.java
echo "$hash"
