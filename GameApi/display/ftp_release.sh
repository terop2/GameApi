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
#copy_it pessimist.php
copy_it gameapi_0.html
copy_it gameapi_0_seamless.html
copy_it gameapi_1.html
copy_it gameapi_2.html
copy_it gameapi_3.html
copy_it gameapi_3_seamless.html
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
copy_it meshpage.php 
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
