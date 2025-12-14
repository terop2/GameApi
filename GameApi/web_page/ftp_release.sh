
#!/bin/bash
SITE=meshpage.org
#SITE=192.168.1.104

INDEX_1=0
INDEX_2=0
INDEX_3=0
INDEX_4=0
INDEX_5=0
copy_it_1()
{
    MACHINE=`uname -n`
    if [ "$MACHINE" == "terop-HP-255-G8-Notebook-PC" ]; then
	INDEX_1=${INDEX_1}+1
	arr_1[${INDEX_1}]=$1
     else
       cp $1 $2
       echo "copying $1 to meshpage.org"
    fi
}
copy_it_2()
{
    MACHINE=`uname -n`
    if [ "$MACHINE" == "terop-HP-255-G8-Notebook-PC" ]; then
	INDEX_2=${INDEX_2}+1
	arr_2[${INDEX_2}]=$1
     else
       cp $1 $2
       echo "copying $1 to meshpage.org"
    fi
}
copy_it_3()
{
    MACHINE=`uname -n`
    if [ "$MACHINE" == "terop-HP-255-G8-Notebook-PC" ]; then
	INDEX_3=${INDEX_3}+1
	arr_3[${INDEX_3}]=$1
     else
       cp $1 $2
       echo "copying $1 to meshpage.org"
    fi
}
copy_it_4()
{
    MACHINE=`uname -n`
    if [ "$MACHINE" == "terop-HP-255-G8-Notebook-PC" ]; then
	INDEX_4=${INDEX_4}+1
	arr_4[${INDEX_4}]=$1
     else
       cp $1 $2
       echo "copying $1 to meshpage.org"
    fi
}
copy_it_5()
{
    MACHINE=`uname -n`
    if [ "$MACHINE" == "terop-HP-255-G8-Notebook-PC" ]; then
	INDEX_5=${INDEX_5}+1
	arr_5[${INDEX_5}]=$1
     else
       cp $1 $2
       echo "copying $1 to meshpage.org"
    fi
}
finish()
{
    MACHINE=`uname -n`
    if [ "$MACHINE" == "terop-HP-255-G8-Notebook-PC" ]; then
	echo "Copying to meshpage.org/";
	scp ${arr_1[@]} terop@$SITE:/home/terop/meshpage.org/
	echo "Copying to meshpage.org/punk/engine/";
	scp ${arr_2[@]} terop@$SITE:/home/terop/meshpage.org/punk/engine/
	echo "Copying to meshpage.org/ld57/engine/";
	scp ${arr_3[@]} terop@$SITE:/home/terop/meshpage.org/ld57/engine/
	echo "Copying to meshpage.org/engine/";
	scp ${arr_4[@]} terop@$SITE:/home/terop/meshpage.org/engine/
	echo "Copying to meshpage.org/demoreboot/engine/";
	scp ${arr_5[@]} terop@$SITE:/home/terop/meshpage.org/demoreboot/engine/
	echo "Copying done.";
    fi
}

#copy_it_1 engine_highmem.worker.js /home/terop/meshpage.org/
copy_it_1 engine_highmem.js /home/terop/meshpage.org/
copy_it_1 engine_highmem.wasm /home/terop/meshpage.org/
copy_it_1 engine_highmem.wasm.br /home/terop/meshpage.org/
copy_it_1 engine_nothreads_highmem.js /home/terop/meshpage.org/
copy_it_1 engine_nothreads_highmem.wasm /home/terop/meshpage.org/
copy_it_1 engine_nothreads_highmem.wasm.br /home/terop/meshpage.org/
copy_it_1 engine_lowmem_nothreads.js /home/terop/meshpage.org/
copy_it_1 engine_lowmem_nothreads.wasm /home/terop/meshpage.org/
copy_it_1 engine_lowmem_nothreads.wasm.br /home/terop/meshpage.org/

