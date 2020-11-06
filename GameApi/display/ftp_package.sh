rm -rf build
mkdir build
(cd build;sftp terop@ps555709.dreamhostps.com <../ftp_get.batch)
cp gameapi_example.html build/gameapi_example.html
cp gameapi.js build/gameapi.js
(cd build;zip ../gameapi_display.zip *)
sftp terop@ps555709.dreamhostps.com <./ftp2.batch
