#!/bin/bash
if [[ $1 ]]; then
(cd ~/.gameapi_builder/deploy/engine; unzip $1)
cat ~/.gameapi_builder/gameapi_1.html ~/.gameapi_builder/gameapi_script.html ~/.gameapi_builder/gameapi_2.html ~/.gameapi_builder/gameapi_date.html ~/.gameapi_builder/gameapi_3.html >~/.gameapi_builder/deploy/gameapi_index.html
(cd ~/.gameapi_builder/deploy; zip -r gameapi_deploy.zip gameapi_index.html engine)
fi
if [[ ! $1 ]]; then
    echo "./deploy.sh ./path_to_zip.zip"
fi
