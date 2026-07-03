#!/bin/bash
if [[ $1 == "" ]]; then
    echo "Usage: ./steam_deploy.sh password"
    exit
fi

MACHINE=`uname -n`
ROOT=`pwd`

./full_deploy.sh $1 steam noweb
cp $ROOT/display/gameapi_display.zip $ROOT/editor/builder/GameApiBuilder/
cp $ROOT/editor/gameapi-builder $ROOT/editor/builder/GameApiBuilder/
cp $ROOT/cmdline/gameapi_cmdline $ROOT/editor/builder/GameApiBuilder/
cp $ROOT/libGameApi.so $ROOT/editor/builder/GameApiBuilder/libs/

cp -R $ROOT/gameapi_content $ROOT/editor/builder/GameApiBuilder/

cp $ROOT/editor/deploy.sh $ROOT/editor/builder/GameApiBuilder/

cp $ROOT/display/gameapi_0.html $ROOT/editor/builder/GameApiBuilder/gameapi_0.html
cp $ROOT/display/gameapi_0_seamless.html $ROOT/editor/builder/GameApiBuilder/gameapi_0_seamless.html
cp $ROOT/display/gameapi_1.html $ROOT/editor/builder/GameApiBuilder/gameapi_1.html
cp $ROOT/display/gameapi_2.html $ROOT/editor/builder/GameApiBuilder/gameapi_2.html
cp $ROOT/display/gameapi_3.html $ROOT/editor/builder/GameApiBuilder/gameapi_3.html
cp $ROOT/display/gameapi_3_seamless.html $ROOT/editor/builder/GameApiBuilder/gameapi_3_seamless.html

cp $ROOT/editor/atlas0.txt $ROOT/editor/builder/GameApiBuilder/atlas0.txt
cp $ROOT/editor/atlas1.txt $ROOT/editor/builder/GameApiBuilder/atlas1.txt
cp $ROOT/editor/atlas1_t.txt $ROOT/editor/builder/GameApiBuilder/atlas1_t.txt
cp $ROOT/editor/atlas2.txt $ROOT/editor/builder/GameApiBuilder/atlas2.txt

cp $ROOT/editor/atlas_bm0.ppm $ROOT/editor/builder/GameApiBuilder/atlas_bm0.ppm
cp $ROOT/editor/atlas_bm1.ppm $ROOT/editor/builder/GameApiBuilder/atlas_bm1.ppm
cp $ROOT/editor/atlas_bm1_t.ppm $ROOT/editor/builder/GameApiBuilder/atlas_bm1_t.ppm
cp $ROOT/editor/atlas_bm2.ppm $ROOT/editor/builder/GameApiBuilder/atlas_bm2.ppm
cp $ROOT/display/get_file_size.php $ROOT/editor/builder/GameApiBuilder/get_file_size.php

(cd $ROOT/editor/builder/; zip -r GameApiBuilder_rel.zip GameApiBuilder)
(cd $ROOT/editor/builder/;scp GameApiBuilder_rel.zip terop@ssh.meshpage.org:/home/terop/)
