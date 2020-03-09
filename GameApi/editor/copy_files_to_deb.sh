#!/bin/sh
cp a.out gameapi-builder_1.0-25/usr/bin/gameapi-builder
cp ../libGameApi.so gameapi-builder_1.0-25/usr/lib/libGameApi.so
strip gameapi-builder_1.0-25/usr/bin/gameapi-builder
strip gameapi-builder_1.0-25/usr/lib/libGameApi.so
cp atlas0.txt gameapi-builder_1.0-25/usr/share/atlas0.txt
cp atlas1.txt gameapi-builder_1.0-25/usr/share/atlas1.txt
cp atlas2.txt gameapi-builder_1.0-25/usr/share/atlas2.txt
cp atlas_bm0.ppm gameapi-builder_1.0-25/usr/share/atlas_bm0.ppm
cp atlas_bm1.ppm gameapi-builder_1.0-25/usr/share/atlas_bm1.ppm
cp atlas_bm2.ppm gameapi-builder_1.0-25/usr/share/atlas_bm2.ppm
