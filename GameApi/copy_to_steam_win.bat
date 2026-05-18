@echo on
if "%1"=="" echo copy_to_steam_win.bat password
if "%1"=="" goto END
git pull
call full_deploy_win32 %1 28 incremental steam

set DISPLAY_DIR=c:\users\terop\cvs\GameApi\GameApi\display
set EDITOR_DIR=c:\users\terop\cvs\GameApi\GameApi\editor
set ROOT_DIR=c:\users\terop\cvs\GameApi\GameApi

set DEPOT_DIR=c:\users\terop\cvs\steam\sdk\tools\ContentBuilder\content\GameApiBuilder\depot_win

copy %DISPLAY_DIR%\gameapi_0.html %DEPOT_DIR%\gameapi_0.html
copy %DISPLAY_DIR%\gameapi_0_seamless.html %DEPOT_DIR%\gameapi_0_seamless.html
copy %DISPLAY_DIR%\gameapi_1.html %DEPOT_DIR%\gameapi_1.html
copy %DISPLAY_DIR%\gameapi_2.html %DEPOT_DIR%\gameapi_2.html
copy %DISPLAY_DIR%\gameapi_3.html %DEPOT_DIR%\gameapi_3.html
copy %DISPLAY_DIR%\gameapi_3_seamless.html %DEPOT_DIR%\gameapi_3_seamless.html

copy %EDITOR_DIR%\atlas0.txt %DEPOT_DIR%\atlas0.txt
copy %EDITOR_DIR%\atlas1.txt %DEPOT_DIR%\atlas1.txt
copy %EDITOR_DIR%\atlas1_t.txt %DEPOT_DIR%\atlas1_t.txt
copy %EDITOR_DIR%\atlas2.txt %DEPOT_DIR%\atlas2.txt

copy %EDITOR_DIR%\atlas_bm0.txt %DEPOT_DIR%\atlas_bm0.txt
copy %EDITOR_DIR%\atlas_bm1.txt %DEPOT_DIR%\atlas_bm1.txt
copy %EDITOR_DIR%\atlas_bm1_t.txt %DEPOT_DIR%\atlas_bm1_t.txt
copy %EDITOR_DIR%\atlas_bm2.txt %DEPOT_DIR%\atlas_bm2.txt

copy %DISPLAY_DIR%\get_file_size.php %DEPOT_DIR%\get_file_size.php

copy %DISPLAY_DIR%\gameapi_display.zip %DEPOT_DIR%\gameapi_display.zip
copy %EDITOR_DIR%\a.exe %DEPOT_DIR%\gameapi_builder.exe
copy %ROOT_DIR%\libGameApi.dll %DEPOT_DIR%\libGameApi.dll
c:\windows\system32\xcopy.exe %DEPOT_DIR%\gameapi_content /E /I /Y
copy %EDITOR_DIR%\release\editor\sed.exe %DEPOT_DIR%\sed.exe
copy %EDITOR_DIR%\deploy.bat %DEPOT_DIR%\deploy.bat
copy %EDITOR_DIR%\tar.exe %DEPOT_DIR%\tar.exe
copy %EDITOR_DIR%\release\editor\zip.exe %DEPOT_DIR%\zip.exe
copy %EDITOR_DIR%\release\editor\bzip2.dll %DEPOT_DIR%\bzip2.dll
copy %EDITOR_DIR%\release\editor\unzip.exe %DEPOT_DIR%\unzip.exe
:END
