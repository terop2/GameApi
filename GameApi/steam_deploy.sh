#!/bin/bash
./full_deploy.sh gtk260774 incremental steam noweb
cp /home/terop/cvs/GameApi/GameApi/editor/gameapi-builder /home/terop/cvs/GameApi/GameApi/editor/builder/GameApiBuilder/
cp /home/terop/cvs/GameApi/GameApi/libGameApi.so /home/terop/cvs/GameApi/GameApi/editor/builder/GameApiBuilder/libs/
(cd /home/terop/cvs/GameApi/GameApi/editor/builder/; zip -r GameApiBuilder_rel.zip GameApiBuilder)
(cd /home/terop/cvs/GameApi/GameApi/editor/builder/;scp GameApiBuilder_rel.zip terop@192.168.1.104:/home/terop/)
