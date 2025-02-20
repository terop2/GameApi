#!/bin/bash
start_it()
{
    MACHINE=`uname -n`
    if [ "$MACHINE" == "terop-HP-255-G8-Notebook-PC" ]; then
      SSHSOCKET=~/.ssh/terop@meshpage.org
      sshpass -p $1 ssh -f -N -o ControlPath=$SSHSOCKET terop@ssh.meshpage.org
    fi
}
copy_it()
{
    MACHINE=`uname -n`
    if [ "$MACHINE" == "terop-HP-255-G8-Notebook-PC" ]; then
	sshpass -p $2 scp -o ControlPath=$SSHSOCKET terop@ssh.meshpage.org:/home/terop/meshpage.org/$1 ./build/
    else
	if [ "$MACHINE" == "terop-pc2" ]; then
	    cp /home/terop/meshpage.org/$1 ./build/
	    echo "copying $1 to build directory"
	else
	    sshpass -p $1 scp https://ssh.meshpage.org/$1 ./build/
	fi
    fi
}
copy_it2()
{
    MACHINE=`uname -n`
    if [ "$MACHINE" == "terop-HP-255-G8-Notebook-PC" ]; then
	INDEX2=${INDEX2}+1
	arr2[${INDEX2}]=$1
    else
	if [ "$MACHINE" == "terop-pc2" ]; then
	    cp $1 /home/terop/meshpage.org/
	    echo "copying $1 to build directory"
	else
	    echo "NOT COPYING $1 SINCE YOU'RE NOT IN OUR DEV COMPUTER"
	fi
    fi
}
finish()
{
    MACHINE=`uname -n`
    if [ "$MACHINE" == "terop-HP-255-G8-Notebook-PC" ]; then
	sshpass -p $1 ssh -S $SSHSOCKET -O exit terop@ssh.meshpage.org
    fi
}
finish2()
{
    MACHINE=`uname -n`
    if [ "$MACHINE" == "terop-HP-255-G8-Notebook-PC" ]; then
	sshpass -p $1 scp ${arr2[@]} terop@ssh.meshpage.org:/home/terop/meshpage.org/
    fi

}


if [[ $1 == "" ]]; then
    echo "Usage: ./ftp_package.sh password"
    exit
fi

rm -rf build
mkdir build
start_it $1
#copy_it web_page.data 
#copy_it web_page.js 
#copy_it web_page.wasm 
#copy_it web_page.worker.js 
copy_it pthread-main.js $1 
#copy_it web_page_lowmem.data 
#copy_it web_page_lowmem.js 
#copy_it web_page_lowmem.wasm 
#copy_it web_page_lowmem.worker.js 
copy_it engine_highmem.data $1 
copy_it engine_highmem.js $1
copy_it engine_highmem.wasm $1
copy_it engine_highmem.dwp $1
copy_it engine_highmem.wasm.br $1
copy_it engine_highmem.worker.js $1
#copy_it web_page_nothreads.data 
#copy_it web_page_nothreads.js 
#copy_it web_page_nothreads.wasm 
#copy_it web_page_nothreads.worker.js 
copy_it engine_lowmem_nothreads.data $1 
copy_it engine_lowmem_nothreads.js $1
copy_it engine_lowmem_nothreads.wasm $1
copy_it engine_lowmem_nothreads.dwp $1
copy_it engine_lowmem_nothreads.wasm.br $1
#copy_it web_page_lowmem_nothreads.worker.js 
copy_it engine_nothreads_highmem.data $1
copy_it engine_nothreads_highmem.js $1
copy_it engine_nothreads_highmem.wasm $1
copy_it engine_nothreads_highmem.dwp $1
copy_it engine_nothreads_highmem.wasm.br $1
copy_it engine_nothreads_highmem.worker.js $1
finish

cp gameapi_example.html build/gameapi_example.html
cp gameapi.js build/gameapi.js
(cd build;zip ../gameapi_display.zip *)
copy_it2 ./gameapi_example.html $1
copy_it2 ./gameapi.js $1
copy_it2 ./gameapi_display.zip $1
finish2
echo "SUCCESS"
