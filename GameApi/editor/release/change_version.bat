@echo off
if "%1"=="" ECHO change_version.bat 27
if "%1"=="" GOTO END
sed s/27/%1/g Builder_27.wxs >Builder.wxs
echo "success: 27=>%1"
:END
