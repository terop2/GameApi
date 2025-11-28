#!/bin/bash
if [ "$HOME" == "" ]; then
    HOME=/home/www-data
fi
ZIP_PATH=/usr/bin
if [ -f "$ZIP_PATH/unzip" ]; then
    echo "found zip"
else
    ZIP_PATH=`./pwd`
fi

if [[ $1 ]]; then
    (cd $HOME/.gameapi_builder/deploy/engine; "$ZIP_PATH/unzip" -o $1)
    if [ $? -ne 0 ]; then
	echo "unzip FAIL"
    fi
    (cd $HOME/.gameapi_builder/deploy/engine; "$ZIP_PATH/cat" ./gameapi.js >gameapi2.js)
    if [ $? -ne 0 ]; then
	echo "cat FAIL"
    fi
    (cd $HOME/.gameapi_builder/deploy/engine; "$ZIP_PATH/mv" gameapi2.js gameapi.js)
    if [ $? -ne 0 ]; then
	echo "mv FAIL"
    fi
    if [[ "$2" == "seamless" ]]; then
	"$ZIP_PATH/cat" $HOME/.gameapi_builder/gameapi_0_seamless.html $HOME/.gameapi_builder/gameapi_homepage.html $HOME/.gameapi_builder/gameapi_1.html $HOME/.gameapi_builder/gameapi_script.html $HOME/.gameapi_builder/gameapi_2.html $HOME/.gameapi_builder/gameapi_date.html $HOME/.gameapi_builder/gameapi_3_seamless.html >$HOME/.gameapi_builder/deploy/display.php
    else
        "$ZIP_PATH/cat" $HOME/.gameapi_builder/gameapi_0.html $HOME/.gameapi_builder/gameapi_homepage.html $HOME/.gameapi_builder/gameapi_1.html $HOME/.gameapi_builder/gameapi_script.html $HOME/.gameapi_builder/gameapi_2.html $HOME/.gameapi_builder/gameapi_date.html $HOME/.gameapi_builder/gameapi_3.html >$HOME/.gameapi_builder/deploy/display.php
    fi
    if [ $? -ne 0 ]; then
	echo "cat2 FAIL"
    fi
    "$ZIP_PATH/cp" $HOME/.gameapi_builder/get_file_size.php $HOME/.gameapi_builder/deploy/get_file_size.php
    (cd $HOME/.gameapi_builder/deploy; "$ZIP_PATH/zip" -r gameapi_deploy.zip *)
    if [ $? -ne 0 ]; then
	echo "zip FAIL"
    fi
fi
if [[ ! $1 ]]; then
    echo "./deploy.sh ./path_to_zip.zip"
fi
