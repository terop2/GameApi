#!/bin/bash
INDEX=0
copy_it()
{
    MACHINE=`uname -n`
    if [ "$MACHINE" == "terop-HP-255-G8-Notebook-PC" ]; then
	INDEX=${INDEX}+1
	arr[${INDEX}]=$1
     else
       cp $1 /home/terop/html/meshpage.org/
       echo "copying $1 to meshpage.org"
    fi
}
finish()
{
    MACHINE=`uname -n`
    if [ "$MACHINE" == "terop-HP-255-G8-Notebook-PC" ]; then
	scp ${arr[@]} terop@ssh.meshpage.org:/home/terop/html/meshpage.org/
    fi
}
copy_it dump_docs
copy_it ../libGameApi.so
copy_it parser
