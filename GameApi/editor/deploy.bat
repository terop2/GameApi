@echo off
if "%1"=="" ECHO deploy path_to_zip_file
if "%1"=="" GOTO END
cd %HOME%\_gameapi_builder/deploy/engine
tar -xf %1
type gameapi.js |%HOME%\_gameapi_builder\sed.exe s@web_page@engine/web_page@ >gameapi2.js
move gameapi2.js gameapi.js
type %HOME%\_gameapi_builder\gameapi_1.html %HOME%\_gameapi_builder\gameapi_script.html %HOME%\_gameapi_builder\gameapi_2.html %HOME%\_gameapi_builder\gameapi_date.html %HOME%\_gameapi_builder\gameapi_3.html >%HOME%\_gameapi_builder\deploy\gameapi_index.php
cd ..
tar -c -f gameapi_deploy.zip gameapi_index.php engine
:END
