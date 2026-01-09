@echo off
if "%1"=="" ECHO full_deploy_win32.bat password version [full,incremental,deploy] [steam]
if "%1"=="" GOTO END
if "%2"=="" ECHO full_deploy_win32.bat password version [full,incremental,deploy] [steam]
if "%2"=="" GOTO END
if "%3"=="deploy" GOTO DEPLOY
if "%3"=="incremental" GOTO INC
if not "%3"=="full" ECHO full_deploy_win32.bat password version [full,incremental,deploy] [steam]
if not "%3"=="full" GOTO END
call set_paths_git
call make clean
GOTO INC2
:INC
call set_paths_git
:INC2
if "%4"=="steam" GOTO STEAM
call make
GOTO DEPLOY
:STEAM
call make "STEAM_ENABLE=yes" "STEAM_DEPLOY_ENABLE=yes"
@echo on
pushd editor & call make "STEAM_ENABLE=yes" & popd
GOTO STEAM_DONE
:DEPLOY
call set_paths_git
@echo on
pushd editor & call make & popd
:STEAM_DONE
pushd editor\release & call copy_files.bat & popd
pushd editor\release & call make_msi.bat %1 & popd
pushd editor\release & call copy Builder.msi GameApi-Builder-v%2.msi & popd
if "%4"=="steam" GOTO NOCOPY
pushd editor\release & call scp GameApi-Builder-v%2.msi terop@meshpage.org:/home/terop/meshpage.org/ & popd
pushd editor\release & call scp GameApi-Builder-v%2.msi terop@meshpage.org:/home/terop/meshpage.org/assets/ & popd
:NOCOPY
if "%4"=="steam" GOTO STEAM2
pushd cmdline & call make -f Makefile.win32 & popd
GOTO STEAM2_DONE
:STEAM2
pushd cmdline & call make -f Makefile.win32 "STEAM_DEPLOY_ENABLE=yes" & popd
:STEAM2_DONE
pushd cmdline & "C:\Program files (x86)\Windows Kits\10\bin\x64\signtool.exe" sign /f ..\..\keys\cert2.pfx /p "%1" gameapi_cmdline.exe & popd
pushd cmdline & call copy gameapi_cmdline.exe zip2\gameapi_cmdline.exe & popd
pushd cmdline & call copy ..\libGameApi.dll zip2\libGameApi.dll & popd
pushd cmdline & call copy ..\libGameApi_bm.dll zip2\libGameApi_bm.dll & popd
pushd cmdline & call copy ..\libGameApi_ml.dll zip2\libGameApi_ml.dll & popd
pushd cmdline & call copy ..\libGameApi_mt.dll zip2\libGameApi_mt.dll & popd
pushd cmdline & call copy ..\libGameApi_pl.dll zip2\libGameApi_pl.dll & popd
pushd cmdline & call copy ..\libGameDep.dll zip2\libGameDep.dll & popd
pushd cmdline\zip2 & call "c:\program files (x86)\gnuwin32\bin\zip" -r gameapi_cmdline.zip *.* & popd
if "%4"=="steam" GOTO NOCP
pushd cmdline\zip2 & call scp gameapi_cmdline.zip terop@meshpage.org:/home/terop/meshpage.org/GameApi_cmdline_%2.zip & popd
:NOCP
:END
