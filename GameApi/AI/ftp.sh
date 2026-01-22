#!/bin/bash

SITE=meshpage.org
#SITE=192.168.1.104

INDEX=0
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
finish()
{
    MACHINE=`uname -n`
    if [ "$MACHINE" == "terop-HP-255-G8-Notebook-PC" ]; then
	scp ${arr[@]} terop@$SITE:/home/terop/meshpage.org/
    fi
    if [ "$MACHINE" == "terop-pc2" ]; then
	scp ${arr[@]} terop@$SITE:/home/terop/meshpage.org/
    fi
}
copy_it AI.php
copy_it promptimage.png
copy_it resources.js
copy_it resource_manip.js
finish
