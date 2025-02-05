#!/bin/bash

# Clean everything
rm -rf base intermediate.zip bundletool-all-0.13.0.jar gameapi_cmdline.aab

# Create base structure
mkdir -p base/manifest
mkdir -p base/res/values
mkdir -p base/res/mipmap-mdpi
mkdir -p base/res/mipmap-hdpi
mkdir -p base/res/mipmap-xhdpi
mkdir -p base/res/mipmap-xxhdpi
mkdir -p base/res/mipmap-xxxhdpi
mkdir -p base/lib/arm64-v8a
mkdir -p base/assets

# Create minimal resources
echo '<?xml version="1.0" encoding="utf-8"?>
<resources>
    <string name="app_name">GameApi</string>
</resources>' > base/res/values/strings.xml

# Create a simple launcher icon in all densities
convert -size 48x48 xc:transparent \
    -fill blue -draw "circle 24,24 24,1" \
    base/res/mipmap-mdpi/ic_launcher.png

convert -size 72x72 xc:transparent \
    -fill blue -draw "circle 36,36 36,1" \
    base/res/mipmap-hdpi/ic_launcher.png

convert -size 96x96 xc:transparent \
    -fill blue -draw "circle 48,48 48,1" \
    base/res/mipmap-xhdpi/ic_launcher.png

convert -size 144x144 xc:transparent \
    -fill blue -draw "circle 72,72 72,1" \
    base/res/mipmap-xxhdpi/ic_launcher.png

convert -size 192x192 xc:transparent \
    -fill blue -draw "circle 96,96 96,1" \
    base/res/mipmap-xxxhdpi/ic_launcher.png

# Copy files
cp android_cmdline/AndroidManifest.xml base/manifest/AndroidManifest.xml
cp android_cmdline/lib/arm64-v8a/*.so base/lib/arm64-v8a/
cp android_cmdline/assets/* base/assets/ 2>/dev/null || :
cp android_cmdline/classes.dex base/

# Process resources
cd base
aapt2 compile --dir res -o compiled_res.zip

# Create package
aapt2 link \
    compiled_res.zip \
    --proto-format \
    -o resources.pb \
    --manifest manifest/AndroidManifest.xml \
    -I $ANDROID_HOME/platforms/android-30/android.jar

# Create zip with proper structure
zip -r ../intermediate.zip manifest/AndroidManifest.xml
zip -r ../intermediate.zip resources.pb
zip -r ../intermediate.zip classes.dex
zip -r ../intermediate.zip lib/
zip -r ../intermediate.zip assets/
cd ..

# Build bundle
java -jar ./bundletool-all-1.18.0.jar build-bundle \
    --modules=intermediate.zip \
    --output=gameapi_cmdline.aab

# Clean up
rm -rf base intermediate.zip
