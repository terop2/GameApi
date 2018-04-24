copy ..\libGameApi.dll release\libGameApi.dll
copy ..\libGameApi.bc release\libGameApi.bc
copy a.exe release\editor\editor.exe
copy ..\GameApi.hh release\GameApi.hh
strip release\libGameApi.dll
strip release\editor\editor.exe
