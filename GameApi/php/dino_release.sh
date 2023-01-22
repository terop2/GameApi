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

copy_it cookies.php
copy_it mesh_background.php
copy_it mesh_addtext.php
copy_it builder_display.php
copy_it mesh_stats.php
copy_it mesh_pre.php
copy_it mesh_api.php
copy_it mesh_embed.php
copy_it mesh_clickable.php
copy_it mesh_server.php
copy_it mesh_css.css 
copy_it mesh_css_mobile.css
copy_it mesh_hide.php 
copy_it mesh_hide_action.php
copy_it mesh_block.php 
copy_it mesh_block_action.php 
copy_it mesh_code.php 
copy_it mesh_faq.php 
copy_it user.php 
copy_it user_page.php 
copy_it create_mesh.php 
copy_it emscripten_frame.php 
#copy_it mesh.php /home/terop/html/meshpage.org/
copy_it mesh_preview.php 
copy_it action_new_mesh.php 
copy_it action_progress_mesh.php 
copy_it builder.php 
copy_it source_table.php 
copy_it save_screenshot.php 
copy_it load_url.php 
copy_it load_date.php 
copy_it mesh_save.php 
copy_it mesh_all.php 
copy_it mesh_tool.php 
copy_it index.html 
copy_it mesh_about.php 
copy_it mesh_login.php 
copy_it mesh_logout.php 
copy_it mesh_login_result.php 
copy_it mapping.txt 
copy_it mesh_tutorial.php 
copy_it mesh_detect.php 
copy_it mesh_html.php 
finish
echo "SUCCESS"
