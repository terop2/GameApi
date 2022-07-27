#!/bin/bash
if [[ $1 ]]; then
    (cd ~/.gameapi_builder/deploy/engine; unzip $1)
    (cd ~/.gameapi_builder/deploy/engine; cat ./gameapi.js |sed s@web_page@engine/web_page@ >gameapi2.js)
    (cd ~/.gameapi_builder/deploy/engine; mv gameapi2.js gameapi.js)
    cat ~/.gameapi_builder/gameapi_1.html ~/.gameapi_builder/gameapi_script.html ~/.gameapi_builder/gameapi_2.html ~/.gameapi_builder/gameapi_date.html ~/.gameapi_builder/gameapi_3.html >~/.gameapi_builder/deploy/gameapi_index.php
    (cd ~/.gameapi_builder/deploy; zip -r gameapi_deploy.zip gameapi_index.php engine)
fi
if [[ ! $1 ]]; then
    echo "./deploy.sh ./path_to_zip.zip"
fi
