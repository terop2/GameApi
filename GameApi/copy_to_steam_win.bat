@echo on
if "%1"=="" echo copy_to_steam_win.bat password
if "%1"=="" goto END
git pull
call full_deploy_win32 %1 28 incremental steam
copy c:\users\terop\cvs\GameApi\GameApi\editor\a.exe c:\users\terop\cvs\steam\sdk\tools\ContentBuilder\content\GameApiBuilder\depot_win\gameapi_builder.exe
copy c:\users\terop\cvs\GameApi\GameApi\libGameApi.dll c:\users\terop\cvs\steam\sdk\tools\ContentBuilder\content\GameApiBuilder\depot_win\libGameApi.dll
c:\windows\system32\xcopy.exe c:\users\terop\cvs\GameApi\GameApi\gameapi_content c:\users\terop\cvs\steam\sdk\tools\ContentBuilder\content\GameApiBuilder\depot_win\gameapi_content /E /I /Y
copy c:\users\terop\cvs\GameApi\GameApi\editor\release\editor\sed.exe c:\users\terop\cvs\steam\sdk\tools\ContentBuilder\content\GameApiBuilder\depot_win\sed.exe
copy c:\users\terop\cvs\GameApi\GameApi\editor\deploy.bat c:\users\terop\cvs\steam\sdk\tools\ContentBuilder\content\GameApiBuilder\depot_win\deploy.bat
copy c:\users\terop\cvs\GameApi\GameApi\editor\release\editor\tar.exe c:\users\terop\cvs\steam\sdk\tools\ContentBuilder\content\GameApiBuilder\depot_win\tar.exe
:END
