#!/bin/bash
INDEX=0
copy_it()
{
    MACHINE=`uname -n`
    if [ "$MACHINE" == "terop-HP-255-G8-Notebook-PC" ]; then
	INDEX=${INDEX}+1
	arr[${INDEX}]=$1
    elif [ "$MACHINE" == "terop-pc" ]; then
	INDEX=${INDEX}+1
	arr[${INDEX}]=$1
    else
	 echo "WRONG COMPUTER!";
    fi
}
finish()
{
    MACHINE=`uname -n`
    if [ "$MACHINE" == "terop-HP-255-G8-Notebook-PC" ]; then
	scp ${arr[@]} terop@46.249.53.66:/home/terop/html/
    fi
    if [ "$MACHINE" == "terop-pc" ]; then
	scp ${arr[@]} terop@46.249.53.66:/home/terop/html/
    fi
}

copy_it web_page_highmem.html 
copy_it web_page_highmem.worker.js 
copy_it web_page_highmem.data 
copy_it web_page_highmem.js 
copy_it web_page_highmem.wasm 
copy_it web_page_highmem.wasm.br 
copy_it web_page_highmem.wasm.map 
copy_it web_page.html 
copy_it web_page.data 
copy_it web_page.js 
copy_it web_page.wasm 
copy_it web_page.wasm.br 
copy_it web_page.wasm.map 
#cp pthread-main.js 
copy_it web_page.worker.js 
copy_it web_page_lowmem.html 
copy_it web_page_lowmem.worker.js 
copy_it web_page_lowmem.data 
copy_it web_page_lowmem.js 
copy_it web_page_lowmem.wasm 
copy_it web_page_lowmem.wasm.br 
#cp web_page_lowmem.wasm.mem 
copy_it web_page_nothreads_highmem.html 
copy_it web_page_nothreads_highmem.data 
copy_it web_page_nothreads_highmem.js 
copy_it web_page_nothreads_highmem.wasm 
copy_it web_page_nothreads_highmem.wasm.br 
copy_it web_page_nothreads_highmem.wasm.map 
copy_it web_page_nothreads.html 
copy_it web_page_nothreads.data 
copy_it web_page_nothreads.js 
copy_it web_page_nothreads.wasm 
copy_it web_page_nothreads.wasm.br 
copy_it web_page_nothreads.wasm.map 
copy_it web_page_lowmem_nothreads.data 
copy_it web_page_lowmem_nothreads.js 
copy_it web_page_lowmem_nothreads.wasm 
copy_it web_page_lowmem_nothreads.wasm.br 
finish
echo "COPIED SUCCESSFULLY"


