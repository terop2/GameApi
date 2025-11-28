scp terop@192.168.1.104:/home/terop/meshpage.org/gameapi_display.zip ..\..\display\gameapi_display.zip
copy ..\steam_api64.dll editor\steam_api64.dll
copy ..\..\display\gameapi_display.zip editor\gameapi_display.zip
copy ..\..\display\gameapi_0.html editor\gameapi_0.html
copy ..\..\display\gameapi_0_seamless.html editor\gameapi_0_seamless.html
copy ..\..\display\gameapi_1.html editor\gameapi_1.html
copy ..\..\display\gameapi_2.html editor\gameapi_2.html
copy ..\..\display\gameapi_3.html editor\gameapi_3.html
copy ..\..\display\gameapi_3_seamless.html editor\gameapi_3_seamless.html
copy ..\..\zip\sed.exe editor\sed.exe
copy ..\deploy.bat editor\deploy.bat
copy ..\freetype.dll editor\freetype.dll
copy ..\pthreadGC2.dll editor\pthreadGC2.dll
copy ..\atlas0.txt editor\atlas0.txt
copy ..\atlas1.txt editor\atlas1.txt
copy ..\atlas2.txt editor\atlas2.txt
c:\windows\system32\xcopy ..\gameapi_content editor\gameapi_content /E /I /Y
copy ..\atlas_bm0.ppm editor\atlas_bm0.ppm
copy ..\atlas_bm1.ppm editor\atlas_bm1.ppm
copy ..\atlas_bm2.ppm editor\atlas_bm2.ppm
copy ..\..\display\get_file_size.php editor\get_file_size.php
copy ..\arrow.ppm editor\arrow.ppm
copy ..\..\libgameapi.dll .
strip libgameapi.dll
copy ..\..\libgameapi.a .
copy ..\a.exe editor\edit.exe
strip editor\edit.exe
