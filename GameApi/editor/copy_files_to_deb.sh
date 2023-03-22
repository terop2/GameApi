#!/bin/bash
if [[ $1 ]]; then
mkdir -p gameapi-builder_1.0-$1
mkdir -p gameapi-builder_1.0-$1/usr
mkdir -p gameapi-builder_1.0-$1/usr/bin
mkdir -p gameapi-builder_1.0-$1/usr/lib
mkdir -p gameapi-builder_1.0-$1/usr/share
mkdir -p gameapi-builder_1.0-$1/DEBIAN
cp a.out gameapi-builder_1.0-$1/usr/bin/gameapi-builder
cp ../libGameApi.so gameapi-builder_1.0-$1/usr/lib/libGameApi.so
strip gameapi-builder_1.0-$1/usr/bin/gameapi-builder
strip gameapi-builder_1.0-$1/usr/lib/libGameApi.so
cp atlas0.txt gameapi-builder_1.0-$1/usr/share/atlas0.txt
cp atlas1.txt gameapi-builder_1.0-$1/usr/share/atlas1.txt
cp atlas2.txt gameapi-builder_1.0-$1/usr/share/atlas2.txt
cp atlas_bm0.ppm gameapi-builder_1.0-$1/usr/share/atlas_bm0.ppm
cp atlas_bm1.ppm gameapi-builder_1.0-$1/usr/share/atlas_bm1.ppm
cp atlas_bm2.ppm gameapi-builder_1.0-$1/usr/share/atlas_bm2.ppm
cp ../display/gameapi_0.html gameapi-builder_1.0-$1/usr/share/gameapi_0.html
cp ../display/gameapi_0_seamless.html gameapi-builder_1.0-$1/usr/share/gameapi_0_seamless.html
cp ../display/gameapi_1.html gameapi-builder_1.0-$1/usr/share/gameapi_1.html
cp ../display/gameapi_2.html gameapi-builder_1.0-$1/usr/share/gameapi_2.html
cp ../display/gameapi_3.html gameapi-builder_1.0-$1/usr/share/gameapi_3.html
cp ../display/gameapi_3_seamless.html gameapi-builder_1.0-$1/usr/share/gameapi_3_seamless.html
cp ../display/gameapi_display.zip gameapi-builder_1.0-$1/usr/share/gameapi_display.zip
cp ../display/get_file_size.php gameapi-builder_1.0-$1/usr/share/get_file_size.php
cp deploy.sh gameapi-builder_1.0-$1/usr/share/deploy.sh
cp arrow.ppm gameapi-builder_1.0-$1/usr/share/arrow.ppm
cat control |sed s/25/$1/g >gameapi-builder_1.0-$1/DEBIAN/control
fi
if [[ ! $1 ]]; then
    echo "./copy_files_to_deb.sh 25"
fi
