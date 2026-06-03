@echo on
setlocal enabledelayedexpansion
set "P3=%3"
set "P2=%2"
set "P1=%1"
set "P1=%P1:"=%"
set "P2=%P2:"=%"
set "P3=%P3:"=%"
if NOT "%P3%"=="" set "FTEMP=%P3%"
if "%P1%"=="" ECHO deploy path_to_zip_file
if "%P1%"=="" GOTO END
set "OLDDIR=%CD%"
IF EXIST "%OLDDIR%\sed.exe" (
   echo "Sed found."
) else (
   set "OLDDIR=."
)
cd "%FTEMP%\_gameapi_builder\deploy\engine"
set "ZIPFILE=%P1%"
"%OLDDIR%\unzip.exe" "%ZIPFILE%"
if exist "gameapi.js" (
    echo Patching gameapi.js...
    powershell -NoProfile -Command ^
        "(Get-Content gameapi.js) -replace 'web_page','engine/web_page' | Set-Content gameapi2.js"

    move /Y gameapi2.js gameapi.js >nul
)
set "OUTPUT=%FTEMP%\_gameapi_builder\deploy\display.html"
if exist "%OUTPUT%" del "%OUTPUT%"
if "%P2%"=="seamless" (
   type "%FTEMP%\_gameapi_builder\gameapi_0_seamless.html" >> "%OUTPUT%"
   type "%FTEMP%\_gameapi_builder\gameapi_homepage.html" >> "%OUTPUT%"
   type "%FTEMP%\_gameapi_builder\gameapi_1.html"  >> "%OUTPUT%"
   type "%FTEMP%\_gameapi_builder\gameapi_script.html" >> "%OUTPUT%"
   type "%FTEMP%\_gameapi_builder\gameapi_2.html" >> "%OUTPUT%"
   type "%FTEMP%\_gameapi_builder\gameapi_date.html" >> "%OUTPUT%" 
   type "%FTEMP%\_gameapi_builder\gameapi_3_seamless.html" >> "%OUTPUT%" 
) else (
   type "%FTEMP%\_gameapi_builder\gameapi_0.html" >> "%OUTPUT%" 
   type "%FTEMP%\_gameapi_builder\gameapi_homepage.html" >> "%OUTPUT%" 
   type "%FTEMP%\_gameapi_builder\gameapi_1.html" >> "%OUTPUT%" 
   type "%FTEMP%\_gameapi_builder\gameapi_script.html" >> "%OUTPUT%" 
   type "%FTEMP%\_gameapi_builder\gameapi_2.html" >> "%OUTPUT%" 
   type "%FTEMP%\_gameapi_builder\gameapi_date.html" >> "%OUTPUT%" 
   type "%FTEMP%\_gameapi_builder\gameapi_3.html" >> "%OUTPUT%" 
)
cd ..
"%OLDDIR%\zip.exe" -r gameapi_deploy.zip *
:END
endlocal
