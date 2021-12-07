copy ..\atlas0.txt editor\atlas0.txt
copy ..\atlas1.txt editor\atlas1.txt
copy ..\atlas2.txt editor\atlas2.txt
copy ..\atlas_bm0.ppm editor\atlas_bm0.ppm
copy ..\atlas_bm1.ppm editor\atlas_bm1.ppm
copy ..\atlas_bm2.ppm editor\atlas_bm2.ppm
copy ..\..\libgameapi.dll .
strip libgameapi.dll
copy ..\..\libgamedep.dll .
strip libgamedep.dll
copy ..\a.exe editor\edit.exe
strip editor\edit.exe
