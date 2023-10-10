#!/bin/bash
start_it()
{
    MACHINE=`uname -n`
    if [ "$MACHINE" == "terop-HP-255-G8-Notebook-PC" ]; then
      SSHSOCKET=~/.ssh/terop@meshpage.org
      ssh -M -f -N -o ControlPath=$SSHSOCKET terop@ssh.meshpage.org
    fi
}
copy_it()
{
    MACHINE=`uname -n`
    if [ "$MACHINE" == "terop-HP-255-G8-Notebook-PC" ]; then
	scp -o ControlPath=$SSHSOCKET terop@ssh.meshpage.org:/home/terop/html/meshpage.org/$1 ./build/
    else
	if [ "$MACHINE" == "terop-pc" ]; then
	    cp /home/terop/html/meshpage.org/$1 ./build/
	    echo "copying $1 to build directory"
	else
	    scp https://ssh.meshpage.org/$1 ./build/
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
	if [ "$MACHINE" == "terop-pc" ]; then
	    cp $1 /home/terop/html/meshpage.org/
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
	ssh -S $SSHSOCKET -O exit terop@ssh.meshpage.org
    fi
}
finish2()
{
    MACHINE=`uname -n`
    if [ "$MACHINE" == "terop-HP-255-G8-Notebook-PC" ]; then
	scp ${arr2[@]} terop@ssh.meshpage.org:/home/terop/html/meshpage.org/
    fi

}

rm -rf build
mkdir build
start_it
#copy_it web_page.data 
#copy_it web_page.js 
#copy_it web_page.wasm 
#copy_it web_page.worker.js 
copy_it pthread-main.js 
#copy_it web_page_lowmem.data 
#copy_it web_page_lowmem.js 
#copy_it web_page_lowmem.wasm 
#copy_it web_page_lowmem.worker.js 
copy_it web_page_highmem.data 
copy_it web_page_highmem.js 
copy_it web_page_highmem.wasm 
copy_it web_page_highmem.dwp
copy_it web_page_highmem.wasm.br
copy_it web_page_highmem.worker.js 
#copy_it web_page_nothreads.data 
#copy_it web_page_nothreads.js 
#copy_it web_page_nothreads.wasm 
#copy_it web_page_nothreads.worker.js 
copy_it web_page_lowmem_nothreads.data 
copy_it web_page_lowmem_nothreads.js 
copy_it web_page_lowmem_nothreads.wasm
copy_it web_page_lowmem_nothreads.dwp
copy_it web_page_lowmem_nothreads.wasm.br
#copy_it web_page_lowmem_nothreads.worker.js 
copy_it web_page_nothreads_highmem.data 
copy_it web_page_nothreads_highmem.js 
copy_it web_page_nothreads_highmem.wasm 
copy_it web_page_nothreads_highmem.dwp
copy_it web_page_nothreads_highmem.wasm.br
copy_it web_page_nothreads_highmem.worker.js
finish

cp gameapi_example.html build/gameapi_example.html
cp gameapi.js build/gameapi.js
(cd build;zip ../gameapi_display.zip *)
copy_it2 ./gameapi_example.html
copy_it2 ./gameapi.js
copy_it2 ./gameapi_display.zip
finish2
echo "SUCCESS"
