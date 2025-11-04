@echo off
if "%1"=="" ECHO full_deploy_win32.bat password version [full,incremental,deploy]
if "%1"=="" GOTO END
if "%2"=="" ECHO full_deploy_win32.bat password version [full,incremental,deploy]
if "%2"=="" GOTO END
if "%3"=="deploy" GOTO DEPLOY
if "%3"=="incremental" GOTO INC
if not "%3"=="full" ECHO full_deploy_win32.bat password version [full,incremental,deploy]
if not "%3"=="full" GOTO END
call set_paths_git
call make clean
:INC
call set_paths_git
call make
:DEPLOY
call set_paths_git
@echo on
pushd editor & call make & popd
pushd editor\release & call copy_files.bat & popd
pushd editor\release & call make_msi.bat %1 & popd
pushd editor\release & call copy Builder.msi GameApi-Builder-v%2.msi & popd
//pushd editor\release & call scp GameApi-Builder-v%2.msi terop@meshpage.org:/home/terop/meshpage.org/assets/ & popd
pushd cmdline & call make -f Makefile.win32 & popd
pushd cmdline & "C:\Program files (x86)\Windows Kits\10\bin\x64\signtool.exe" sign /f ..\..\keys\cert2.pfx /p "%1" gameapi_cmdline.exe & popd
pushd cmdline & call copy gameapi_cmdline.exe zip2\gameapi_cmdline.exe & popd
pushd cmdline & call copy ..\libGameApi.dll zip2\libGameApi.dll & popd
pushd cmdline & call copy ..\libGameApi_bm.dll zip2\libGameApi_bm.dll & popd
pushd cmdline & call copy ..\libGameApi_ml.dll zip2\libGameApi_ml.dll & popd
pushd cmdline & call copy ..\libGameApi_mt.dll zip2\libGameApi_mt.dll & popd
pushd cmdline & call copy ..\libGameApi_pl.dll zip2\libGameApi_pl.dll & popd
pushd cmdline & call copy ..\libGameDep.dll zip2\libGameDep.dll & popd
pushd cmdline\zip2 & call "c:\program files (x86)\gnuwin32\bin\zip" -r gameapi_cmdline.zip *.* & popd
//pushd cmdline\zip2 & call scp gameapi_cmdline.zip terop@meshpage.org:/home/terop/meshpage.org/assets/GameApi_cmdline_%2.zip & popd
:END