#copy_it_2 engine_highmem.worker.js /home/terop/meshpage.org/punk/engine/
copy_it_2 engine_highmem.js /home/terop/meshpage.org/punk/engine/
copy_it_2 engine_highmem.wasm /home/terop/meshpage.org/punk/engine/
copy_it_2 engine_highmem.wasm.br /home/terop/meshpage.org/punk/engine/
copy_it_2 engine_nothreads_highmem.js /home/terop/meshpage.org/punk/engine/
copy_it_2 engine_nothreads_highmem.wasm /home/terop/meshpage.org/punk/engine/
copy_it_2 engine_nothreads_highmem.wasm.br /home/terop/meshpage.org/punk/engine/
copy_it_2 engine_lowmem_nothreads.js /home/terop/meshpage.org/punk/engine/
copy_it_2 engine_lowmem_nothreads.wasm /home/terop/meshpage.org/punk/engine/
copy_it_2 engine_lowmem_nothreads.wasm.br /home/terop/meshpage.org/punk/engine/

#copy_it_3 engine_highmem.worker.js /home/terop/meshpage.org/ld57/engine/
copy_it_3 engine_highmem.js /home/terop/meshpage.org/ld57/engine/
copy_it_3 engine_highmem.wasm /home/terop/meshpage.org/ld57/engine/
copy_it_3 engine_highmem.wasm.br /home/terop/meshpage.org/ld57/engine/
copy_it_3 engine_nothreads_highmem.js /home/terop/meshpage.org/ld57/engine/
copy_it_3 engine_nothreads_highmem.wasm /home/terop/meshpage.org/ld57/engine/
copy_it_3 engine_nothreads_highmem.wasm.br /home/terop/meshpage.org/ld57/engine/
copy_it_3 engine_lowmem_nothreads.js /home/terop/meshpage.org/ld57/engine/
copy_it_3 engine_lowmem_nothreads.wasm /home/terop/meshpage.org/ld57/engine/
copy_it_3 engine_lowmem_nothreads.wasm.br /home/terop/meshpage.org/ld57/engine/


#copy_it_4 engine_highmem.worker.js /home/terop/meshpage.org/engine/
copy_it_4 engine_highmem.js /home/terop/meshpage.org/engine/
copy_it_4 engine_highmem.wasm /home/terop/meshpage.org/engine/
copy_it_4 engine_highmem.wasm.br /home/terop/meshpage.org/engine/
copy_it_4 engine_nothreads_highmem.js /home/terop/meshpage.org/engine/
copy_it_4 engine_nothreads_highmem.wasm /home/terop/meshpage.org/engine/
copy_it_4 engine_nothreads_highmem.wasm.br /home/terop/meshpage.org/engine/
copy_it_4 engine_lowmem_nothreads.js /home/terop/meshpage.org/engine/
copy_it_4 engine_lowmem_nothreads.wasm /home/terop/meshpage.org/engine/
copy_it_4 engine_lowmem_nothreads.wasm.br /home/terop/meshpage.org/engine/


copy_it_5 engine_highmem.js /home/terop/meshpage.org/demoreboot/engine/
copy_it_5 engine_highmem.wasm /home/terop/meshpage.org/demoreboot/engine/
copy_it_5 engine_highmem.wasm.br /home/terop/meshpage.org/demoreboot/engine/
copy_it_5 engine_nothreads_highmem.js /home/terop/meshpage.org/demoreboot/engine/
copy_it_5 engine_nothreads_highmem.wasm /home/terop/meshpage.org/demoreboot/engine/
copy_it_5 engine_nothreads_highmem.wasm.br /home/terop/meshpage.org/demoreboot/engine/
copy_it_5 engine_lowmem_nothreads.js /home/terop/meshpage.org/demoreboot/engine/
copy_it_5 engine_lowmem_nothreads.wasm /home/terop/meshpage.org/demoreboot/engine/
copy_it_5 engine_lowmem_nothreads.wasm.br /home/terop/meshpage.org/demoreboot/engine/


finish

echo COPIED SUCCESSFULLY


