@echo off
if "%1"=="" ECHO make_msi password
if "%1"=="" GOTO END
"C:\Program Files (x86)\WiX Toolset v3.11\bin\candle.exe" Builder.wxs
"C:\Program Files (x86)\WiX Toolset v3.11\bin\light.exe" Builder.wixobj
"C:\Program Files (x86)\Windows Kits\10\bin\x86\signtool.exe" sign /f ..\guid\Builder.pfx /p "%1" Builder.msi
:END