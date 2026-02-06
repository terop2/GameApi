#!/bin/bash

SITE=meshpage.org
#SITE=192.168.1.104

INDEX=0
INDEX_2=0
copy_it()
{
    MACHINE=`uname -n`
    if [ "$MACHINE" == "terop-HP-255-G8-Notebook-PC" ]; then
	INDEX=${INDEX}+1
	arr[${INDEX}]=$1
    elif [ "$MACHINE" == "terop-pc2" ]; then
	INDEX=${INDEX}+1
	arr[${INDEX}]=$1
    else
       scp $1 terop@$SITE:/home/terop/meshpage.org/
       echo "copying $1 to $SITE"
    fi
}
copy_it_2()
{
    MACHINE=`uname -n`
    if [ "$MACHINE" == "terop-HP-255-G8-Notebook-PC" ]; then
	INDEX_2=${INDEX_2}+1
	arr_2[${INDEX_2}]=$1
    elif [ "$MACHINE" == "terop-pc2" ]; then
	INDEX_2=${INDEX_2}+1
	arr_2[${INDEX_2}]=$1
    else
       scp $1 terop@$SITE:/home/terop/meshpage.org/AI_engine
       echo "copying $1 to $SITE"
    fi
}
finish()
{
    MACHINE=`uname -n`
    if [ "$MACHINE" == "terop-HP-255-G8-Notebook-PC" ]; then
	scp ${arr[@]} terop@$SITE:/home/terop/meshpage.org/
	scp ${arr_2[@]} terop@$SITE:/home/terop/meshpage.org/AI_engine/
    fi
    if [ "$MACHINE" == "terop-pc2" ]; then
	scp ${arr[@]} terop@$SITE:/home/terop/meshpage.org/
	scp ${arr_2[@]} terop@$SITE:/home/terop/meshpage.org/AI_engine/
    fi
}
copy_it github_access.php
copy_it AI.php
copy_it AI_display.php
copy_it AI_result.php
copy_it promptimage.png
copy_it resources.js
copy_it resource_manip.js
copy_it gameapi_manip.js
copy_it_2 AI_engine/gameapi.js
copy_it_2 AI_engine/engine_highmem.js
copy_it_2 AI_engine/engine_highmem.wasm
copy_it_2 AI_engine/engine_highmem.wasm.br
copy_it_2 AI_engine/engine_lowmem_nothreads.js
copy_it_2 AI_engine/engine_lowmem_nothreads.wasm
copy_it_2 AI_engine/engine_lowmem_nothreads.wasm.br
copy_it_2 AI_engine/engine_nothreads_highmem.js
copy_it_2 AI_engine/engine_nothreads_highmem.wasm
copy_it_2 AI_engine/engine_nothreads_highmem.wasm.br
finish
