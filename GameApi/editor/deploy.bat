@echo off
if "%1"=="" ECHO deploy path_to_zip_file
if "%1"=="" GOTO END
set "OLDDIR=%CD%"
cd %TEMP%\_gameapi_builder/deploy/engine
tar -xf %1
type gameapi.js |"%OLDDIR%\sed.exe" s@web_page@engine/web_page@ >gameapi2.js
move gameapi2.js gameapi.js
type %TEMP%\_gameapi_builder\gameapi_1.html %TEMP%\_gameapi_builder\gameapi_script.html %TEMP%\_gameapi_builder\gameapi_2.html %TEMP%\_gameapi_builder\gameapi_date.html %TEMP%\_gameapi_builder\gameapi_3.html >%TEMP%\_gameapi_builder\deploy\gameapi_index.php
cd ..
tar -c -f gameapi_deploy.zip gameapi_index.php engine
:END