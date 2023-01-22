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
#copy_it pessimist.php
cp meshpage.php games.php
copy_it dino3.webp
copy_it get_file_size.php
copy_it AI_prompt.php
copy_it article.php
copy_it gameapi.js
copy_it ini_print.php
copy_it save_grab.php
copy_it backend.php 
copy_it view.php 
copy_it view_fetch.php
copy_it submit_contents.php 
copy_it games.php 
copy_it cert.php 
copy_it cert_linux.php 
copy_it login.php 
copy_it oauth2.php 
copy_it oauth2_callback.php 
copy_it material_db.txt 
copy_it border_db.txt 
copy_it background_db.txt 
copy_it model_db.txt 
copy_it gameapi_example.php
copy_it save_tmp_script.php
copy_it view_load_file.php
copy_it zip.min.js
copy_it simple_file_cache
finish
echo "SUCCESS"
