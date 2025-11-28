@echo off
set "P3=%3"
set "P2=%2"
set "P1=%1"
set "P1=%P1:"=%"
set "P2=%P2:"=%"
set "P3=%P3:"=%"
if NOT "%P3%"=="" set "TEMP=%P3%"
if "%P1%"=="" ECHO deploy path_to_zip_file
if "%P1%"=="" GOTO END
set "OLDDIR=%CD%"
IF EXIST "%OLDDIR%\sed.exe" (
   echo "Sed found."
) else (
   set "OLDDIR=."
)
cd "%TEMP%\_gameapi_builder/deploy/engine"
set "ZIPFILE=%P1%"
"%OLDDIR%\tar.exe" -xf "%ZIPFILE%"
type gameapi.js |"%OLDDIR%\sed.exe" s@web_page@engine/web_page@ >gameapi2.js
move gameapi2.js gameapi.js
set "OUTPUT=%TEMP%\_gameapi_builder\deploy\display.php"
if "%P2%"=="seamless" (
> "%OUTPUT%" (
   type "%TEMP%\_gameapi_builder\gameapi_0_seamless.html" 
   type "%TEMP%\_gameapi_builder\gameapi_homepage.html" 
   type "%TEMP%\_gameapi_builder\gameapi_1.html" 
   type "%TEMP%\_gameapi_builder\gameapi_script.html" 
   type "%TEMP%\_gameapi_builder\gameapi_2.html" 
   type "%TEMP%\_gameapi_builder\gameapi_date.html" 
   type "%TEMP%\_gameapi_builder\gameapi_3_seamless.html" 
   )
)
if NOT "%P2%"=="seamless" (
> "%OUTPUT%" (
   type "%TEMP%\_gameapi_builder\gameapi_0.html" 
   type "%TEMP%\_gameapi_builder\gameapi_homepage.html" 
   type "%TEMP%\_gameapi_builder\gameapi_1.html" 
   type "%TEMP%\_gameapi_builder\gameapi_script.html" 
   type "%TEMP%\_gameapi_builder\gameapi_2.html" 
   type "%TEMP%\_gameapi_builder\gameapi_date.html" 
   type "%TEMP%\_gameapi_builder\gameapi_3.html" 
   )
)
copy "%TEMP%\_gameapi_builder\get_file_size.php" "%TEMP%\_gameapi_builder\deploy\get_file_size.php"
cd ..
powershell Compress-Archive * gameapi_deploy.zip
:END
