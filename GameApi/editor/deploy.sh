#!/bin/bash
if [[ $1 ]]; then
    (cd $HOME/.gameapi_builder/deploy/engine; unzip -o $1)
    if [ $? -ne 0 ]; then
	echo "unzip FAIL"
    fi
    (cd $HOME/.gameapi_builder/deploy/engine; cat ./gameapi.js |sed s@web_page@engine/web_page@ >gameapi2.js)
    if [ $? -ne 0 ]; then
	echo "cat FAIL"
    fi
    (cd $HOME/.gameapi_builder/deploy/engine; mv gameapi2.js gameapi.js)
    if [ $? -ne 0 ]; then
	echo "mv FAIL"
    fi
    if [[ "$2" == "seamless" ]]; then
	cat $HOME/.gameapi_builder/gameapi_0.html $HOME/.gameapi_builder/gameapi_homepage.html $HOME/.gameapi_builder/gameapi_1.html $HOME/.gameapi_builder/gameapi_script.html $HOME/.gameapi_builder/gameapi_2.html $HOME/.gameapi_builder/gameapi_date.html $HOME/.gameapi_builder/gameapi_3_seamless.html >$HOME/.gameapi_builder/deploy/index.html
    else
        cat $HOME/.gameapi_builder/gameapi_0.html $HOME/.gameapi_builder/gameapi_homepage.html $HOME/.gameapi_builder/gameapi_1.html $HOME/.gameapi_builder/gameapi_script.html $HOME/.gameapi_builder/gameapi_2.html $HOME/.gameapi_builder/gameapi_date.html $HOME/.gameapi_builder/gameapi_3.html >$HOME/.gameapi_builder/deploy/index.html
    fi
    if [ $? -ne 0 ]; then
	echo "cat2 FAIL"
    fi
    cp $HOME/.gameapi_builder/get_file_size.php $HOME/.gameapi_builder/deploy/get_file_size.php
    (cd $HOME/.gameapi_builder/deploy; zip -r gameapi_deploy.zip *)
    if [ $? -ne 0 ]; then
	echo "zip FAIL"
    fi
fi
if [[ ! $1 ]]; then
    echo "./deploy.sh ./path_to_zip.zip"
fi
