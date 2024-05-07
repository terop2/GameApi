#!/bin/bash
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
       cp $1 /home/terop/meshpage.org/
       echo "copying $1 to meshpage.org"
    fi
}
finish()
{
    MACHINE=`uname -n`
    if [ "$MACHINE" == "terop-HP-255-G8-Notebook-PC" ]; then
	scp ${arr[@]} terop@ssh.meshpage.org:/home/terop/meshpage.org/
    fi
    if [ "$MACHINE" == "terop-pc2" ]; then
	scp ${arr[@]} terop@ssh.meshpage.org:/home/terop/meshpage.org/
    fi
}
copy_it dump_docs
copy_it parser
copy_it dump.txt
copy_it ../libGameApi.so
finish
