#!/bin/bash
if [[ $1 == "" ]]; then
    echo "Usage: ./steam_deploy.sh password"
    exit
fi

./full_deploy.sh $1 steam noweb
cp /home/terop/cvs/GameApi/GameApi/display/gameapi_display.zip /home/terop/cvs/GameApi/GameApi/editor/builder/GameApiBuilder/
cp /home/terop/cvs/GameApi/GameApi/editor/gameapi-builder /home/terop/cvs/GameApi/GameApi/editor/builder/GameApiBuilder/
cp /home/terop/cvs/GameApi/GameApi/cmdline/gameapi_cmdline /home/terop/cvs/GameApi/GameApi/editor/builder/GameApiBuilder/
cp /home/terop/cvs/GameApi/GameApi/libGameApi.so /home/terop/cvs/GameApi/GameApi/editor/builder/GameApiBuilder/libs/

cp -R /home/terop/cvs/GameApi/GameApi/gameapi_content /home/terop/cvs/GameApi/GameApi/editor/builder/GameApiBuilder/

cp /home/terop/cvs/GameApi/GameApi/display/gameapi_0.html /home/terop/cvs/GameApi/GameApi/editor/builder/GameApiBuilder/gameapi_0.html
cp /home/terop/cvs/GameApi/GameApi/display/gameapi_0_seamless.html /home/terop/cvs/GameApi/GameApi/editor/builder/GameApiBuilder/gameapi_0_seamless.html
cp /home/terop/cvs/GameApi/GameApi/display/gameapi_1.html /home/terop/cvs/GameApi/GameApi/editor/builder/GameApiBuilder/gameapi_1.html
cp /home/terop/cvs/GameApi/GameApi/display/gameapi_2.html /home/terop/cvs/GameApi/GameApi/editor/builder/GameApiBuilder/gameapi_2.html
cp /home/terop/cvs/GameApi/GameApi/display/gameapi_3.html /home/terop/cvs/GameApi/GameApi/editor/builder/GameApiBuilder/gameapi_3.html
cp /home/terop/cvs/GameApi/GameApi/display/gameapi_3_seamless.html /home/terop/cvs/GameApi/GameApi/editor/builder/GameApiBuilder/gameapi_3_seamless.html

cp /home/terop/cvs/GameApi/GameApi/editor/atlas0.txt /home/terop/cvs/GameApi/GameApi/editor/builder/GameApiBuilder/atlas0.txt
cp /home/terop/cvs/GameApi/GameApi/editor/atlas1.txt /home/terop/cvs/GameApi/GameApi/editor/builder/GameApiBuilder/atlas1.txt
cp /home/terop/cvs/GameApi/GameApi/editor/atlas1_t.txt /home/terop/cvs/GameApi/GameApi/editor/builder/GameApiBuilder/atlas1_t.txt
cp /home/terop/cvs/GameApi/GameApi/editor/atlas2.txt /home/terop/cvs/GameApi/GameApi/editor/builder/GameApiBuilder/atlas2.txt

cp /home/terop/cvs/GameApi/GameApi/editor/atlas_bm0.ppm /home/terop/cvs/GameApi/GameApi/editor/builder/GameApiBuilder/atlas_bm0.ppm
cp /home/terop/cvs/GameApi/GameApi/editor/atlas_bm1.ppm /home/terop/cvs/GameApi/GameApi/editor/builder/GameApiBuilder/atlas_bm1.ppm
cp /home/terop/cvs/GameApi/GameApi/editor/atlas_bm1_t.ppm /home/terop/cvs/GameApi/GameApi/editor/builder/GameApiBuilder/atlas_bm1_t.ppm
cp /home/terop/cvs/GameApi/GameApi/editor/atlas_bm2.ppm /home/terop/cvs/GameApi/GameApi/editor/builder/GameApiBuilder/atlas_bm2.ppm
cp /home/terop/cvs/GameApi/GameApi/display/get_file_size.php /home/terop/cvs/GameApi/GameApi/editor/builder/GameApiBuilder/get_file_size.php

(cd /home/terop/cvs/GameApi/GameApi/editor/builder/; zip -r GameApiBuilder_rel.zip GameApiBuilder)
(cd /home/terop/cvs/GameApi/GameApi/editor/builder/;scp GameApiBuilder_rel.zip terop@ssh.meshpage.org:/home/terop/)
