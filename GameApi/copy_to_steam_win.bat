@echo off
if "%1"=="" echo copy_to_steam_win.bat password
if "%1"=="" goto END
git pull
full_deploy_win32 %1 28 incremental steam
copy c:\users\terop\cvs\GameApi\GameApi\editor\a.exe c:\users\terop\cvs\steam\sdk\tools\ContentBuilder\content\GameApiBuilder\depot_win\gameapi_builder.exe
copy c:\users\terop\cvs\GameApi\GameApi\libGameApi.dll c:\users\terop\cvs\steam\sdk\tools\ContentBuilder\content\GameApiBuilder\depot_win\libGameApi.dll
:END
